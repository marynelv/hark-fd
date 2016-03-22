// ---------------------------------------------------------------------
/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#include "iostream"
#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Map.h"
#include "fft.hpp"

using namespace std;
using namespace FD;
class PreEmphasis;
DECLARE_NODE(PreEmphasis)
/*Node
 *
 * @name PreEmphasis
 * @category HARK:FeatureExtraction
 * @description Pre-emphasis for acoustic feature in automatic speech recognition
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description ObjectRef is a spectrum (Vector<complex<float> >) or a wave form (Vector<float>).
 * 
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description OUTPUT has the same data type as INPUT
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description window length in sample [default: 512]
 *
 * @parameter_name SAMPLING_RATE
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_description Sampling rate in Hz [default: 16000]
 *
 * @parameter_name PREEMCOEF
 * @parameter_type float
 * @parameter_value 0.97
 * @parameter_description pre-emphasis coefficient [default: 0.97]
 * 
 * @parameter_name INPUT_TYPE
 * @parameter_type string
 * @parameter_list WAV:SPECTRUM
 * @parameter_value WAV
 * @parameter_description The domain to perform pre-emphasis [default: WAV]
 *
 END*/

class PreEmphasis : public BufferedNode{
  int inputID;
  int outputID;

  int length;
  int pslength;
  int sampling_rate;
  float preemcoef;

  int separ_length;
  int cutoff;
  string input_type;

  std::vector<std::complex<float> > preempha;


public :
  PreEmphasis(string nodeName, ParameterSet params) : BufferedNode(nodeName, params){
    //constructor
    inputID = addInput("INPUT");
    outputID =addOutput("OUTPUT");
    
    length = dereference_cast<int>(parameters.get("LENGTH"));
    sampling_rate = dereference_cast<int>(parameters.get("SAMPLING_RATE"));
    preemcoef = dereference_cast<float>(parameters.get("PREEMCOEF"));
    input_type = object_cast<String>(parameters.get("INPUT_TYPE"));

    if (input_type == "SPECTRUM") {
      preemcoef = -preemcoef;
      pslength = length / 2 +1;
      cutoff = 7900;
      separ_length = length * cutoff / sampling_rate;
      
      rfft<float> rfftf;
    
      // making a spectral filter
      preempha.resize(length);
      rfftf.initialize(length);
      rfftf.a[0]=1;
      rfftf.a[1]=preemcoef;
      for(int i=2; i<length; i++){
	rfftf.a[i]=0;
      }
      rfftf.analyze();
      
      for(int i=0; i <pslength; i++){
	if(i < 4 ||  i > separ_length){
	  rfftf.A[i]=0.0f;
	}
	preempha[i] = rfftf.A[i];
      }
    }

  }  
  void calculate(int output_id, int count, Buffer &out){
    // processing
    RCPtr<Map<int, ObjectRef> > in_map_ptr = getInput(inputID, count);
    const Map<int, ObjectRef>& in_map = *in_map_ptr;
    RCPtr<Map<int, ObjectRef> > output(new Map<int, ObjectRef>);    
    Map<int, ObjectRef>::const_iterator it = in_map.begin();

    if (input_type == "WAV"){
      for(; it != in_map.end(); ++it){
	const Vector<float>& wav = 
	  object_cast<Vector<float> > (it -> second);
	RCPtr<Vector<float> > x(new Vector<float>(length));

	for(int i = wav.size() -1; i >= 1; i--){
	  (*x)[i] = wav[i]-wav[i - 1] * preemcoef;
	}
	(*x)[0] = wav[0]*(1.0f -preemcoef);
	(*output)[it->first] = x;
	/*	if(it->first==0){
	  for(int i=0 ; i< wav.size(); i++){
	    cout <<(*x)[i] <<endl;
	  }
	  }*/
      }
      out[count]=output;
    }
    else if (input_type == "SPECTRUM") {
      for(; it != in_map.end(); ++it){
	const Vector<complex<float> >& spec = 
	  object_cast<Vector<complex<float> > > (it -> second);
	RCPtr<Vector<complex<float> > >
	  X(new Vector<complex<float> >(pslength));
	for(int i=0; i<spec.size(); i++){
	  (*X)[i] = spec[i]*preempha[i];
	}
	(*output)[it->first] = X;
      }
      out[count]=output;
    }
    else{
      cout << "input_type?"<<endl;
    }
  }
    IN_ORDER_NODE_SPEEDUP(PreEmphasis);
};
  
