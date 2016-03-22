/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <complex>
#include "BufferedNode.h" 
#include "Matrix.h"
#include "Map.h"
#include "Vector.h"

using namespace FD;
using namespace std;

class PowerCalcForMatrix;

DECLARE_NODE(PowerCalcForMatrix)
/*Node
 *  
 * @name PowerCalcForMatrix
 * @category HARK:MISC
 * @description Calculate power spectrum from complex spectrum for Matrix data.
 *
 * @input_name INPUT
 * @input_type Matrix<complex<float> >
 * @input_description Complex spectrum. Row: source ID, Col: Data for each source.
 *
 * @output_name OUTPUT
 * @output_type Matrix<float>
 * @output_description Power spectrum. Row: source ID, Col: Data for each source.
 * 
 * @parameter_name POWER_TYPE
 * @parameter_type string
 * @parameter_list POW:MAG
 * @parameter_value POW
 * @parameter_description Measure for computing the POW or MAG (i.e. power or magnitude) of the complex spectrum [default: POW]
 * 
 END*/

class PowerCalcForMatrix : public BufferedNode {
  int inputID;
  int powerID;
  int phaseID;
  string powerType;

public:
  PowerCalcForMatrix(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  { 
    inputID = addInput("INPUT");
    powerID = addOutput("OUTPUT");
    phaseID = addOutput("PHASE");
    powerType = object_cast<String>(parameters.get("POWER_TYPE"));        
  } 

  virtual void initialize()
  {
    outputs[powerID].lookAhead
      = outputs[phaseID].lookAhead
      = 1 + max(outputs[powerID].lookAhead, outputs[phaseID].lookAhead);
    outputs[powerID].lookBack
      = outputs[phaseID].lookBack
      = 1 + max(outputs[powerID].lookBack, outputs[phaseID].lookBack);
    this->BufferedNode::initialize();
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    
    int iNRow, iNCol;
    int i, k;
    float fTemp;
    complex<float> cTemp;
    ObjectRef orInputTemp, orOutputTemp, orOutputTemp2;
    Map<int, ObjectRef>::iterator pmTemp;
    
    orInputTemp = getInput(inputID, count);

    if (typeid(*orInputTemp)==typeid(Matrix<complex<float> >)){
      Matrix<complex<float> > &mInputValue = object_cast<Matrix<complex<float> > > (orInputTemp);

      iNRow = mInputValue.nrows();
      iNCol = mInputValue.ncols();

      orOutputTemp = ObjectRef(new Matrix<float>(iNRow, iNCol));
      Matrix<float> &mOutputValue = object_cast<Matrix<float> >(orOutputTemp);
      (*(outputs[powerID].buffer))[count] = orOutputTemp; 

      orOutputTemp2 = ObjectRef(new Matrix<complex<float> >(iNRow, iNCol));
      Matrix<complex<float> > &mOutputValue2 = object_cast<Matrix<complex<float> > >(orOutputTemp2);
      (*(outputs[phaseID].buffer))[count] = orOutputTemp2; 
      
      // norm
      for (i = 0 ; i < iNRow ; i++){
	for (k = 0 ; k < iNCol ; k++){
	  
	  cTemp = mInputValue(i, k);

	  if(powerType == "POW")
	    fTemp = norm(cTemp);
	  else if(powerType == "MAG")
	    fTemp = abs(cTemp);

	  mOutputValue(i, k) = fTemp;
	  if(abs(cTemp) != 0.0)
	    mOutputValue2(i, k) = cTemp / abs(cTemp);
	  else
	    mOutputValue2(i, k) = complex<float>(0.0f, 0.0f);	    
	}
      }
    }
#if 0
    else if(typeid(*orInputTemp)==typeid(Map<int, ObjectRef>)) {
      // Input
      Map<int, ObjectRef> &mpInputValue = object_cast<Map<int, ObjectRef> >(orInputTemp);

      // Output
      orOutputTemp = ObjectRef(new Map<int, ObjectRef>);
      Map<int, ObjectRef> &mpOutputValue = object_cast<Map<int, ObjectRef> >(orOutputTemp);
      out[count] = orOutputTemp;

      mpOutputValue.clear();

      // norm
      for (pmTemp = mpInputValue.begin() ; pmTemp != mpInputValue.end() ; pmTemp++){
	orInputTemp = pmTemp->second;
	Vector<complex<float> > &vcfInputTemp = object_cast<Vector<complex<float> > >(orInputTemp);

	orOutputTemp2 = ObjectRef(Vector<float>::alloc(vcfInputTemp.size()));
	Vector<float> &vfOutputTemp = object_cast<Vector<float> >(orOutputTemp2);

	for (i = 0 ; i < vcfInputTemp.size() ; i++){
	  vfOutputTemp[i] = norm(vcfInputTemp[i]);
	}

	mpOutputValue.insert(make_pair(pmTemp->first, orOutputTemp2));
	  
      }
    }
#endif

  }

};
