/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#include "../config.h"

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
#include "Map.h"

#include <cstdlib>
#include <string.h>

#include "LocalizationFunction.hpp"

using namespace std;
using namespace boost;
using namespace FD;
using namespace boost::numeric;

typedef Matrix<complex<float> > FCMatrix;

#ifdef ENABLE_HARKIO3
extern "C" {
#include <libharkio3.h>
}
#endif

class CMLoad;

DECLARE_NODE(CMLoad);
/*Node
 *
 * @name CMLoad
 * @category HARK:Localization:CorrelationMatrix
 * @description This module loads a correlation matrix file saved by CMSave when one of following conditions is satisfied, 1) OPERATION_FLAG=1, 2) FILENAMER or FILENAMEI is changed. 
 *
 * @input_name FILENAMER
 * @input_type string
 * @input_description file name of the noise correlation matrix (real part)
 *
 * @input_name FILENAMEI
 * @input_type string
 * @input_description file name of the noise correlation matrix (imaginary part)
 *
 * @input_name OPERATION_FLAG
 * @input_type any
 * @input_description Integer or Bool flag. The file loading is done if this flag is 1/true. Set this 0/false and make this 1/true at appropriate time since this block costs a lot of computation power. If this flag is 0/false, the operation is done when the input filenames are changed. 
 *
 * @output_name OUTPUT
 * @output_type Matrix<complex<float> >
 * @output_description  Correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver.
 *
 * @parameter_name ENABLE_DEBUG
 * @parameter_type bool
 * @parameter_list true:false
 * @parameter_value false
 * @parameter_description enable debug print
 *
END*/

class CMLoad : public BufferedNode {
  int filenamerID;
  int filenameiID;
  int operationID;
  int outputID;

  int operation_flag;
  bool enable_debug;

  string opFileNamer;
  string opFileNamei;  

  vector<fcmatrix> Rxx;

public:
  CMLoad(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params),
      filenamerID(-1),filenameiID(-1)
  {
    operationID = addInput("OPERATION_FLAG");
    outputID   = addOutput("OUTPUT");

    enable_debug = dereference_cast<bool>(parameters.get("ENABLE_DEBUG"));

    operation_flag = 0;

    opFileNamer = "";
    opFileNamei = "";    

    inOrder = true;
  }

#ifdef ENABLE_HARKIO3
  virtual int translateInput(string inputName) {
    if (inputName == "FILENAMER") {
      filenamerID = addInput(inputName);
      return filenamerID;
    }
    else if (inputName == "FILENAMEI") {
      filenameiID = addInput(inputName);
      return filenameiID;
    }
    else if (inputName == "OPERATION_FLAG") {
      for (unsigned int i = 0; i < inputs.size(); i++) {
	if (inputs[i].name == inputName) {
	  return i;
	}
      }
    }
    else {throw new NodeException(this, inputName+ " is not supported.", __FILE__, __LINE__);}
  }
#endif

  void calculate(int output_id, int count, Buffer &out)
  {

    if (filenamerID == -1)
      throw new NodeException(this, string("FILENAMER not connected"),__FILE__, __LINE__);

    if (operationID == -1)
      throw new NodeException(this, string("OPERATION_FLAG not connected"),__FILE__, __LINE__);

    // Load CM file


    ObjectRef filenamerValue = getInput(filenamerID, count);
    const String &fileNamer = object_cast<String> (filenamerValue);

#ifndef ENABLE_HARKIO3
    if (filenameiID == -1)
      throw new NodeException(this, string("FILENAMEI not connected"),__FILE__, __LINE__);
    ObjectRef filenameiValue = getInput(filenameiID, count);
    const String &fileNamei = object_cast<String> (filenameiValue);
#else
    const String &fileNamei = "";
#endif

    char charnamer[100] = "";
    char charnamei[100] = "";

    strcpy(charnamer,fileNamer.c_str());
    strcpy(charnamei,fileNamei.c_str());

    ObjectRef operation = getInput(operationID, count);
    if (typeid(*operation) == typeid(Bool)) {
      bool bInput = dereference_cast<bool>(operation);
      operation_flag = (bInput ? 1 : 0);
    }else if(typeid(*operation) == typeid(Int)){
      operation_flag = dereference_cast<int> (operation);
    }else{
      throw new NodeException(this, string("Input type is not appropriate in CMLoad."), __FILE__, __LINE__);
    }

    if((operation_flag == 1)||(opFileNamer != fileNamer)||(opFileNamei != fileNamei)){    
      
#ifdef ENABLE_HARKIO3
      Rxx.resize(0);
      harkio_TransferFunction *mat = NULL;
      mat = harkio_TransferFunction_fromFile(charnamer,"correlation matrix");
      int nbins = mat->poses->size;
      COMPLEX_TYPE complexValue;
      for (int k = 0; k < nbins; k++) {
	fcmatrix cormat;
	cormat.resize(mat->loctfs[0]->rows, mat->loctfs[0]->cols);
	for(int i=0; i < mat->loctfs[0]->rows; i++){
	  for(int j=0; j < mat->loctfs[0]->cols; j++){
	    harkio_Matrix_getValueComplex(mat->loctfs[k], i, j, &complexValue);
	    cormat(i,j) = complexValue;	    
	  }
	}
	Rxx.push_back(cormat);
      }
#else
      LoadCorrelation(charnamer, charnamei, Rxx);
#endif

      if(enable_debug) printf("Count[%d] : CM Loaded (Size : %d x %d x %d)\n", count, Rxx.size(), Rxx[0].size1(), Rxx[0].size2());

      opFileNamer = fileNamer;
      opFileNamei = fileNamei;      

    }

    RCPtr<FCMatrix> output(new FCMatrix(Rxx.size(), Rxx[0].size1()*Rxx[0].size2()));    
    (*(outputs[outputID].buffer))[count] = output;

    for (int k = 0; k < Rxx.size(); k++) {
      for (int i = 0; i < Rxx[k].size1(); i++) {
	for (int j = 0; j < Rxx[k].size2(); j++) {
	  (*output)(k, i*Rxx[k].size2() + j) = Rxx[k](i, j);
	}
      }
    }   

  }

  //IN_ORDER_NODE_SPEEDUP(CMLoad);
};
