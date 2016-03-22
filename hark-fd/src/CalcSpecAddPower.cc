// ---------------------------------------------------------------------
/**
 Copyright (c) 2010 Honda Research Institute Japan, Co., Ltd.

 \file CalcSpecAddPower.cc
 \brief	\~English It estimates the voice spectra.
 \brief	\~Japanese パワースペクトルの足し算を行う
 \author	Kenichi Morio
 \date	28 Aug 2008
 \version	$Id: CalcSpecAddPower.cc 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include "BufferedNode.h" 
#include "Map.h"
#include "Vector.h"
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
// ---------------------------------------------------------------------

// namespace使用を宣言すること
using namespace FD;
using namespace std;

//forward declaration of class VAdd for use with the DECLARE_NODE macro
class CalcSpecAddPower; // ←ノード

//Declaration of the node. This definition is transformed into XML data for the GUI, as well as documentation for the node
// ノードの入出力端子を作成する
DECLARE_NODE(CalcSpecAddPower) // ←ノード

/*Node
 *  
 * @name CalcSpecAddPower
 * @category HARK:Separation
 * @description This add the power spectrum.
 *
 * @input_name INPUT_POWER_SPEC1
 * @input_type Map<int,ObjectRef>
 * @input_description Power spectrum(Vector<float>) with a key(source ID).
 *
 * @input_name INPUT_POWER_SPEC2
 * @input_type Map<int,ObjectRef>
 * @input_description Power spectrum(Vector<float>) with a key(source ID).
 *
 * @output_name OUTPUT_POWER_SPEC
 * @output_type Map<int,ObjectRef>
 * @output_description Estimated voice spectrum(Vector<float>) with a key(source ID).
 * 
END*/

// ---------------------------------------------------------------------

//Class definition/implementation. Note that because we won't need to derive from this class, we don't need a header file (.h) and we can put everything in the .cc. Our node, like most other nodes, derives from BufferedNode.

class CalcSpecAddPower : public BufferedNode {

	int m_iInputSpec1ID;
	int m_iInputSpec2ID;

	int m_iOutputSpecID;

public:
	CalcSpecAddPower(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
	{ 
		//In the constructor, we create both the inputs and outputs.
		m_iInputSpec1ID = addInput("INPUT_POWER_SPEC1");
		m_iInputSpec2ID = addInput("INPUT_POWER_SPEC2");
		
		m_iOutputSpecID = addOutput("OUTPUT_POWER_SPEC");
	} 

	~CalcSpecAddPower()
	{ 
//
	}

	//This is the main method for the node, it is called from the BufferedNode class each time a result needs to be calculated.
  void calculate(int output_id, int count, Buffer &out)
  {
	
		int i, k;
		int iNCh, iNFreq;
		Map<int, ObjectRef>::iterator	pmapInputSpec1, pmapInputSpec2,pmapOutputSpec;

		// Input
		ObjectRef orInputSpec1Temp = getInput(m_iInputSpec1ID, count);
		Map<int, ObjectRef> &mapInputSpec1 = object_cast<Map<int, ObjectRef> >(orInputSpec1Temp);

		ObjectRef orInputSpec2Temp = getInput(m_iInputSpec2ID, count);
		Map<int, ObjectRef> &mapInputSpec2 = object_cast<Map<int, ObjectRef> >(orInputSpec2Temp);



		// Output
		ObjectRef orOutputSpecTemp = ObjectRef(new Map<int, ObjectRef>());
		Map<int, ObjectRef> &mapOutputSpec = object_cast<Map<int, ObjectRef> >(orOutputSpecTemp);
		out[count] = orOutputSpecTemp;

		if (mapInputSpec1.size()!=mapInputSpec2.size()){
			throw (new NodeException(NULL, "Exception caught in CalcSpecAddPower calculate. InputMap size not match.", __FILE__, __LINE__));
		}

		iNCh = mapInputSpec1.size();
		if (iNCh == 0){
			return;
		}

		pmapInputSpec1 = mapInputSpec1.begin();
		pmapInputSpec2 = mapInputSpec2.begin();
		pmapOutputSpec = mapOutputSpec.begin();

		for (i=0 ; i<mapInputSpec1.size() ; i++){
			Vector<float>	&vfInputPower1 = object_cast<Vector<float> >(pmapInputSpec1->second);
			Vector<float>	&vfInputPower2 = object_cast<Vector<float> >(pmapInputSpec2->second);
			ObjectRef orvOutputTemp = ObjectRef(new Vector<float>);
			Vector<float>	&vfOutputTemp = object_cast<Vector<float> >(orvOutputTemp);

			iNFreq = vfInputPower1.size();
			vfOutputTemp.resize(iNFreq);
			for (k = 0 ; k < iNFreq ; k++){
				vfOutputTemp[k]=vfInputPower1[k]+vfInputPower2[k];
			}
			
			mapOutputSpec.insert(pair<int, ObjectRef>(pmapInputSpec1->first, orvOutputTemp));
			pmapInputSpec1++;
			pmapInputSpec2++;
		}

  }


			

};
