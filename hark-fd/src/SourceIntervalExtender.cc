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
#include <deque>

using namespace std;
using namespace FD;

ObjectRef find_source(RCPtr<Vector<ObjectRef> > sources, int id)
{
  Vector<ObjectRef>::iterator it = sources->begin();
  for (; it != sources->end(); ++it) {
    RCPtr<Source> src(*it);
    if (src->id == id) {
      return src;
    }
  }
  return nilObject;
}

class SourceIntervalExtender;

DECLARE_NODE(SourceIntervalExtender);
/*Node
 *
 * @name SourceIntervalExtender
 * @category HARK:Localization
 * @description Output source localization information PREROLL_LENGTH frames earlier than input.
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name OUTPUT
 * @output_type Vector<ObjectRef>
 * @output_description Source locations PREROLL_LENGTH frames earlier than SOURCES.
 *
 * @parameter_name PREROLL_LENGTH
 * @parameter_type int
 * @parameter_value 50
 * @parameter_description Preroll length in frame. [default: 50]
 *
 END*/

class SourceIntervalExtender : public BufferedNode {
  int sourcesID;
  int outputID;

  int preroll_length;
  map<int, int> remaining_time;
  deque<Vector<ObjectRef> > xbuf;

public:
  SourceIntervalExtender(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    sourcesID = addInput("SOURCES");
    outputID = addOutput("OUTPUT");

    preroll_length = dereference_cast<int>(parameters.get("PREROLL_LENGTH"));

    if (preroll_length < 0) {
      throw new NodeException(NULL, "PREROLL_LENGTH should be positive.",
                              __FILE__, __LINE__);
    }

#if 0
    xbuf.resize(0);
#else
    inputsCache[sourcesID].lookAhead = preroll_length;
#endif
    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {

#if 0
    RCPtr<Vector<ObjectRef> > tmp_sources = getInput(sourcesID, count);
    RCPtr<Vector<ObjectRef> > log_sources(new Vector<ObjectRef>(*tmp_sources));
    if((xbuf.size() < preroll_length)||(xbuf.size()==0)){
      xbuf.push_back(*log_sources);
    }else{
      xbuf.push_back(*log_sources);      
      xbuf.pop_front();      
    }

    RCPtr<Vector<ObjectRef> > cur_sources(new Vector<ObjectRef>(xbuf[0]));
    RCPtr<Vector<ObjectRef> > later_sources(new Vector<ObjectRef>(xbuf[xbuf.size()-1]));
    RCPtr<Vector<ObjectRef> > new_sources(new Vector<ObjectRef>(*cur_sources));    
#else
    RCPtr<Vector<ObjectRef> > cur_sources = getInput(sourcesID, count);
    RCPtr<Vector<ObjectRef> > later_sources = getInput(sourcesID, count + preroll_length);
    RCPtr<Vector<ObjectRef> > new_sources(new Vector<ObjectRef>(*cur_sources));    
#endif

    if (count > 0) {
    RCPtr<Vector<ObjectRef> > last_sources = out[count - 1];

    for (map<int, int>::iterator it = remaining_time.begin();
         it != remaining_time.end(); ++it) {
      const int id = it->first;
      it->second--;

      ObjectRef cur_src = find_source(new_sources, id);
      if (cur_src->isNil()) {
        ObjectRef last_src = find_source(last_sources, id);
        if (!last_src->isNil()) {
          new_sources->push_back(last_src);
        }
      }
    }

    for (map<int, int>::iterator it = remaining_time.begin();
         it != remaining_time.end();) {
      if (it->second == 0) {
        remaining_time.erase(it++);
      }
      else {
        ++it;
      }
    }
    }
    
    for (Vector<ObjectRef>::iterator it_later_src = later_sources->begin();
         it_later_src != later_sources->end(); ++it_later_src) {
      RCPtr<Source> later_src = *it_later_src;

      ObjectRef cur_src = find_source(new_sources, later_src->id);
      if (cur_src->isNil()) {
        new_sources->push_back(later_src);
        remaining_time[later_src->id] = preroll_length;
      }
    }

    out[count] = new_sources;
  }
};
