// ---------------------------------------------------------------------
/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#include <stdlib.h>
#include "BufferedNode.h" 
#include "Vector.h"
#include <complex>
#include <algorithm>
#include "Matrix.h"

#include "Map.h"
#include "Source.h"
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
#include "ArrayInit.h"
#include "PF_Init.h"
#include "PF_exec.h"
#include "CPF_exec.h"

// ---------------------------------------------------------------------

// namespace使用を宣言すること
using namespace FD;
using namespace std;

//forward declaration of class VAdd for use with the DECLARE_NODE macro
class PostFilter; // ←ノード

//Declaration of the node. This definition is transformed into XML data for the GUI, as well as documentation for the node
// ノードの入出力端子を作成する
DECLARE_NODE(PostFilter) // ←ノード

/*Node
 *  
 * @name PostFilter
 * @category HARK:Separation
 * @description This executes Post Filter.
 *
 * @input_name INPUT_SPEC
 * @input_type Map<int,ObjectRef>
 * @input_description Input spectrum with a source ID(key),and the value is input audio spectrum(Vector<complex<float>>)
 *
 * @input_name INIT_NOISE_POWER
 * @input_type Matrix<float>
 * @input_description Initialized noise power spectrum.
 *
 * @output_name OUTPUT_SPEC
 * @output_type Map<int,ObjectRef>
 * @output_description Output spectrum with a source ID(key),and the value is output audio spectrum(Vector<complex<float>>)
 *  
 * @output_name EST_NOISE_POWER
 * @output_type Map<int,ObjectRef>
 * @output_description Estimated noise power spectrum with a source ID(key),and the value is estimated noise power spectrum(Vector<float>)
 *
 * @parameter_name MCRA_SETTING
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Set the MCRA parameters [default:false]
 *
 * @parameter_name EST_LEAK_SETTING
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Set the EST_LEAK parameters[default:false]
 * 
 * @parameter_name EST_REV_SETTING
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Set the EST_REV parameters[default:false]
 * 
 * @parameter_name EST_SN_SETTING
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Set the EST_SN parameters[default:false]
 * 
 * @parameter_name EST_VOICEP_SETTING
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Set the EST_REV parameters[default:false]
 * 
 * @parameter_name STATIONARY_NOISE_FACTOR
 * @parameter_type float
 * @parameter_value 1.2
 * @parameter_valid MCRA_SETTING=true
 * @parameter_description Stationary noise estimation factor[default:1.2]
 *
 * @parameter_name SPEC_SMOOTH_FACTOR
 * @parameter_type float
 * @parameter_value 0.5
 * @parameter_valid MCRA_SETTING=true
 * @parameter_description Smoothing factor for input power spectrum [default:0.5]
 *
 * @parameter_name AMP_LEAK_FACTOR
 * @parameter_type float
 * @parameter_value 1.5
 * @parameter_valid MCRA_SETTING=true
 * @parameter_description Leak factor [default:1.5]
 *
 * @parameter_name STATIONARY_NOISE_MIXTURE_FACTOR
 * @parameter_type float
 * @parameter_value 0.98
 * @parameter_valid MCRA_SETTING=true
 * @parameter_description The mixture rate of stationary noise [default:0.98]
 *
 * @parameter_name LEAK_FACTOR
 * @parameter_type float
 * @parameter_value 0.25
 * @parameter_valid EST_LEAK_SETTING=true
 * @parameter_description Leak rate [default:0.25]
 *
 * @parameter_name OVER_CANCEL_FACTOR
 * @parameter_type float
 * @parameter_value 1
 * @parameter_valid EST_LEAK_SETTING=true
 * @parameter_description The weighting factor for leak rate [default:1]
 *
 * @parameter_name REVERB_DECAY_FACTOR
 * @parameter_type float
 * @parameter_value 0.5
 * @parameter_valid EST_REV_SETTING=true
 * @parameter_description The decay factor for reverblate power [default:0.5]
 *
 * @parameter_name DIRECT_DECAY_FACTOR
 * @parameter_type float
 * @parameter_value 0.2
 * @parameter_valid EST_REV_SETTING=true
 * @parameter_description The decay factor for separated spectrum [default:0.2]
 *
 * @parameter_name PRIOR_SNR_FACTOR
 * @parameter_type float
 * @parameter_value 0.8
 * @parameter_valid EST_SN_SETTING=true
 * @parameter_description The rate of prior and posterior SNR [default:0.8]
 *
 * @parameter_name PRIOR_SNR_SMOOTH_FACTOR
 * @parameter_type float
 * @parameter_value 0.7
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Smoothing factor [default:0.7]
 *
 * @parameter_name LEAK_FLOOR
 * @parameter_type float
 * @parameter_value 0.1
 * @parameter_valid MCRA_SETTING=true 
 * @parameter_description The min value of leak noise [default:0.1]
 *
 * @parameter_name BLOCK_LENGTH
 * @parameter_type int
 * @parameter_value 80
 * @parameter_valid MCRA_SETTING=true
 * @parameter_description Time interval for detection [default:80]
 *
 * @parameter_name VOICEP_THRESHOLD
 * @parameter_type float
 * @parameter_value 3
 * @parameter_valid MCRA_SETTING=true
 * @parameter_description Threshold level for check the voice probability [default:3]
 * 
 * @parameter_name VOICEP_PROB_FACTOR
 * @parameter_type float
 * @parameter_value 0.9
 * @parameter_valid EST_SN_SETTING=true
 * @parameter_description Amplitude factor for voice probability [default:0.9]
 *
 * @parameter_name MIN_VOICEP_PROB
 * @parameter_type float
 * @parameter_value 0.05
 * @parameter_valid EST_SN_SETTING=true
 * @parameter_description Min value of voice probability [default:0.05]
 *
 * @parameter_name MAX_PRIOR_SNR
 * @parameter_type float
 * @parameter_value 100
 * @parameter_valid EST_SN_SETTING=true
 * @parameter_description Max value of prior SNR [default:100]
 *
 * @parameter_name MAX_OPT_GAIN
 * @parameter_type float
 * @parameter_value 20
 * @parameter_valid EST_SN_SETTING=true
 * @parameter_description Max value of optimum gain mid variable(v) [default:20]
 *
 * @parameter_name MIN_OPT_GAIN
 * @parameter_type float
 * @parameter_value 6
 * @parameter_valid EST_SN_SETTING=true
 * @parameter_description Min value of optimum gain mid variable(v) [default:6]
 *
 * @parameter_name MIN_FRAME_SMOOTH_SNR
 * @parameter_type float
 * @parameter_value 0.1
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Min value of frequency smoothing SNR(frame) [default:0.1]
 *
 * @parameter_name MAX_FRAME_SMOOTH_SNR
 * @parameter_type float
 * @parameter_value 0.316
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Max value of frequency smoothing SNR(frame) [default:0.316]
 *
 * @parameter_name MIN_GLOBAL_SMOOTH_SNR
 * @parameter_type float
 * @parameter_value 0.1
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Min value of frequency smoothing SNR(global) [default:0.1]
 *
 * @parameter_name MAX_GLOBAL_SMOOTH_SNR
 * @parameter_type float
 * @parameter_value 0.316
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Max value of frequency smoothing SNR(global) [default:0.316]
 *
 * @parameter_name MIN_LOCAL_SMOOTH_SNR
 * @parameter_type float
 * @parameter_value 0.1
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Min value of frequency smoothing SNR(local) [default:0.1]
 *
 * @parameter_name MAX_LOCAL_SMOOTH_SNR
 * @parameter_type float
 * @parameter_value 0.316
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Max value of frequency smoothing SNR(local) [default:0.316]
 *
 * @parameter_name UPPER_SMOOTH_FREQ_INDEX
 * @parameter_type int
 * @parameter_value 99
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Upper bound frequency index used smoothing [default:99]
 *
 * @parameter_name LOWER_SMOOTH_FREQ_INDEX
 * @parameter_type int
 * @parameter_value 8
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Lower bound frequency index used smoothing [default:8]
 *
 * @parameter_name GLOBAL_SMOOTH_BANDWIDTH
 * @parameter_type int
 * @parameter_value 29
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Frequency smoothing bandwidth(global) [default:29]
 *
 * @parameter_name LOCAL_SMOOTH_BANDWIDTH
 * @parameter_type int
 * @parameter_value 5
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Frequency smoothing bandwidth(local) [default:5]
 *
 * @parameter_name FRAME_SMOOTH_SNR_THRESH
 * @parameter_type float
 * @parameter_value 1.5
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Threshold of frequency smoothing SNR(frame) [default:1.5]
 *
 * @parameter_name MIN_SMOOTH_PEAK_SNR
 * @parameter_type float
 * @parameter_value 1
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Min peak value of frequency smoothing SNR [default:1]
 *
 * @parameter_name MAX_SMOOTH_PEAK_SNR
 * @parameter_type float
 * @parameter_value 10
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Max peak value of frequency smoothing SNR [default:10]
 *
 * @parameter_name FRAME_VOICEP_PROB_FACTOR
 * @parameter_type float
 * @parameter_value 0.7
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description The voice probability smoothing factor(frame) [default:0.7]
 *
 * @parameter_name GLOBAL_VOICEP_PROB_FACTOR
 * @parameter_type float
 * @parameter_value 0.9
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description The voice probability smoothing factor(global) [default:0.9]
 *
 * @parameter_name LOCAL_VOICEP_PROB_FACTOR
 * @parameter_type float
 * @parameter_value 0.9
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description The voice probability smoothing factor(local) [default:0.9]
 *
 * @parameter_name MIN_VOICE_PAUSE_PROB
 * @parameter_type float
 * @parameter_value 0.02
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Min value of voice pause probability [default:0.02]
 *
 * @parameter_name MAX_VOICE_PAUSE_PROB
 * @parameter_type float
 * @parameter_value 0.98
 * @parameter_valid EST_VOICEP_SETTING=true
 * @parameter_description Max value of voice pause probability [default:0.98]
 * 
END*/

// ---------------------------------------------------------------------

//Class definition/implementation. Note that because we won't need to derive from this class, we don't need a header file (.h) and we can put everything in the .cc. Our node, like most other nodes, derives from BufferedNode.

class PostFilter : public BufferedNode {

	int m_iInputSpecID;
	int m_iInitNoisePowerID;
	int m_iExecuteID ;
	int m_iOutputSpecID;
	int m_iEstNoisePowerID;
	CPF_exec	m_cPFexec;
	MICARY_ComplexSpecArray	m_caInputSpec;	// 入力信号スペクトル
	MICARY_FloatSpecArray m_faInitNoisePSpec;	//初期雑音パワースペクトル
	MICARY_ComplexSpecArray m_caOutputSpec ;	//出力信号スペクトル
	MICARY_FloatSpecArray	m_faEstNoisePSpec;	// 推定雑音パワースペクトル
 
  MICARY_IntArray m_iaSrcLast ;//Front frame src
  MICARY_IntArray m_iaSrc ;// current frame src
  bool m_bFlgIni ; //Initial process flag
  float m_fInNoise_Compens ; // Noise Compens for MCRA

public:
	PostFilter(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
	{ 


			m_iInputSpecID = addInput("INPUT_SPEC");
			m_iInitNoisePowerID = addInput("INIT_NOISE_POWER") ;
			m_iOutputSpecID = addOutput("OUTPUT_SPEC");
			m_iEstNoisePowerID = addOutput("EST_NOISE_POWER");


		AllocComplexSpecArray(&m_caInputSpec,0,0,0) ;
		AllocFloatSpecArray(&m_faInitNoisePSpec,0,0,0) ;
		AllocComplexSpecArray(&m_caOutputSpec,0,0,0) ;
		AllocFloatSpecArray(&m_faEstNoisePSpec,0,0,0) ;
		IntArrayAlloc(&m_iaSrcLast,0) ;
		IntArrayAlloc(&m_iaSrc,0) ;
	
		m_cPFexec.Initialize(0,0,0,0) ;
		m_cPFexec.InitSavedArray(m_faInitNoisePSpec,0,0,0,0) ;

		//パラメータの取得
		float fTemp1,fTemp2,fTemp3,fTemp4,fTemp5,fTemp6,fTemp7,fTemp8,fTemp9,fTemp10,
			fTemp11,fTemp12,fTemp13,fTemp14,fTemp15,fTemp16,fTemp17,fTemp18,fTemp19  ;
		int iTemp ;
		int iTemp1,iTemp2,iTemp3,iTemp4 ;
	

		//MCRA
		if (parameters.exist("STATIONARY_NOISE_FACTOR")){
		  fTemp1 = dereference_cast<float>(parameters.get("STATIONARY_NOISE_FACTOR"));
		}
		else {
		  fTemp1 = 1.2;
		}
		if (parameters.exist("SPEC_SMOOTH_FACTOR")){
		  fTemp2 = dereference_cast<float>(parameters.get("SPEC_SMOOTH_FACTOR"));
		}
		else {
		  fTemp2 = 0.5;
		}
		if (parameters.exist("AMP_LEAK_FACTOR")){
		  fTemp3 = dereference_cast<float>(parameters.get("AMP_LEAK_FACTOR"));
		}
		else {
		  fTemp3 = 1.5;
		}
		if (parameters.exist("STATIONARY_NOISE_MIXTURE_FACTOR")){
		  fTemp4 = dereference_cast<float>(parameters.get("STATIONARY_NOISE_MIXTURE_FACTOR"));
		}
		else {
		  fTemp4 = 0.98;
		}
		if (parameters.exist("LEAK_FACTOR")){
		  fTemp5 = dereference_cast<float>(parameters.get("LEAK_FACTOR"));
		}
		else {
		  fTemp5 = 0.25;
		}
		if (parameters.exist("LEAK_FLOOR")){
		  fTemp6 = dereference_cast<float>(parameters.get("LEAK_FLOOR"));
		}
		else {
		  fTemp6 = 0.1;
		}
		if (parameters.exist("BLOCK_LENGTH")){
		  iTemp = dereference_cast<int>(parameters.get("BLOCK_LENGTH"));
		}
		else {
		  iTemp = 80;
		}
		if (parameters.exist("VOICEP_THRESHOLD")){
		  fTemp7 = dereference_cast<float>(parameters.get("VOICEP_THRESHOLD"));
		}
		else {
		  fTemp7 = 3;
		}
		m_cPFexec.Set_PF_PrmMCRA(fTemp1,fTemp2,fTemp3,fTemp4,fTemp5,fTemp6,iTemp,fTemp7) ;

		// Leak
		if (parameters.exist("LEAK_FACTOR")){
		  fTemp1 = dereference_cast<float>(parameters.get("LEAK_FACTOR"));
		}
		else {
		  fTemp1 = 0.25;
		}
		if (parameters.exist("OVER_CANCEL_FACTOR")){
		  fTemp2 = dereference_cast<float>(parameters.get("OVER_CANCEL_FACTOR"));
		}
		else {
		  fTemp2 = 1;
		}
		m_cPFexec.Set_PF_PrmLeak(fTemp1,fTemp2) ;

		// Reverb
		if (parameters.exist("REVERB_DECAY_FACTOR")){
		  fTemp1 = dereference_cast<float>(parameters.get("REVERB_DECAY_FACTOR"));
		}
		else {
		  fTemp1 = 0.5;
		}
		if (parameters.exist("DIRECT_DECAY_FACTOR")){
		  fTemp2 = dereference_cast<float>(parameters.get("DIRECT_DECAY_FACTOR"));
		}
		else {
		  fTemp2 = 0.2;
		}
		m_cPFexec.Set_PF_PrmRev(fTemp1,fTemp2) ;

		// SN ratio
		if (parameters.exist("PRIOR_SNR_FACTOR")){
		  fTemp1 = dereference_cast<float>(parameters.get("PRIOR_SNR_FACTOR"));
		}
		else {
		  fTemp1 = 0.8;
		}
		if (parameters.exist("VOICEP_PROB_FACTOR")){
		  fTemp2 = dereference_cast<float>(parameters.get("VOICEP_PROB_FACTOR"));
		}
		else {
		  fTemp2 = 0.9;
		}
		if (parameters.exist("MIN_VOICEP_PROB")){
		  fTemp3 = dereference_cast<float>(parameters.get("MIN_VOICEP_PROB"));
		}
		else {
		  fTemp3 = 0.05;
		}
		if (parameters.exist("MAX_PRIOR_SNR")){
		  fTemp4 = dereference_cast<float>(parameters.get("MAX_PRIOR_SNR"));
		}
		else {
		  fTemp4 = 100;
		}
		if (parameters.exist("MAX_OPT_GAIN")){
		  fTemp5 = dereference_cast<float>(parameters.get("MAX_OPT_GAIN"));
		}
		else {
		  fTemp5 = 20;
		}
		if (parameters.exist("MIN_OPT_GAIN")){
		  fTemp6 = dereference_cast<float>(parameters.get("MIN_OPT_GAIN"));
		}
		else {
		  fTemp6 = 6;
		}
		m_cPFexec.Set_PF_PrmSN(fTemp1,fTemp2,fTemp3,fTemp4,fTemp5,fTemp6) ;
		//VoiceP
		if (parameters.exist("PRIOR_SNR_SMOOTH_FACTOR")){
		  fTemp1 = dereference_cast<float>(parameters.get("PRIOR_SNR_SMOOTH_FACTOR"));
		}
		else {
		  fTemp1 = 0.7;
		}
		if (parameters.exist("MIN_FRAME_SMOOTH_SNR")){
		  fTemp2 = dereference_cast<float>(parameters.get("MIN_FRAME_SMOOTH_SNR"));
		}
		else {
		  fTemp2 = 0.1;
		}
		if (parameters.exist("MAX_FRAME_SMOOTH_SNR")){
		  fTemp3 = dereference_cast<float>(parameters.get("MAX_FRAME_SMOOTH_SNR"));
		}
		else {
		  fTemp3 = 0.316;
		}
		if (parameters.exist("MIN_GLOBAL_SMOOTH_SNR")){
		  fTemp4 = dereference_cast<float>(parameters.get("MIN_GLOBAL_SMOOTH_SNR"));
		}
		else {
		  fTemp4 = 0.1;
		}
		if (parameters.exist("MAX_GLOBAL_SMOOTH_SNR")){
		  fTemp5 = dereference_cast<float>(parameters.get("MAX_GLOBAL_SMOOTH_SNR"));
		}
		else {
		  fTemp5 = 0.316;
		}
		if (parameters.exist("MIN_LOCAL_SMOOTH_SNR")){
		  fTemp6 = dereference_cast<float>(parameters.get("MIN_LOCAL_SMOOTH_SNR"));
		}
		else {
		  fTemp6 = 0.1;
		}
		if (parameters.exist("MAX_LOCAL_SMOOTH_SNR")){
		  fTemp7 = dereference_cast<float>(parameters.get("MAX_LOCAL_SMOOTH_SNR"));
		}
		else {
		  fTemp7 = 0.316;
		}
		if (parameters.exist("UPPER_SMOOTH_FREQ_INDEX")){
		  iTemp1 = dereference_cast<int>(parameters.get("UPPER_SMOOTH_FREQ_INDEX"));
		}
		else {
		  iTemp1 = 99;
		}
		if (parameters.exist("LOWER_SMOOTH_FREQ_INDEX")){
		  iTemp2 = dereference_cast<int>(parameters.get("LOWER_SMOOTH_FREQ_INDEX"));
		}
		else {
		  iTemp2 = 8;
		}
		if (parameters.exist("GLOBAL_SMOOTH_BANDWIDTH")){
		  iTemp3 = dereference_cast<int>(parameters.get("GLOBAL_SMOOTH_BANDWIDTH"));
		}
		else {
		  iTemp3 = 29;
		}
		if (parameters.exist("LOCAL_SMOOTH_BANDWIDTH")){
		  iTemp4 = dereference_cast<int>(parameters.get("LOCAL_SMOOTH_BANDWIDTH"));
		}
		else {
		  iTemp4 = 5;
		}
		if (parameters.exist("FRAME_SMOOTH_SNR_THRESH")){
		  fTemp12 = dereference_cast<float>(parameters.get("FRAME_SMOOTH_SNR_THRESH"));
		}
		else {
		  fTemp12 = 1.5;
		}
		if (parameters.exist("MIN_SMOOTH_PEAK_SNR")){
		  fTemp13 = dereference_cast<float>(parameters.get("MIN_SMOOTH_PEAK_SNR"));
		}
		else {
		  fTemp13 = 1;
		}
		if (parameters.exist("MAX_SMOOTH_PEAK_SNR")){
		  fTemp14 = dereference_cast<float>(parameters.get("MAX_SMOOTH_PEAK_SNR"));
		}
		else {
		  fTemp14 = 10;
		}
		if (parameters.exist("FRAME_VOICEP_PROB_FACTOR")){
		  fTemp15 = dereference_cast<float>(parameters.get("FRAME_VOICEP_PROB_FACTOR"));
		}
		else {
		  fTemp15 = 0.7;
		}
		if (parameters.exist("GLOBAL_VOICEP_PROB_FACTOR")){
		  fTemp16 = dereference_cast<float>(parameters.get("GLOBAL_VOICEP_PROB_FACTOR"));
		}
		else {
		  fTemp16 = 0.9;
		}	
		if (parameters.exist("LOCAL_VOICEP_PROB_FACTOR")){
		  fTemp17 = dereference_cast<float>(parameters.get("LOCAL_VOICEP_PROB_FACTOR"));
		}
		else {
		  fTemp17 = 0.9;
		}
		if (parameters.exist("MIN_VOICE_PAUSE_PROB")){
		  fTemp18 = dereference_cast<float>(parameters.get("MIN_VOICE_PAUSE_PROB"));
		}
		else {
		  fTemp18 = 0.02;
		}
		if (parameters.exist("MAX_VOICE_PAUSE_PROB")){
		  fTemp19 = dereference_cast<float>(parameters.get("MAX_VOICE_PAUSE_PROB"));
		}
		else {
		  fTemp19 = 0.98;
		}


		m_cPFexec.Set_PF_PrmVoiceP(fTemp1,fTemp2,fTemp3,fTemp4,fTemp5,fTemp6,fTemp7,iTemp1,iTemp2,
			iTemp3,iTemp4,fTemp12,fTemp13,fTemp14,fTemp15,fTemp16,fTemp17,fTemp18,fTemp19) ;
		// Remove 2009.6.12

		/*// Gain
		if (parameters.exist("MINGAIN")){
		  fTemp1 = dereference_cast<float>(parameters.get("MINGAIN"));
		}
		else {
		  fTemp1 = 0.1;
		}
		m_cPFexec.Set_PF_PrmGain(fTemp1) ;
		*/
		//m_VsrcLast.resize(0) ; 
		m_bFlgIni = true ;

		// Init MCRA
		if(parameters.exist("INIT_NOISE_COMPENS")){
		  m_fInNoise_Compens = dereference_cast<float>(parameters.get("INIT_NOISE_COMPENS")) ;
		} 
		else{
		  m_fInNoise_Compens = 1.2 ;
		}
		// add inOrder
		inOrder = true ;
	} 

	~PostFilter()
	{ 
		FreeComplexSpecArray(&m_caInputSpec) ;
		FreeFloatSpecArray(&m_faInitNoisePSpec) ;
		FreeComplexSpecArray(&m_caOutputSpec) ;
		FreeFloatSpecArray(&m_faEstNoisePSpec) ;
		IntArrayFree(&m_iaSrcLast) ;
		IntArrayFree(&m_iaSrc) ;
		
	}

  // Add Initialize() 2008/2/15 
  virtual void initialize(){
   
    outputs[m_iOutputSpecID].lookAhead = outputs
      [m_iEstNoisePowerID].lookAhead 
      
          = max(outputs[m_iOutputSpecID].lookAhead,
		outputs[m_iEstNoisePowerID].lookAhead) ;
    this->BufferedNode::initialize() ;

  }

	//This is the main method for the node, it is called from the BufferedNode class each time a result needs to be calculated.
  void calculate(int output_id, int count, Buffer &out)
  {

      int iNCh, iNFreq,iNPos;
      int i, m, k;
      int iFlgChg = 1 ;
      float fTemp;
      //HA_Float	*pha_fTemp;
      bool bExecute ;
      ObjectRef orOutputSpecTemp , orEstNoisePTemp ;
      complex<float> cTemp ;
      Map<int,ObjectRef>::iterator itTemp ;
      //vector<int> Vsrc;
      
      vector<complex<float> > vcfInputSpec ;
	 	
		
      // Input Spectrum
      ObjectRef orInputSpecTemp = getInput(m_iInputSpecID, count);
      Map<int,ObjectRef> &mpInputSpecValue = object_cast<Map<int,ObjectRef> >(orInputSpecTemp) ;

      // Npos,Vsrc,Nfreq
      iNPos = mpInputSpecValue.size() ;
      //Vsrc.resize(iNPos) ;
      if(m_iaSrc.ha_numValue != iNPos){
	IntArrayFree(&m_iaSrc) ;
	IntArrayAlloc(&m_iaSrc,iNPos) ;
      }
      m = 0 ;
    
      for(itTemp = mpInputSpecValue.begin() ; itTemp != mpInputSpecValue.end() ; itTemp++){
	//Vsrc[m] = itTemp->first ;
	m_iaSrc.ha_iValue[m] = itTemp->first ;

        orInputSpecTemp = itTemp->second ;
	vcfInputSpec = object_cast<Vector<complex<float> > >(orInputSpecTemp) ;
	m++ ;
	
	if(itTemp == mpInputSpecValue.begin()){
	  iNFreq = vcfInputSpec.size() ;	//周波数数
	}
      }
               
      // Initial Noise
      ObjectRef orInitNoisePTemp = getInput(m_iInitNoisePowerID, count);
      Matrix<float> &mfInitNoiseP = object_cast<Matrix<float> > (orInitNoisePTemp);
      iNCh = mfInitNoiseP.nrows() ;	//チャンネル数
					
      // Outputの準備
      Buffer &outputBuffer = *(outputs[m_iOutputSpecID].buffer) ;
      Buffer &estnoiseBuffer = *(outputs[m_iEstNoisePowerID].buffer) ;
      
      // Output Spectrum
      orOutputSpecTemp = ObjectRef(new Map<int,ObjectRef>) ;
      Map<int,ObjectRef> &mpOutputSpecValue = object_cast<Map<int,ObjectRef> >(orOutputSpecTemp);
      outputBuffer[count] = orOutputSpecTemp;
	
      // Estimate Noise
      orEstNoisePTemp = ObjectRef(new Map<int,ObjectRef>);
      Map<int,ObjectRef> &mEstNoisePValue = object_cast<Map<int,ObjectRef> >(orEstNoisePTemp);
      estnoiseBuffer[count] = orEstNoisePTemp;
		
      if (iNPos != 0){
	if(mfInitNoiseP.ncols() != iNFreq){ //周波数ライン数のチェック
	  cout << "Error! Frequency size is mismatch!" << endl ;
	 throw(new NodeException(NULL, "Exception caught in Freq size error[PostFilter]", __FILE__, __LINE__));
	  return;
	}
			
        // 計算用InputSpecの準備			
	if (m_caInputSpec.ha_numCh != iNPos ||
	    (m_caInputSpec.ha_iUpperFreq - m_caInputSpec.ha_iLowerFreq+1) != iNFreq){
	  if(&m_caInputSpec){
	    FreeComplexSpecArray(&m_caInputSpec);
	  }
	  AllocComplexSpecArray(&m_caInputSpec, iNPos, 0, iNFreq - 1);

    }
        // 計算用初期ノイズの準備
	if (m_faInitNoisePSpec.ha_numCh != iNCh ||
	    (m_faInitNoisePSpec.ha_iUpperFreq - m_faInitNoisePSpec.ha_iLowerFreq + 1) != iNFreq){
	 
	  if(&m_faInitNoisePSpec){
	    FreeFloatSpecArray(&m_faInitNoisePSpec);
	  }

	  AllocFloatSpecArray(&m_faInitNoisePSpec, iNCh, 0, iNFreq - 1);
	}
	
	// 計算用OutputSpecの準備
	if (m_caOutputSpec.ha_numCh != iNPos ||
	    (m_caOutputSpec.ha_iUpperFreq - m_caOutputSpec.ha_iLowerFreq + 1) != iNFreq){
	  if(&m_caOutputSpec){
	    FreeComplexSpecArray(&m_caOutputSpec);
	  }
	  AllocComplexSpecArray(&m_caOutputSpec, iNPos, 0, iNFreq - 1);
			
	}
	// 計算用推定ノイズの準備
	if (m_faEstNoisePSpec.ha_numCh != iNPos ||
	    (m_faEstNoisePSpec.ha_iUpperFreq - m_faEstNoisePSpec.ha_iLowerFreq + 1) != iNFreq){
	  if(&m_faEstNoisePSpec){
	    FreeFloatSpecArray(&m_faEstNoisePSpec);
	  }

	  AllocFloatSpecArray(&m_faEstNoisePSpec, iNPos, 0, iNFreq - 1);
	}
				
	// Input Spectrum :Vector -> Array
	i = 0 ;
	for(itTemp = mpInputSpecValue.begin() ; itTemp != mpInputSpecValue.end() ; itTemp++){
	  orInputSpecTemp = itTemp->second ;
	  vcfInputSpec = object_cast<Vector<complex<float> > >(orInputSpecTemp) ;
	  for(k = 0 ; k < vcfInputSpec.size() ; k++){
	    m_caInputSpec.micary_cmpaSpec.ha_cmpValue[i*iNFreq+k].re = vcfInputSpec[k].real() ;
	    m_caInputSpec.micary_cmpaSpec.ha_cmpValue[i*iNFreq+k].im = vcfInputSpec[k].imag() ;
	  }
	  i++ ;
	}
	
	// Init Noise : Matrix -> Array
	for(i = 0 ; i < iNCh ; i++){
	  for(k = 0 ; k < iNFreq ; k++){
	    m_faInitNoisePSpec.micary_faSpec.ha_fValue[i*iNFreq+k] = mfInitNoiseP(i,k) ;
	  }
	}

    	if(m_iaSrcLast.ha_numValue != m_iaSrc.ha_numValue){
	  iFlgChg = 1 ;

	}
	else{
	  for(i = 0 ; i < m_iaSrcLast.ha_numValue ; i++){
	    for(m = 0 ; m < m_iaSrc.ha_numValue ; m++){
	      if(m_iaSrc.ha_iValue[m] == m_iaSrcLast.ha_iValue[i]){
		iFlgChg = 0;

	      }
	     else{
	       iFlgChg = 1;

	     }	    
	    }
	  }
	}
	
	// core process
	if(iFlgChg == 1){//入力信号ノイズの初期化(1フレーム、音源変更時)
	  //if(count == 0){
	  if(m_bFlgIni == true){
	    cout << "First frame process." << endl ;
	    if((m_cPFexec.m_pfsNoiseSpec->ha_numCh != iNPos) || 
	       (m_cPFexec.m_pfsNoiseSpec->ha_iUpperFreq - m_cPFexec.m_pfsNoiseSpec->ha_iLowerFreq + 1 != iNFreq )){
	       m_cPFexec.Initialize(0,iNFreq-1,iNPos,count) ;
	    }
	    //m_cPFexec.InitSavedArray(m_faInitNoisePSpec,0,iNFreq-1,iNPos,iNCh) ;
	     m_cPFexec.InitSavedArray(m_faInitNoisePSpec,0,iNFreq-1,iNPos,iNCh,m_fInNoise_Compens) ;
	    m_bFlgIni = false ;
	  }
	  else{
	   
	    m_cPFexec.ChangeSrcPos(m_faInitNoisePSpec,m_iaSrc,m_iaSrcLast,m_fInNoise_Compens) ;
	    
	    //IntArrayFree(&iaSrcCur) ;
	     //IntArrayFree(&iaSrcIni) ;
	  } // End count
        } // End iFlgChg

        // Post-Filter
	if(m_cPFexec.ExecutePF(m_caInputSpec,&m_caOutputSpec,&m_faEstNoisePSpec)== 0){
	  cout << "Fail to execute PostFilter." << endl ;
throw(new NodeException(NULL, "Exception caught in PostFilter calculate[PostFilter]", __FILE__, __LINE__));
 return ;
	}

	// output(OutputSpec, EstNoisePower)
	for(i = 0 ; i < iNPos ; i++){
	  int id = m_iaSrc.ha_iValue[i] ;
	  
	  //OutputSpec
	  ObjectRef orOutputSpecTemp2 = ObjectRef(Vector<complex<float> >::alloc(iNFreq)) ;
	  vector<complex<float> > & vcfOutputSpec = object_cast<vector<complex<float> > >(orOutputSpecTemp2) ;
	  
	  //EstNoisePower
	  ObjectRef orEstNoisePTemp2 = ObjectRef(Vector<float>::alloc(iNFreq)) ;
	  vector<float> & vfEstNoise = object_cast<vector<float> >(orEstNoisePTemp2) ;
	  
	  for(k = 0 ; k < iNFreq ; k++){
	    // OutputSpec
	    vcfOutputSpec[k] = complex<float>(m_caOutputSpec.micary_cmpaSpec.ha_cmpValue[i*iNFreq+k].re,
					      m_caOutputSpec.micary_cmpaSpec.ha_cmpValue[i*iNFreq+k].im) ;
	    
	    // EstNoisePower
	    vfEstNoise[k] = m_faEstNoisePSpec.micary_faSpec.ha_fValue[i*iNFreq+k] ;
	  }
	  // OutputSpec
	  mpOutputSpecValue.insert(make_pair(id,orOutputSpecTemp2)) ;

	  // EstNoisePower
	  mEstNoisePValue.insert(make_pair(id,orEstNoisePTemp2)) ;
	}
             
      } //end iNPos = 0
      
      // Update Vsrc(2007/09/03) 
      if(m_iaSrcLast.ha_numValue != m_iaSrc.ha_numValue){
	IntArrayFree(&m_iaSrcLast) ;
	IntArrayAlloc(&m_iaSrcLast,m_iaSrc.ha_numValue) ;
      }
      for(i = 0 ; i < m_iaSrcLast.ha_numValue; i++){
	m_iaSrcLast.ha_iValue[i] = m_iaSrc.ha_iValue[i] ;
      }


  }

};
