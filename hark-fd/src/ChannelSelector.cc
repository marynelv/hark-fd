/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <iostream>
#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Matrix.h"
#include <math.h>
#include <assert.h>

using namespace std;
using namespace FD;

class ChannelSelector;

DECLARE_NODE(ChannelSelector);
/*Node
 *
 * @name ChannelSelector
 * @category HARK:MISC
 * @description Select specified channels from multi-channel windowed PCM.
 *
 * @input_name INPUT
 * @input_type any
 * @input_description Multi-channel windowed PCM or spectrum. The type should be Matrix<float> for PCM or Matrix<complex<float> > for spectrum.
 *
 * @output_name OUTPUT
 * @output_type any
 * @output_description Multi-channel windowed PCM or spectrum consisting of selected channels in the input. The output type is the same as the input type.
 *
 * @parameter_name SELECTOR
 * @parameter_type object
 * @parameter_value <Vector<int> >
 * @parameter_description Channel selection setting which is a vector consisting of channel IDs (ID starts with 0). When the first three channels are selected from four channels, this should be set to <Vector<int> 0 1 2>.
 *
 END*/

class ChannelSelector : public BufferedNode {
  int inputID;
  int outputID;
  Vector<int> selector;
  int num_chan;
  int length;

public:
  ChannelSelector(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");

    selector = object_cast<Vector<int> >(parameters.get("SELECTOR"));

    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {

    ObjectRef inputp = getInput(inputID, count);
    if (typeid(*inputp)==typeid(Matrix<float>)){
      RCPtr<Matrix<float> > input = getInput(inputID, count);
      RCPtr<Matrix<float> > output = ObjectRef(new Matrix<float>(selector.size(),input->ncols()));
      (*(outputs[outputID].buffer))[count] = output;
      if (count == 0) {
	num_chan = input->nrows();
	length = input->ncols();
	for (int c = 0; c < selector.size(); c++) {
	  if (!(selector[c] >= 0 && selector[c] < num_chan)) {
	    throw new NodeException(this, string("Nonexistent channel is selected. Please check the parameter SELECTOR."), __FILE__, __LINE__);
	  }
	}
      }
      assert(input->nrows() == num_chan && input->ncols() == length);
      for (int c = 0; c < selector.size(); c++) {
	for (int j = 0; j < length; j++) {
	  (*output)(c, j) = (*input)(selector[c], j);
	}
      }
    }
    if (typeid(*inputp)==typeid(Matrix<complex<float> >)){
      RCPtr<Matrix<complex<float> > > input = getInput(inputID, count);
      RCPtr<Matrix<complex<float> > > output = ObjectRef(new Matrix<complex<float> >(selector.size(),input->ncols()));
      (*(outputs[outputID].buffer))[count] = output;
      if (count == 0) {
	num_chan = input->nrows();
	length = input->ncols();
	for (int c = 0; c < selector.size(); c++) {
	  if (!(selector[c] >= 0 && selector[c] < num_chan)) {
	    throw new NodeException(this, string("Nonexistent channel is selected. Please check the parameter SELECTOR."), __FILE__, __LINE__);
	  }
	}
      }
      assert(input->nrows() == num_chan && input->ncols() == length);
      for (int c = 0; c < selector.size(); c++) {
	for (int j = 0; j < length; j++) {
	  (*output)(c, j) = (*input)(selector[c], j);
	}
      }
    }
		
  }
};
