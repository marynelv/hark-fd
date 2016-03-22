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
#include "Source.h"
#include <sstream>
#include <iomanip>

#include <boost/shared_ptr.hpp>

using namespace std;
using namespace FD;
using namespace boost;

class SaveFeatures;

DECLARE_NODE(SaveFeatures)
/*Node
 *
 * @name SaveFeatures
 * @category HARK:FeatureExtraction
 * @description Save a sequence of feature vectors as a binary file per utterace.
 *
 * @input_name FEATURES
 * @input_type Map<int,ObjectRef>
 * @input_description Feature vectors. The key is source ID, the value is a feature vector (Vector<float>).
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source". Feature vectors are saved for each source ID. This parameter is optional.
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description The same as FEATURE.
 *
 * @parameter_name BASENAME
 * @parameter_type string
 * @parameter_description Base name for save-file.
 *
END*/


class SaveFeatures : public BufferedNode {
  int featuresID;
  int outputID;
  int sourcesID;
  string baseName;
  map<int, shared_ptr<ofstream> > streams;
    
public:
  SaveFeatures(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params), sourcesID(-1)
  {
    featuresID = addInput("FEATURES");
    outputID = addOutput("OUTPUT");
    baseName = object_cast<String> (parameters.get("BASENAME"));
    inOrder = true;
  }
    
  ~SaveFeatures() {}
    
  void reset()
  {
    streams.clear();
  }

  virtual int translateInput(string inputName)
  {
    for (unsigned int i = 0; i < inputs.size(); i++) {
      if (inputs[i].name == inputName) {
        return i;
      }
    }
    if (inputName == "SOURCES") {
      return sourcesID = addInput(inputName);
    }
    else {
      throw new NodeException(this, inputName + " is not supported.", __FILE__, __LINE__);
    }
  }
  
  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Map<int, ObjectRef> > input = getInput(featuresID, count);
    RCPtr<Vector<ObjectRef> > sources;
    if (sourcesID != -1) {
      sources = getInput(sourcesID, count);
    }

    if (sources.isNil()) {
      Map<int, ObjectRef>::const_iterator it;
      for (it = input->begin(); it != input->end(); ++it) {
        if (streams.find(it->first) == streams.end()) {
          ostringstream filename;
          filename << baseName << it->first << ".spec";
          streams[it->first].reset(new ofstream(filename.str().c_str()));
        }
        
        ofstream& stream = *streams[it->first];
        
        const Vector<float>& feature = object_cast<Vector<float> >(it->second);
        stream.write((const char*)&feature[0], sizeof(float) * feature.size());
      }
    }
    else {
      for (int i = 0; i < sources->size(); i++) {
        RCPtr<Source> src = (*sources)[i];
        if (streams.find(src->id) == streams.end()) {
          ostringstream filename;
          filename << baseName << src->id << ".spec";
          streams[src->id].reset(new ofstream(filename.str().c_str()));
        }
        ofstream& stream = *streams[src->id];

        Map<int, ObjectRef>::iterator it = input->find(src->id);
        if (it != input->end()) {
          const Vector<float>& feature = object_cast<Vector<float> >(it->second);
          stream.write((const char*)&feature[0], sizeof(float) * feature.size());
        }
      }
    }
    
    if (count % 100 == 0) {
      closeFiles(input);
    }
        
    out[count] = input;
  }

  void closeFiles(RCPtr<Map<int, ObjectRef> > input)
  {
    map<int, shared_ptr<ofstream> >::iterator streams_it = streams.begin();
    while (streams_it != streams.end()) {
      if (input->find(streams_it->first) == input->end()) {
        streams.erase(streams_it++);
      }
      else {
        ++streams_it;
      }
    }
  }
    
  IN_ORDER_NODE_SPEEDUP(SaveFeatures)

};
