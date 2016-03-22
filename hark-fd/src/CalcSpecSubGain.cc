// ---------------------------------------------------------------------
/**
 Copyright (c) 2010 Honda Research Institute Japan, Co., Ltd.

 \file CalcSpecSubGain.cc
 \brief	\~English Output INPUTPOWER minus EST_NOISEPOWER.
 \brief	\~Japanese 2つの入力値の引き算結果を出力する 
 \author	Kenichi Morio
 \date	28 May 2010
*/
// ---------------------------------------------------------------------
#include "BufferedNode.h" 
#include "Map.h"
#include "Vector.h"
#include "math.h"
// ---------------------------------------------------------------------


// ---------------------------------------------------------------------

// namespace使用を宣言すること
using namespace FD;
using namespace std;

//forward declaration of class VAdd for use with the DECLARE_NODE macro
class CalcSpecSubGain; // ←ノード

//Declaration of the node. This definition is transformed into XML data for the GUI, as well as documentation for the node
// ノードの入出力端子を作成する
DECLARE_NODE(CalcSpecSubGain) // ←ノード

/*Node
 *  
 * @name CalcSpecSubGain
 * @category HARK:Separation
 * @description This subtracts noise power spectrum from input power spectrum, and estimates optimum gain coeffcients.
 *
 * @input_name INPUT_POWER_SPEC
 * @input_type Map<int,ObjectRef>
 * @input_description Input power spectrum(Vector<float>) with a key(source ID).
 *
 * @input_name NOISE_SPEC
 * @input_type Map<int,ObjectRef>
 * @input_description Estimated Noise spectrum(Vector<float>) with a key(source ID).
 *
 * @output_name VOICE_PROB
 * @output_type Map<int,ObjectRef>
 * @output_description Probability of voice existence(Vector<float>) with a key(source ID).
 *
 * @output_name GAIN
 * @output_type Map<int,ObjectRef>
 * @output_description Optimum gain coefficients(Vector<float>) with a key(source ID).
 * 
 * @output_name OUTPUT_POWER_SPEC
 * @output_type Map<int,ObjectRef>
 * @output_description Power spectrum by subtracting NOISE_SPEC from INPUT_POWER_SPEC(Vector<float>) with a key(source ID).
 *
* @parameter_name ALPHA
* @parameter_type float
* @parameter_value 1.0
* @parameter_description Overestimation factor.
*
* @parameter_name BETA
* @parameter_type float
* @parameter_value 0.0
* @parameter_description Spectral floor.
*
* @parameter_name SS_METHOD
* @parameter_type int
* @parameter_value 2
* @parameter_description 1: Magnitude Spectral Subtraction, 2: Power SS
*
END*/

// ---------------------------------------------------------------------

//Class definition/implementation. Note that because we won't need to derive from this class, we don't need a header file (.h) and we can put everything in the .cc. Our node, like most other nodes, derives from BufferedNode.

class CalcSpecSubGain : public BufferedNode {

	int m_iInputPowerID;
	int m_iEst_NoisePower;
	int m_iCalcSpecSubGainID;
	int m_iGainID;
	int m_iVoicePID;
	float alpha, beta;
	int method;
	

public:
	CalcSpecSubGain(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
	{ 
	
		//In the constructor, we create both the inputs and outputs.
		m_iInputPowerID = addInput("INPUT_POWER_SPEC");
		m_iEst_NoisePower = addInput("NOISE_SPEC");
		m_iCalcSpecSubGainID = addOutput("OUTPUT_POWER_SPEC");
		m_iGainID = addOutput("GAIN");
		m_iVoicePID = addOutput("VOICE_PROB");
		alpha = dereference_cast<float>(parameters.get("ALPHA"));
		beta = dereference_cast<float>(parameters.get("BETA"));
		method = dereference_cast<int>(parameters.get("SS_METHOD"));
	} 

	virtual void initialize(){
		int maxval;
		maxval=max(outputs[m_iCalcSpecSubGainID].lookAhead,outputs[m_iGainID].lookAhead);
		maxval=max(maxval,outputs[m_iVoicePID].lookAhead);

		outputs[m_iCalcSpecSubGainID].lookAhead = maxval;
		outputs[m_iGainID].lookAhead = maxval;
		outputs[m_iVoicePID].lookAhead = maxval;
		this->BufferedNode::initialize();
		//printf("ini %d\n",maxval);

	}	

	//This is the main method for the node, it is called from the BufferedNode class each time a result needs to be calculated.
  void calculate(int output_id, int count, Buffer &out)
  {

		int i, k;
		int iNCh, iNFreq;
		Map<int, ObjectRef>::iterator pmapInputPower;
		Map<int, ObjectRef>::iterator pmapEst_NoisePower;
		Map<int, ObjectRef>::iterator pmapCalcSpecSubGain;
		Map<int, ObjectRef>::iterator pmapGain;
		Map<int, ObjectRef>::iterator pmapVoiceP;
		// Input
		ObjectRef orInputTemp1 = getInput(m_iInputPowerID, count);
		Map<int, ObjectRef> &mapInputPower = object_cast<Map<int, ObjectRef> > (orInputTemp1);

		ObjectRef orInputTemp2 = getInput(m_iEst_NoisePower, count);
		Map<int, ObjectRef> &mapEst_NoisePower = object_cast<Map<int, ObjectRef> > (orInputTemp2);
		
		// Outputの準備
		Buffer &subBuffer = *(outputs[m_iCalcSpecSubGainID].buffer) ;
		Buffer &gainBuffer = *(outputs[m_iGainID].buffer) ;
		Buffer &voicePBuffer = *(outputs[m_iVoicePID].buffer) ;

		// CalcSpecSubGain
		ObjectRef orCalcSpecSubGainTemp = ObjectRef(new Map<int, ObjectRef>());
		Map<int, ObjectRef> &mapCalcSpecSubGain = object_cast<Map<int, ObjectRef> >(orCalcSpecSubGainTemp);
		subBuffer[count] = orCalcSpecSubGainTemp;

		// Gain
		ObjectRef orGainTemp = ObjectRef(new Map<int, ObjectRef>());
		Map<int, ObjectRef> &mapGain = object_cast<Map<int, ObjectRef> >(orGainTemp);
		gainBuffer[count] = orGainTemp;

		// VoiceP
		ObjectRef orVoicePTemp = ObjectRef(new Map<int, ObjectRef>());
		Map<int, ObjectRef> &mapVoiceP = object_cast<Map<int, ObjectRef> >(orVoicePTemp);
		voicePBuffer[count] = orVoicePTemp;

		iNCh = mapInputPower.size();

		if (mapInputPower.size()!=mapEst_NoisePower.size()){
			throw (new NodeException(NULL, "Exception caught in CalcSpecSubGain calculate. InputMap size not match.", __FILE__, __LINE__));
		}

	

		if (iNCh == 0){
			return;
		}

	
		pmapInputPower = mapInputPower.begin();
		pmapEst_NoisePower = mapEst_NoisePower.begin();				
		pmapCalcSpecSubGain = mapCalcSpecSubGain.begin();
		pmapGain = mapGain.begin();
		pmapVoiceP = mapVoiceP.begin();

		for (i=0 ; i<mapInputPower.size() ; i++){
			Vector<float>	&vfInputPower = object_cast<Vector<float> >(pmapInputPower->second);
			Vector<float>	&vfEst_NoisePower = object_cast<Vector<float> >(mapEst_NoisePower[pmapInputPower->first]);
			
			ObjectRef orvCalcSpecSubGainTemp = ObjectRef(new Vector<float>);
			Vector<float>	&vfCalcSpecSubGainTemp = object_cast<Vector<float> >(orvCalcSpecSubGainTemp);
			
			ObjectRef orvGainTemp = ObjectRef(new Vector<float>);
			Vector<float>	&vfGainTemp = object_cast<Vector<float> >(orvGainTemp);

			ObjectRef orvVoicePTemp = ObjectRef(new Vector<float>);
			Vector<float>	&vfVoicePTemp = object_cast<Vector<float> >(orvVoicePTemp);
			
			iNFreq = vfInputPower.size();
			vfCalcSpecSubGainTemp.resize(iNFreq);
			vfGainTemp.resize(iNFreq);
			vfVoicePTemp.resize(iNFreq);
				
		
			for (k = 0 ; k < iNFreq ; k++){

			  if (method == 1) vfGainTemp[k] = max( 1.0f - alpha * (sqrt(vfEst_NoisePower[k]) / max(sqrt(vfInputPower[k]), 0.0001f)), beta); // Magnitude spectral subtraction
			  if (method == 2) {
			    //same representation in the above formula: vfGainTemp[k] = sqrt(max( 1.0f - alpha * (vfEst_NoisePower[k] / max(vfInputPower[k], 0.0001f)), beta)); // Power spectral subtraction
				
				//original power SS method of Nakajima-san:
				  vfCalcSpecSubGainTemp[k]=vfInputPower[k]-vfEst_NoisePower[k];
				
				//減算結果が負の場合は0を返す
				if(vfCalcSpecSubGainTemp[k]<0){
					vfCalcSpecSubGainTemp[k]=0;
				}
				if(vfInputPower[k]!=0.0){
					vfGainTemp[k]=sqrt(vfCalcSpecSubGainTemp[k]/vfInputPower[k]);
					}
				else{
					vfGainTemp[k]=0.0;
				}
			  }
			  vfVoicePTemp[k]=1.0;

			
			}
			// 音源ID, 減算結果の出力
			mapCalcSpecSubGain.insert(pair<int, ObjectRef>(pmapInputPower->first, orvCalcSpecSubGainTemp));
			mapGain.insert(pair<int, ObjectRef>(pmapInputPower->first, orvGainTemp));
			mapVoiceP.insert(pair<int, ObjectRef>(pmapInputPower->first, orvVoicePTemp));
			pmapInputPower++;
			pmapEst_NoisePower++;
				
		}

  }

};
