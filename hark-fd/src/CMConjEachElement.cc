/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#include <iostream>
#include <BufferedNode.h>
#include <Buffer.h>
#include <Vector.h>
#include <Matrix.h>
#include <math.h>
#include <Source.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace FD;

class CMConjEachElement;

DECLARE_NODE(CMConjEachElement);
/*Node
 *
 * @name CMConjEachElement
 * @category HARK:Localization:CorrelationMatrix
 * @description Calculate [OUTPUTCM = conj(INPUTCM)] like MATLAB and output the latest OUTPUTCM.
 *
 * @input_name INPUTCM
 * @input_type Matrix<complex<float> >
 * @input_description  Correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver.
 *
 * @output_name OUTPUTCM
 * @output_type Matrix<complex<float> >
 * @output_description  Correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver.
 *
END*/

class CMConjEachElement : public BufferedNode {
  int inputcmID;
  int outputcmID;
  typedef Matrix<complex<float> > FCMatrix; // FlowdesignerÍÑ

public:
  CMConjEachElement(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputcmID = addInput("INPUTCM");
    outputcmID = addOutput("OUTPUTCM");
    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    RCPtr<FCMatrix> inputcm = getInput(inputcmID, count);    
    RCPtr<FCMatrix> outputcm(new FCMatrix(inputcm->nrows(), inputcm->ncols()));    
    (*(outputs[outputcmID].buffer))[count] = outputcm;
    for (int m1 = 0; m1 < inputcm->nrows(); m1++) {
      for (int m2 = 0; m2 < inputcm->ncols(); m2++) {
	(*outputcm)(m1,m2) = conj((*inputcm)(m1,m2));
      }
    }  
  }

  //IN_ORDER_NODE_SPEEDUP(CMConjEachElement);
};
