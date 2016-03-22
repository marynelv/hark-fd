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
#include "LocalizationFunction.hpp"

using namespace std;
using namespace boost;
using namespace FD;
using namespace boost::numeric;

class CMInverseMatrix;

DECLARE_NODE(CMInverseMatrix);
/*Node
 *
 * @name CMInverseMatrix
 * @category HARK:Localization:CorrelationMatrix
 * @description Calculate [OUTPUTCM = inv(INPUTCM)] like MATLAB and output the latest result.
 *
 * @input_name INPUTCM
 * @input_type Matrix<complex<float> >
 * @input_description  Correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver.
 *
 * @input_name OPERATION_FLAG
 * @input_type any
 * @input_description Integer or bool flag. The main routine of this block is done if this flag is 1/true. Set this 0/false and make this 1/true at appropriate time since this block costs a lot of computation power. 
 *
 * @output_name OUTPUTCM
 * @output_type Matrix<complex<float> >
 * @output_description  Inverse of the INPUTCM.
 *
 * @parameter_name FIRST_FRAME_EXECUTION
 * @parameter_type bool
 * @parameter_list true:false
 * @parameter_value false
 * @parameter_description This flag enables to do this calculation in the first frame even if OPERATION_FLAG=0.
 *
 * @parameter_name ENABLE_DEBUG
 * @parameter_type bool
 * @parameter_list true:false
 * @parameter_value false
 * @parameter_description enable debug print
 *
END*/

class CMInverseMatrix : public BufferedNode {
  int inputcmID;
  int operationID;
  int outputcmID;

  int nb_channels;

  int operation_flag;

  bool first_frame_execution;
  bool enable_debug;

  int pslength;
  vector<fcmatrix> Rxx;

  typedef Matrix<complex<float> > FCMatrix; // FlowdesignerÍÑ

public:
  CMInverseMatrix(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputcmID = addInput("INPUTCM");
    operationID = addInput("OPERATION_FLAG");
    outputcmID = addOutput("OUTPUTCM");

    first_frame_execution = dereference_cast<bool>(parameters.get("FIRST_FRAME_EXECUTION"));
    enable_debug = dereference_cast<bool>(parameters.get("ENABLE_DEBUG"));

    operation_flag = 0;

    inOrder = true;

  }

  void calculate(int output_id, int count, Buffer &out)
  {
    
    RCPtr<FCMatrix> inputcm = getInput(inputcmID, count);

    if(count == 0){
      pslength = inputcm->nrows();
      nb_channels = (int)(floor(sqrtf((float)inputcm->ncols())));    
      if(inputcm->ncols() != nb_channels * nb_channels)
	throw new NodeException(this, string("CMInverseMatrix. Incorrect correlation matrix size."), __FILE__, __LINE__);
      Rxx.resize(pslength);
      for (int k = 0; k < pslength; k++) {
	Rxx[k] = ublas::zero_matrix<complex<float> >(nb_channels, nb_channels);
      }
    }

    ObjectRef operation = getInput(operationID, count);
    if (typeid(*operation) == typeid(Bool)) { 
      bool bInput = dereference_cast<bool>(operation); 
      operation_flag = (bInput ? 1 : 0); 
    }else if(typeid(*operation) == typeid(Int)){ 
      operation_flag = dereference_cast<int> (operation); 
    }else{ 
      throw new NodeException(this, string("Input type is not appropriate in CMInverseMatrix."), __FILE__, __LINE__); 
    } 

    RCPtr<FCMatrix> outputcm(new FCMatrix(Rxx.size(), Rxx[0].size1()*Rxx[0].size2()));    
    (*(outputs[outputcmID].buffer))[count] = outputcm;

    if((operation_flag == 1)||(first_frame_execution)){    

      fcmatrix Atmp(nb_channels, nb_channels);
      fcmatrix invAtmp(nb_channels, nb_channels);

      if(enable_debug) printf("Count[%d] : CMInverseMatrix Executed\n", count);

      for(int k = 0 ; k < pslength ; k++){

	for (int i = 0; i < nb_channels; i++) {
	  for (int j = 0; j < nb_channels; j++) {
	    Atmp(i, j) = (*inputcm)(k, i*nb_channels + j);
	  }
	}
	
	invMAT(Atmp, invAtmp);
	
	for (int i = 0; i < nb_channels; i++) {
	  for (int j = 0; j < nb_channels; j++) {
	    (*outputcm)(k, i*nb_channels + j) = invAtmp(i, j);
	    Rxx[k](i, j) = invAtmp(i, j);
	  }
	}
	
      }

      first_frame_execution = false;

    }else{

      for (int k = 0; k < pslength; k++) {
	for (int m1 = 0; m1 < nb_channels; m1++) {
	  for (int m2 = 0; m2 < nb_channels; m2++) {
	    (*outputcm)(k, m1*nb_channels + m2) = Rxx[k](m1, m2);
	  }
	}
      }      

    }
    
  }

  //IN_ORDER_NODE_SPEEDUP(CMInverseMatrix);
};
