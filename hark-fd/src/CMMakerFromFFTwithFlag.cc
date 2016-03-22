/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#include <iostream>
#include <BufferedNode.h>
#include <Buffer.h>
#include <Vector.h>
#include <Matrix.h>
#include <math.h>
#include <Source.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "Map.h"
#include <deque> 

#include "LocalizationFunction.hpp"
#include "fft.hpp"

using namespace std;
using namespace boost;
using namespace FD;
using namespace boost::numeric;

class CMMakerFromFFTwithFlag;

DECLARE_NODE(CMMakerFromFFTwithFlag);
/*Node
 *
 * @name CMMakerFromFFTwithFlag
 * @category HARK:Localization:CorrelationMatrix
 * @description This makes the correlation matrix of the output of MultiFFT when the "ADDER_FLAG" is 1. When ADDER_FLAG becomes 1 from 0, this block resets the correlation matrix for generating new correlation martix.
 *
 * @input_name INPUT
 * @input_type Matrix<complex<float> >
 * @input_description Multi-channel audio spectrum from MultiFFT. In this matrix, a row is a channel, and a column is a spectra.
 *
 * @input_name ADDER_FLAG
 * @input_type any
 * @input_description Integer- or bool-typed flag for generating a correlation matrix. This flag has 4 phases. When this flag is 1/true, this block adds current correlation matrix (AddCorrelation). When this block becomes 0/false from 1/true, it outputs newest correlation matrix (NormalizeCorrelation). When the flag is 0/false, it just outputs current correlation matrix. When this flag becomes 1/true from 0/false, this block resets the correlation matrix and starts to generate new correlation matrix.
 *
 * @output_name OUTPUT
 * @output_type Matrix<complex<float> >
 * @output_description  Correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver.
 *
 * @parameter_name DURATION_TYPE
 * @parameter_type string
 * @parameter_value FLAG_PERIOD
 * @parameter_list FLAG_PERIOD:WINDOW_PERIOD
 * @parameter_description If FLAG_PERIOD, CM is generated based on ADDER_FLAG. If WINDOW_PERIOD, CM is generated every PERIOD frames.
 *
 * @parameter_name WINDOW
 * @parameter_type int
 * @parameter_value 50
 * @parameter_valid DURATION_TYPE=WINDOW_PERIOD
 * @parameter_description The number of frames used for calculating a correlation function. 
 *
 * @parameter_name PERIOD
 * @parameter_type int
 * @parameter_value 50
 * @parameter_valid DURATION_TYPE=WINDOW_PERIOD
 * @parameter_description The period for outputting the correlation matrix. Even if ADDER_FLAG is kept to be 1, this block forcely executes NormalizeCorrelation when sum_count is over PERIOD. Set this value 0 if you do not want to use this function.
 * 
 * @parameter_name WINDOW_TYPE
 * @parameter_type string
 * @parameter_list PAST:MIDDLE:FUTURE
 * @parameter_value FUTURE
 * @parameter_valid DURATION_TYPE=WINDOW_PERIOD
 * @parameter_description Window selection to accumulate a correlation function. If PAST, the past WINDOW frames from the current frame are used for the accumulation. If MIDDLE, the current frame will be the middle of the accumulated frames. If FUTURE, the future WINDOW frames from the current frame are used for the accumulation. FUTURE is the default from version 1.0, but this makes a delay since we have to wait for the future information. PAST generates a internal buffers for the accumulation, which realizes no delay for localization. 
 *
 * @parameter_name MAX_SUM_COUNT
 * @parameter_type int
 * @parameter_value 100
 * @parameter_valid DURATION_TYPE=FLAG_PERIOD
 * @parameter_description The maximum count for adding the correlation matrix. Even if ADDER_FLAG is kept to be 1, this block forcely executes NormalizeCorrelation when sum_count is over MAX_SUM_COUNT. Set this value 0 if you do not want to use this function. 
 *
 * @parameter_name ENABLE_ACCUM
 * @parameter_type bool
 * @parameter_list true:false
 * @parameter_value false
 * @parameter_valid DURATION_TYPE=FLAG_PERIOD
 * @parameter_description enable to accumulate the hitory of correlation matrix. 
 *
 * @parameter_name ENABLE_DEBUG
 * @parameter_type bool
 * @parameter_list true:false
 * @parameter_value false
 * @parameter_description enable debug print
 *
 END*/

class CMMakerFromFFTwithFlag : public BufferedNode {
  int inputID;
  int adderflagID;
  int outputID;
  int operationflagID;

  int nb_channels;

  string duration_type;

  int max_sum_count;
  int period;
  int window;
  string window_type;
  int look_ahead;
  int look_back;

  int adder_flag;
  int normalize_flag;
  bool enable_accum;
  bool enable_debug;

  int pslength;
  fcmatrix x;
  deque<fcmatrix> xbuf;
  vector<fcmatrix> Rxx;  // Correlation Matrix for output
  vector<fcmatrix> RxxT; // Correlation Matrix for generating new one
  int adder_period;
  int sum_count;
  int sum_count_accum;

  typedef Matrix<complex<float> > FCMatrix; // FlowdesignerÍÑ

public:
  CMMakerFromFFTwithFlag(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    adderflagID = addInput("ADDER_FLAG");
    outputID = addOutput("OUTPUT");
    operationflagID = addOutput("OPERATION_FLAG");

    duration_type = object_cast<string>(parameters.get("DURATION_TYPE"));
    period = dereference_cast<int>(parameters.get("PERIOD"));
    window = dereference_cast<int>(parameters.get("WINDOW"));
    window_type = object_cast<String>(parameters.get("WINDOW_TYPE"));
    max_sum_count = dereference_cast<int>(parameters.get("MAX_SUM_COUNT"));
    enable_accum = dereference_cast<bool>(parameters.get("ENABLE_ACCUM"));
    enable_debug = dereference_cast<bool>(parameters.get("ENABLE_DEBUG"));

    adder_flag = 1;
    normalize_flag = 1;

    xbuf.resize(0);

    if(duration_type == "WINDOW_PERIOD"){

      if(window_type == "FUTURE"){
        look_ahead = window - 1;
        look_back = 0;
      }else if(window_type == "MIDDLE"){
        look_ahead = window / 2;
        look_back = window / 2 + (window % 2) - 1;
      }else if(window_type == "PAST"){
        look_ahead = 0;
        look_back  = 0;
      }

      // look_ahead = window / 2;
      // look_back = window / 2 + (window % 2) - 1;
      // look_ahead = window - 1;
      // look_back = 0;
      
      inputsCache[inputID].lookAhead = look_ahead;
      inputsCache[inputID].lookBack = look_back;

    }

    adder_period = 0;
    sum_count = 0;
    sum_count_accum = 0;

    inOrder = true;
  }

  virtual void initialize()
  {
    outputs[outputID].lookAhead
      = outputs[operationflagID].lookAhead
      = 1 + max(outputs[outputID].lookAhead, outputs[operationflagID].lookAhead);

    outputs[outputID].lookBack
      = outputs[operationflagID].lookBack
      = 1 + max(outputs[outputID].lookBack,  outputs[operationflagID].lookBack);
    
    this->BufferedNode::initialize();
  }

  void calculate(int output_id, int count, Buffer &out)
  {

    // -----------------------------------------------------------------
    // Initialization of the routine

    RCPtr<Matrix<complex<float> > > inputp = getInput(inputID, count);
    const Matrix<complex<float> >& in = *inputp;

    Buffer &eofBuffer = *(outputs[operationflagID].buffer);
    eofBuffer[count] = FalseObject;

    if(count == 0){	
      pslength = inputp->ncols();
      nb_channels = inputp->nrows();
      x.resize(nb_channels, pslength);
      Rxx.resize(pslength);
      RxxT.resize(pslength);
      for (int k = 0; k < pslength; k++) {
	//Rxx[k].resize(nb_channels, nb_channels);
	Rxx[k] = ublas::zero_matrix<complex<float> >(nb_channels, nb_channels);
	RxxT[k] = ublas::zero_matrix<complex<float> >(nb_channels, nb_channels);
      }
      // Initialize correlation matrix (unit matrix)
      for (int k = 0; k < Rxx.size(); k++) {
	for (int i = 0; i < Rxx[k].size1(); i++) {
	  for (int j = 0; j < Rxx[k].size2(); j++) {
	    if(i==j){
	      Rxx[k](i, j) = complex<float>(1.0f,0.0f);
	    }else{
	      Rxx[k](i, j) = complex<float>(0.0f,0.0f);
	    }
	  }
	}
      }    
    }

    ObjectRef addertmp = getInput(adderflagID, count);
    if (typeid(*addertmp) == typeid(Bool)) { 
      bool bInput = dereference_cast<bool>(addertmp); 
      adder_flag = (bInput ? 1 : 0); 
    }else if(typeid(*addertmp) == typeid(Int)){ 
      adder_flag = dereference_cast<int> (addertmp); 
    }else{ 
      throw new NodeException(this, string("Input type is not appropriate in CMMakerFromFFTwithFlag."), __FILE__, __LINE__); 
    } 

    RCPtr<FCMatrix> outtmp(new FCMatrix(Rxx.size(), Rxx[0].size1()*Rxx[0].size2()));    
    (*(outputs[outputID].buffer))[count] = outtmp;


    // ------------------------------------------------------------------------------------------------
    // DURATION_TYPE == "FLAG_PERIOD"

    if(duration_type == "FLAG_PERIOD"){	

      // Renew correlation matrix
      for (int k = 0; k < Rxx.size(); k++) {
	for (int i = 0; i < Rxx[k].size1(); i++) {
	  for (int j = 0; j < Rxx[k].size2(); j++) {
	    (*outtmp)(k, i*Rxx[k].size2() + j) = Rxx[k](i, j);
	  }
	}
      }
      
      if((max_sum_count <= sum_count + 1)&&(max_sum_count != 0)){
	adder_flag = 0;
	if(enable_debug) printf("Count[%d] : Summation Counter[%d] : CM Forcely Created\n", count, sum_count + 1);
      }
      
      // -----------------------------------------------------------------
      // Generate the correlation matrix
      
      // PHASE1 : Reset the matrix
      if(((adder_flag == 1)&&(normalize_flag == 1))||(max_sum_count==1)){
	
	for (int k = 0; k < pslength; k++) {
	  RxxT[k] = ublas::zero_matrix<complex<float> >(nb_channels, nb_channels);
	}
	normalize_flag = 0;
	if(enable_debug) printf("Count[%d] : CM Creation Start\n", count);
	
      }
      
      // PHASE2 : Generating the matrix
      if((adder_flag == 1)&&(normalize_flag == 0)){
	
	for (int chan = 0; chan < nb_channels; chan++) {
	  for (int k = 0; k < pslength; k++) {
	    x(chan, k) = in(chan, k);
	  }
	}
	
	AddToCorrelationAllFreq(RxxT, pslength, x);
	sum_count++;
	
	return;
	
      }
      
      // PHASE3 : Complete to make new matrix
      if(normalize_flag == 0){    
	
	for (int chan = 0; chan < nb_channels; chan++) {
	  for (int k = 0; k < pslength; k++) {
	    x(chan, k) = in(chan, k);
	  }
	}
	
	AddToCorrelationAllFreq(RxxT, pslength, x);
	sum_count++;
	
	NormalizeCorrelationAllFreq(RxxT, pslength, sum_count);
	
	for (int k = 0; k < Rxx.size(); k++) {
	  for (int i = 0; i < Rxx[k].size1(); i++) {
	    for (int j = 0; j < Rxx[k].size2(); j++) {
	      if(enable_accum){
		(*outtmp)(k, i*Rxx[k].size2() + j) = (Rxx[k](i, j) * (float)sum_count_accum + RxxT[k](i, j) * (float)sum_count) / ((float)sum_count_accum + (float)sum_count);
		Rxx[k](i, j) = (Rxx[k](i, j) * (float)sum_count_accum + RxxT[k](i, j) * (float)sum_count) / ((float)sum_count_accum + (float)sum_count);
	      }else{
		(*outtmp)(k, i*Rxx[k].size2() + j) = RxxT[k](i, j);	      
		Rxx[k](i, j) = RxxT[k](i, j);
	      }
	    }
	  }
	}
	sum_count_accum += sum_count;
	if(enable_debug) printf("Count[%d] : Summation Counter[%d] : Summation Count Accum[%d] : CM Created\n", count, sum_count, sum_count_accum);
	eofBuffer[count] = TrueObject;
	sum_count = 0;
	normalize_flag = 1;
	
      }

    }

    // ------------------------------------------------------------------------------------------------
    // DURATION_TYPE == "WINDOW_PERIOD"

    if(duration_type == "WINDOW_PERIOD"){	

      bool fflag = false;
      if(adder_flag == 1){
	adder_period++;
      }else if((adder_flag == 0)&&(adder_period != 0)){
	adder_period = 0;
	fflag = true;
      }else {
	adder_period = 0;	
      }      
      
      if(window_type == "PAST"){

	inputp = getInput(inputID, count);
	for (int chan = 0; chan < nb_channels; chan++) { 
	  for (int k = 0; k < pslength; k++) { 
	    x(chan, k) = (*inputp)(chan, k); 
	  } 
	} 
	if(xbuf.size() < window){ 
	  xbuf.push_back(x); 
	}else{ 
	  xbuf.push_back(x);       
	  xbuf.pop_front();       
	} 
	if ((adder_period % period == 0 || fflag)&&((adder_period != 0)||(period == 1))) {
	  for (int i = 0; i < xbuf.size(); i++) { 
	    for (int chan = 0; chan < nb_channels; chan++) {
	      for (int k = 0; k < pslength; k++) {
		x(chan, k) = xbuf[i](chan, k);
	      }
	    }
	    AddToCorrelationAllFreq(RxxT, pslength, x);
	    sum_count++;
	  }
	  NormalizeCorrelationAllFreq(RxxT, pslength, sum_count);
	  for (int k = 0; k < Rxx.size(); k++) {
	    for (int i = 0; i < Rxx[k].size1(); i++) {
	      for (int j = 0; j < Rxx[k].size2(); j++) {
		Rxx[k](i, j) = RxxT[k](i, j);
	      }
	    }
	  }    
	  if(enable_debug) printf("Count[%d] : Summation Counter[%d] : Period[%d] : CM Created\n", count, sum_count, period);
	  eofBuffer[count] = TrueObject;
	  sum_count = 0;
	  // Renew correlation matrix
	  for (int k = 0; k < Rxx.size(); k++) {
	    for (int i = 0; i < Rxx[k].size1(); i++) {
	      for (int j = 0; j < Rxx[k].size2(); j++) {
		(*outtmp)(k, i*Rxx[k].size2() + j) = Rxx[k](i, j);
	      }
	    }
	  }    
	}else{
	  // Renew correlation matrix
	  for (int k = 0; k < Rxx.size(); k++) {
	    for (int i = 0; i < Rxx[k].size1(); i++) {
	      for (int j = 0; j < Rxx[k].size2(); j++) {
		(*outtmp)(k, i*Rxx[k].size2() + j) = Rxx[k](i, j);
	      }
	    }
	  }    
	  return;
	}

      }else{
 
	if ((adder_period % period == 0 || fflag) && adder_period >= look_back) {
	  for (int i = -look_back; i <= look_ahead; i++) {
	    inputp = getInput(inputID, count + i);
	
	    for (int chan = 0; chan < nb_channels; chan++) {
	      for (int k = 0; k < pslength; k++) {
		x(chan, k) = (*inputp)(chan, k);
	      }
	    }

	    AddToCorrelationAllFreq(RxxT, pslength, x);
	    sum_count++;
	  }
	  NormalizeCorrelationAllFreq(RxxT, pslength, sum_count);
	  for (int k = 0; k < Rxx.size(); k++) {
	    for (int i = 0; i < Rxx[k].size1(); i++) {
	      for (int j = 0; j < Rxx[k].size2(); j++) {
		Rxx[k](i, j) = RxxT[k](i, j);
	      }
	    }
	  }    
	  if(enable_debug) printf("Count[%d] : Summation Counter[%d] : Period[%d] : CM Created\n", count, sum_count, period);
	  eofBuffer[count] = TrueObject;
	  sum_count = 0;
	  // Renew correlation matrix
	  for (int k = 0; k < Rxx.size(); k++) {
	    for (int i = 0; i < Rxx[k].size1(); i++) {
	      for (int j = 0; j < Rxx[k].size2(); j++) {
		(*outtmp)(k, i*Rxx[k].size2() + j) = Rxx[k](i, j);
	      }
	    }
	  }
	}else{
	  // Renew correlation matrix
	  for (int k = 0; k < Rxx.size(); k++) {
	    for (int i = 0; i < Rxx[k].size1(); i++) {
	      for (int j = 0; j < Rxx[k].size2(); j++) {
		(*outtmp)(k, i*Rxx[k].size2() + j) = Rxx[k](i, j);
	      }
	    }
	  }    
	  return;
	}

      }

      ClearCorrelationAllFreq(RxxT, pslength);
      
    }
      
  }

  //IN_ORDER_NODE_SPEEDUP(CMMakerFromFFTwithFlag);
};
