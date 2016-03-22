/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#include "../config.h"
#ifdef ENABLE_HARKIO_NH

#include <iostream>
#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include <math.h>
#include "Source.h"
#include <Map.h>
#include "HarkParamsObjects.h"
#include <string>
#include <algorithm>

#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "fft.hpp"

using namespace std;
using namespace boost;
using namespace FD;

class SourceTracker;

DECLARE_NODE(SourceTracker);
/*Node
 *
 * @name SourceTracker
 * @category HARK:Localization
 * @description Track sources. When a localization result is close to a source, the localization result is given the same ID as the source.
 *
 * @input_name INPUT
 * @input_type Vector<ObjectRef>
 * @input_description Source localization results. Each element of the vector is a location without ID specified by "Source".
 *
 * @output_name OUTPUT
 * @output_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source".
 *
 * @parameter_name THRESH
 * @parameter_type float
 * @parameter_description Power threshold for localization results. A localization result with higher power than THRESH is tracked, otherwise ignored.
 *
 * @parameter_name PAUSE_LENGTH
 * @parameter_type float
 * @parameter_value 800
 * @parameter_description Life duration of source in ms. When any localization result for a source is found for more than PAUSE_LENGTH / 10 iterations, the source is terminated. [default: 800]
 *
 * @parameter_name MIN_SRC_INTERVAL
 * @parameter_type float
 * @parameter_value 20
 * @parameter_description Source interval threshold in degree. When the angle between a localization result and a source is smaller than MIN_SRC_INTERVAL, the same ID is given to the localization result. [default: 20]
 *
 * @parameter_name MIN_ID
 * @parameter_type int
 * @parameter_value 0
 * @parameter_description Minimum ID of source locations. MIN_ID should be greater than 0 or equal.
 *
 * @parameter_name DEBUG
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Output debug information if true [default: false]
 *
 END*/



void findClosestSrc(Vector<ObjectRef>& sources, RCPtr<Source>& target,
                    int* min_id, float* min_angle, float Thresh){
  *min_angle = 100;
  *min_id = 0;

  for (int j = 0; j < sources.size(); j++) {
    RCPtr<Source> src = sources[j];
    if (target->power < Thresh) {
      continue;
    }

    float angle = src->angle(target->x);
    if (angle < *min_angle) {
      *min_angle = angle;
      *min_id = j;
    }
  }
}



class SourceTracker : public BufferedNode {
  int inputID;
  int paramsID;
  int parammapID;
  int outputID;
  bool print_debug;

  float Thresh;
  float PauseLength;
  Source::CompareMode compare_mode;
  float MinSourceInterval;
  float MinTFIndexInterval;
  int next_id;
  RCPtr<Vector<ObjectRef> > plast_src;
  RCPtr<Vector<ObjectRef> > pnew_src;

public:
  SourceTracker(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params),
      paramsID(-1), parammapID(-1)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");

    // read parameterers
    print_debug = dereference_cast<bool>(parameters.get("DEBUG"));
    Thresh = dereference_cast<float>(parameters.get("THRESH"));
    PauseLength = dereference_cast<float>(parameters.get("PAUSE_LENGTH"));

    // select src compare mode
    // DEG: ordinal, TFINDEX: index search for faster
    compare_mode = Source::DEG;
    MinSourceInterval = dereference_cast<float>(parameters.get("MIN_SRC_INTERVAL"));
    MinTFIndexInterval = 1.0;
 
    // MIN_ID must not be negative.
    next_id = dereference_cast<int>(parameters.get("MIN_ID"));
    if (!(next_id >= 0)) {
      throw new NodeException(NULL, "MIN_ID should be greater than 0 or equal. MIN_ID = " + lexical_cast<string>(next_id), __FILE__, __LINE__);
    }
    plast_src = Vector<ObjectRef>::alloc(0);

    inOrder = true;
  }

  virtual int translateInput(string inputName) {
    if (inputName == "PARAMS") {
      paramsID = addInput(inputName);
      return paramsID;
    }
    else if (inputName == "PARAMMAP") {
      parammapID = addInput(inputName);
      return parammapID;
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
    RCPtr<Vector<ObjectRef> > pcur_src = getInput(inputID, count);
    const Vector<ObjectRef>& cur_sources = *pcur_src;

    pnew_src = Vector<ObjectRef>::alloc(0);
    Vector<ObjectRef>& new_sources = *pnew_src;
    Vector<ObjectRef>& last_sources = *plast_src;
    Vector<ObjectRef> remaining_sources(cur_sources);

    if((paramsID != -1)&&(parammapID == -1)){
      RCPtr<Vector<ObjectRef> > param_ptr = getInput(paramsID, count);
      const Vector<ObjectRef>& param_vec = *param_ptr;
      if(param_vec.size() != 0){
      	RCPtr<ParamsSourceTracker> value_SourceTracker = param_vec[0];
      	Thresh = (*value_SourceTracker).thresh;
      	PauseLength = (*value_SourceTracker).pause_length;
        compare_mode = (*value_SourceTracker).compare_mode;
        if(compare_mode == Source::DEG){
          MinSourceInterval = (*value_SourceTracker).min_src_interval;
        }
        else if(compare_mode == Source::TFINDEX){
        	MinSourceInterval = (*value_SourceTracker).min_tfindex_interval;
        }
        else{
          throw new NodeException(NULL, "Unknown COMPARE_MODE. This must be DEG/TFINDEX",
                                  __FILE__, __LINE__);
        }
      }
    }

    if(parammapID != -1){      
      ObjectRef p_input = getInput(parammapID, count);
      const Map<string, ObjectRef>& p_input_map =
	object_cast<Map<string, ObjectRef> >(p_input);    
      Map<string, ObjectRef>::const_iterator it;
      for (it = p_input_map.begin(); it != p_input_map.end(); ++it) {
	if(it->first != "SourceTracker") continue;
	const Map<string, ObjectRef>& map_contents =
	  object_cast<Map<string, ObjectRef> >(it->second);
	Map<string, ObjectRef>::const_iterator it_sub;
	for (it_sub = map_contents.begin(); it_sub != map_contents.end(); ++it_sub) {
	  if(it_sub->first == "THRESH")
	    if(typeid(*it_sub->second) == typeid(Float))		
	      Thresh = dereference_cast<float>(it_sub->second);
	  if(it_sub->first == "PAUSE_LENGTH")
	    if(typeid(*it_sub->second) == typeid(Float))
	      PauseLength = dereference_cast<float>(it_sub->second);
	  if(it_sub->first == "MIN_SRC_INTERVAL")
	    if(typeid(*it_sub->second) == typeid(Float))
	      MinSourceInterval = dereference_cast<float>(it_sub->second);
	}
      }
    }

    for (int i = 0; i < last_sources.size(); i++) {
      RCPtr<Source> last_src = last_sources[i];

      if (last_src->remaining < 0) {
        cerr << "Source " << last_src->id << " is removed" << compare_mode << endl;;
        continue;
      }

      if(compare_mode == Source::DEG){
        float min_angle = 100;
        int min_id = 0;
        for (int j = 0; j < remaining_sources.size(); j++) {
          RCPtr<Source> cur_src = remaining_sources[j];
          if (cur_src->power < Thresh) {
            continue;
          }
          float angle = cur_src->angle(last_src->x);
          if (angle < min_angle) {
            min_angle = angle;
            min_id = j;
          }
        }
        if (min_angle < MinSourceInterval * M_PI / 180.0) {
          RCPtr<Source> old = remaining_sources[min_id]->clone();
          old->id = last_src->id;
          old->remaining = PauseLength;
          remaining_sources.erase(remaining_sources.begin() + min_id);
          new_sources.push_back(old);
        }
        else {
          RCPtr<Source> old = last_src->clone();
          old->remaining -= 10;
          new_sources.push_back(old);
        }
      }
      else{
        int min_distance = 100;
        int min_id = 0;
        for(int j = 0; j<remaining_sources.size(); j++){
          RCPtr<Source> cur_src = remaining_sources[j];
          if (cur_src->power < Thresh) {
            continue;
          }
          int distance = cur_src->tfindexDistance(last_src->tfindex);
          if(distance < min_distance){
            min_distance = distance;
            min_id = j;
          }
        }
        if(min_distance < MinTFIndexInterval){
          RCPtr<Source> old = remaining_sources[min_id]->clone();
          old->id = last_src->id;
          old->remaining = PauseLength;
          remaining_sources.erase(remaining_sources.begin() + min_id);
          new_sources.push_back(old);
        }
        else{
          RCPtr<Source> old = last_src->clone();
          old->remaining -= 10;
          new_sources.push_back(old);
        }
      }
    }
    for (int i = 0; i < cur_sources.size(); i++) {
      RCPtr<Source> cur_src = cur_sources[i]->clone();
      if (cur_src->power < Thresh) {
        continue;
      }
      if(compare_mode == Source::DEG){
        float min_angle = 100;
        int min_id = 0;
        for (int j = 0; j < new_sources.size(); j++) {
          RCPtr<Source> new_src = new_sources[j];
          float angle = new_src->angle(cur_src->x);
          if (angle < min_angle) {
            min_angle = angle;
            min_id = j;
          }
        }
        if (min_angle >= MinSourceInterval * M_PI / 180.0) {
            cerr << "Source " << next_id << " is created (DEG)" << endl;
            cur_src->id = next_id++;
            cur_src->remaining = PauseLength;
            new_sources.push_back(cur_src);
        }
      }
      else{
        int min_distance = 100;
        int min_id = 0;
        for(int j = 0; j<new_sources.size(); j++){
          RCPtr<Source> new_src = new_sources[j];
          int distance = new_src->tfindexDistance(cur_src->tfindex);
          if(distance < min_distance){
            min_distance = distance;
            min_id = j;
          }
        }
        if(min_distance >= MinTFIndexInterval){
          cerr << "Source " << next_id << " is created (TFINDEX)." << endl;
          cur_src->id = next_id++;
          cur_src->remaining = PauseLength;
          new_sources.push_back(cur_src);
        }
      }
    }

    if (print_debug == true) {
    	fprintf(stdout, "time: %d, ", count);
    	for (int i = 0; i < new_sources.size(); i++) {
    	    RCPtr<Source> src = new_sources[i];

    	    fprintf(stdout, "ID: %d, x: %e %e %e, power: %e, ",
    		    src->id,
    		    src->x[0],
    		    src->x[1],
    		    src->x[2],
    		    src->power);
    	}
    	fprintf(stdout, "\n");
    }


    plast_src = pnew_src;
    out[count] = pnew_src;
  }
};

#endif
