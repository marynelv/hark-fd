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

class CMIdentityMatrix;

DECLARE_NODE(CMIdentityMatrix);
/*Node
 *
 * @name CMIdentityMatrix
 * @category HARK:Localization:CorrelationMatrix
 * @description This makes the identity correlation matrix for the localization.
 *
 * @output_name OUTPUT
 * @output_type Matrix<complex<float> >
 * @output_description  Correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver.
 * 
 * @parameter_name NB_CHANNELS
 * @parameter_type int
 * @parameter_value 8
 * @parameter_description The number of input channels.
 * 
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description The length of a frame (per channel).
 *
END*/

class CMIdentityMatrix : public BufferedNode {
  int outputID;
  int nb_channels;
  int length;
  int pslength;
  typedef Matrix<complex<float> > FCMatrix; // FlowdesignerÍÑ

public:
  CMIdentityMatrix(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    outputID = addOutput("OUTPUT");
    nb_channels = dereference_cast<int>(parameters.get("NB_CHANNELS"));
    length = dereference_cast<int>(parameters.get("LENGTH"));
    pslength = length / 2 + 1;
    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    
    RCPtr<FCMatrix> output(new FCMatrix(pslength, nb_channels * nb_channels));    
    (*(outputs[outputID].buffer))[count] = output;

    for (int k = 0; k < pslength; k++) {
      for (int i = 0; i < nb_channels; i++) {
 	for (int j = 0; j < nb_channels; j++) {
	  if(i==j){
	    (*output)(k, i*nb_channels + j) = complex<float>(1.0f,0.0f);
	  }else{
	    (*output)(k, i*nb_channels + j) = complex<float>(0.0f,0.0f);
	  }
 	}
      }
    } 

  }

  //IN_ORDER_NODE_SPEEDUP(CMIdentityMatrix);
};
