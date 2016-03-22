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

class CMMakerFromFFT;

DECLARE_NODE(CMMakerFromFFT);
/*Node
 *
 * @name CMMakerFromFFT
 * @category HARK:Localization:CorrelationMatrix
 * @description This makes the correlation matrix of the output of MultiFFT, which is averaged over PERIOD frames.
 *
 * @input_name INPUT
 * @input_type Matrix<complex<float> >
 * @input_description Multi-channel audio spectrum from MultiFFT. In this matrix, a row is a channel, and a column is a spectra.
 *
 * @output_name OUTPUT
 * @output_type Matrix<complex<float> >
 * @output_description  Correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver.
 * 
 * @parameter_name WINDOW
 * @parameter_type int
 * @parameter_value 50
 * @parameter_description The number of frames used for calculating a correlation function.
 * 
 * @parameter_name PERIOD
 * @parameter_type int
 * @parameter_value 50
 * @parameter_description The period in which the output correlation matrix is renewed.
 * 
 * @parameter_name WINDOW_TYPE
 * @parameter_type string
 * @parameter_list PAST:MIDDLE:FUTURE
 * @parameter_value FUTURE
 * @parameter_description Window selection to accumulate a correlation function. If PAST, the past WINDOW frames from the current frame are used for the accumulation. If MIDDLE, the current frame will be the middle of the accumulated frames. If FUTURE, the future WINDOW frames from the current frame are used for the accumulation. FUTURE is the default from version 1.0, but this makes a delay since we have to wait for the future information. PAST generates a internal buffers for the accumulation, which realizes no delay for localization. 
 *
 * @parameter_name ENABLE_DEBUG
 * @parameter_type bool
 * @parameter_list true:false
 * @parameter_value false
 * @parameter_description enable debug print
 *
 END*/

class CMMakerFromFFT : public BufferedNode {
  int inputID;
  int outputID;
  int operationflagID;

  int nb_channels;
  int period;
  int window;
  string window_type;
  int look_ahead;
  int look_back;
  bool enable_debug;

  int pslength;
  fcmatrix x;
  deque<fcmatrix> xbuf;
  vector<fcmatrix> Rxx;
  vector<fcmatrix> RxxT;
  int sum_count;

  typedef Matrix<complex<float> > FCMatrix; // FlowdesignerÍÑ

public:
  CMMakerFromFFT(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");
    operationflagID = addOutput("OPERATION_FLAG");

    window = dereference_cast<int>(parameters.get("WINDOW"));
    period = dereference_cast<int>(parameters.get("PERIOD"));
    window_type = object_cast<String>(parameters.get("WINDOW_TYPE"));
    enable_debug = dereference_cast<bool>(parameters.get("ENABLE_DEBUG"));

    xbuf.resize(0);

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

    sum_count = 0;

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
    RCPtr<Matrix<complex<float> > > inputp = getInput(inputID, count);

    Buffer &eofBuffer = *(outputs[operationflagID].buffer);
    eofBuffer[count] = FalseObject;

    if (count == 0){
      pslength = inputp->ncols();
      nb_channels = inputp->nrows();
      x.resize(nb_channels, pslength);
      Rxx.resize(pslength);
      for (int k = 0; k < pslength; k++) {
	//Rxx[k].resize(nb_channels, nb_channels);
	Rxx[k] = ublas::zero_matrix<complex<float> >(nb_channels, nb_channels);
      }
      RxxT.resize(pslength);
      for (int k = 0; k < pslength; k++) {
	//RxxT[k].resize(nb_channels, nb_channels);
	RxxT[k] = ublas::zero_matrix<complex<float> >(nb_channels, nb_channels);
      }
    }

    RCPtr<FCMatrix> outtmp(new FCMatrix(Rxx.size(), Rxx[0].size1()*Rxx[0].size2()));    
    (*(outputs[outputID].buffer))[count] = outtmp;

    // -----------------------------------------------------------------
    // Generate the correlation matrix

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
      if ((count % period == 0)&&((count != 0)||(period == 1))) {
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

      if (count % period == 0 && count >= look_back) {
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

  //IN_ORDER_NODE_SPEEDUP(CMMakerFromFFT);
};
