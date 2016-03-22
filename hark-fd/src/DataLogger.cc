#include <iostream>
#include <BufferedNode.h>
#include <Buffer.h>
#include <Matrix.h>
#include <Vector.h>
#include <Stream.h>
#include "Map.h"
#include "Time.h"

#include <boost/shared_ptr.hpp>

using namespace FD;
using namespace std;
using namespace boost;

class DataLogger;

DECLARE_NODE(DataLogger)
/*Node
 *
 * @name DataLogger
 * @category HARK:MISC
 * @description Log the input to stdout.
 *
 * @input_name INPUT
 * @input_type any
 * @input_description The input is what you want to log.
 *
 * @output_name OUTPUT
 * @output_type any
 * @output_description The output is the same as the input.
 *
 * @parameter_name LABEL
 * @parameter_type string
 * @parameter_description add a label to logged data
 *
 END*/

class DataLogger : public BufferedNode {
  int inputID;
  int outputID;

  string label;

public:
  DataLogger(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");

    if (parameters.exist("LABEL")) {
      label = object_cast<string>(parameters.get("LABEL"));
    }

    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    ObjectRef input(getInput(inputID, count));
    out[count] = input;

    cout << label << count << " ";
    if (typeid(*input) == typeid(Map<int, ObjectRef>)) {
      RCPtr<Map<int, ObjectRef> > in = input;
      Map<int, ObjectRef>::iterator it = in->begin();
      for (; it != in->end(); ++it) {
        const int id = it->first;
        if (typeid(*(it->second)) == typeid(Vector<float>)) {
          RCPtr<Vector<float> > vec = it->second;
          cout << id << " ";
          for (int i = 0; i < vec->size(); i++) {
            cout << (*vec)[i] << " ";
          }
        }
        else if (typeid(*(it->second)) == typeid(Vector<complex<float> >)) {
          RCPtr<Vector<complex<float> > > vec = it->second;
          cout << id << " ";
          for (int i = 0; i < vec->size(); i++) {
            cout << (*vec)[i] << " ";
          }
        }
      }
    }
    else if (typeid(*input) == typeid(Map<int, float>)) {
      RCPtr<Map<int, float> > in = input;
      Map<int, float>::iterator it = in->begin();
      for (; it != in->end(); ++it) {
        const int id = it->first;
        cout << id << " " << it->second << " ";
      }
    }
    else if (typeid(*input) == typeid(Map<int, int>)) {
      RCPtr<Map<int, int> > in = input;
      Map<int, int>::iterator it = in->begin();
      for (; it != in->end(); ++it) {
        const int id = it->first;
        cout << id << " " << it->second << " ";
      }
    }
    else if (typeid(*input) == typeid(Time)) {
      RCPtr<Time> in = input;
      cout << to_iso_extended_string(in->time);
    }
    cout << endl;
  }

  IN_ORDER_NODE_SPEEDUP(DataLogger);
};
