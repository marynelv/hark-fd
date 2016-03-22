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

class DeltaPowerMask;

DECLARE_NODE(DeltaPowerMask)
/*Node
 *
 * @name DeltaPowerMask
 * @category HARK:MFM
 * @description Add a delta power mask to an input mask vector.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Input mask vector. The key is a source ID, and the value is a mask vector (Vector<float>).
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Output mask vector including the calculated delta power mask (Vector<float>).
 *
 END*/

class DeltaPowerMask : public BufferedNode {
	int inputID;
    int outputID;

public:
  DeltaPowerMask(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");
    
    inOrder = true;
  }
  
  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Map<int, ObjectRef> > input = getInput(inputID, count);
    RCPtr<Map<int, ObjectRef> > output(new Map<int, ObjectRef>);

    out[count] = output;

    Map<int, ObjectRef>::iterator it = input->begin();
    for (; it != input->end(); ++it) {
      RCPtr<Vector<float> > mask = it->second->clone();
      mask->push_back(1.0f);
      (*output)[it->first] = mask;
    }
  }

  IN_ORDER_NODE_SPEEDUP(DeltaPowerMask);
};


