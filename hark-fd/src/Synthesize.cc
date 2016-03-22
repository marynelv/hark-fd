/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Map.h"

#include "fft.hpp"

using namespace std;
using namespace FD;
using namespace boost::numeric;

class Synthesize;

DECLARE_NODE(Synthesize)
/*Node
 *
 * @name Synthesize
 * @category HARK:MISC
 * @description Resynthesize audio signals in time domain from spectra.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Input spectra. The key is source ID, and the value is a spectrum (Vector<complex<float> >).
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Resynthesized audio signals in time domain.
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description Size of window length in sample. [default: 512]
 *
 * @parameter_name ADVANCE
 * @parameter_type int
 * @parameter_value 160
 * @parameter_description The length in sample between a frame and a previous frame. [default: 160]
 *
 * @parameter_name SAMPLING_RATE
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_description Sampling rate (Hz) [default: 16000].
 *
 * @parameter_name MIN_FREQUENCY
 * @parameter_type int
 * @parameter_value 125
 * @parameter_description Minimum frequency (Hz) [default: 125]
 *
 * @parameter_name MAX_FREQUENCY
 * @parameter_type int
 * @parameter_value 7900
 * @parameter_description Maximum frequency (Hz) [default: 7900]
 *
 * @parameter_name WINDOW
 * @parameter_type string
 * @parameter_value HAMMING
 * @parameter_list CONJ:HAMMING:RECTANGLE:HANNING
 * @parameter_description A window function for overlap-add. WINDOW should be CONJ, HAMMING, RECTANGLE, or HANNING. [default: HAMMING]
 *
 * @parameter_name OUTPUT_GAIN
 * @parameter_type float
 * @parameter_value 1.0
 * @parameter_description Output gain factor. [default: 1.0]
 *
 END*/

enum WINDOW_TYPE {
  CONJ_WINDOW, RECTANGLE_WINDOW, HAMMING_WINDOW, HANNING_WINDOW
};

class Synthesize : public BufferedNode {
  int inputID;
  int outputID;

  int length;
  int advance;
  int overlap_frame;
  int sampling_rate;
  int window_type;
  int min_freq;
  int max_freq;

  int min_freq_ind;
  int max_freq_ind;

  rfft<float> rfft1;
  RCPtr<Buffer> wave_buffer;
  ublas::vector<float> window;
  ublas::vector<float> hamming;
  vector<float> wave;
  map<int, vector<float> > last_frame;
  bool enable_synthesis;
  bool enable_sw_save;
  float wn_factor;
  float output_gain;
    
public:
  Synthesize(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");

    length = dereference_cast<int>(parameters.get("LENGTH"));
    advance = dereference_cast<int>(parameters.get("ADVANCE"));
    sampling_rate = dereference_cast<int>(parameters.get("SAMPLING_RATE"));
    output_gain = dereference_cast<float>(parameters.get("OUTPUT_GAIN"));
    min_freq = dereference_cast<int>(parameters.get("MIN_FREQUENCY"));
    max_freq = dereference_cast<int>(parameters.get("MAX_FREQUENCY"));
    if (!(min_freq >= 0 && min_freq <= sampling_rate / 2 &&
          max_freq >= 0 && max_freq <= sampling_rate / 2 &&
          min_freq <= max_freq)) {
      throw new NodeException(NULL, "SAMPLING_RATE, MIN_FREQUENCY or MAX_FREQUENCY is wrong.",
                              __FILE__, __LINE__);
    }

    min_freq_ind = length * min_freq / sampling_rate;
    max_freq_ind = length * max_freq / sampling_rate;

    overlap_frame = length / advance - (length % advance ? 0 : 1);

    wn_factor = sqrtf(length) / 2.0f;

    inputsCache[inputID].lookAhead = overlap_frame;

    rfft1.initialize(length);
    wave_buffer = new Buffer(overlap_frame + 1);

    string window_name = object_cast<String>(parameters.get("WINDOW"));
    if (window_name == "CONJ") {
      window_type = CONJ_WINDOW;
    }
    else if (window_name == "RECTANGLE") {
      window_type = RECTANGLE_WINDOW;
    }
    else if (window_name == "HAMMING") {
      window_type = HAMMING_WINDOW;
    }
    else if (window_name == "HANNING") {
      window_type = HANNING_WINDOW;
    }
    else {
      throw new NodeException(NULL, "MultiFFT: WINDOW should be RECTANGLE, CONJ or HAMMING.", __FILE__, __LINE__);
    }

    window.resize(length);
    switch (window_type) {
    case CONJ_WINDOW:
      ConjWindow(window);
      break;
    case RECTANGLE_WINDOW:
      for (int i = 0; i < window.size(); i++) {
        window[i] = 1.0f;
      }
      break;
    case HAMMING_WINDOW:
      Hamming(window);
      break;
    case HANNING_WINDOW:
      Hanning(window);
      break;
    }


    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Map<int, ObjectRef> > output(new Map<int, ObjectRef>);
    Buffer& outputBuffer = *(outputs[outputID].buffer);
    outputBuffer[count] = output;

    // Perform overlap add for 0 time frame.
    if (count == 0) {
      for (int i = 0; i < overlap_frame; i++) {
        SynthesizeFrame(i);

        const Map<int, ObjectRef>& wave_map =
          object_cast<Map<int, ObjectRef> >((*wave_buffer)[i]);
        Map<int, ObjectRef>::const_iterator it = wave_map.begin();
        int begin = 0;
        int end = length - advance * (i + 1);
        int lf_begin = advance * i;
        int lf_end = length - advance;
                
        for (; it != wave_map.end(); ++it) {
          const Vector<float>& w =
            object_cast<Vector<float> >(it->second);
          vector<float>& lf = last_frame[it->first];
          lf.resize(length, 0.0f);

          for (int i = 0; i < end; i++) {
            lf[lf_begin + i] += w[i];
          }
        }
      }
    }
    SynthesizeFrame(count + overlap_frame);

    // Perform overlap add.
    for (int frame = 0; frame <= overlap_frame; frame++) {
      const Map<int, ObjectRef>& wave_map =
        object_cast<Map<int, ObjectRef> >((*wave_buffer)[count+frame]);
      Map<int, ObjectRef>::const_iterator it = wave_map.begin();
            
      int begin = max(length - advance * (frame + 1), 0);
      int end = length - advance * frame;
      int lf_begin = length - (end - begin);
      for (; it != wave_map.end(); ++it) {
        const Vector<float>& w =
          object_cast<Vector<float> >(it->second);
        vector<float>& lf = last_frame[it->first];
        lf.resize(length, 0.0f);

        for (int i = 0; i < end - begin; i++) {
          lf[lf_begin + i] += w[begin + i];
        }
      }
    }

    // Remove sound sources which have gone from the last_frame map.
    map<int, vector<float> >::iterator it = last_frame.begin();
    while (it != last_frame.end()) {
      bool srcid_exists = false;
      for (int frame = 0; frame <= overlap_frame; frame++) {
        const Map<int, ObjectRef>& wave_map =
          object_cast<Map<int, ObjectRef> >((*wave_buffer)[count+frame]);
        if (wave_map.find(it->first) != wave_map.end()) {
          srcid_exists = true;
          break;
        }
      }
      if (!srcid_exists) {
        last_frame.erase(it++);
      }
      else {
        ++it;
      }
    }
    
    for (map<int, vector<float> >::iterator it_lf = last_frame.begin();
         it_lf != last_frame.end(); ++it_lf) {
      RCPtr<Map<int, ObjectRef> > spec_map = getInput(inputID, count);
      if (spec_map->find(it_lf->first) != spec_map->end()) {
        RCPtr<Vector<float> > x(Vector<float>::alloc(it_lf->second.size()));
        copy(it_lf->second.begin(), it_lf->second.end(), x->begin());
        for (int i = 0; i < x->size(); i++) {
          (*x)[i] *= output_gain;
        }
        (*output)[it_lf->first] = x;
      }
            
      for (int i = 0; i < length - advance; i++) {
        it_lf->second[i] = it_lf->second[i + advance];
      }
      for (int i = length - advance; i < length; i++) {
        it_lf->second[i] = 0.0f;
      }
    }

  }

  void SynthesizeFrame(int count)
  {
    RCPtr<Map<int, ObjectRef> > spec_map_ptr = getInput(inputID, count);
    const Map<int, ObjectRef>& spec_map = *spec_map_ptr;
    RCPtr<Map<int, ObjectRef> > wave_map(new Map<int, ObjectRef>);

    Map<int, ObjectRef>::const_iterator it = spec_map.begin();
    for (; it != spec_map.end(); ++it) {
      const Vector<complex<float> >& spec =
        object_cast<Vector<complex<float> > >(it->second);
      RCPtr<Vector<float> > wave(Vector<float>::alloc(length));

      for (int k = 0; k < rfft1.flength(); k++) {
        if (k < min_freq_ind || k > max_freq_ind) {
          //fft.complex_data[k] = 0.0f;
          rfft1.A[k] = 0.0f;
        }
        else {
          //fft.complex_data[k] = spec[k];
          rfft1.A[k] = spec[k];
        }
      }
      //fft.CalcBackward();
      rfft1.synthesize();
			
      for (int i = 0; i < length; i++) {
        //wave[i] = fft.real_data[i] * (1.0f / fft.length) * window[i];
        //wave[i] = fft.real_data[i] * (1.0f / fft.length) * hamming[i];
        (*wave)[i] = rfft1.a[i] * (1.0f / rfft1.tlength()) * window[i];
      }
      (*wave_map)[it->first] = wave;
    }
    (*wave_buffer)[count] = wave_map;
  }

  IN_ORDER_NODE_SPEEDUP(Synthesize);

};


