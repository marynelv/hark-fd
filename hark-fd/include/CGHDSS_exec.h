// ---------------------------------------------------------------------
/**
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.

 \file CGHDSS_exec.h
 \brief \~English Header file for execute GHDSS.
 \brief \~Japanese GHDSS実行クラスのヘッダ(CGHDSS)
 \author Sachiko Suzuki
 \date 24 Dec 2009
 \version $Id$
*/

// ---------------------------------------------------------------------
#ifndef	CGHDSS_exec_h_defined
#define	CGHDSS_exec_h_defined
// ---------------------------------------------------------------------
///#include "MICARYHA_struct.h"	// ClassView によって追加されました。
///#include "ArrayInit.h"
//#include "ArrayUtil.h"
///#include "MICARY_GHDSS_struct.h"
///#include "GHDSS_SeparateMat.h"
//#include "GHDSS_MathMat2.h"		2011.01.26
//#include "GHDSS_ReadLocationFile.h"
//#include "GHDSS_TFData.h"
// 2014/8/27 changed by N.Tanaka
#ifdef ENABLE_HARKIO_NH
#include "Cexec_NH.h"
#else
#include "Cexec.h"
#endif
//#include "GHDSS_MathMatDouble.h"  2011.01.26

#include <string>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

//#define __MICPOSITION_MOVE_ORIGINALPOINT	(0)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------------------------------------------------------------
/**
 \class CGHDSS_exec
 \brief \~English Class for executing GHDSS process 
 \brief \~Japanese GHDSS実行クラス
*/
class CGHDSS_exec : public Cexec{
private:
	bool m_bIni;
	
	// modified 2008/07/01
	//MICARY_ComplexSpecArray *m_pcsRxx ,*m_pcsRyy ;
	MICARY_dComplexSpecArray *m_pcsRxx ,*m_pcsRyy ;
	MICARY_PrmSeparation_GHDSS m_prmSep ;

	// 発散防止のためのパラメータ
	HA_Float m_fWMyu;
	HA_Float m_fThreshold;
	HA_Integer m_iUpdate;
	
public:
	void SetbIni(bool bIni);
	/**
	 \breif \~Japanese パラメータの設定
	 \param ha_fWmyu \~English Stepsize parameter for update Separate matrix
	 \~Japanese 分離行列更新のためのステップサイズパラメータ
	 \param ha_fThreshold\~English Threshold for update separate matrix
	 \~Japanese 分離行列更新のためのthreshold[dB]
	 \param ha_iUpdate \~English update method
	 \~Japanese 分離行列更新方法(0:段階的，1:統合的)
	*/
	void SetPrmGHDSS2(HA_Float ha_fWmyu,HA_Float ha_fThreshold,HA_Integer ha_iUpdate);

	void Initialize_GHDSS(HA_Integer iNch,HA_Integer iNpos,HA_Integer iLower_freq,HA_Integer iUpper_freq );
	/**
	 \brief \~Japanese コンストラクタ
	*/
	CGHDSS_exec() ;
	/**
	 \brief \~Japanese デストラクタ
	*/
	~CGHDSS_exec() ;

	/**
	 \breif \~Japanese パラメータの設定
	 \param ha_iSSMethod \~English SS stepsize calculation method(0:Fix,1:lc_myu,2:Adaptive)
	 \~Japanese SSステップサイズ計算方法(0:Fix,1:lc_myu,2:Adaptive)
	 \param ha_fSSMyu \~English SS stepsize parameter
	 \~Japanese SSステップサイズパラメータ
	 \param ha_fNoiseFloor \~English Noise floor value  
	 \~Japanese ノイズフロア値
	 \param ha_iLCConst \~English LC_Const calculation method(0:Full,1:Diag)  
	 \~Japanese LCConst計算方法(0:Full,1:Diag)
	 \param ha_iLCMethod \~English LC stepsize calculation method(0:Fix,1:Adaptive)  
	 \~Japanese LCステップサイズパラメータ計算方法(0:Fix,1:Adaptive)
	 \param ha_fLCMyu \~English LC stepsize parameter  
	 \~Japanese LCステップサイズパラメータ
	*/
	void SetPrmGHDSS(HA_Integer ha_iSSMethod,HA_Float ha_fSSMyu ,HA_Float ha_fSSScal,
								HA_Float ha_fNoiseFloor , HA_Integer ha_iLCConst ,
								HA_Integer ha_iLCMethod , HA_Float ha_fLCMyu) ;
	

	/**
	 \breif \~Japanese GHDSSの実行
	 \param pcsInputCSpec \~English Input spectra(complex)
	 \~Japanese 入力スペクトル(complex)
	 \param pcsOutputCSpec \~English Output spectra(complex)
	 \~Japanese 出力スペクトル(complex)
	*/
	int ExecuteGHDSS(MICARY_ComplexSpecArray pcsInputCSpec,MICARY_ComplexSpecArray *pcsOutputCSpec) ;
	

		
};
// ---------------------------------------------------------------------
#endif // CGHDSS_exec_h_defined
