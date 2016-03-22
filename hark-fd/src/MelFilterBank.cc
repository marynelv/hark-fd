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

using namespace std;
using namespace FD;

class MelFilterBank;

DECLARE_NODE(MelFilterBank)
/*Node
 *
 * @name MelFilterBank
 * @category HARK:FeatureExtraction
 * @description Calculate a Mel-scale spectrum from a linear-scale spectrum.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Input Spectrum. The key is source ID, and the value is a spectrum (Vector<float> or Vector<complex<float> >).
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Output Mel-scale spectrum. The key is source ID, the value is a Mel-scale spectrum (Vector<float>).
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description Size of window length in sample. [default: 512]
 *
 * @parameter_name SAMPLING_RATE
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_description Sampling rate in Hz.  [default: 16000]
 *
 * @parameter_name CUTOFF
 * @parameter_type int
 * @parameter_value 8000
 * @parameter_description Cutoff frequency in Hz. Mel-filterbanks are placed between 0 Hz and CUTOFF Hz. [default: 8000]
 *
 * @parameter_name MIN_FREQUENCY
 * @parameter_type int
 * @parameter_value 63
 * @parameter_description Minimum frequency (Hz) [default: 63]
 *
 * @parameter_name MAX_FREQUENCY
 * @parameter_type int
 * @parameter_value 8000
 * @parameter_description Maximum frequency (Hz) [default: 8000]
 *
 * @parameter_name FBANK_COUNT
 * @parameter_type int
 * @parameter_value 13
 * @parameter_description The number of Mel filter banks. [default: 13] 
 *
 END*/

inline
float Mel(int k, int fs, int n) {
  /* k  : frequency bin ID         */
  /* fs : sampling frequency [Hz]  */
  /* n  : analysis frame length    */
  return 1127 * log(1 + (k - 1) * fs * (1 / 700.0f) / n);
}
inline 
float Mel(int f) {
  /* f : frequency [Hz] */
  return 1127 * log(1 + f * (1 / 700.0f));
}


class MelFilterBank : public BufferedNode {
  int inputID;
  int outputID;

  int length;
  int sampling_rate;
  int cutoff;
  int fbank_count;

  vector<float> mel_center;
  vector<short> lo_index;
  vector<float> lo_weight;
  int lo;
  int hi;

  vector<float> spectrum;

public:
  MelFilterBank(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");

    length = dereference_cast<int>(parameters.get("LENGTH"));
    sampling_rate = dereference_cast<int>(parameters.get("SAMPLING_RATE"));
    cutoff = dereference_cast<int>(parameters.get("CUTOFF"));
    fbank_count = dereference_cast<int>(parameters.get("FBANK_COUNT"));
    int min_freq = dereference_cast<int>(parameters.get("MIN_FREQUENCY"));
    int max_freq = dereference_cast<int>(parameters.get("MAX_FREQUENCY"));
    if (!(min_freq >= 0 && min_freq <= sampling_rate / 2 &&
          max_freq >= 0 && max_freq <= sampling_rate / 2 &&
          min_freq <= max_freq)) {
      throw new NodeException(NULL, "SAMPLING_RATE, MIN_FREQUENCY or MAX_FREQUENCY is wrong.",
                              __FILE__, __LINE__);
    }

    lo = min_freq * length / sampling_rate;
    hi = max_freq * length / sampling_rate;

    spectrum.resize(length / 2 + 1);

    PrepareForFilter();

    //inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Map<int, ObjectRef> > input = getInput(inputID, count);
    const Map<int, ObjectRef>& in = *input;
        
    //Buffer& outputBuffer = *(outputs[outputID].buffer);
    Map<int,ObjectRef>& output = *new Map<int,ObjectRef>;
    out[count] = &output;
    //outputBuffer[count] = &output;

    for (Map<int, ObjectRef>::const_iterator it = in.begin(); it != in.end(); ++it) {
      int id = it->first;
      
      RCPtr<Vector<float> > fbank(Vector<float>::alloc(fbank_count * 2));
      for(int cnt = 0; cnt < (*fbank).size(); cnt++) (*fbank)[cnt] = 0.0;

      if (typeid(*it->second) == typeid(Vector<float>)) {
        const Vector<float>& spec = object_cast<Vector<float> >(it->second);
        
        Filter(spec, *fbank);
      }
      else if (typeid(*it->second) == typeid(Vector<complex<float> >)) {
        const Vector<complex<float> >& spec = object_cast<Vector<complex<float> > >(it->second);
        for (int k = 0; k < spec.size(); k++) {
          spectrum[k] = abs(spec[k]);
        }
        Filter(spectrum, *fbank);
      }
      else {
        cerr << "Input type is not Vector<float> or Vector<complex<float> >." << endl;
      }

      output[id] = fbank;
    }
  }
    
  void PrepareForFilter() {
    int slength = length / 2 + 1;
        
    mel_center.resize(fbank_count + 2);

    //float mel_hi = Mel(slength, fs, length);
    float mel_hi = Mel(cutoff);
    float mel_lo = 0;
    float mel_s = mel_hi - mel_lo;
    for (int c = 1; c < mel_center.size(); c++) {
      mel_center[c] = ((float)c / (fbank_count + 1)) * mel_s + mel_lo;
    }

    lo_index.resize(slength);
    for (int k = 1, c = 1; k < slength; k++) {
      float mel_k = Mel(k, sampling_rate, length);
      while (mel_center[c] < mel_k && c < mel_center.size()) {
        c++;
      }
      lo_index[k] = c - 1;
    }

    lo_weight.resize(slength);
    for (int k = 1; k <= hi; k++) {
      int c = lo_index[k];
      if (c > 0) {
        lo_weight[k] = (mel_center[c + 1] - Mel(k, sampling_rate, length)) /
          (mel_center[c + 1] - mel_center[c]);
      }
      else {
        lo_weight[k] = (mel_center[1] - Mel(k, sampling_rate, length)) /
          (mel_center[1] - mel_lo);
      }
    }
  }
    
  void Filter(const vector<float>& spec, vector<float>& fbank) {
    //VecComplex& spec = *specptr;
    //VecReal& fbank = *fbankptr;

    for (int i = 0; i < fbank_count; i++) {
      fbank[i] = 0.0f;
    }

    for (int k = lo; k <= hi; k++) {
      //for (int k = 4; k <= hi; k++) {
      //float a = sqrtf(spec[k]);
      float a = spec[k];
      int bin = lo_index[k];
      float weighted_a = lo_weight[k] * a;

      if (bin > 0 && bin <= fbank_count) {
        fbank[bin - 1] += weighted_a;
      }
      if (bin < fbank_count) {
        fbank[bin] += a - weighted_a;
      }
    }
    /*
      for (int bin = 1; bin <= fbank_count; bin++) {
      fbank[bin - 1] = logf(fbank[bin - 1] < 1.0 ? 1.0 : fbank[bin - 1]);
      }
    */
  }
  IN_ORDER_NODE_SPEEDUP(MelFilterBank);
};
