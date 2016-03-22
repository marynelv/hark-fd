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
#include <math.h>
#include "Source.h"

using namespace std;
using namespace FD;

class SourceSelectorByDirection;

DECLARE_NODE(SourceSelectorByDirection);
/*Node
 *
 * @name SourceSelectorByDirection
 * @category HARK:MISC
 * @description Select sources within the specified directional range.
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name OUTPUT
 * @output_type Vector<ObjectRef>
 * @output_description Source locations with ID within the specified directional range.
 *
 * @parameter_name MIN_AZIMUTH
 * @parameter_type float
 * @parameter_value -20.0
 * @parameter_description Minimum azimuth for source selection in degree. [default: -20.0]
 *
 * @parameter_name MAX_AZIMUTH
 * @parameter_type float
 * @parameter_value 20.0
 * @parameter_description Maximum azimuth for source selection in degree. [default: 20.0]
 *
 * @parameter_name MIN_ELEVATION
 * @parameter_type float
 * @parameter_value -90.0
 * @parameter_description Minimum elevation for source selection in degree. [default: -90.0]
 *
 * @parameter_name MAX_ELEVATION
 * @parameter_type float
 * @parameter_value 90.0
 * @parameter_description Maximum elevation for source selection in degree. [default: 90.0]
 *
 END*/

class SourceSelectorByDirection : public BufferedNode {
  int sourcesID;
  int outputID;
  float min_azimuth;
  float max_azimuth;
  float min_elevation;
  float max_elevation;

public:
	SourceSelectorByDirection(string nodeName, ParameterSet params)
      : BufferedNode(nodeName, params)
  {
    sourcesID = addInput("SOURCES");
    outputID = addOutput("OUTPUT");

    min_azimuth = dereference_cast<float>(parameters.get("MIN_AZIMUTH"));
    max_azimuth = dereference_cast<float>(parameters.get("MAX_AZIMUTH"));
    min_elevation = dereference_cast<float>(parameters.get("MIN_ELEVATION"));
    max_elevation = dereference_cast<float>(parameters.get("MAX_ELEVATION"));

    while( max_azimuth - min_azimuth > 360 ) max_azimuth -= 360; 
    while( max_azimuth - min_azimuth <= 0  ) max_azimuth += 360; 

    while( max_elevation - min_elevation > 360 ) max_elevation -= 360; 
    while( max_elevation - min_elevation <= 0  ) max_elevation += 360; 
    
    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<Vector<ObjectRef> > sources = getInput(sourcesID, count);
    RCPtr<Vector<ObjectRef> > selected_sources(new Vector<ObjectRef>);
    out[count] = selected_sources;

    for (int i = 0; i < sources->size(); i++) {
      RCPtr<Source> src = (*sources)[i];

      float azimuth = 180 / M_PI * atan2(src->x[1], src->x[0]);
      while( azimuth - min_azimuth > 360 ) azimuth -= 360; 
      while( azimuth - min_azimuth <= 0 )  azimuth += 360; 

      float elevation = 180 / M_PI * atan2( src->x[2], sqrt ( src->x[1] * src->x[1] + src->x[0] * src->x[0] ) );
      while( elevation - min_elevation > 360 ) elevation -= 360; 
      while( elevation - min_elevation <= 0 )  elevation += 360; 

      if (azimuth >= min_azimuth && azimuth <= max_azimuth && elevation >= min_elevation && elevation <= max_elevation) {
        //cout << "azimuth = " << azimuth << ", source_id = " << src->id << endl;
        selected_sources->push_back(src);
      }
    }
  }
};
