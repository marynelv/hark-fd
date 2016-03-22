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

class MFCCExtraction;

DECLARE_NODE(MFCCExtraction)
/*Node
 *
 * @name MFCCExtraction
 * @category HARK:FeatureExtraction
 * @description Calculate a MFCC feature vector from a Mel-scale spectrum.
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
 * @output_description Output MFCC feature vector. The key is source ID, and the value is MFCC feature vector (Vector<float>).
 *
 * @parameter_name FBANK_COUNT
 * @parameter_type int
 * @parameter_value 24
 * @parameter_description Number of filter banks of FBANK. [default: 24]
 *
 * @parameter_name NUM_CEPS
 * @parameter_type int
 * @parameter_value 12
 * @parameter_description Number of Cepstrum coefficients. [default: 12]
 *
 * @parameter_name USE_POWER
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Use power feature if true. [default: false]
 *
 END*/

class MFCCExtraction : public BufferedNode {
  int fbankID;
  int spectrumID;
  int outputID;

  int fbank_count;
  int num_ceps;
  bool use_power;
    
  int duration;
  int dim_static;

  vector<float> lifter;

public:
  MFCCExtraction(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    fbankID = addInput("FBANK");
    spectrumID = addInput("SPECTRUM");
    outputID = addOutput("OUTPUT");

    fbank_count = dereference_cast<int>(parameters.get("FBANK_COUNT"));
    if (fbank_count>0) {
    }
    else{
      throw (new NodeException(NULL,"MFCC:FBANK_COUNT is not invalid value.",__FILE__,__LINE__)) ;
    }
    num_ceps = dereference_cast<int>(parameters.get("NUM_CEPS"));
    if (num_ceps>0) {
    }
    else{
      throw (new NodeException(NULL,"MFCC:NUM_CEPS is not invalid value.",__FILE__,__LINE__)) ;
    }
    use_power = dereference_cast<bool>(parameters.get("USE_POWER"));
    dim_static = num_ceps + (use_power ? 1 : 0);

    lifter.resize(num_ceps + 1);
    for (int i = 0; i < lifter.size(); i++) {
      lifter[i] = 1.0f + (22 / 2.0f) * sin((i + 1) * (M_PI / 22));
    }

    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Map<int, ObjectRef> > fbanks_ptr = getInput(fbankID, count);
    const Map<int, ObjectRef>& fbanks = *fbanks_ptr;
    RCPtr<Map<int, ObjectRef> > spectra = getInput(spectrumID, count);

    Buffer& outputBuffer = *(outputs[outputID].buffer);
        
    Map<int,ObjectRef>& output = *new Map<int,ObjectRef>;
    outputBuffer[count] = &output;

    for (Map<int, ObjectRef>::const_iterator it = fbanks.begin();
         it != fbanks.end(); ++it) {
      const Vector<float>& in = object_cast<Vector<float> >(it->second);
      Vector<float>& mfcc = *Vector<float>::alloc(dim_static * 2);
      for (int i = 0; i < mfcc.size(); i++) {
        mfcc[i] = 0.0f;
      }
      output[it->first] = &mfcc;
      vector<float> fbank(fbank_count * 2);

      for (int i = 0; i < fbank_count; i++) {
        fbank[i] = logf(in[i] < 1.0f ? 1.0f : in[i]);
      }
            
      Lifter(fbank, mfcc);

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
        mfcc[dim_static - 1] = (power >= 1e-40 ? log(power) : 0.0f);
      }
    }
  }

  void Lifter(const vector<float>& fbank, vector<float>& mfcc)
  {
    makeMFCC(fbank, mfcc);
    for (int i = 0; i < num_ceps; i++) {
      mfcc[i] *= lifter[i];
    }
  }

  void makeMFCC(const vector<float>& fbank, vector<float>& mfcc)
  {
    float A = sqrt(2.0 / fbank_count);
    float B = M_PI / fbank_count;

    for (int i = 0; i < num_ceps; i++) {
      mfcc[i] = 0.0f;
      float C = (i + 1) * B;
      for (int j = 0; j < fbank_count; j++) {
        mfcc[i] += fbank[j] * cos(C * (j + 1.0 - 0.5));
      }
      mfcc[i] *= A;
    }
  }

  IN_ORDER_NODE_SPEEDUP(MFCCExtraction);
};
