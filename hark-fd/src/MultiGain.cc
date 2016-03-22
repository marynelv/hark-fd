#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Matrix.h"
#include "Map.h"
#include <sstream>
#include <iomanip>

using namespace std;
using namespace FD;

class MultiGain;

DECLARE_NODE(MultiGain)
/*Node
 *
 * @name MultiGain
 * @category HARK:MISC
 * @description Change gain of multi-channel input signal.
 *
 * @input_name INPUT
 * @input_type Matrix<float>
 * @input_description Input multi-channel signal.
 *
 * @output_name OUTPUT
 * @output_type Matrix<float>
 * @output_description Output multi-channel signal
 *
 * @parameter_name GAIN
 * @parameter_type float
 * @parameter_value 1.0
 * @parameter_description Gain factor.
 *
END*/


class MultiGain : public BufferedNode {
  int inputID;
  int outputID;
  int paramsID;

  float gain;

public:
  MultiGain(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params), gain(0.0f), paramsID(-1)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");

    gain = dereference_cast<float>(parameters.get("GAIN"));

    inOrder = true;
  }
    
  virtual int translateInput(string inputName) {
    if (inputName == "PARAMS") {
      paramsID = addInput(inputName);
      return paramsID;
    }
    else if (inputName == "INPUT") {
      for (unsigned int i = 0; i < inputs.size(); i++) {
	if (inputs[i].name == inputName) {
	  return i;
	}
      }
    }
    else {throw new NodeException(this, inputName+ " is not supported.", __FILE__, __LINE__);}
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Matrix<float> > input = getInput(inputID, count);
    RCPtr<Matrix<float> > output(new Matrix<float>(input->nrows(),
						   input->ncols()));
    out[count] = output;

    if(paramsID != -1){
      gain = dereference_cast<float>(getInput(paramsID, count));
    }
    
    for (int i = 0; i < input->nrows(); i++) {
      for (int j = 0; j < input->ncols(); j++) {
	(*output)(i, j) = (*input)(i, j) * gain;
      }
    }
  }

  IN_ORDER_NODE_SPEEDUP(MultiGain)

};
