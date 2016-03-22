/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#include <iostream>
#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include <math.h>
#include "Source.h"

#include <boost/scoped_ptr.hpp>

using namespace std;
using namespace boost;
using namespace FD;

class CombineSource;

DECLARE_NODE(CombineSource);
/*Node
 *
 * @name CombineSource
 * @category HARK:MISC
 * @description Combine two source results
 *
 * @input_name SOURCE1
 * @input_type Vector<ObjectRef>
 * @input_description 
 *
 * @input_name SOURCE2
 * @input_type Vector<ObjectRef>
 * @input_description
 *
 * @output_name SRCOUT
 * @output_type Vector<ObjectRef>
 * @output_description
 *
 END*/

class CombineSource : public BufferedNode {

  int source1_ID;
  int source2_ID;

  int srcout_ID;

public:
  CombineSource(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {

    source1_ID = addInput("SOURCE1");
    source2_ID = addInput("SOURCE2");
    srcout_ID  = addOutput("SRCOUT");
    inOrder = true;

  }

  void calculate(int output_id, int count, Buffer &out)
  {

    RCPtr<Vector<ObjectRef> > p_source1 = getInput(source1_ID, count);
    const Vector<ObjectRef>& source1 = *p_source1;

    RCPtr<Vector<ObjectRef> > p_source2 = getInput(source2_ID, count);
    const Vector<ObjectRef>& source2 = *p_source2;

    Vector<ObjectRef>& src_out = *new Vector<ObjectRef>;
    (*(outputs[srcout_ID].buffer))[count] = &src_out;
    
    for(int i = 0; i < source1.size(); i++){
      src_out.push_back(source1[i]);
    }
    for(int i = 0; i < source2.size(); i++){
      src_out.push_back(source2[i]);
    } 
  }
};
