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

class PowerCalcForMap;

DECLARE_NODE(PowerCalcForMap)
/*Node
 *  
 * @name PowerCalcForMap
 * @category HARK:MISC
 * @description Calculate power spectrum from complex spectrum for Map data.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Complex spectrum. Key: source ID, Value: Data for each source.
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Power spectrum. Key: source ID, Value: Data for each source.
 * 
 * @parameter_name POWER_TYPE
 * @parameter_type string
 * @parameter_list POW:MAG
 * @parameter_value POW
 * @parameter_description Measure for computing the POW or MAG (i.e. power or magnitude) of the complex spectrum [default: POW]
 * 
 END*/

class PowerCalcForMap : public BufferedNode {
  int inputID;
  int powerID;
  int phaseID;
  string powerType;

public:
  PowerCalcForMap(string nodeName, ParameterSet params)
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
    ObjectRef orInputTemp;
    ObjectRef orOutputTemp, orOutputTemp2;
    ObjectRef orOutputVal, orOutputVal2;
    Map<int, ObjectRef>::iterator pmTemp;
    
    orInputTemp = getInput(inputID, count);

#if 0
    if (typeid(*orInputTemp)==typeid(Matrix<complex<float> >)){// Matrix
      /*
	Matrix<complex<float> > &mInputValue = object_cast<Matrix<complex<float> > > (orInputTemp);

	iNRow = mInputValue.nrows();
	iNCol = mInputValue.ncols();
		


	// Output
	orOutputTemp = ObjectRef(new Matrix<float>(iNRow, iNCol));
	Matrix<float> &mOutputValue = object_cast<Matrix<float> >(orOutputTemp);
	out[count] = orOutputTemp; 
      
	// norm
	for (i = 0 ; i < iNRow ; i++){
	for (k = 0 ; k < iNCol ; k++){
	  
	cTemp = mInputValue(i, k);
	fTemp = norm(cTemp);
	mOutputValue(i, k) = fTemp;
				
	}
	}
      */
    }
#endif
    if(typeid(*orInputTemp)==typeid(Map<int, ObjectRef>)) {// Map<int, ObjectRef>
      // Input
      Map<int, ObjectRef> &mpInputValue = object_cast<Map<int, ObjectRef> >(orInputTemp);

      orOutputTemp = ObjectRef(new Map<int, ObjectRef>);
      Map<int, ObjectRef> &mpOutputValue = object_cast<Map<int, ObjectRef> >(orOutputTemp);
      (*(outputs[powerID].buffer))[count] = orOutputTemp;
      mpOutputValue.clear();

      orOutputTemp2 = ObjectRef(new Map<int, ObjectRef>);
      Map<int, ObjectRef> &mpOutputValue2 = object_cast<Map<int, ObjectRef> >(orOutputTemp2);
      (*(outputs[phaseID].buffer))[count] = orOutputTemp2;
      mpOutputValue2.clear();

      for (pmTemp = mpInputValue.begin() ; pmTemp != mpInputValue.end() ; pmTemp++){
	orInputTemp = pmTemp->second;
	Vector<complex<float> > &vcfInputTemp = object_cast<Vector<complex<float> > >(orInputTemp);

	orOutputVal = ObjectRef(Vector<float>::alloc(vcfInputTemp.size()));
	Vector<float> &vfOutputTemp = object_cast<Vector<float> >(orOutputVal);

	orOutputVal2 = ObjectRef(Vector<complex<float> >::alloc(vcfInputTemp.size()));
	Vector<complex<float> > &vfOutputTemp2 = object_cast<Vector<complex<float> > >(orOutputVal2);

	for (i = 0 ; i < vcfInputTemp.size() ; i++){
	  if(powerType == "POW")
	    vfOutputTemp[i] = norm(vcfInputTemp[i]);
	  else if(powerType == "MAG")
	    vfOutputTemp[i] = abs(vcfInputTemp[i]);

	  if(abs(vcfInputTemp[i])!=0.0)
	    vfOutputTemp2[i] = vcfInputTemp[i] / abs(vcfInputTemp[i]);
	  else
	    vfOutputTemp2[i] = complex<float>(0.0f,0.0f);
	}

	mpOutputValue.insert(make_pair(pmTemp->first, orOutputVal));
	mpOutputValue2.insert(make_pair(pmTemp->first, orOutputVal2));
	  
      }
	
    }
  }

};
