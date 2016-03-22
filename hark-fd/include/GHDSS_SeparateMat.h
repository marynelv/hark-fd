// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_SeparateMat.h
 \brief	\~English Header file for making Separation matrix (GHDSS program).
 \brief	\~Japanese 分離行列のためのヘッダファイル(GHDSS program) 
 \author	Sachiko Suzuki, Aki Yamamoto(2010)
 \date	24 Dec 2009
 \version	$Id: GHDSS_SeparateMat.h 3538 2012-02-29 07:25:35Z tanaka $
*/

// ---------------------------------------------------------------------
#ifndef GHDSS_SeparateMat_h_defined
#define GHDSS_SeparateMat_h_defined
// ---------------------------------------------------------------------

///#include "MICARY_struct.h"
///#include "MICARY_GHDSS_struct.h"
///#include "ArrayInit.h"
//#include "GHDSS_MathMatDouble.h"
//#include "GHDSS_MathMat2.h"

//2011.01.19
#include "CMICARY_Matrix.h"

// 2007/09/04
#ifdef __cplusplus
extern "C"{
#endif

#define _MINIMUM_ZERO_VALUE 0.0000001
#define _OCRA_METHOD_NONE	0
#define	_OCRA_METHOD_FIX	1
#define _OCRA_METHOD_ADAPTIVE	2

#define _SS_METHOD_FIX	0
#define _SS_METHOD_LCMYU	1
#define _SS_METHOD_ADAPTIVE	2

#define _LC_CONST_FULL	0
#define _LC_CONST_DIAG	1

#define _LC_METHOD_FIX	0
#define _LC_METHOD_ADAPTIVE	1

#ifndef M_pi
#define M_pi (3.141592653589793238)
#endif
//
// ---------------------------------------------------------------------
/**
 \brief \~English It calculats TF value in free field .(1:true, 0:false)
 \brief \~Japanese 自由音場でのTFの計算（1:正常終了，0:エラー）
 \param psTFDB \~English It's the TF value database struct pointer.\n
 The elements of this database is (the number of source positions * the number of microphones * the number of frequency lines)
 \~Japanese TF値を格納する配列を含む構造体ポインタ\n
 このデータベースの要素は(音源位置数*マイクロホン位置数*周波数ライン数)
 \param iUpperFreq \~English It's the upper frequency line
 \~Japanese 上限周波数ライン番号
 \param locaSrcPos \~English It's the source position struct
 \~Japanese 音源位置座標
 \param locaMicPos \~English It's the mic position struct
 \~Japanese マイク位置座標
 \param fTemperature \~English It's the speed of sound[m/sec]
 \~Japanese 音速[m/sec]
*/
int CalcTFFreeH(MICARY_TFDatabase * psTFDB , 
					 HA_Integer iUpperFreq ,
					 MICARY_PrmFrame sPrmFrame ,
					 MICARY_LocationArray locaSrcPos ,
					 MICARY_LocationArray locaMicPos ,
					 HA_Float fSpeedSound) ;

// 2012/01/13 added
/**
 \brief \~English It calculats TF value in free field .(1:true, 0:false)
\brief \~Japanese 自由音場でのTFの計算(SrcPosのインデックスを指定)（1:正常終了，0:エラー）
 \param psTFDB \~English It's the TF value database struct pointer.\n
 The elements of this database is (the number of source positions * the number of microphones * the number of frequency lines)
 \~Japanese TF値を格納する配列を含む構造体ポインタ\n
 このデータベースの要素は(音源位置数*マイクロホン位置数*周波数ライン数)
 \param fTemperature \~English It's the speed of sound[m/sec]
 \~Japanese 音速[m/sec]
*/
int CalcTFFreeH2(MICARY_TFDatabase * psTFDB , 
					 MICARY_PrmFrame sPrmFrame ,
					 int idsrc,
					 HA_Float fSpeedSound) ;

/**
 \brief \~English It makes separation matrix.(1:true, 0:false)
 \brief \~Japanese 分離行列の初期値を作成（1:正常終了，0:エラー）
 \param sTFDB \~English It's the Transfer function struct.
 \~Japanese TFデータベース
 \param pcsSeparationMat \~English It's the separated spectrum struct pointer.
 \~Japanese 分離行列
 */
int MakeWMatGHDSS(MICARY_TFDatabase sTFDB,
						MICARY_SeparationMat * psSeparationMat);

// 2012/01/13 added
/**
 \brief \~English It makes separation matrix.(1:true, 0:false)
 \brief \~Japanese 分離行列の初期値を音源インデックス指定で読み込み（1:正常終了，0:エラー）
 \param sTFDB \~English It's the Transfer function struct.
 \~Japanese TFデータベース
 \param pcsSeparationMat \~English It's the separated spectrum struct pointer.
 \~Japanese 分離行列
 \param idsrc \~English Index of the source to load
 \~Japanese 読み込む音源インデックス
 */
int MakeWMatGHDSS2(MICARY_TFDatabase sTFDB,
						MICARY_SeparationMat * psSeparationMat,
						int idsrc);

/**
 \brief \~English It calcurates py ,qy for Ryy(GHDSS).
 \brief \~Japanese Ryy計算用py,qyの算出[GHDSS]
 \param caPY \~English It's calculated PY.
 \~Japanese PY計算結果
 \param caQY \~English It's calculated QY.
 \~Japanese QY計算結果
 \param caY \~English It's separated spectrum of a freq line.
 \~Japanese 分離スペクトル
 \param ha_fSSScal \~English It's scaling factor.
 \~Japanese スケーリングファクター
*/
void _dGetPQY(MICARY_ComplexArray *caPY,MICARY_ComplexArray *caQY,
							 MICARY_ComplexArray caY,HA_Float ha_fSSScal) ;

//void _dGetPQY(MICARY_dComplexArray *caPY,MICARY_dComplexArray *caQY,
//							 MICARY_dComplexArray caY,HA_Float ha_fSSScal) ;

/**
 \brief \~English It calcurates step size parameter for SS(GHDSS).(1:true, 0:false)
 \brief \~Japanese SS用ステップサイズ更新量の計算[GHDSS]（1:正常終了，0:エラー）
 \param caW \~English It's the Separation matrix array of a freq line.
 \~Japanese 分離行列
 \param caA \~English It's the Transfer function matrix array of a freq line.
 \~Japanese 伝達関数行列
 \param caY \~English It's separated spectrum of a freq line .
 \~Japanese 分離スペクトル
 \param caX \~English It's input spectrum of a freq line.
 \~Japanese 入力スペクトル
 \param iSSMethod \~English It's stepsize method(0:Fix, 1:same as LC_myu, 2:Adaptive).
 \~Japanese ステップサイズ更新量の計算方法 (0:固定, 1:LC_myuと共通, 2:Adaptive)
 \param fSSMyu \~English It's step size parameter.
 \~Japanese SS用ステップサイズパラメータ
 \param fSSScal \~English It's scaling parameter.
 \~Japanese SS用スケーリングパラメータ
 \param fNoiseFloor \~English It's trancation value.
 \~Japanese ノイズフロアレベル値
 \param pcamyu_J_SS \~English It's separate matrix updated matrix.
 \~Japanese 分離行列更新値
*/
int _dCalcSSStepSize_GHDSS(MICARY_ComplexArray caW ,
							MICARY_ComplexArray caA ,
							MICARY_ComplexArray caY ,
							MICARY_ComplexArray caX , 
							HA_Integer iSSMethod ,
							HA_Float fSSMyu ,
							HA_Float fSSScal ,
							HA_Float fNoiseFloor ,
							MICARY_ComplexArray *pcamyu_J_SS ) ;
//int _dCalcSSStepSize_GHDSS(MICARY_dComplexArray caW ,
//							MICARY_dComplexArray caA ,
//							MICARY_dComplexArray caY ,
//							MICARY_dComplexArray caX , 
//							HA_Integer iSSMethod ,
//							HA_Float fSSMyu ,
//							HA_Float fSSScal ,
//							HA_Float fNoiseFloor ,
//							MICARY_dComplexArray *pcamyu_J_SS ) ;


/**
 \brief \~English It calcurates step size parameter for LC(GHDSS).(1:true, 0:false)
 \brief \~Japanese LC用ステップサイズ更新量の計算[GHDSS]（1:正常終了，0:エラー）
 \param caW \~English It's the Separation matrix.
 \~Japanese 分離行列
 \param caA \~English It's the Transfer Function matrix.
 \~Japanese 伝達関数行列
 \param iNsrc \~English It's the source number.
 \~Japanese 音源数
 \param iNmic \~English It's the microphone number.
 \~Japanese マイク数
 \param iLCConst \~English It's the LC method(0:FULL ,1:DIAG).
 \~Japanese LC計算方法(0:FULL, 1:DIAG)
 \param iLCMethod \~English It's the LC_myu method(0:Fix ,1:Adaptive).
 \~Japanese LC用ステップサイズ更新量の計算方法(0:固定,1:最適化)
 \param fLCMyu \~English It's step size parameter.
 \~Japanese LC用ステップサイズパラメータ
 \param pcamyu_J_LC \~English It's separate matrix updated value.
 \~Japanese 分離行列更新値
*/

int _dCalcLCStepSize_GHDSS(MICARY_ComplexArray caW ,
							MICARY_ComplexArray caA ,
							HA_Integer iNsrc ,
							HA_Integer iNmic ,
							HA_Integer	iLCConst ,
							HA_Integer	iLCMethod ,
							HA_Float fLCMyu ,
							MICARY_ComplexArray *pcamyu_J_LC ) ;


/**
 \brief \~English It updates separation matrix for GHDSS.(1:true, 0:false)
 \brief \~Japanese 分離行列の更新[GHDSS] （1:正常終了，0:エラー）
 \param psSavedSeparation \~English It's the saved value for executing GHDSS struct pointer.
 \~Japanese 分離処理保存値
 \param csSeparationSpec \~English It's separated spectrum .
 \~Japanese 分離スペクトル
 \param csInputSpec \~English It's input spectrum .
 \~Japanese 入力スペクトル
 \param pcsRxx \~English It's input correlation matrix(Rxx) spectrum[double] .
 \~Japanese 1フレーム前の入力信号相関行列
 \param pcsRyy \~English It's output crrelation matrix(Ryy) spectrum[couble] .
 \~Japanese 1フレーム前の出力信号相関行列
 \param sPrmSeparationASOCRA \~English It's setting parameter for GSS_AS_OCRA .
 \~Japanese 分離処理設定パラメータ 
 \param HA_fWmyu \~English It's stepsize parameter for update .
 \~Japanese 更新用ステップサイズパラメータ
 \param HA_fThreshold \~English It's update thereshold level[dB] .
 \~Japanese 更新用Threshold[dB]
 \param HA_iUpdate \~English It's update method .
 \~Japanese 更新方法

*/

int dUpdateSeparationMatGHDSS(MICARY_SavedSeparation * psSavedSeparation ,
							MICARY_ComplexSpecArray csSeparationSpec ,
							MICARY_ComplexSpecArray csInputSpec , 
							MICARY_dComplexSpecArray *pcsRxx ,
							MICARY_dComplexSpecArray *pcsRyy ,
							MICARY_PrmSeparation_GHDSS sPrmSeparation,
							HA_Float ha_fWmyu,
							HA_Float ha_fThreshold,
							HA_Integer ha_iUpdate) ;

// added 2012/03/07
	// モジュール内変数の初期化
void __SepMat_InitializeValues();
	
// added 2012/03/07
	// モジュール内変数のAlloc (Nsrc, Nmicにより変化するもののみ)
void __SepMat_AllocValues(int Nsrc, int Nmic);
	
// added 2012/03/07
	// モジュール内変数のFree
void __SepMat_FreeValues(bool bFreeSrc, bool bFreeMic);


#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------

#endif
