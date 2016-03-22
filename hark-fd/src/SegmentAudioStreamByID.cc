/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <iostream>
#include <BufferedNode.h>
#include <Buffer.h>
#include <Matrix.h>
#include <Vector.h>
#include <Map.h>
#include <Stream.h>
#include "Source.h"

using namespace FD;
using namespace std;

class SegmentAudioStreamByID;

DECLARE_NODE(SegmentAudioStreamByID)
/*Node
 *
 * @name SegmentAudioStreamByID
 * @category HARK:MISC
 * @description Segment the part of the audio stream that is labeled with an ID. It converts Matrix<complex<float> > or Matrix<float> to Map<int,ObjectRef> with the given ID data that is provided by the sources input. Note that it supports only single channel stream.
 *
 * @input_name INPUT
 * @input_type any
 * @input_description A matrix of complex float or float.
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source". Feature vectors are saved for each source ID. This parameter is mandatory.
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description ObjectRef is a vector of complex float or float.
 *
 END*/

static const int ROW_MAJOR_ORDER = 0;
static const int COLUMN_MAJOR_ORDER = 1;

class SegmentAudioStreamByID : public BufferedNode {
  int inputID;
  int outputID;
  int sourcesID;
  
public:
  SegmentAudioStreamByID(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params) {
	inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");
    sourcesID = addInput("SOURCES");
  }
    
  void calculate(int output_id, int count, Buffer &out) {
    ObjectRef input = getInput(inputID, count);
    ObjectRef outputTemp;
    RCPtr<Vector<ObjectRef> > sources;
    sources = getInput(sourcesID, count);
    RCPtr<Map<int, ObjectRef> > output(new Map<int, ObjectRef>);
		if (typeid(*input) == typeid(Matrix<complex<float> >)) 
    {
      const Matrix<complex<float> >& mat = object_cast<Matrix<complex<float> > >(input);
      for (int row = 0; row < mat.nrows(); row++) 
      {
        RCPtr<Vector<complex<float> > > spec(new Vector<complex<float> >(mat.ncols()));
	    	for (int col = 0; col < mat.ncols(); col++) 
	    	{
          (*spec)[col] = mat(row, col);
        }
     		for (int i = 0; i < sources->size(); i++) 
      	{
        	RCPtr<Source> src = (*sources)[i];
         (*output)[src->id] = spec;
       	}
      }
      out[count] = output;
    }
    else if (typeid(*input) == typeid(Matrix<float>)) 
    {
      const Matrix<float>& mat = object_cast<Matrix<float> >(input);
      for (int row = 0; row < mat.nrows(); row++) 
      {
        RCPtr<Vector<float> > wave(new Vector<float>(mat.ncols()));
        for (int col = 0; col < mat.ncols(); col++) 
        {
          (*wave)[col] = mat(row, col);
        }
      	for (int i = 0; i < sources->size(); i++) 
      	{
       		RCPtr<Source> src = (*sources)[i];
       	 (*output)[src->id] = wave;
        }
	    }
      out[count] = output;
    }
  }
  NO_ORDER_NODE_SPEEDUP(SegmentAudioStreamByID);
};
