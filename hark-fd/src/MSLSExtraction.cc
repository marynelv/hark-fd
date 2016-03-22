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

class MSLSExtraction;

DECLARE_NODE(MSLSExtraction)
/*Node
 *
 * @name MSLSExtraction
 * @category HARK:FeatureExtraction
 * @description Calculate a feature vector from a Mel-scale spectrum.
 *
 * @input_name FBANK
 * @input_type Map<int,ObjectRef>
 * @input_description Input Mel-scale spectrum. The key is source ID, and the value is the feature vector (Vector<float>).
 *
 * @input_name SPECTRUM
 * @input_type Map<int,ObjectRef>
 * @input_description Input spectrum. The key is source ID, and the value is the spectrum vector (Vector<complex<float> >).
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description MSLS feature vector. The key is source ID, and the value is MFCC feature vector (Vector<float>).
 *
 * @parameter_name FBANK_COUNT
 * @parameter_type int
 * @parameter_value 13
 * @parameter_description Size of the static part of MSLS feature vector. [default: 13]
 *
 * @parameter_name NORMALIZATION_MODE
 * @parameter_type string
 * @parameter_value CEPSTRAL
 * @parameter_list CEPSTRAL:SPECTRAL
 * @parameter_description The domain to perform normalization. CEPSTRAL or SPECTRAL. [default: CEPSTRAL]
 *
 * @parameter_name USE_POWER
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Use power feature if true. [default: false]
 *
 END*/

class MSLSExtraction : public BufferedNode {
  int fbankID;
  int spectrumID;
  int outputID;

  int fbank_count;

  int duration;
  enum {CEPSTRAL_MODE, SPECTRAL_MODE} normalization_mode;
  bool use_power;
  int dim_static;

  vector<float> lifter;
  vector<float> cepstrum;

public:
  MSLSExtraction(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params), spectrumID(-1)
  {
    fbankID = addInput("FBANK");
    //spectrumID = addInput("SPECTRUM");
    outputID = addOutput("OUTPUT");

    fbank_count = dereference_cast<int>(parameters.get("FBANK_COUNT"));
    if (fbank_count>0) {
    }
    else{
      throw (new NodeException(NULL,"MSLS:FBANK_COUNT must be positive value.",__FILE__,__LINE__)) ;
    }
    string mode = object_cast<String>(parameters.get("NORMALIZATION_MODE"));
    if (mode == "CEPSTRAL") {
      normalization_mode = CEPSTRAL_MODE;
    }
    else if (mode == "SPECTRAL") {
      normalization_mode = SPECTRAL_MODE;
    }
    else{
      throw (new NodeException(NULL,"MSLS:NORMALIZATION_MODE is not CEPSTRAL nor SPECTRAL",__FILE__,__LINE__)) ;
    }
    use_power = dereference_cast<bool>(parameters.get("USE_POWER"));
    dim_static = fbank_count + (use_power ? 1 : 0);

    lifter.resize(13);
    for (int i = 0; i < 13; i++) {
      lifter[i] = 1.0f + (22 / 2.0f) * sin((i + 1) * (M_PI / 22));
    }
    cepstrum.resize(fbank_count);

    inOrder = true;
  }

  virtual int translateInput(string inputName)
  {
    for (unsigned int i = 0; i < inputs.size(); i++) {
      if (inputs[i].name == inputName) {
        return i;
      }
    }
    if (inputName == "SPECTRUM") {
      return spectrumID = addInput(inputName);
    }
    throw new NodeException(this, string("Unknown input in translateInput the name must be SPECTRUM: ") + inputName,
                            __FILE__, __LINE__);
  }

  virtual void verifyConnect()
  {
    if (use_power && spectrumID == -1) {
      throw new NodeException(this, "USE_POWER is true but SPECTRUM is not properly connected.",
                              __FILE__, __LINE__);
    }
    BufferedNode::verifyConnect();
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Map<int, ObjectRef> > fbanks_ptr = getInput(fbankID, count);
    const Map<int, ObjectRef>& fbanks = *fbanks_ptr;
    RCPtr<Map<int, ObjectRef> > spectra(NULL);
    if (use_power) {
      spectra = getInput(spectrumID, count);
    }

    Buffer& outputBuffer = *(outputs[outputID].buffer);
        
    Map<int,ObjectRef>& output = *new Map<int,ObjectRef>;
    outputBuffer[count] = &output;

    for (Map<int, ObjectRef>::const_iterator it = fbanks.begin();
         it != fbanks.end(); ++it) {
      const Vector<float>& in = object_cast<Vector<float> >(it->second);
      Vector<float>& fbank = *Vector<float>::alloc(dim_static * 2);
      for(int cnt = 0; cnt < fbank.size(); cnt++) fbank[cnt] = 0.0;      
      output[it->first] = &fbank;

      for (int i = 0; i < fbank_count; i++) {
        fbank[i] = logf(in[i] < 1.0f ? 1.0f : in[i]);
      }

      switch (normalization_mode) {
      case CEPSTRAL_MODE:
        LifterOld(fbank);
        break;
      case SPECTRAL_MODE:
        SubtractMean(fbank);
        EmphasizeSpectralPeak(fbank);
        break;
      }
      if (use_power) {
        Map<int, ObjectRef>::iterator itspec = spectra->find(it->first);
        float power = 0.0f;
        if (itspec != spectra->end()) {
          Vector<complex<float> >& spec =
            object_cast<Vector<complex<float> > >(itspec->second);
          int k = 0;
          power += norm(spec[k++]);
          for (; k < spec.size() - 1; k++) {
            power += norm(spec[k]) * 2;
          }
          power += norm(spec[k]);
          power /= (spec.size() - 1) * 2;
        }
        fbank[dim_static - 1] = (power >= 1e-40 ? log(power) : 0.0f);
      }
    }
  }

  void LifterOld(Vector<float>& fbank)
  {
    dct(&fbank[0], &cepstrum[0], fbank_count);

    cepstrum[0] = 0.0f;
    for (int i = 13; i < fbank_count; i++) {
      cepstrum[i] = 0.0f;
    }
    idct(&cepstrum[0], &fbank[0], fbank_count);
  }

  void SubtractMean(Vector<float>& fbank)
  {
    float mean = 0.0f;

    for (int i = 0; i < fbank_count; i++) {
      mean += fbank[i];
    }
    mean /= fbank_count;

    for (int i = 0; i < fbank_count; i++) {
      fbank[i] -= mean;
    }
  }
    
  void dct(float* x, float* y, int n)
  {
    for (int i = 0; i < n; i++) {
      y[i] = 0;
      for (int j = 0; j < n; j++) {
        y[i] += x[j] * cos(M_PI * (j + 0.5f) * i / n);
      }
      y[i] *= 2.0f;
    }
  }

  void idct(float* x, float* y, int n)
  {
    const float a = 1.0 / n;
    for (int i = 0; i < n; i++) {
      y[i] = x[0] * 0.5f;
      for (int j = 1; j < n; j++) {
        y[i] += x[j] * cos(M_PI * j * (i + 0.5f) / n);
      }
      y[i] *= a;
    }
  }

  void EmphasizeSpectralPeak(Vector<float>& vec)
  {
    const float p = 0.9f;

    for (int i = vec.size() - 1; i > 0; i--) {
      vec[i] -= p * vec[i - 1];
    }
    // 旧wav2nsspecと合わせるための修正
    //vec[0] *= 1.0 - p;
  }

  IN_ORDER_NODE_SPEEDUP(MSLSExtraction);
};
