// ---------------------------------------------------------------------
/**
 Copyright (c) 2008 Honda Research Institute Japan, Co., Ltd.

 \file EstimateLeak.cc
 \brief	\~English It estimates the leak noise.
 \brief	\~Japanese �R�ꐬ���𐄒肷��
 \author	Natsu Tanaka
 \date	6 Aug 2010
 \version	$Id: $
*/
// ---------------------------------------------------------------------
#include "BufferedNode.h" 
#include "Map.h"
#include "Vector.h"
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
#include "PF_values.h"
#include "ArrayInit.h"
#include "PF_Init.h"
#include "PF_est_leak.h"
// ---------------------------------------------------------------------

// namespace�g�p��錾���邱��
using namespace FD;
using namespace std;

//forward declaration of class VAdd for use with the DECLARE_NODE macro
class EstimateLeak; // ���m�[�h

//Declaration of the node. This definition is transformed into XML data for the GUI, as well as documentation for the node
// �m�[�h�̓��o�͒[�q���쐬����
DECLARE_NODE(EstimateLeak) // ���m�[�h

/*Node
 *  
 * @name EstimateLeak
 * @category HARK:Separation
 * @description This estimates the leak noise.
 *
 * @input_name INPUT_POWER_SPEC
 * @input_type Map<int,ObjectRef>
 * @input_description Smoothed power. The key is a source ID,and the value is smoothed spectra(Vector<float>).
 *
 * @output_name LEAK_POWER_SPEC
 * @output_type Map<int,ObjectRef>
 * @output_description Leak power. The key is a source ID,and the value is leak power spectra(Vector<float>).
 * 
 * @parameter_name LEAK_FACTOR
 * @parameter_type float
 * @parameter_value 0.25
 * @parameter_description Leak factor [default:0.25]
 *
 * @parameter_name OVER_CANCEL_FACTOR
 * @parameter_type float
 * @parameter_value 1
 * @parameter_description Over cancel value [default:1]
END*/

// ---------------------------------------------------------------------

//Class definition/implementation. Note that because we won't need to derive from this class, we don't need a header file (.h) and we can put everything in the .cc. Our node, like most other nodes, derives from BufferedNode.

class EstimateLeak : public BufferedNode {

	int m_iInputID;
	int m_iOutputID;
	MICARY_PrmLeakNoise	m_sPrmLeakNoise;
	MICARY_FloatSpecArray	m_sInputSpec;	// ���͐M���X�y�N�g��
	MICARY_FloatSpecArray	m_sOutputSpec;	// �o�͐M���X�y�N�g��

public:
	EstimateLeak(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
	{ 
		float	fLeakFator, fOverCancel;

		//In the constructor, we create both the inputs and outputs.
		m_iInputID = addInput("INPUT_POWER_SPEC");
		m_iOutputID = addOutput("LEAK_POWER_SPEC");

		// ������
		AllocFloatSpecArray(&m_sInputSpec, 0, 0, 0);
		AllocFloatSpecArray(&m_sOutputSpec, 0, 0, 0);
		InitPFsPrm(NULL, &m_sPrmLeakNoise, NULL, NULL, NULL, NULL);

		if (parameters.exist("LEAK_FACTOR")){
		  fLeakFator = dereference_cast<float>(parameters.get("LEAK_FACTOR"));
		}
		else {
		  fLeakFator = 0.25;
		}
		if (parameters.exist("OVER_CANCEL_FACTOR")){
		  fOverCancel = dereference_cast<float>(parameters.get("OVER_CANCEL_FACTOR"));
		}
		else {
		  fOverCancel = 1.0;
		}
		Input_PF_PrmLeak(&m_sPrmLeakNoise, fLeakFator, fOverCancel);

	} 

	~EstimateLeak()
	{ 
	  FreeFloatSpecArray(&m_sInputSpec);
	  FreeFloatSpecArray(&m_sOutputSpec);
	}

	//This is the main method for the node, it is called from the BufferedNode class each time a result needs to be calculated.
  void calculate(int output_id, int count, Buffer &out)
  {
		// Input�����Map��FloatSpecArray�ɕϊ����CLeak�v�Z���s��
		// �v�Z���ʂ�Map�ɕϊ����o�͂���

		int i, k, iCount;
		int iNCh, iNFreq;
		HA_Float	*pfSpecTemp, *pfSpecTempEnd;
		Map<int, ObjectRef>::iterator pmapInputValue, pmapInputValueEnd;

		// Input
    ObjectRef orInputTemp = getInput(m_iInputID, count);
    Map<int, ObjectRef> &mapInputValue = object_cast<Map<int, ObjectRef> > (orInputTemp);

		iNCh = mapInputValue.size();

		// Output
		ObjectRef orOutputTemp = ObjectRef(new Map<int, ObjectRef>());
		Map<int, ObjectRef> &mapOutputValue = object_cast<Map<int, ObjectRef> >(orOutputTemp);
		out[count] = orOutputTemp;

		if (iNCh == 0){
			return;
		}


		iCount = 0;
		pmapInputValue = mapInputValue.begin();
		pmapInputValueEnd = mapInputValue.end();
		for (i=0 ; i<mapInputValue.size() ; i++){
			Vector<float>	&vfValueTemp = object_cast<Vector<float> >(pmapInputValue->second);
			iNFreq = vfValueTemp.size();
				
			// �v�Z�p�̔���p�ӂ���
			if (iNFreq != (m_sInputSpec.ha_iUpperFreq-m_sInputSpec.ha_iLowerFreq+1) || 
				iNCh != m_sInputSpec.ha_numCh){
				if (iCount == 0){	// ������
					FreeFloatSpecArray(&m_sInputSpec);
					FreeFloatSpecArray(&m_sOutputSpec);
					AllocFloatSpecArray(&m_sInputSpec, iNCh, 0, iNFreq-1);
					AllocFloatSpecArray(&m_sOutputSpec, iNCh, 0, iNFreq-1);
				}
				else {
					throw (new NodeException(NULL, "Exception caught in EstimateLeak calculate(Initialize values)", __FILE__, __LINE__));
				}
			}

			// �l�̃R�s�[
			pfSpecTemp = m_sInputSpec.micary_faSpec.ha_fValue+iNFreq*i;
			pfSpecTempEnd = pfSpecTemp+iNFreq-1;
			for (k = 0 ; k < iNFreq ; k++){
				*pfSpecTemp = vfValueTemp[k];

				if (pfSpecTemp == pfSpecTempEnd){
					break;
				}
				pfSpecTemp++;
			}

			pmapInputValue++;
			iCount++;
		}


		// EstimateLeak
		if (PF_EstLeak(&m_sInputSpec, m_sPrmLeakNoise, &m_sOutputSpec) != 1){
			throw (new NodeException(NULL, "Exception caught in EstimateLeak calculate(PF_EstLeak)", __FILE__, __LINE__));
		}


		// �o�͗p�̔��ɓ����
		pmapInputValue = mapInputValue.begin();
		pmapInputValueEnd = mapInputValue.end();
		for (i=0 ; i<mapInputValue.size() ; i++){
			ObjectRef orOutputValueTemp = ObjectRef(new Vector<float>);
			Vector<float>	&vfValueTemp = object_cast<Vector<float> >(orOutputValueTemp);

			// �l�̃R�s�[
			vfValueTemp.resize(iNFreq);
			pfSpecTemp = m_sOutputSpec.micary_faSpec.ha_fValue+iNFreq*i;
			pfSpecTempEnd = pfSpecTemp+iNFreq-1;
			for (k = 0 ; k < iNFreq ; k++){
				vfValueTemp[k] = *pfSpecTemp;

				if (pfSpecTemp == pfSpecTempEnd){
					break;
				}
				pfSpecTemp++;
			}
			
			// ����ID, Leak�X�y�N�g���̏o��
			mapOutputValue.insert(pair<int, ObjectRef>(pmapInputValue->first, orOutputValueTemp));

			pmapInputValue++;
		}

  }

};
