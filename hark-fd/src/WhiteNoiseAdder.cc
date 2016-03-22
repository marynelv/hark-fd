/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <boost/scoped_ptr.hpp>
#include <boost/random.hpp>

#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Map.h"

#include "fft.hpp"

using namespace std;
using namespace FD;
using namespace boost::numeric;
using namespace boost;

class WhiteNoiseAdder;

DECLARE_NODE(WhiteNoiseAdder)
/*Node
 *
 * @name WhiteNoiseAdder
 * @category HARK:MISC
 * @description Add white noise to input spectrum.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Input spectrum. The key is source ID, and the value is a spectrum (Vector<complex<float> >).
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description The white-noise-added spectrum. The key is source ID, and the value is output spectrum (Vector<complex<float> >).
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description Size of window length in sample. [default: 512]
 *
 * @parameter_name WN_LEVEL
 * @parameter_type float
 * @parameter_value 0
 * @parameter_description An amplitude of white noise to be added. [default: 0]
 *
 END*/


class WhiteNoiseAdder : public BufferedNode {
  int inputID;
  int outputID;

  int length;
  float wn_level;
  float wn_factor;

  scoped_ptr<variate_generator<mt19937, uniform_real<float> > > rand_uniform;
    
public:
  WhiteNoiseAdder(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");

    length = dereference_cast<int>(parameters.get("LENGTH"));
    wn_level = dereference_cast<float>(parameters.get("WN_LEVEL"));

    wn_factor = sqrtf(length) / 2.0f;

    mt19937 gen(1234);
    uniform_real<float> uniform_dist(0, 1.0f);
    rand_uniform.reset(new variate_generator<mt19937, uniform_real<float> >
                       (gen, uniform_dist));

    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    Map<int, ObjectRef>& output = *new Map<int, ObjectRef>;
    Buffer& outputBuffer = *(outputs[outputID].buffer);
    outputBuffer[count] = &output;

        
    RCPtr<Map<int, ObjectRef> >
      spec_map_ptr = getInput(inputID, count);
    const Map<int, ObjectRef>& spec_map = *spec_map_ptr;
        
    Map<int, ObjectRef>::const_iterator it = spec_map.begin();
    for (; it != spec_map.end(); ++it) {
      const Vector<complex<float> >& spec_in =
        object_cast<Vector<complex<float> > >(it->second);
      RCPtr<Vector<complex<float> > >
        spec_out(new Vector<complex<float> >(spec_in.size()));
      for (int k = 0; k < spec_in.size(); k++) {
        float theta = (*rand_uniform)() * 2 * M_PI;
        (*spec_out)[k] = (spec_in[k] +
                          wn_factor * wn_level * complex<float>(cos(theta), sin(theta)));
      }
      output[it->first] = spec_out;
    }
  }

  IN_ORDER_NODE_SPEEDUP(WhiteNoiseAdder);
};


