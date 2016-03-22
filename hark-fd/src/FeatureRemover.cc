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
#include <set>

using namespace std;
using namespace FD;

class FeatureRemover;

DECLARE_NODE(FeatureRemover)
/*Node
 *
 * @name FeatureRemover
 * @category HARK:FeatureExtraction
 * @description Remove specified components (basically a power component) from a feature vector.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Input feature vectors. The key is a source ID, and the value is a feature vector (Vector<float>).
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description output feature vectors without specified components (power component).
 *
 * @parameter_name SELECTOR
 * @parameter_type object
 * @parameter_value <Vector<int> >
 * @parameter_description Component indices in a feature vector to remove. E.g. <Vector<int> 13> to remove 14th comopnent (The index start with 0).
 *
 END*/

class FeatureRemover : public BufferedNode {
  int inputID;
  int outputID;

  Vector<int> selector;
  set<int> removed_id;

public:
  FeatureRemover(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");
      
    //num_ceps = dereference_cast<int>(parameters.get("NUM_STATIC"));
    selector = object_cast<Vector<int> >(parameters.get("SELECTOR"));

    for (int i = 0; i < selector.size(); i++) {
      removed_id.insert(selector[i]);
    }
    
    //inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Map<int, ObjectRef> > input = getInput(inputID, count);
    RCPtr<Map<int, ObjectRef> > output(new Map<int, ObjectRef>);

    Map<int, ObjectRef>::iterator it = input->begin();
    for (; it != input->end(); ++it) {
      RCPtr<Vector<float> > in_feature = it->second;
      RCPtr<Vector<float> > out_feature(Vector<float>::alloc(0));
      //out_feature->reserve(in_feature->size() - removed_id.size());

      for (int i = 0; i < in_feature->size(); i++) {
        if (removed_id.find(i) == removed_id.end()) {
          out_feature->push_back((*in_feature)[i]);
        }
      }

      (*output)[it->first] = out_feature;
    }
      
    out[count] = output;
  }

  IN_ORDER_NODE_SPEEDUP(FeatureRemover);

};
