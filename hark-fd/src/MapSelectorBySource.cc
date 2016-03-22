
/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Map.h"
#include "Source.h"
#include <math.h>

using namespace FD;
using namespace std;

/********************************************************************************/

class MapSelectorBySource;

DECLARE_NODE(MapSelectorBySource)
/*Node
 *
 * @name MapSelectorBySource
 * @category HARK:MISC
 * @description This module selects/rejects Map-type input data based on Vector<Source>.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Any Map<int,ObjectRef> type input. The key is a source ID, the value can be any ObjectRef.
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations.
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Selected Map<int,ObjectRef>.
 *
 * @parameter_name SELECTION_TYPE
 * @parameter_type string
 * @parameter_value ID
 * @parameter_list ALL:ID:DIRECTION:POWER
 * @parameter_description Criterion for selecting Map. If SOURCES is not connected, ID should be selected.
 * 
 * @parameter_name ID_SELECTION_TYPE
 * @parameter_type string
 * @parameter_value LATEST
 * @parameter_list LATEST:OLDEST:BETWEEN
 * @parameter_valid SELECTION_TYPE=ID
 * @parameter_description Criterion for selecting Map using ID.
 * 
 * @parameter_name ID_RANGE_MIN
 * @parameter_type int
 * @parameter_value 0
 * @parameter_valid ID_SELECTION_TYPE=BETWEEN
 * @parameter_description ID range to select Map. The format is int.
 * 
 * @parameter_name ID_RANGE_MAX
 * @parameter_type int
 * @parameter_value 0
 * @parameter_valid ID_SELECTION_TYPE=BETWEEN
 * @parameter_description ID range to select Map. The format is int.
 * 
 * @parameter_name POWER_SELECTION_TYPE
 * @parameter_type string
 * @parameter_value HIGHEST
 * @parameter_list HIGHEST:LOWEST:BETWEEN
 * @parameter_valid SELECTION_TYPE=POWER
 * @parameter_description Criterion for selecting Map using POWER.
 * 
 * @parameter_name POWER_RANGE_MIN
 * @parameter_type float
 * @parameter_value 0
 * @parameter_valid POWER_SELECTION_TYPE=BETWEEN
 * @parameter_description Power range to select Map. The format is float.
 * 
 * @parameter_name POWER_RANGE_MAX
 * @parameter_type float
 * @parameter_value 40.0
 * @parameter_valid POWER_SELECTION_TYPE=BETWEEN
 * @parameter_description Power range to select Map. The format is float.
 * 
 * @parameter_name DIRECTION_SELECTION_TYPE
 * @parameter_type string
 * @parameter_value BETWEEN
 * @parameter_list NEAREST:BETWEEN
 * @parameter_valid SELECTION_TYPE=DIRECTION
 * @parameter_description Criterion for selecting Map using DIRECTION.
 * 
 * @parameter_name DIRECTION
 * @parameter_type float
 * @parameter_value 0.0
 * @parameter_valid DIRECTION_SELECTION_TYPE=NEAREST
 * @parameter_description Direction to select Map when NEAREST is selected.
  * 
 * @parameter_name DIRECTION_RANGE_MIN
 * @parameter_type float
 * @parameter_value 0.0
 * @parameter_valid DIRECTION_SELECTION_TYPE=BETWEEN
 * @parameter_description Direction range to select Map. The format is float.
 * 
 * @parameter_name DIRECTION_RANGE_MAX
 * @parameter_type float
 * @parameter_value 360.0
 * @parameter_valid DIRECTION_SELECTION_TYPE=BETWEEN
 * @parameter_description Direction range to select Map. The format is float.
 *
 * @parameter_name DEBUG_PRINT
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description print debug message of this module in case of true.
 *
 END*/

class MapSelectorBySource : public BufferedNode {
  int inputID;
  int sourcesID;
  int outputID;

  string selection_type;
  
  string id_selection_type;
  //Vector<int> id_range;
  int max_id;
  int min_id;
  
  string power_selection_type;
  //Vector<float> power_range;
  float max_power;
  float min_power;
  
  string direction_selection_type;
  float max_direction;
  float min_direction;
  float direction;

  bool debug_print;
    
public:
  MapSelectorBySource(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params), sourcesID(-1)
  {

    inputID   = addInput("INPUT");
    outputID  = addOutput("OUTPUT");

    selection_type = object_cast<string>(parameters.get("SELECTION_TYPE"));

    if(selection_type == "ID"){
      id_selection_type = object_cast<string>(parameters.get("ID_SELECTION_TYPE"));
      if(id_selection_type == "BETWEEN"){
	max_id = dereference_cast<int>(parameters.get("ID_RANGE_MAX"));   
	min_id = dereference_cast<int>(parameters.get("ID_RANGE_MIN"));     
	if (max_id < min_id){
	  throw new NodeException(NULL, string("max_id must not be smaller than min_id"), __FILE__, __LINE__);
	}
	if (min_id < 0){
	  throw new NodeException(NULL, string("ID_RANGE must be lager than or equal 0"), __FILE__, __LINE__);
	}
      }
    }

    if(selection_type == "POWER"){
      power_selection_type = object_cast<string>(parameters.get("POWER_SELECTION_TYPE"));
      if(power_selection_type == "BETWEEN"){
	max_power = dereference_cast<float>(parameters.get("POWER_RANGE_MAX"));
	min_power = dereference_cast<float>(parameters.get("POWER_RANGE_MIN"));        
	if (max_power < min_power){
	  throw new NodeException(NULL, string("POWER_RANGE is not appropriate."), __FILE__, __LINE__);
	}
      }
    }

    if(selection_type == "DIRECTION"){
      direction_selection_type = object_cast<string>(parameters.get("DIRECTION_SELECTION_TYPE"));
      if(direction_selection_type == "BETWEEN"){
		max_direction = dereference_cast<float>(parameters.get("DIRECTION_RANGE_MAX"));     
		min_direction = dereference_cast<float>(parameters.get("DIRECTION_RANGE_MIN"));   
		if (max_direction < min_direction){
	  		throw new NodeException(NULL, string("DIRECTION_RANGE is not appropriate."), __FILE__, __LINE__);
		} 	
	while (min_direction < -180) min_direction += 360;
	while (min_direction > 180)  min_direction -= 360;
	while (max_direction < -180) max_direction += 360;
	while (max_direction > 180)  max_direction -= 360;
      }
      if(direction_selection_type == "NEAREST"){
	direction = dereference_cast<float>(parameters.get("DIRECTION"));
	while (direction < -180) direction += 360;
	while (direction > 180)  direction -= 360;
      }
    }
        
    debug_print = dereference_cast<bool>(parameters.get("DEBUG_PRINT"));
    inOrder = true;
  }

  virtual int translateInput(string inputName) {
    if (inputName == "SOURCES") {
      sourcesID = addInput(inputName);
      return sourcesID;
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

  virtual void initialize()
  {
    if (sourcesID == -1 && selection_type != "ID")
      throw new NodeException(NULL, string("Select ID if SOURCES is not connected."), __FILE__, __LINE__);    
    this->BufferedNode::initialize();
  }

  void calculate(int output_id, int count, Buffer &out)
  {

    RCPtr<Map<int, ObjectRef> > output_ptr(new Map<int, ObjectRef>);
    Map<int, ObjectRef>& output = *output_ptr;
    (*(outputs[outputID].buffer))[count] = output_ptr;
    
    ObjectRef input_ptr = getInput(inputID, count);
    const Map<int,ObjectRef>& input_map =
      object_cast<Map<int,ObjectRef> >(input_ptr);

    // If SOURCES is not connected
    if (sourcesID == -1) {
      int id = input_map.begin()->first;
      Map<int, ObjectRef>::const_iterator it;
      for (it = input_map.begin(); it != input_map.end(); ++it) {
	if(id_selection_type == "LATEST"){
	  if(id < it->first) id = it->first;
	}
	if(id_selection_type == "OLDEST"){
	  if(id > it->first) id = it->first;
	}
	if(id_selection_type == "BETWEEN"){
	  if(max_id >= it->first && min_id <= it->first) 
	    (*output_ptr)[it->first] = it->second;
	}
      }
      if(id_selection_type != "BETWEEN"){
	if (input_map.find(id) != input_map.end())
	  (*output_ptr)[id] = input_map.find(id)->second;
      }	
    }
     
    // If SOURCES is connected
    else{

      RCPtr<Vector<ObjectRef> > sources_ptr = getInput(sourcesID, count);
      const Vector<ObjectRef>& sources = *sources_ptr;

      int id;
      float power;
      float dir;
      bool nosrcflag = true;
      for (unsigned int srci = 0; srci < sources.size(); srci++) {
	RCPtr<Source> src = sources[srci];	
	if (input_map.find(src->id) == input_map.end()) continue;
	id = src->id;
	power = src->power;
	dir = 180 / M_PI * atan2(src->x[1], src->x[0]);	
	nosrcflag = false;
	break;
      }

      if(nosrcflag) return;
      
      for (unsigned int srci = 0; srci < sources.size(); srci++) {
	RCPtr<Source> src = sources[srci];
	
	if (input_map.find(src->id) == input_map.end()) continue;

	if(selection_type == "ALL"){
	  (*output_ptr)[src->id] = input_map.find(src->id)->second;
	}
	if(selection_type == "ID"){
	  if(id_selection_type == "LATEST"){
	    if(id < src->id) id = src->id;
	  }
	  if(id_selection_type == "OLDEST"){
	    if(id > src->id) id = src->id;
	  }
	  if(id_selection_type == "BETWEEN"){
	    if(max_id >= src->id && min_id <= src->id) 
	      (*output_ptr)[src->id] = input_map.find(src->id)->second;
	  }
	}
	if(selection_type == "POWER"){
	  if(power_selection_type == "HIGHEST"){
	    if(power < src->power) {power = src->power; id = src->id;}
	  }
	  if(power_selection_type == "LOWEST"){
	    if(power > src->power) {power = src->power; id = src->id;}
	  }
	  if(power_selection_type == "BETWEEN"){
	    if(max_power >= src->power && min_power <= src->power) 
	      (*output_ptr)[src->id] = input_map.find(src->id)->second;
	  }
	}
	if(selection_type == "DIRECTION"){
	  float tdir = 180 / M_PI * atan2(src->x[1], src->x[0]);	
	  if(direction_selection_type == "NEAREST"){
	    if(abs(tdir - direction) < abs(dir - direction)) {dir = tdir; id = src->id;}
	  }
	  if(direction_selection_type == "BETWEEN"){
	    if(max_direction >= tdir && min_direction <= tdir) 
	      (*output_ptr)[src->id] = input_map.find(src->id)->second;
	  }
	}
      }
      	
      if(selection_type == "ID"){
	if(id_selection_type != "BETWEEN"){
	  (*output_ptr)[id] = input_map.find(id)->second;
	}	
      }
      if(selection_type == "POWER"){
	if(power_selection_type != "BETWEEN"){
	  (*output_ptr)[id] = input_map.find(id)->second;
	}	
      }
      if(selection_type == "DIRECTION"){
	if(direction_selection_type != "BETWEEN"){
	  (*output_ptr)[id] = input_map.find(id)->second;
	}	
      }
      
    }
  }

  //NO_ORDER_NODE_SPEEDUP(MapSelectorBySource);
};

