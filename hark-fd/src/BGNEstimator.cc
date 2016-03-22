// ---------------------------------------------------------------------
/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#include <complex>
#include "BufferedNode.h" 
#include "Matrix.h"
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
#include "PF_values.h"
#include "ArrayInit.h"
#include "PF_Init.h"
#include "PF_init_state_noise.h"
#include "CPF_init_state_noise.h"
// ---------------------------------------------------------------------

// namespace使用を宣言すること
using namespace FD;
using namespace std;

//forward declaration of class VAdd for use with the DECLARE_NODE macro
class BGNEstimator; // ←ノード

//Declaration of the node. This definition is transformed into XML data for the GUI, as well as documentation for the node
// ノードの入出力端子を作成する
DECLARE_NODE(BGNEstimator) // ←ノード

/*Node
 *  
 * @name BGNEstimator
 * @category HARK:Separation
 * @description Estimation of initial background noise based on MCRA.
 *
 * @input_name INPUT_POWER
 * @input_type Matrix<float>
 * @input_description Input power spectrum
 *
 * @output_name NOISE_POWER
 * @output_type Matrix<float>
 * @output_description Estimated background noise power spectrum
 * 
 * @parameter_name DELTA
 * @parameter_type float
 * @parameter_value 3.0
 * @parameter_description Power ratio used to decide a threshold to distinguish target signal and background noise. When this value is large, frequency bins with higher power are regarded as background noise, vice versa [default:3.0]
 *
 * @parameter_name L
 * @parameter_type int
 * @parameter_value 150
 * @parameter_description Time window length (in frame) to decide the lowest noise power spectrum, Smin [default:150]
 *
 * @parameter_name ALPHA_S
 * @parameter_type float
 * @parameter_value 0.7
 * @parameter_description A coefficient for temporal smoothing of input spectrum [default:0.7]
 *
 * @parameter_name NOISE_COMPENS
 * @parameter_type float
 * @parameter_value 1.0
 * @parameter_description Weight factor for background noise frames on smoothing [default: 1.0]
 *
 * @parameter_name ALPHA_D_MIN
 * @parameter_type float
 * @parameter_value 0.05
 * @parameter_description Minimum weight factor for signal (speech) frames [default:0.05]
 *
 * @parameter_name NUM_INIT_FRAME
 * @parameter_type int
 * @parameter_value 100
 * @parameter_description Duration in frame to initialize background noise power spectrum. All frames within this duration at the begining of processing are regarded as noise. [default:100]
 *
END*/

// ---------------------------------------------------------------------

//Class definition/implementation. Note that because we won't need to derive from this class, we don't need a header file (.h) and we can put everything in the .cc. Our node, like most other nodes, derives from BufferedNode.

class BGNEstimator : public BufferedNode {

	int m_iInputID;
	int m_iOutputID;
	CPF_init_state_noise	m_cStateNoise;
	MICARY_FloatSpecArray	m_sInputSpec;	// 入力信号スペクトル
	MICARY_FloatSpecArray	m_sOutputSpec;	// 出力信号スペクトル

public:
	BGNEstimator(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
	{ 

		//In the constructor, we create both the inputs and outputs.
		// 2008/08/19 changed by N.Tanaka
		/*
		try {
			m_iInputID = addInput("INPUT_POWER");
			m_iOutputID = addOutput("NOISE_POWER");
		}
		catch(BaseException *e){
			e->add(new NodeException(NULL, "Exception caught in StationaryNoiseEstimation constructor", __FILE__, __LINE__));
		}
		*/
		m_iInputID = addInput("INPUT_POWER");
		m_iOutputID = addOutput("NOISE_POWER");
        
		AllocFloatSpecArray(&m_sInputSpec, 0, 0, 0);
		AllocFloatSpecArray(&m_sOutputSpec, 0, 0, 0);


		MICARY_PrmInitMCRA	sPrm;
		float	fTemp;
		int iTemp;

		InitPrmInMCRA(&sPrm);

		if (parameters.exist("DELTA")){
		  fTemp = dereference_cast<float>(parameters.get("DELTA"));
		}
		else {
		  fTemp = 3.0;
		}
		sPrm.ha_fDelta = (HA_Float)fTemp;
		if (parameters.exist("L")){
		  iTemp = dereference_cast<int>(parameters.get("L"));
		}
		else {
		  iTemp = 150;
		}
		sPrm.ha_iL = (HA_Integer)iTemp;
		if (parameters.exist("ALPHA_S")){
		  fTemp = dereference_cast<float>(parameters.get("ALPHA_S"));
		}
		else {
		  fTemp = 0.7;
		}
		sPrm.ha_fAlp_s = (HA_Float)fTemp;
		if (parameters.exist("NOISE_COMPENS")){
		  fTemp = dereference_cast<float>(parameters.get("NOISE_COMPENS"));
		}
		else {
		  fTemp = 1.0;
		}
		sPrm.ha_fNoise_compense = (HA_Float)fTemp;
		if (parameters.exist("ALPHA_D_MIN")){
		  fTemp = dereference_cast<float>(parameters.get("ALPHA_D_MIN"));
		}
		else {
		  fTemp = 0.05;
		}
		sPrm.ha_fAlp_dmin = (HA_Float)fTemp;
		if (parameters.exist("NUM_INIT_FRAME")){
		  iTemp = dereference_cast<int>(parameters.get("NUM_INIT_FRAME"));
		}
		else {
		  iTemp = 100;
		}
		sPrm.ha_iInit_frame = (HA_Integer)iTemp;

		m_cStateNoise.Clear();
		m_cStateNoise.SetPrmInitMCRA(sPrm);

		// 2008/08/19 added by N.Tanaka
		inOrder = true;
	} 

	~BGNEstimator()
	{ 
	  FreeFloatSpecArray(&m_sInputSpec);
	  FreeFloatSpecArray(&m_sOutputSpec);
	}

	//This is the main method for the node, it is called from the BufferedNode class each time a result needs to be calculated.
  void calculate(int output_id, int count, Buffer &out)
  {
    
    int iNRowCh, iNColFreq;
    int i, k;
    float fTemp;
	HA_Float	*pha_fTemp;
	ObjectRef orOutputTemp;
    
	  // 2008/08/19 deleted by N.Tanaka
		// BaseExceptionはcatchせず，flowdesignerにまかせる
//    try {
      	// Input
      	ObjectRef orInputTemp = getInput(m_iInputID, count);
      	Matrix<float> &mInputValue = object_cast<Matrix<float> > (orInputTemp);

      	iNRowCh = mInputValue.nrows();	// チャンネル数
      	iNColFreq = mInputValue.ncols();	// 周波数数
		
      	// Output
	orOutputTemp = ObjectRef(new Matrix<float>(iNRowCh, iNColFreq));
      	Matrix<float> &mOutputValue = object_cast<Matrix<float> >(orOutputTemp);
      	out[count] = orOutputTemp;
			       
	
		// 計算用Specアレイの準備			
		if (m_sInputSpec.ha_numCh != iNRowCh ||
		   (m_sInputSpec.ha_iUpperFreq - m_sInputSpec.ha_iLowerFreq + 1) != iNColFreq){
				
			FreeFloatSpecArray(&m_sInputSpec);
			AllocFloatSpecArray(&m_sInputSpec, iNRowCh, 0, iNColFreq - 1);

		}
		if (m_sOutputSpec.ha_numCh != iNRowCh ||
		    (m_sOutputSpec.ha_iUpperFreq - m_sOutputSpec.ha_iLowerFreq + 1) != iNColFreq){
				
			FreeFloatSpecArray(&m_sOutputSpec);
			AllocFloatSpecArray(&m_sOutputSpec, iNRowCh, 0, iNColFreq - 1);
			
		}
				       
		// Matrix -> Specアレイ
      	pha_fTemp = m_sInputSpec.micary_faSpec.ha_fValue;
		for (i = 0 ; i < iNRowCh ; i++){
			for (k = 0 ; k < iNColFreq ; k++){
	  
				fTemp = mInputValue(i, k);
				*pha_fTemp = (HA_Float)fTemp;
				pha_fTemp++;							       
			}
      	}
		
		// 定常雑音推定
		if (m_cStateNoise.InitStateNoise(m_sInputSpec, &m_sOutputSpec) != 1){
			// 失敗
			// 2008/08/19 added by N.Tanaka メッセージの追加
			throw (new NodeException(NULL, "Exception caught in InitialStateNoise calculate", __FILE__, __LINE__));
			return;
		}
		


		// Specアレイ -> Matrix
      	pha_fTemp = m_sOutputSpec.micary_faSpec.ha_fValue;
		for (i = 0 ; i < iNRowCh ; i++){
			for (k = 0 ; k < iNColFreq ; k++){
	  
				fTemp = (float)(*pha_fTemp);
				mOutputValue(i, k) = fTemp;
				pha_fTemp++;				

			}
      	}
		
    
//	}
//    catch (BaseException *e){
//    	e->add(new NodeException(NULL, "Exception caught in StationaryNoiseEstimation calculate", __FILE__, __LINE__));
//    }


  }

};
