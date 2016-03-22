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

using namespace std;
using namespace FD;

class CMChannelSelector;

DECLARE_NODE(CMChannelSelector);
/*Node
 *
 * @name CMChannelSelector
 * @category HARK:Localization:CorrelationMatrix
 * @description Channel selector for correlation matrices. This is similar to ChannelSelector.
 *
 * @input_name INPUTCM
 * @input_type Matrix<complex<float> >
 * @input_description  Correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver.
 *
 * @output_name OUTPUTCM
 * @output_type Matrix<complex<float> >
 * @output_description  Correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver.
 *
 * @parameter_name SELECTOR
 * @parameter_type object
 * @parameter_value <Vector<int> >
 * @parameter_description Channel selection setting which is a vector consisting of channel IDs (ID starts with 0). When the first three channels are selected from four channels, this should be set to <Vector<int> 0 1 2>.
 *
END*/

class CMChannelSelector : public BufferedNode {
  int inputcmID; 
  int outputcmID;

  int nb_channels;
  int pslength;
  Vector<int> selector;

  typedef Matrix<complex<float> > FCMatrix; // FlowdesignerÍÑ

public:
  CMChannelSelector(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {

    inputcmID    = addInput("INPUTCM");
    outputcmID  = addOutput("OUTPUTCM");
    selector = object_cast<Vector<int> >(parameters.get("SELECTOR"));
    inOrder = true;

  }

  void calculate(int output_id, int count, Buffer &out)
  {
    
    RCPtr<FCMatrix> inputcm = getInput(inputcmID, count);

    if(count==0){
      pslength = inputcm->nrows();
      nb_channels = (int)(floor(sqrtf((float)inputcm->ncols())));
      if(inputcm->ncols() != nb_channels * nb_channels)
	throw new NodeException(this, string("CMChannelSelector. Incorrect correlation matrix size."), __FILE__, __LINE__);

      for (int c = 0; c < selector.size(); c++) {
	if (!(selector[c] >= 0 && selector[c] < nb_channels)) {
	  throw new NodeException(this, string("CMChannelSelector. Nonexistent channel is selected."), __FILE__, __LINE__);
	}
      }
    }    

    RCPtr<FCMatrix> outputcm(new FCMatrix(pslength, selector.size()*selector.size()));    
    (*(outputs[outputcmID].buffer))[count] = outputcm;

    for (int k = 0; k < pslength; k++) {
      for (int m1 = 0; m1 < selector.size(); m1++) {
	for (int m2 = 0; m2 < selector.size(); m2++) {
	  (*outputcm)(k, m1 * selector.size() + m2) = (*inputcm)(k, selector[m1] * nb_channels + selector[m2]);
	}
      }
    }
    
  }

  //IN_ORDER_NODE_SPEEDUP(CMChannelSelector);
};
