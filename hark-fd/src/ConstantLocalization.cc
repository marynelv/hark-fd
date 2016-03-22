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
#include <assert.h>
#include "Source.h"

using namespace std;
using namespace FD;

class ConstantLocalization;

DECLARE_NODE(ConstantLocalization);
/*Node
 *
 * @name ConstantLocalization
 * @category HARK:Localization
 * @description Output constant source locations.
 *
 * @output_name SOURCES
 * @output_type Vector<ObjectRef>
 * @output_description Source locations. The type of the element is "Source".
 *
 * @parameter_name ANGLES
 * @parameter_type object
 * @parameter_value <Vector<float> >
 * @parameter_description A vector to specify source locations. Each vector element shows azimuth of a source in degree.
 *
 * @parameter_name ELEVATIONS
 * @parameter_type object
 * @parameter_value <Vector<float> >
 * @parameter_description Elevations of source locations. Each element represents an elevation of each source location.
 *
 * @parameter_name POWER
 * @parameter_type object
 * @parameter_value <Vector<float> >
 * @parameter_description Power of sources. Each element represents a power of each source.
 *
 * @parameter_name MIN_ID
 * @parameter_type int
 * @parameter_value 0
 * @parameter_description Minimum ID of source locations. ID is given from MIN_ID and incremented for the latter sources.
 *
 END*/

class ConstantLocalization : public BufferedNode {
  int sourcesID;
  Vector<float> angles;
  Vector<float> elevations;
  float distance;
  Vector<float> power;
  int min_id;

public:
	ConstantLocalization(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
    {
        sourcesID = addOutput("SOURCES");

	min_id = dereference_cast<int>(parameters.get("MIN_ID"));
		angles = object_cast<Vector<float> >(parameters.get("ANGLES"));
		elevations = object_cast<Vector<float> >(parameters.get("ELEVATIONS"));
		if(parameters.exist("POWER"))
		  power = object_cast<Vector<float> >(parameters.get("POWER"));
		else
		  power.resize(0);

		assert(angles.size() == elevations.size());
		if(power.size() == 0){
		  power.resize(angles.size());
		  for(int i = 0; i < power.size(); i++) power[i] = 1.0f;
		}else{
		  assert(angles.size() == power.size());
		}

		inOrder = true;
	}

    void calculate(int output_id, int count, Buffer &out)
    {
		RCPtr<Vector<ObjectRef> > psources(new Vector<ObjectRef>);
        Vector<ObjectRef>& sources = *psources;
        (*(outputs[sourcesID].buffer))[count] = psources;

		for (int i = 0; i < angles.size(); i++) {
            RCPtr<Source> src(new Source);

            src->power = power[i];
            float theta = angles[i] * M_PI / 180.0;
            float phi = elevations[i] * M_PI / 180.0;
            float cos_phi = cos(phi);
            src->x[0] = (float)(cos(theta) * cos_phi);
            src->x[1] = (float)(sin(theta) * cos_phi);
            src->x[2] = (float)(sin(phi));
            src->id = min_id + i;
            src->compare_mode = Source::DEG;

            psources->push_back(src);
		}
    }
};
