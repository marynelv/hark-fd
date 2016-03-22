
// -----------------------------------------------------
/*
// Copyright (C) 2009/12/24 Sachiko Suzuki
 \file GHDSS.cc
 \brief \~English It executes GHDSS process.
 \brief \~Japanese GHDSSの実行
 \author	Sachiko Suzuki
 \date 24 Dec 2009
 \version	$Id$
*/
// -----------------------------------------------------
// 2014/8/27 added by N.Tanaka
#include "../config.h"
#ifndef ENABLE_HARKIO_NH

#include <string>
#include <iostream>
#include <cstdlib>
#include <complex>
#include <cmath>

#include "BufferedNode.h"
#include "Vector.h"
#include "Matrix.h"
#include "Map.h"
#include "Source.h"

// -----------------------------------------------------
#include "ArrayInit.h"
#include "MICARY_struct.h"
#include "CGHDSS_exec.h"
#include "HarkFileIO.h"

// -----------------------------------------------------
// name spaceの宣言
using namespace std;
using namespace FD;


// Chaenged by Morio 2013/02/01
#ifdef ENABLE_HARKIO2
using namespace harkio2;
#elif ENABLE_HARKIO1
using namespace harkio;
#endif


// ソートのための比較関数
int HA_Id_cmp  (const void *, const void *);
int sortLocationArray(MICARY_LocationArray *locaSrc, MICARY_LocationArray *locaDest);

// Node name
class GHDSS;

DECLARE_NODE(GHDSS);
/*Node
 *
 * @name GHDSS
 * @category HARK:Separation
 * @description Execute sound source separation with GHDSS.
 *
 * @input_name INPUT_FRAMES
 * @input_type Matrix<complex<float> >
 * @input_description Input multi-channel spectrum. A row is a channel, and a column is a spectrum for the corresponding channel. 
 *
 * @input_name INPUT_SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Separated spectrum with a source ID(key), and the value is the separated spectrum for each sound source (Vector<complex<float> >).
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description The frame length of each channel (in samples) [default: 512].
 *
 * @parameter_name ADVANCE
 * @parameter_type int
 * @parameter_value 160
 * @parameter_description The shift length beween adjacent frames (in samples)[default: 160].
 *
 * @parameter_name SAMPLING_RATE
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_description Sampling rate (Hz) [default: 16000].
 *
 * @parameter_name LOWER_BOUND_FREQUENCY
 * @parameter_type int
 * @parameter_value 0
 * @parameter_description Lower bound of frequency (Hz). [default: 0]
 * 
 * @parameter_name UPPER_BOUND_FREQUENCY
 * @parameter_type int
 * @parameter_value 8000
 * @parameter_description Upper bound of frequency (Hz). [default: 8000]
 *
 * @parameter_name TF_CONJ_FILENAME
 * @parameter_type string
 * @parameter_description Filename of a pre-measured transfer function for separation.
 *
 * @parameter_name INITW_FILENAME
 * @parameter_type string
 * @parameter_valid COMPARE_MODE=DEG
 * @parameter_description Filename of an initial separation matrix. If specified, a matrix in INITW_FILENAME is used as an initial separation matrix. Otherwise, initial separation matrix is estimated from the geometrical relationship or pre-measured TF according to TF_CONJ.
 *
 * @parameter_name SS_METHOD
 * @parameter_type string
 * @parameter_value ADAPTIVE
 * @parameter_list FIX:LC_MYU:ADAPTIVE
 * @parameter_description The calculation method for SS step size parameter corresponding to the blind separation part. "FIX" uses a fixed step size,"LC_MYU" uses the same value as LC_MYU, and "ADAPTIVE" adaptively estimates an optimal step size. [default: ADAPTIVE]
 *
 * @parameter_name SS_SCAL
 * @parameter_type float
 * @parameter_value 1.0
 * @parameter_description Scaling factor for SS step size. [default: 1.0]
 *
 * @parameter_name SS_MYU
 * @parameter_type float
 * @parameter_value 0.001
 * @parameter_valid SS_METHOD=FIX
 * @parameter_description SS step size value. [default 0.001]
 *
 * @parameter_name NOISE_FLOOR
 * @parameter_type float
 * @parameter_value 0.0
 * @parameter_description Noise floor value. [default 0.0]
 *
 * @parameter_name LC_CONST
 * @parameter_type string
 * @parameter_value FULL
 * @parameter_list FULL:DIAG
 * @parameter_description The calculation method for geometric constraints. "FULL" uses all elements of a matrix, and "DIAG" only uses diagonal parts. [default: FULL]
 *
 * @parameter_name LC_METHOD
 * @parameter_type string
 * @parameter_value ADAPTIVE
 * @parameter_list FIX:ADAPTIVE
 * @parameter_description The calculation method for LC step size corresponding to geometric constraints. "FIX" uses a fixed value, and "Adaptive" adaptively estimates an optimal step size. [default: ADAPTIVE]
 *
 * @parameter_name LC_MYU
 * @parameter_type float
 * @parameter_value 0.001
 * @parameter_valid LC_METHOD=FIX
 * @parameter_description LC step size value. [default 0.001]
 *
 * @parameter_name UPDATE_METHOD_TF_CONJ
 * @parameter_type string
 * @parameter_value POS
 * @parameter_list ID:POS
 * @parameter_description Switching method of TF_CONJ data. [default: POS]
 *
 * @parameter_name UPDATE_METHOD_W
 * @parameter_type string
 * @parameter_value ID
 * @parameter_list ID:POS:ID_POS
 * @parameter_description Switching method of separation matrix, W. [default: ID]
 *
 * @parameter_name COMPARE_MODE
 * @parameter_type string
 * @parameter_value DEG
 * @parameter_list DEG:TFINDEX
 * @parameter_description The comparison algorithm of Sources. DEG uses inner product, and TFINDEX uses index search (faster).
 *
 * @parameter_name UPDATE_SEARCH_AZIMUTH
 * @parameter_type float
 * @parameter_value 
 * @parameter_valid COMPARE_MODE=DEG
 * @parameter_description Angle of azimuth allowance to serch TF or separation matrix in degree. [default: ]
 *
 * @parameter_name UPDATE_SEARCH_ELEVATION
 * @parameter_type float
 * @parameter_value 
 * @parameter_valid COMPARE_MODE=DEG
 * @parameter_description Angle of elevation allowance to serch TF or separation matrix in degree. [default: ]
 *
 * @parameter_name UPDATE_ACCEPT_ANGLE
 * @parameter_type float
 * @parameter_value 5.0
 * @parameter_valid COMPARE_MODE=DEG
 * @parameter_description Angle allowance to switch separation matrix in degree. available when when UPDATE_METHOD_W is POS or ID_POS. [default: 5.0]
 *
 * @parameter_name UPDATE_ACCEPT_TFINDEX_DISTANCE
 * @parameter_type int
 * @parameter_value 1
 * @parameter_valid COMPARE_MODE=TFINDEX
 * @parameter_description Distance allowance to switch separation matrix in TFIndex. available when when UPDATE_METHOD_W is POS or ID_POS. [default: 1]
 *
 * @parameter_name CHECK_SOURCE_DISTANCE
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_valid COMPARE_MODE=TFINDEX
 * @parameter_description Check flag for confirm source distace when using TFIndex. [default: false]
 *
 * @parameter_name EXPORT_W
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_valid COMPARE_MODE=DEG
 * @parameter_description Separation matrix W is exported if true. [default: false]
 *
 * @parameter_name EXPORT_W_FILENAME
 * @parameter_type string
 * @parameter_valid EXPORT_W=true
 * @parameter_description The filename to export W.
 *
 * @parameter_name UPDATE
 * @parameter_type string
 * @parameter_value STEP
 * @parameter_list STEP:TOTAL
 * @parameter_description The update method of separation matrix. "STEP" updates W sequentially, i.e., based on SS and then on LC cost. "TOTAL" updates W based on an integrated value of SS and LC cost [default: STEP]
 *
 
 END*/

class GHDSS : public BufferedNode {
    int input_frameID;
    int input_srcID;
    int outputID;
    int nb_channels ;
    int nb_source ;
    int length ;
    int advance ;
    int sampling_rate ;
    
    string tf_conj ;            // change from "sTF_CONJ"
    string tf_conj_filename ;	// change from "filename_TF"
    string mic_filename ;       // change from"filename"
    string mic_pos_shift ;      // change from "sTmp"
    bool fixed_noise;	        // change from m_bASIMO
    string fixed_noise_filename ;	//change from filename_ASIMO
    string initw_filename ;	// change from filename_SM
    float speed_sound;
    //2010.08.26
  int lower_bound ;
  int upper_bound ;
    int enum_window ;
    
    int iFlgTf ;	       // TF作成Flag(0:CalcFree,1:Read data file)
    int iFlgW ;                // Flag for SeparateMatrix(0:Calc,1:Read)
    bool m_bFlgIni ;           // Initial process flag
    int m_iMicShift ;
    string sTmp ;
    int iss_method ;
    float ss_myu ;
    float noise_floor ;
    int ilc_const ;
    int ilc_method ;
    float lc_myu ;
    float ss_scal ;
        
    CGHDSS_exec m_cGHDSSExec;
    MICARY_ComplexSpecArray m_csaInputSpec ; // Input spectra
    MICARY_ComplexSpecArray m_csaOutputSpec ;// Output spectra
    MICARY_PrmFrame m_psPrmFrame ; // Frame parameter
    MICARY_LocationArray locaSrcPos ;	// Source position array 
	MICARY_LocationArray locaInputSrcPos;	// 2011/01/13 Source position array for copy
    
    float w_myu ;
    float threshold ;
    int iupdate ;
    
    int iUpdate_TF_CONJ,iUpdate_W, iSrc_Compare_mode ,iUpdate_tfindex_distance;
    float fUpdate_angle ;
	float fsearch_azimuth;   //Add by Morio 2013/01/20
	float fsearch_elevation; //Add by Morio 2013/01/20
    bool m_bExpW , bCheck_distance;
    string filename_expW ;  
    
public:
    GHDSS(string nodeName, ParameterSet params)
	: BufferedNode(nodeName, params)
	{
	    // In the constructor ,we create both the inputs and outputs.
	    input_frameID = addInput("INPUT_FRAMES");
	    input_srcID = addInput("INPUT_SOURCES") ;
	    outputID = addOutput("OUTPUT");
	    length = dereference_cast<int>(parameters.get("LENGTH"));
	    if(length >0){
	    }
	    else{
		throw (new NodeException(NULL,"GHDSS:LENGTH is not invalid value.",__FILE__,__LINE__)) ;
	    }
	    sampling_rate = dereference_cast<int>(parameters.get("SAMPLING_RATE"));
	    if(sampling_rate >0){
	    }
	    else{
		throw (new NodeException(NULL,"GHDSS:SAMPLING_RATE is not invalid value.",__FILE__,__LINE__)) ;
	    }
	    advance = dereference_cast<int>(parameters.get("ADVANCE"));
	    

		//Chaged by Morio 2013/01/20 Calcはつかわない
	    //tf_conj = object_cast<string>(parameters.get("TF_CONJ"));
		tf_conj = "DATABASE";

	    if(tf_conj == "DATABASE"){
		
			if(parameters.exist("TF_CONJ_FILENAME") == true){	
				tf_conj_filename = object_cast<string>(parameters.get("TF_CONJ_FILENAME")) ;
				iFlgTf = 1 ;
			}
			
			else{
				throw (new NodeException(NULL,"GHDSS:TF_CONJ_FILENAME is not set.",__FILE__,__LINE__)) ;
			}
	    }
	  //  else{
			//tf_conj_filename = "" ;
			//if(parameters.exist("MIC_FILENAME") == true){
			//	mic_filename = object_cast<string>(parameters.get("MIC_FILENAME")) ;
			//}
			//else{
			//	throw (new NodeException(NULL,"GHDSS:MIC_FILENAME is not set.",__FILE__,__LINE__)) ;
			//}
		
			//iFlgTf = 0 ;
	  //  }

		//Delete by Morio 2013/01/20 CALCは使わないので削除
	    // mis position shift

	 //   if(parameters.exist("MIC_POS_SHIFT") == true)
		//{
		//	mic_pos_shift = object_cast<string>(parameters.get("MIC_POS_SHIFT")) ;
		//	if(mic_pos_shift == "SHIFT"){
		//		m_iMicShift = 0 ;
		//	}
		//	else if(mic_pos_shift == "FIX"){
		//		m_iMicShift = 1 ;
		//	}
		//	else
		//	{
		//		throw (new NodeException(NULL,"GHDSS: MIC_POS_SHIFT should be SHIFT or FIX.",__FILE__,__LINE__)) ;
		//    
		//	}
	 //   }
	 //   else{
		//	throw (new NodeException(NULL,"GHDSS: MIC_POS_SHIFT should be SHIFT or FIX.",__FILE__,__LINE__)) ; 
	 //   }


	//Changed by Morio 2013/01/23　GHDSSでのFixed Noiseは廃止

		fixed_noise=false;
		fixed_noise_filename ="" ;
	 //   if(parameters.exist("FIXED_NOISE") == true){
		//	fixed_noise = dereference_cast<bool>(parameters.get("FIXED_NOISE")) ;
	 //   }
	 //   if(fixed_noise == true){
		//	if(parameters.exist("FIXED_NOISE_FILENAME") == true){
		//		fixed_noise_filename = object_cast<string>(parameters.get("FIXED_NOISE_FILENAME")) ;
		//	}
		//	else{
		//		throw (new NodeException(NULL,"GHDSS:FIXED_NOISE_FILENAME is not set.",__FILE__,__LINE__)) ;
		//	}
	 //   }
	 //   else{
		//fixed_noise_filename ="" ;
	 //   }
	    	    
	    if(parameters.exist("INITW_FILENAME") == true){
		initw_filename = object_cast<string>(parameters.get("INITW_FILENAME")) ;
		iFlgW = 1 ;
	    }
	    else{
		initw_filename = "" ;
		iFlgW = 0 ;
	    }

		//Delete by Morio 2013/01/20 CLAC廃止対応
	    //speed_sound = dereference_cast<float>(parameters.get("SPEED_OF_SOUND"));
	    
		// 2010.08.26
	    lower_bound = dereference_cast<int>(parameters.get("LOWER_BOUND_FREQUENCY"));
	    upper_bound = dereference_cast<int>(parameters.get("UPPER_BOUND_FREQUENCY"));
	   	   
	   /* 
	   if(parameters.exist("STEPSIZE")){
		stepsize = dereference_cast<float>(parameters.get("STEPSIZE"));
	    }
	    else{
		stepsize = 0.01 ;
	    }
	   */
	    enum_window = 1 ;
	    
	    // GHDSS parameter
	    if(parameters.exist("SS_METHOD")){
		sTmp = object_cast<string>(parameters.get("SS_METHOD")) ;
		if(sTmp == "FIX"){
		    iss_method = _SS_METHOD_FIX ;
		}
		else if(sTmp == "LC_MYU"){
		    iss_method = _SS_METHOD_LCMYU ;
		}
		else if(sTmp == "ADAPTIVE"){
		    iss_method = _SS_METHOD_ADAPTIVE ;
		}
	    }
	    else{
		throw (new NodeException(NULL,"GHDSS: SS_METHOD should be FIX,LC_METHOD or ADAPTIVE",__FILE__,__LINE__)) ;
		
	    }
	if(parameters.exist("SS_SCAL")){
			ss_scal = dereference_cast<float>(parameters.get("SS_SCAL"));
	}
	else{
		ss_scal = 1.0 ;
	}

	    if(parameters.exist("SS_MYU")){
		// 2010/09/15 changed by N.Tanaka
		// Windows版のFlowdesignerでは0.00の精度までしか入力できない
		//ss_myu = dereference_cast<float>(parameters.get("SS_MYU"));
		/*
		#ifndef WIN32
		ss_myu = dereference_cast<float>(parameters.get("SS_MYU"));
		#else
		sTmp = object_cast<string>(parameters.get("SS_MYU"));
		ss_myu = atof(sTmp.c_str());
		#endif
		*/
		ss_myu = dereference_cast<float>(parameters.get("SS_MYU"));
	    }
	    else{
		ss_myu = 0.001 ;
	    }
	    if(parameters.exist("NOISE_FLOOR")){
		noise_floor = dereference_cast<float>(parameters.get("NOISE_FLOOR"));
	    }
	    else{
		noise_floor = 0.0 ;
	    }
	    if(parameters.exist("LC_CONST")){
		sTmp = object_cast<string>(parameters.get("LC_CONST")) ;
		if(sTmp == "FULL"){
		    ilc_const = _LC_CONST_FULL ;
		}
		else if(sTmp == "DIAG"){
		    ilc_const = _LC_CONST_DIAG ;
		}
	    } 
	    else{
		throw (new NodeException(NULL,"GHDSS: LC_CONST should be FULL or DIAG",__FILE__,__LINE__)) ;
		
	    }
	    if(parameters.exist("LC_METHOD")){
		sTmp = object_cast<string>(parameters.get("LC_METHOD")) ;
		if(sTmp == "FIX"){
		    ilc_method = _LC_METHOD_FIX ;
		}
		else if(sTmp == "ADAPTIVE"){
		    ilc_method = _LC_METHOD_ADAPTIVE ;
		}
	    } 
	    else{
		throw (new NodeException(NULL,"GHDSS: LC_METHOD should be FIX or ADAPTIVE",__FILE__,__LINE__)) ;
	    }
	    if(parameters.exist("LC_MYU")){
		// 2010/09/15 changed by N.Tanaka
		// Windows版のFlowdesignerでは0.00の精度までしか入力できない
		// lc_myu = dereference_cast<float>(parameters.get("LC_MYU"));
		/*
		#ifndef WIN32
		lc_myu = dereference_cast<float>(parameters.get("LC_MYU"));
		#else
		sTmp = object_cast<string>(parameters.get("LC_MYU"));
		lc_myu = atof(sTmp.c_str());
		#endif
		*/
		lc_myu = dereference_cast<float>(parameters.get("LC_MYU"));
	    }
	    else{
		lc_myu = 0.001 ;
	    }
	    
	    AllocComplexSpecArray(&m_csaInputSpec,0,0,0) ;
	    AllocComplexSpecArray(&m_csaOutputSpec,0,0,0) ;
	    LocationArrayAlloc(&locaSrcPos, 0);	// 2012/01/19 added
	    LocationArrayAlloc(&locaInputSrcPos, 0);	// 2012/01/19 added
	    
	    m_cGHDSSExec.SetPrmFrame(&m_psPrmFrame,sampling_rate,length,advance,enum_window) ;
	    LocationArrayAlloc_r(&locaSrcPos, 0, __MICARY_def_MAX_NSRC+1);	// 2012/01/13 changed
	    LocationArrayAlloc_r(&locaInputSrcPos, 0, __MICARY_def_MAX_NSRC+1);	// 2012/01/13 changed
	    m_bFlgIni = true ;
	    m_cGHDSSExec.SetPrmGHDSS(iss_method,ss_myu,ss_scal,noise_floor,ilc_const,ilc_method,lc_myu) ;
	    
	    
	    // add inOrder
	    inOrder = true ;
	    
	    /*
	    if(parameters.exist("W_MYU")){
		w_myu = dereference_cast<float>(parameters.get("W_MYU"));
	    }
	    else{
		w_myu = 0.001 ;
	    }
	    */
	    w_myu = 0.0;
	    if(parameters.exist("UPDATE")){
		sTmp = object_cast<string>(parameters.get("UPDATE")) ;
		if(sTmp == "STEP"){
		    iupdate = 0 ;
		}
		else if(sTmp == "TOTAL"){
		    iupdate = 1 ;
		}
	    } 
	    else{
		throw (new NodeException(NULL,"GHDSS:UPDATE should be STEP or TOTAL",__FILE__,__LINE__)) ;
		
	    }
	    
	    threshold = 1000 ;
	    
	    // add parameter2
	    m_cGHDSSExec.SetPrmGHDSS2(w_myu,threshold,iupdate) ;
	    
	    // UPDATE parameter
	    if(parameters.exist("UPDATE_METHOD_TF_CONJ")){
		sTmp = object_cast<string>(parameters.get("UPDATE_METHOD_TF_CONJ")) ;
		if(sTmp == "ID"){
		    iUpdate_TF_CONJ = __UPDATE_METHOD_ID ;
		}
		else if(sTmp == "POS"){
		    iUpdate_TF_CONJ = __UPDATE_METHOD_POS ;
		}
	    }
	    else{
		throw (new NodeException(NULL,"GHDSS:UPDATE_TF_CONJ should be ID or POS.",__FILE__,__LINE__)) ;
		
	    }
	    
	    if(parameters.exist("UPDATE_METHOD_W")){
		sTmp = object_cast<string>(parameters.get("UPDATE_METHOD_W")) ;
		if(sTmp == "ID"){
		    iUpdate_W = __UPDATE_METHOD_ID ;
		}
		else if(sTmp == "POS"){
		    iUpdate_W = __UPDATE_METHOD_POS ;
		}
		else if(sTmp == "ID_POS"){
		    iUpdate_W = __UPDATE_METHOD_ID_POS ;
		}
	    }
	    else{
		throw (new NodeException(NULL,"GHDSS_ASOCRA:UPDATE_W should be ID,POS or ID_POS.",__FILE__,__LINE__)) ;
		
	    }
    
	    // select src compare mode	//Add by Morio 2013/06/19
		// DEG: ordinal, TFINDEX: index search for faster
		string cmpmode = object_cast<String>(parameters.get("COMPARE_MODE"));
		if(cmpmode == "DEG"){
		  iSrc_Compare_mode = __COMPARE_MODE_DEG;
		}
		else if(cmpmode == "TFINDEX"){
		  iSrc_Compare_mode = __COMPARE_MODE_TFINDEX;
		  iFlgW = 0 ;//InitW読み込まない
		  initw_filename = "" ;
		}
		else{
		  throw new NodeException(NULL, "Unknown COMPARE_MODE. This must be DEG/TFINDEX",
		                          __FILE__, __LINE__);
		}
		
		if(parameters.exist("UPDATE_SEARCH_AZIMUTH")){
			fsearch_azimuth = dereference_cast<float>(parameters.get("UPDATE_SEARCH_AZIMUTH"));
	    }
	    else{
			fsearch_azimuth = 180.0 ; //設定されていない場合は全サーチ
	    }
		if(parameters.exist("UPDATE_SEARCH_ELEVATION")){
			fsearch_elevation = dereference_cast<float>(parameters.get("UPDATE_SEARCH_ELEVATION"));
	    }
	    else{
			fsearch_elevation = 90.0 ;//設定されていない場合は全サーチ
	    }
	    if(parameters.exist("UPDATE_ACCEPT_ANGLE")){
			fUpdate_angle = dereference_cast<float>(parameters.get("UPDATE_ACCEPT_ANGLE"));
	    }
	    else{
			fUpdate_angle = 5.0 ;
	    }
		if(parameters.exist("UPDATE_ACCEPT_TFINDEX_DISTANCE")){
			iUpdate_tfindex_distance = dereference_cast<int>(parameters.get("UPDATE_ACCEPT_TFINDEX_DISTANCE"));
	    }
	    else{
			iUpdate_tfindex_distance = 1 ;
	    }
	    if(parameters.exist("CHECK_SOURCE_DISTANCE")){
			bCheck_distance = dereference_cast<bool>(parameters.get("CHECK_SOURCE_DISTANCE"));
	    }
	    else{
			bCheck_distance = false ;
	    }
		m_cGHDSSExec.SetPrmUpdate(iUpdate_TF_CONJ,iUpdate_W,iSrc_Compare_mode, fUpdate_angle, iUpdate_tfindex_distance,fsearch_azimuth,fsearch_elevation, bCheck_distance) ;
	    
	    if(parameters.exist("EXPORT_W") == true){
			m_bExpW = dereference_cast<bool>(parameters.get("EXPORT_W")) ;
			if (iSrc_Compare_mode==__COMPARE_MODE_TFINDEX){
				m_bExpW=false;//ExportW未対応
			}
	    }
	    
	    if(m_bExpW == true){
		if(parameters.exist("EXPORT_W_FILENAME") == true){	
		    filename_expW = object_cast<string>(parameters.get("EXPORT_W_FILENAME")) ;
		}
	    }
	    else{
		filename_expW = "" ;
	    }
	    
	   //ファイルフォーマットの設定
	   m_cGHDSSExec.SetTF_CONJ_FileFormat(__FILE_FORMAT_HARK) ;
	   m_cGHDSSExec.SetInitW_FileFormat(__FILE_FORMAT_HARK) ;
	   m_cGHDSSExec.SetExportW_FileFormat(__FILE_FORMAT_HARK) ;
	   m_cGHDSSExec.SetMicPos_FileFormat(__FILE_FORMAT_HARK) ;
	   	// Changed by Morio 2013/01/23　GHDSSでのFixed Noiseは廃止
	   //m_cGHDSSExec.SetFixedNoisePos_FileFormat(__FILE_FORMAT_HARK) ;

	   // mic pos shift
	   m_cGHDSSExec.SetiMicPosShift(m_iMicShift) ;
		
	   if(m_cGHDSSExec.Initialize(mic_filename.c_str(),speed_sound,iFlgTf,tf_conj_filename.c_str(),fixed_noise,fixed_noise_filename.c_str(),iFlgW,initw_filename.c_str()) == 0){
		    throw(new NodeException(NULL, "Exception caught in Initialize calculate[GHDSS]", __FILE__, __LINE__));
		    return ;
		}
	   m_cGHDSSExec.Initialize_GHDSS(0,0,0,0) ;
	   //cout << "TF = " << iFlgTf << ",INITW = " << iFlgW << ",FixedNoise = " <<fixed_noise << endl ; 
		//cout << "SSMethod = " << iss_method << "LC_CONST = "<< ilc_const << "LC_Method = " << ilc_method << endl ;
	    
	}
    
    // destructor ,we free inputs and outputs
    ~GHDSS()
	{
	    if(m_bExpW){
		m_cGHDSSExec.ExportAllUpdateW(&m_psPrmFrame,filename_expW.c_str()) ;
	    }
	    FreeComplexSpecArray(&m_csaInputSpec) ;
	    FreeComplexSpecArray(&m_csaOutputSpec) ;
	    LocationArrayFree(&locaSrcPos) ;
	    LocationArrayFree(&locaInputSrcPos) ; // 2012/01/13 added
	    
	}
    
    // This is the main method for the node,it is called from the BufferedNode class each time a result needs to be calculated.
    void calculate(int output_id, int count, Buffer &out)
	{
//cout << "count:" << count << endl;
		
	    int iNRowCh,iNColFreq,iNVecSrc ;
	    int i,k ;
	    complex<float> cTemp ;
	    ObjectRef orOutputTemp ,orOutputTempV ;
	    Map<int, ObjectRef>::iterator pmTemp ;
	    MICARY_LocationArray locaSrcPosTmp ;
	    int iFlagChange ;
		
		// 2012/01/13 added
		int iNFixedNoise;
			
	    // Input(Matrix:frames)
	    ObjectRef orInputTemp = getInput(input_frameID,count) ;
	    Matrix<complex<float> > &mInputValue = object_cast<Matrix<complex<float> > > (orInputTemp) ;
	    
	    iNRowCh = mInputValue.nrows() ;	//チャンネル数
	    iNColFreq = mInputValue.ncols() ;	//周波数ライン数
	    	    
	    // Input(Vector:sources)
	    RCPtr<Vector<ObjectRef> > pSources = getInput(input_srcID, count) ;
	    const Vector<ObjectRef>& sources = *pSources ;
	    iNVecSrc = sources.size() ;	//音源数
/*	    
cout << "---------input src----------"	 <<endl;
for (int iq = 0; iq < iNVecSrc; iq++){
RCPtr<Source> src = sources[iq] ;
cout << "X " << src->tfindex[0] << " " << src->x[0] <<endl;
cout << "Y " << src->tfindex[1] << " " << src->x[1] <<endl;
cout << "Z " << src->tfindex[2] << " " << src->x[2]  <<endl;
} 
*/
	    // Output(MAP)    
	    orOutputTemp = ObjectRef(new Map<int,ObjectRef>) ;
	    Map<int,ObjectRef> &mpOutputValue = object_cast<Map<int,ObjectRef> > (orOutputTemp) ;
	    out[count] = orOutputTemp ;
	    mpOutputValue.clear() ;
		
		// 2012/01/12 以下処理手順の見直し
		// 入力音源数が0以上の場合のみ処理を続行
		if (iNVecSrc == 0 && !fixed_noise){
			return;
		}
		
		// 2012/10/25 最大音源数，マイク数，周波数ライン数を越えた場合の例外処理
		if(iNVecSrc > __MICARY_def_MAX_NSRC){
			    std::cout << "Error: iNVecSrc > __MICARY_def_MAX_NSRC " << std::endl ;
			    throw(new NodeException(NULL, "Exception caught in calculate[GHDSS]", __FILE__, __LINE__));
			    return ;
		}
		
		if(iNRowCh > __MICARY_def_MAX_NCH){
			    std::cout << "Error: iNRowCh > __MICARY_def_MAX_NSRC " << std::endl ;
			    throw(new NodeException(NULL, "Exception caught in calculate[GHDSS]", __FILE__, __LINE__));
			    return ;
		}
		
		if(iNColFreq > __MICARY_def_MAX_NFL){
			    std::cout << "Error: iNColFreq > __MICARY_def_MAX_NFL " << std::endl ;
			    throw(new NodeException(NULL, "Exception caught in calculate[GHDSS]", __FILE__, __LINE__));
			    return ;
		}

		// 音源位置配列の作成・コピー（ソート）
			// 計算用音源座標Arrayの準備
	    if(locaInputSrcPos.ha_numLocation != iNVecSrc){
			LocationArrayAlloc_r(&locaInputSrcPos,iNVecSrc,__MICARY_def_MAX_NSRC+1) ;
	    }
		// 座標,TFIndexコピー
			
		for (i=0 ; i < iNVecSrc ; i++){
			RCPtr<Source> src = sources[i] ;
			locaInputSrcPos.micary_Location[i].ha_fX = src->x[0] ;
			locaInputSrcPos.micary_Location[i].ha_fY = src->x[1] ;
			locaInputSrcPos.micary_Location[i].ha_fZ = src->x[2] ;
			locaInputSrcPos.micary_Location[i].micary_LocationFlag = 7 ;
			locaInputSrcPos.ha_idLocation[i] = src -> id ;
			locaInputSrcPos.micary_Location[i].ha_idPos = src -> id ;
			// test
			locaInputSrcPos.micary_Location[i].ha_idDir = 0;
			
			//角度算出はDEGのみ
			if(iSrc_Compare_mode == __COMPARE_MODE_DEG){
				//方位角
				locaInputSrcPos.micary_Location[i].ha_fTheta = 180.0/acos(-1.0)*atan2((double)src->x[1],(double)src->x[0]) ;	
				//仰角
				locaInputSrcPos.micary_Location[i].ha_fPhi = 180.0/acos(-1.0)*atan2((double)src->x[2],(double)sqrt(src->x[0]*src->x[0]+src->x[1]*src->x[1])) ;//任意座標用
			}
			//TFIndexのコピー
			for(int ii = 0; ii < 3; ii++ ){
				locaInputSrcPos.micary_Location[i].tfindex[ii] = src->tfindex[ii];
				locaInputSrcPos.micary_Location[i].tfindex_min[ii] = src->tfindex_min[ii];
				locaInputSrcPos.micary_Location[i].tfindex_max[ii] = src->tfindex_max[ii];

			}
				
		}
/*			
		cout << "---------input locaInputSrcPos----------"	 <<endl;
		for (int i=0; i<iNVecSrc; i++){
cout << "X " << locaInputSrcPos.micary_Location[i].tfindex[0] << " " << locaInputSrcPos.micary_Location[i].ha_fX <<endl;
cout << "Y " << locaInputSrcPos.micary_Location[i].tfindex[1] << " " << locaInputSrcPos.micary_Location[i].ha_fY <<endl;
cout << "Z " << locaInputSrcPos.micary_Location[i].tfindex[2] << " " << locaInputSrcPos.micary_Location[i].ha_fZ <<endl;
}
*/			

			// ソート
		sortLocationArray(&locaInputSrcPos, &locaSrcPos);
		
	    iNVecSrc = locaSrcPos.ha_numLocation ;
		
		// 音源が検出されていない場合
		if (iNVecSrc == 0){
			return;
		}
		
		//計算用Spec Arrayの準備
	    if(m_csaInputSpec.micary_cmpaSpec.ha_numValue != iNRowCh * iNColFreq)
	    {
			//FreeComplexSpecArray(&m_csaInputSpec) ;
			AllocComplexSpecArray_r(&m_csaInputSpec,(HA_Num)iNRowCh,0,iNColFreq-1,
									__MICARY_def_MAX_NCH, __MICARY_def_MAX_NFL) ;
			m_csaInputSpec.ha_iLowerFreq = (int)((float)lower_bound*length / sampling_rate) ;
			m_csaInputSpec.ha_iUpperFreq = (int)((float)upper_bound*length / sampling_rate) ;
	    }
		
	    // 入力信号のコピー Matrix->Array(frames)
	    for(i = 0 ; i < iNRowCh ; i++){
			for(k = 0 ; k < iNColFreq ; k++){
			    cTemp = mInputValue(i,k) ;
			    m_csaInputSpec.micary_cmpaSpec.ha_cmpValue[i*iNColFreq+k].re = cTemp.real() ;
		    	m_csaInputSpec.micary_cmpaSpec.ha_cmpValue[i*iNColFreq+k].im = cTemp.imag() ;
			}
	    }
		
		// 初期化済み?
		if(m_bFlgIni == true){
		    m_cGHDSSExec.Initialize_Change(iNRowCh,iNVecSrc,0,iNColFreq-1) ;
		    m_cGHDSSExec.Initialize_GHDSS(iNRowCh,iNVecSrc,0,iNColFreq-1) ;
		    if(m_cGHDSSExec.AssignSrcPos(locaSrcPos) == 0){
				cout << "Fail to assign source position." << endl ;
				throw(new NodeException(NULL, "Exception caught in Source Position assign[GHDSS]", __FILE__, __LINE__));
				return ;
		    }
		    
		    if(m_cGHDSSExec.InitTFMatrix(&m_psPrmFrame) == 0){
			std:: cout << "Error Initialize TF matrix." << std:: endl;
			throw(new NodeException(NULL, "Exception caught in InitTFMatrix [GHDSS]", __FILE__, __LINE__));
			return ;
		    }
		    
		    if(m_cGHDSSExec.InitSepMatrix(&m_psPrmFrame) == 0){
			std:: cout << "Error Initialize Separate matrix." << std:: endl;
			throw(new NodeException(NULL, "Exception caught in InitSeparate Matrix calculate[GHDSS]", __FILE__, __LINE__));
			return ;
		    }
		    
		    if(m_cGHDSSExec.InitUpdateW() == 0){
			std::cout<< "Error Initialize Update W  matrix." << std:: endl;
			throw(new NodeException(NULL, "Exception caught in InitUpdateW calculate[GHDSS]", __FILE__, __LINE__));
			return ;
		    }

		    if(m_bExpW == true){
			if(m_cGHDSSExec.InitExportW(filename_expW.c_str(),&m_psPrmFrame)== 0){
			    std::cout << "Error Initialize Update W matrix." << std::endl ;
			    throw(new NodeException(NULL, "Exception caught in InitExportW calculate[GHDSS]", __FILE__, __LINE__));
			    return ;
			}
		    }
		    m_bFlgIni = false ;
		}
		else{
				    
		    // TF_CONJ, Wの更新・引継
		    iFlagChange = m_cGHDSSExec.ChangeSrcPos_TF_CONJ_W(locaSrcPos,m_psPrmFrame) ;
		    
		    if(iFlagChange != 0){	//音源位置または数の変更
		        //Rxx,Ryyの初期化
			  m_cGHDSSExec.Initialize_GHDSS(iNRowCh,iNVecSrc,0,iNColFreq-1) ;
		    } 
		    
		    
		}
		
		
	
		if(m_csaOutputSpec.ha_numCh != iNVecSrc || 
	       (m_csaOutputSpec.ha_iUpperFreq-m_csaOutputSpec.ha_iLowerFreq +1) != iNColFreq )
	    	{
			//FreeComplexSpecArray(&m_csaOutputSpec) ;
			AllocComplexSpecArray_r(&m_csaOutputSpec,(HA_Num)iNVecSrc,0,iNColFreq-1,
									__MICARY_def_MAX_NSRC+1, __MICARY_def_MAX_NFL) ;
	    	}
		
		// 2010.8.26
		// 初期化
		AllocComplexSpecArray_r(&m_csaOutputSpec,(HA_Num)iNVecSrc,0,iNColFreq-1,
								__MICARY_def_MAX_NSRC+1, __MICARY_def_MAX_NFL) ;
	        

		//GHDSSの実行
		if(m_cGHDSSExec.ExecuteGHDSS(m_csaInputSpec,&m_csaOutputSpec)== 0){
		    std::cout << "Fail to execute GHDSS."<< std::endl ;
		    throw(new NodeException(NULL, "Exception caught in GHDSS calculate[GHDSS]", __FILE__, __LINE__));
		    return ;
		}
	       
		// Set UpdateW matrix
		m_cGHDSSExec.SetUpdateW() ;	
		// export W
		//Changed by Morio 2013/01/21 ファイル保存はここではやらない。デストラクタで保存
		m_cGHDSSExec.AddUpdateW(&m_psPrmFrame,filename_expW.c_str(),false) ;
		//m_cGHDSSExec.AddUpdateW(&m_psPrmFrame,filename_expW.c_str(),m_bExpW) ;
	       		
       	for(i = 0 ; i < m_csaOutputSpec.ha_numCh ; i++){
		    int id = locaSrcPos.ha_idLocation[i] ;
		    orOutputTempV = ObjectRef(Vector<complex<float> >::alloc(iNColFreq)) ;
		    Vector<complex<float> > &vcfOutputValue = object_cast<Vector<complex<float> > > (orOutputTempV) ;
		    
		    for(k = 0 ; k < iNColFreq ; k ++){
			vcfOutputValue[k] = complex<float>(m_csaOutputSpec.micary_cmpaSpec.ha_cmpValue[i*iNColFreq+k].re,
							   m_csaOutputSpec.micary_cmpaSpec.ha_cmpValue[i*iNColFreq+k].im) ;
		    }
		    mpOutputValue.insert(make_pair(id,orOutputTempV)) ;   
		}
		
	}// End Calculate

};

int sortLocationArray(MICARY_LocationArray *locaSrc, MICARY_LocationArray *locaDest)
{
	int i,k;
	
	if (!locaSrc || !locaDest){
		return 0;
	}
	if(locaDest->ha_numLocation != locaSrc->ha_numLocation){
		LocationArrayAlloc_r(locaDest, locaSrc->ha_numLocation, __MICARY_def_MAX_NSRC+1) ;
	}
	// IDをコピー
	for (i=0 ; i < locaSrc->ha_numLocation ; i++){
		locaDest->ha_idLocation[i] = locaSrc->ha_idLocation[i];
	}
	// コピーしたIDをソート
	qsort(locaDest->ha_idLocation, locaDest->ha_numLocation, sizeof(HA_Id), HA_Id_cmp);
	// ソート結果に応じた座標をコピー
	for (i = 0 ; i < locaDest->ha_numLocation ; i++){
		for (k = 0 ; k < locaSrc->ha_numLocation ; k++){
			if (locaDest->ha_idLocation[i] == locaSrc->ha_idLocation[k]){
				locaDest->micary_Location[i].ha_fX = locaSrc->micary_Location[k].ha_fX ;
				locaDest->micary_Location[i].ha_fY = locaSrc->micary_Location[k].ha_fY ;
				locaDest->micary_Location[i].ha_fZ = locaSrc->micary_Location[k].ha_fZ ;
				locaDest->micary_Location[i].ha_fPhi = locaSrc->micary_Location[k].ha_fPhi ;
				locaDest->micary_Location[i].ha_fTheta = locaSrc->micary_Location[k].ha_fTheta ;
				locaDest->micary_Location[i].micary_LocationFlag = locaSrc->micary_Location[k].micary_LocationFlag ;
				locaDest->micary_Location[i].ha_idPos = locaSrc->micary_Location[k].ha_idPos ;
				locaDest->micary_Location[i].ha_idDir = locaSrc->micary_Location[k].ha_idDir ;
				
				for (int ii=0 ; ii < 3 ; ii++){
					locaDest->micary_Location[i].tfindex[ii] = locaSrc->micary_Location[k].tfindex[ii] ;
					locaDest->micary_Location[i].tfindex_min[ii] = locaSrc->micary_Location[k].tfindex_min[ii] ;
					locaDest->micary_Location[i].tfindex_max[ii] = locaSrc->micary_Location[k].tfindex_max[ii] ;
				}			
				break;
			}
		}
	}


	
	return 1;
}

int HA_Id_cmp  (const void *arg0, const void *arg1){
  HA_Id a = *(HA_Id*)arg0;
  HA_Id b = *(HA_Id*)arg1;
  if(a == b)       return 0;
  else if(a > b)   return 1;
  else             return -1;
}

#endif // ENABLE_HARKIO_NH
