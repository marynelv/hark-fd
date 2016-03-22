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
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/shared_ptr.hpp>
#include "htkheader.hpp"

using namespace std;
using namespace FD;
using namespace boost;

class SaveHTKFeatures;

DECLARE_NODE(SaveHTKFeatures)
/*Node
 *
 * @name SaveHTKFeatures
 * @category HARK:FeatureExtraction
 * @description Save a sequence of feature vectors with HTK format.
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
 * @parameter_description Base name for saved file.
 *
 * @parameter_name HTK_PERIOD
 * @parameter_type int
 * @parameter_value 100000
 * @parameter_description Period for the HTK header
 *
 * @parameter_name FEATURE_TYPE
 * @parameter_type string
 * @parameter_value USER
 * @parameter_list USER:MFCC_E_D:MFCC_E_D_Z
 * @parameter_description Output HTK feature format
 *
END*/

RCPtr<Map<int, ObjectRef> > input_SaveHTKFeatures;

class SaveHTKFeatures : public BufferedNode {
  int featuresID;
  int outputID;
  int sourcesID;
  string baseName;
  int htk_period;
  string feature_type;
  map<int, shared_ptr<ofstream> > streams;
  map<int, vector<vector<float> >* > feature_map;
  map<int, int> map_BytesPerSample;
  map<int, int> map_nSamples;
    
public:
  SaveHTKFeatures(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params), sourcesID(-1)
  {
    featuresID = addInput("FEATURES");
    outputID = addOutput("OUTPUT");
    baseName   = object_cast<String> (parameters.get("BASENAME"));
    htk_period = dereference_cast<int>(parameters.get("HTK_PERIOD"));
    feature_type = object_cast<string>(parameters.get("FEATURE_TYPE"));
    inOrder = true;
  }
    
  ~SaveHTKFeatures() {
    closeFilesFinal(input_SaveHTKFeatures);
  }
    
  void reset()
  {
    streams.clear();
    feature_map.clear();
    map_BytesPerSample.clear();
    map_nSamples.clear();    
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
    input_SaveHTKFeatures = getInput(featuresID, count);
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
	  feature_map[it->first] = new vector<vector<float> >;
	  const Vector<float>& feature_tmp = object_cast<Vector<float> >(it->second);
	  map_BytesPerSample[it->first] = feature_tmp.size() * sizeof(float);
	  map_nSamples[it->first] = 0;
        }        
	const Vector<float>& feature = object_cast<Vector<float> >(it->second);
	feature_map[it->first]->push_back(feature);
	map_nSamples[it->first]++;
      }
    }
    else {
      for (int i = 0; i < sources->size(); i++) {
        RCPtr<Source> src = (*sources)[i];
        if (streams.find(src->id) == streams.end()) {
          ostringstream filename;
          filename << baseName << src->id << ".spec";
          streams[src->id].reset(new ofstream(filename.str().c_str()));
	  feature_map[src->id] = new vector<vector<float> >;
	  Map<int, ObjectRef>::iterator it = input->find(src->id);
	  const Vector<float>& feature_tmp = object_cast<Vector<float> >(it->second);
	  map_BytesPerSample[src->id] = feature_tmp.size() * sizeof(float);
	  map_nSamples[src->id] = 0;
        }
        Map<int, ObjectRef>::iterator it = input->find(src->id);
        if (it != input->end()) {
          const Vector<float>& feature = object_cast<Vector<float> >(it->second);
	  feature_map[src->id]->push_back(feature);
	  map_nSamples[src->id]++;
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

        ofstream& stream = *streams[streams_it->first];

	HTKHeader header;
	header.BytesPerSample = map_BytesPerSample[streams_it->first]; 
	header.nSamples = map_nSamples[streams_it->first];
	header.Period = htk_period;
	if (feature_type == "USER") {
	  header.FeatureType = HTK_USER;
	}
	else if (feature_type == "MFCC_E_D") {
	  header.FeatureType = HTK_MFCC_E_D;
	}
	else if (feature_type == "MFCC_E_D_Z") {
	  header.FeatureType = HTK_MFCC_E_D_Z;
	}
	header.write(stream);

	int nSamples_tmp = feature_map[streams_it->first]->size();
		
	for(int cnt = 0; cnt < nSamples_tmp; cnt++){
	  swap_endian((*feature_map[streams_it->first])[cnt].begin(), 
		      (*feature_map[streams_it->first])[cnt].end());	  
	  stream.write((const char*)&(*feature_map[streams_it->first])[cnt][0], 
		       sizeof(float) * (*feature_map[streams_it->first])[cnt].size());  
	}

        streams.erase(streams_it);
        feature_map.erase(streams_it->first);
        map_BytesPerSample.erase(streams_it->first);
        map_nSamples.erase(streams_it->first);
        ++streams_it;

      }
      else {
        ++streams_it;
      }
    }
  }

  void closeFilesFinal(RCPtr<Map<int, ObjectRef> > input)
  { 
    map<int, shared_ptr<ofstream> >::iterator streams_it = streams.begin();
    while (streams_it != streams.end()) {
      
      ofstream& stream = *streams[streams_it->first];
      
      HTKHeader header;
      header.BytesPerSample = map_BytesPerSample[streams_it->first]; 
      header.nSamples = map_nSamples[streams_it->first];
      header.Period = htk_period;
      if (feature_type == "USER") {
	header.FeatureType = HTK_USER;
      }
      else if (feature_type == "MFCC_E_D") {
	header.FeatureType = HTK_MFCC_E_D;
      }
      else if (feature_type == "MFCC_E_D_Z") {
	header.FeatureType = HTK_MFCC_E_D_Z;
      }
      header.write(stream);
      
      int nSamples_tmp = feature_map[streams_it->first]->size();
            
      for(int cnt = 0; cnt < nSamples_tmp; cnt++){
	swap_endian((*feature_map[streams_it->first])[cnt].begin(), 
		    (*feature_map[streams_it->first])[cnt].end());	  
	stream.write((const char*)&(*feature_map[streams_it->first])[cnt][0], 
		     sizeof(float) * (*feature_map[streams_it->first])[cnt].size());  
      }
      
      streams.erase(streams_it);
      feature_map.erase(streams_it->first);
      map_BytesPerSample.erase(streams_it->first);
      map_nSamples.erase(streams_it->first);
      ++streams_it;
      
    }
  }

    
  IN_ORDER_NODE_SPEEDUP(SaveHTKFeatures)

};
