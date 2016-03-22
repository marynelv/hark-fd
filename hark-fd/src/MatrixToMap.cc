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

using namespace FD;
using namespace std;

class MatrixToMap;

DECLARE_NODE(MatrixToMap)
/*Node
 *
 * @name MatrixToMap
 * @category HARK:MISC
 * @description Convert Matrix<complex<float> > or Matrix<float> to Map<int,ObjectRef>. A row index is assigned to a key of map.
 *
 * @input_name INPUT
 * @input_type any
 * @input_description A matrix of complex float or float.
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description ObjectRef is a vector of complex float or float.
 *
 END*/

static const int ROW_MAJOR_ORDER = 0;
static const int COLUMN_MAJOR_ORDER = 1;

class MatrixToMap : public BufferedNode {
  int inputID;
  int outputID;

public:
  MatrixToMap(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params) {
	inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");
  }
    
  void calculate(int output_id, int count, Buffer &out) {
    ObjectRef input = getInput(inputID, count);
    RCPtr<Map<int, ObjectRef> > output(new Map<int, ObjectRef>);

    if (typeid(*input) == typeid(Matrix<complex<float> >)) {
      const Matrix<complex<float> >& mat = object_cast<Matrix<complex<float> > >(input);
      
      for (int row = 0; row < mat.nrows(); row++) {
        RCPtr<Vector<complex<float> > > spec(new Vector<complex<float> >(mat.ncols()));
	    for (int col = 0; col < mat.ncols(); col++) {
          (*spec)[col] = mat(row, col);
        }
	    (*output)[row] = spec;
      }
      out[count] = output;
    }
    else if (typeid(*input) == typeid(Matrix<float>)) {
      const Matrix<float>& mat = object_cast<Matrix<float> >(input);
      
      for (int row = 0; row < mat.nrows(); row++) {
        RCPtr<Vector<float> > wave(new Vector<float>(mat.ncols()));
        for (int col = 0; col < mat.ncols(); col++) {
          (*wave)[col] = mat(row, col);
        }
        (*output)[row] = wave;
      }
      out[count] = output;
    }
  }

  NO_ORDER_NODE_SPEEDUP(MatrixToMap);
};
