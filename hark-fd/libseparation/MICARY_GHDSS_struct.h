/**
Copywright (c)2006 Nittobo Acoustic Engineering,Co.,Ltd.

\file MICARY_GHDSS_struct.h
\brief\~Japanese GHDSS用構造体
\author Sachiko Suzuki
\date 5 Oct 2006
\Version $Id: MICARY_GHDSS_struct.h 3538 2012-02-29 07:25:35Z tanaka $
*/
// ---------------------------------------------------------------------
#ifndef	MICARY_GHDSS_struct_h_defined
#define	MICARY_GHDSS_struct_h_defined
// ---------------------------------------------------------------------
#include	<stdio.h>
#include	"MICARY_def.h"
#include	"MICARY_struct.h"
#include	"ArrayInit.h"
// ---------------------------------------------------------------------
// 2007/07/09 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif

// CmdLinesArgsの初期化

// --------------------------------------------------------------------
/**
The enum contains Command TF Referense form .
\author Sachiko Suzuki
\date 5 Oct 2006
*/

typedef enum{
	ntrCalcFree,
	ntrDB
} MICARY_TFRefEnum ;

/**
The enum contains Command separation matrix initialize form .
\author Sachiko Suzuki
\date 8 Dec 2006
*/

typedef enum{
	nsmInit,
	nsmLib
} MICARY_SepMatInitEnum ;

//----------------------------------------------------------------------

/**
The structure contains Command Line Args.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct {
	HA_String cOriginalWavFile ;	///<\~Japanese 分析ファイル名
	HA_Float ha_fStartTime ;	///<\~Japanese 開始時間[sec]
	HA_Float ha_fEndTime ;		///<\~Japanese 終了時間[sec]
	HA_String cSeparateWavFile ;	///<\~Japanese 分離ファイル名
	HA_String cOriginalSpecFile ;	///<\~Japanese 入力スペクトルファイル名
	HA_Integer ha_iStartFrame ;	///<\~Japanese 分析開始フレーム[frame]
	HA_Integer ha_iEndFrame ;	///<\~Japanese 分析終了フレーム[frame]
	HA_String cSeparateSpecFile ;	///<\~Japanese 出力スペクトルファイル名
	HA_String cSrcLocFile ;			///<\~Japanese 音源位置ファイル名
	HA_String dMicLocFile ;			///<\~Japanese 受音点位置ファイル名
	HA_String cTFDBFile ;			///<\~Japanese TFデータベース名
	MICARY_TFRefEnum ntrTFRef ;	///<\~Japanese TF参照方法
	HA_Integer ha_iNFFT ;		///<\~Japanese FFT長[point]
	HA_Integer ha_iWindow ;		///<\~Japanese 窓長[sample]
	MICARY_WindowEnum micary_nwnWindow ;	///<\~Japanese 窓
	HA_Integer ha_iShift ;		///<\~Japanese シフト数[sample]
	HA_Float ha_fUpperFreq ;	///<\~Japanese 分析上限周波数[Hz]
	HA_Float ha_fStepSizePrm ;	///<\~Japanese ステップサイズパラメータ
	HA_Float ha_fTemperature ;	///<\~Japanese 気温[℃]
	MICARY_SepMatInitEnum  ntrSepMat ;	///<\~Japanese 分離行列の初期化 
	HA_Float ha_fOutMag ;           ///<\~Japanese 出力ゲイン 

} MICARY_GHDSSCmdArgs ;

// --------------------------------------------------------------------
/**
The structure contains Separation matrix.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct{
	MICARY_IntArray micary_iaFreqLinex ;	///<\~Japanese 周波数インデックス
	HA_Num ha_numInput ;			///<\~Japanese 入力信号数
	HA_Num ha_numOutput ;			///<\~Japanese 出力信号数
	MICARY_ComplexArray micary_cmpaSeparation ;	///<\~Japanese 分離行列
	// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num ha_numReservedInput;	///<\~ 入力信号の確保数
	HA_Num ha_numReservedOutput;	///<\~ 出力信号の確保数
#endif
} MICARY_SeparationMat ;

//----------------------------------------------------------------------
/**
The structure contains Saved Separation.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct{
	MICARY_TFDatabase micary_tdTFDB ;			///<\~Japanese TF読み出し内部保存値
	MICARY_SeparationMat micary_smSeparation ;	///<\~Japanese 分離行列
} MICARY_SavedSeparation ;

// ---------------------------------------------------------------------
/**
The structure contains Parameter Separation.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct{
	HA_Float ha_fStepSizePrm ;	///<\~Japanese ステップサイズパラメータ
} MICARY_PrmSeparation ;

//-----------------------------------------------------------------------
/**
The structure contains Parameter Separation for ASOCRA.
\author Sachiko Suzuki
\date 14 May 2008
*/
typedef struct{
	HA_Integer	ha_iSSMethod ; ///<\~Japanese SS方法
	HA_Float ha_fSSMyu ;	///<\~Japanese SS用ステップサイズパラメータ
	HA_Integer	ha_iLCMethod ;	///<~Japanese LC方法
	HA_Float	ha_fLCMyu ;	///<~Japanese	LC用ステップサイズパラメータ
	HA_Integer	ha_iAvg_Method ;	 ///<~Japanese	平均化方法
	HA_Float	ha_fAlpha ;	///<~Japanese 平均化の重み係数
	HA_Float	ha_fBeta ;	///<~Japanese 窓長係数
	HA_Integer	ha_iNmax ;	 ///<~Japanese 窓長の最大値
	HA_Float	ha_fTran ;	///<~Japanese trancation
} MICARY_PrmSeparationASOCRA ;

//-----------------------------------------------------------------------

// 2008/09/03 changed by N.Tanaka
// Version番号，ファイルタイプの追加
/*
/ **
The structure contains TFDataBase File header.
\author Sachiko Suzuki
\date 5 Oct 2006
* /
typedef struct{
	int lFs ;		///<\~Japanese サンプリング周波数[Hz]
	int lNFFT ;	///<\~Japanese FFT長[point]
	int lNMic ;	///<\~Japanese マイクロホン位置数
	int lNSrc ;	///<\~Japanese 音源位置数
	int lNFreq ;	///<\~Japanese 周波数ライン数[line]
	int lMeshFlg ;	///<\~Japanese 音源位置が格子状になっているかのflag
	int lMeshX ;	///<\~Japanese X方向の格子数
	int lMeshY ;	///<\~Japanese Y方向の格子数
	int lMeshZ ;	///<\~Japanese Z方向の格子数
	char cPad[100] ;	///<\~Japanese パッド
} MICARY_TFDBFileHeader ;
*/
/**
The structure contains TFDataBase File header.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct{
	int lFs ;		///<\~Japanese サンプリング周波数[Hz]
	int lNFFT ;	///<\~Japanese FFT長[point]
	int lNMic ;	///<\~Japanese マイクロホン位置数
	int lNSrc ;	///<\~Japanese 音源位置数
	int lNFreq ;	///<\~Japanese 周波数ライン数[line]
	int lMeshFlg ;	///<\~Japanese 音源位置が格子状になっているかのflag
	int lMeshX ;	///<\~Japanese X方向の格子数
	int lMeshY ;	///<\~Japanese Y方向の格子数
	int lMeshZ ;	///<\~Japanese Z方向の格子数
	int iVersion;	///<\~Japanese バージョン番号
	int iFileType;	///<\~Japanese ファイルタイプ（0:TF_CONJ, 1:分離行列）
	char cPad[92] ;	///<\~Japanese パッド
} MICARY_TFDBFileHeader ;


/**
 2Dimention array struct
 \author	Sachiko Suzuki
 \date	27 Nov 2006
*/
typedef struct{
	HA_Float **	ha_fValue; ///<\~Japanese HA_Float配列
	HA_Num	ha_numValue1; ///<\~Japanese １次元配列のサイズ
	HA_Num ha_numValue2 ; ///<\~Japanese ２次元配列のサイズ
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved1; ///<\~Japanese 配列の実際のサイズ
	HA_Num	ha_numReserved2; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_FloatArray2;

/**
 2Dimention complex array struct
 \author	Sachiko Suzuki
 \date	27 Nov 2006
*/
typedef struct{
	HA_Complex **	ha_cmpValue; ///<\~Japanese HA_Complex配列
	HA_Num	ha_numValue1; ///<\~Japanese １次元配列のサイズ
	HA_Num ha_numValue2 ; ///<\~Japanese ２次元配列のサイズ
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved1; ///<\~Japanese 配列の実際のサイズ
	HA_Num	ha_numReserved2; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_ComplexArray2;

/**
 GHDSS_Location Array struct
 \author	Sachiko Suzuki
 \date	19 Dec 2006
*/
typedef struct{
	HA_Id *	ha_idLocation; ///<\~Japanese 位置座標ID配列
	HA_Id *	ha_idExistence; ///<\~Japanese 位置座標配列
	int micary_MoveFlg ;	///<\~Japanese 音源移動フラグ
	HA_Num ha_numLocation ;	///<\~Japanese 座標位置数
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_GHDSSLocationId;

/**
 GHDSS_Frequency Array struct
 \author	Sachiko Suzuki
 \date	26 Feb 2007
*/
typedef struct{
	MICARY_IntArray *micary_iaFreqLine ;	///<\~Japanese 周波数ライン配列
	HA_Integer 	ha_iLowerFreq; ///<\~Japanese 下限周波数ライン番号
	HA_Integer 	ha_iUpperFreq; ///<\~Japanese 上限周波数ライン番号
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_GHDSSFreqLine;


// ---------------------------------------------------------------------
/**
The structure contains Parameter Separation.
\author Sachiko Suzuki
\date 12 May 2008
*/
typedef struct{
	HA_Integer	ha_iSSMethod ;	///<\~Japanese SSステップサイズと更新量の計算方法(0:Fix,1:myu_LC,2:Adaptive)
	HA_Float	ha_fSSMyu ;	///<\~Japanese SS用ステップサイズパラメータ
	HA_Integer	ha_iOCRAMethod ;	///<\~Japanese OCRA方法(0:No,1:alpha->Fix,2:alpha->Adaptive)
	HA_Float	ha_fAlpha ;	///<\~Japanese 平均化の重み係数
	HA_Float	ha_fBeta ; ///<\~Japanese 窓長決定の際の係数
	HA_Integer	ha_iNmax ;	///<\~Japanese 窓長の最大値
	HA_Float	ha_fNoiseFloor ;	///<\~Japanese ノイズフロア値
	HA_Integer	ha_iLCConst ;	///<\~Japanese LC_Const方法(0:Full ,1:Diag)
	HA_Integer	ha_iLCMethod ;	///<\~Japanese LCステップサイズと更新量の計算方法(0:Fix,1:Adaptive)
	HA_Float	ha_fLCMyu ;	///<\~Japanese LC用ステップサイズパラメータ
} MICARY_PrmSeparation_ASOCRA ;

//----------------------------------------------------------------------
/**
The structure contains Parameter Separation for GHDSS.
\author Sachiko Suzuki
\date 24 Dec 2009
*/
typedef struct{
	HA_Integer	ha_iSSMethod ;	///<\~Japanese SSステップサイズと更新量の計算方法(0:Fix,1:myu_LC,2:Adaptive)
	HA_Float	ha_fSSMyu ;	///<\~Japanese SS用ステップサイズパラメータ
	HA_Float	ha_fSSScal ;	///<\~Japanese SS用スケーリングファクター
	HA_Float	ha_fNoiseFloor ;	///<\~Japanese ノイズフロア値
	HA_Integer	ha_iLCConst ;	///<\~Japanese LC_Const方法(0:Full ,1:Diag)
	HA_Integer	ha_iLCMethod ;	///<\~Japanese LCステップサイズと更新量の計算方法(0:Fix,1:Adaptive)
	HA_Float	ha_fLCMyu ;	///<\~Japanese LC用ステップサイズパラメータ
} MICARY_PrmSeparation_GHDSS ;

// ---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief \~English It allocates float array(dim:2)
 \brief \~Japanese float型の二次元配列を確保する
 \param array \~English It's the array of float. (iArraySize1 * iArraySize2)
 \~Japanese float型の二次元配列のポインタ
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese 次元1の配列の個数
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese 次元2の配列の個数
*/
void FloatArray2Alloc(MICARY_FloatArray2 * array, int iArraySize1, int iArraySize2) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates float array(dim:2)
 \brief \~Japanese float型の二次元配列を確保する
 \param array \~English It's the array of float. (iArraySize1 * iArraySize2)
 \~Japanese float型の二次元配列のポインタ
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese 次元1の配列の個数
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese 次元2の配列の個数
 \param iReservedSize1 \~English It's the size of array (dim:1)
 \~Japanese 次元1の配列の確保個数
 \param iReservedSize2 \~English It's the size of array (dim:2)
 \~Japanese 次元2の配列の確保個数
*/
void FloatArray2Alloc_r(MICARY_FloatArray2 * array, int iArraySize1, int iArraySize2,
												  int iReservedSize1, int iReservedSize2) ;
#endif

/**
 \brief \~English It frees float array(dim:2)
 \brief \~Japanese float型の二次元配列を開放する
 \param array \~English It's the array of float. (iArraySize1 * iArraySize2)
 \~Japanese int型の二次元配列のポインタ
*/
void FloatArray2Free(MICARY_FloatArray2 * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees float array(dim:2)
 \brief \~Japanese float型の二次元配列の使用サイズをクリアする
 \param array \~English It's the array of float. (iArraySize1 * iArraySize2)
 \~Japanese int型の二次元配列のポインタ
*/
void FloatArray2Clear(MICARY_FloatArray2 * array);
#endif

/**
 \brief \~English It allocates complex array(dim:2)
 \brief \~Japanese complex型の二次元配列を確保する
 \param array \~English It's the array of complex. (iArraySize1 * iArraySize2)
 \~Japanese complex型の二次元配列のポインタ
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese 次元1の配列の個数
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese 次元2の配列の個数
*/
void ComplexArray2Alloc(MICARY_ComplexArray2 * array, int iArraySize1, int iArraySize2) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates complex array(dim:2)
 \brief \~Japanese complex型の二次元配列を確保する
 \param array \~English It's the array of complex. (iArraySize1 * iArraySize2)
 \~Japanese complex型の二次元配列のポインタ
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese 次元1の配列の個数
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese 次元2の配列の個数
 \param iReservedSize1 \~English It's the size of array (dim:1)
 \~Japanese 次元1の配列の確保個数
 \param iReservedSize2 \~English It's the size of array (dim:2)
 \~Japanese 次元2の配列の確保個数
*/
void ComplexArray2Alloc_r(MICARY_ComplexArray2 * array, int iArraySize1, int iArraySize2,
													  int iReservedSize1, int iReservedSize2) ;
#endif

/**
 \brief \~English It frees complex array(dim:2)
 \brief \~Japanese complex型の二次元配列を開放する
 \param array \~English It's the array of complex. (iArraySize1 * iArraySize2)
 \~Japanese complex型の二次元配列のポインタ
*/
void ComplexArray2Free(MICARY_ComplexArray2 * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex array(dim:2)
 \brief \~Japanese complex型の二次元配列の使用数をクリアする
 \param array \~English It's the array of complex. (iArraySize1 * iArraySize2)
 \~Japanese complex型の二次元配列のポインタ
*/
void ComplexArray2Clear(MICARY_ComplexArray2 * array);
#endif

/**
 \brief \~English It allocates separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_SeparationMat構造体の確保（1:正常終了，0:エラー）
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese 分離行列構造体ポインタ
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese 周波数ライン数
 \param iLocSize \~English It's the number of source location.
 \~Japanese 音源位置数
 \param iMicSize \~English It's the number of microphone .
 \~Japanese マイク数
*/
void SeparationMatAlloc(MICARY_SeparationMat * SPM, int iFreqSize, int iLocSize, int iMicSize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_SeparationMat構造体の確保（1:正常終了，0:エラー）
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese 分離行列構造体ポインタ
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese 周波数ライン数
 \param iLocSize \~English It's the number of source location.
 \~Japanese 音源位置数
 \param iMicSize \~English It's the number of microphone .
 \~Japanese マイク数
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese 確保する要素数(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese 確保する要素数(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese 確保する要素数(Mic)
*/
void SeparationMatAlloc_r(MICARY_SeparationMat * SPM, int iFreqSize, int iLocSize, int iMicSize,
													int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) ;
#endif

/**
 \brief \~English It frees separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_SeparationMat構造体を開放する（1:正常終了，0:エラー）
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese 分離行列構造体ポインタ
*/
 void SeparationMatFree(MICARY_SeparationMat * SPM) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_SeparationMat構造体の使用個数をクリアする（1:正常終了，0:エラー）
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese 分離行列構造体ポインタ
*/
 void SeparationMatClear(MICARY_SeparationMat * SPM) ;
#endif

/**
 \brief \~English It allocates saved separation .(1:true, 0:false)
 \brief \~Japanese MICARY_SavedSeparation構造体の確保（1:正常終了，0:エラー）
 \param SSP \~English It's the saved separation struct pointer .
 \~Japanese 分離処理保存値構造体ポインタ
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese 周波数ライン数
 \param iLocSize \~English It's the number of source location.
 \~Japanese 音源位置数
 \param iMicSize \~English It's the number of microphone .
 \~Japanese マイク数
*/
void SavedSeparationAlloc(MICARY_SavedSeparation * SSP ,int iFreqSize, int iLocSize, int iMicSize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates saved separation .(1:true, 0:false)
 \brief \~Japanese MICARY_SavedSeparation構造体の確保（1:正常終了，0:エラー）
 \param SSP \~English It's the saved separation struct pointer .
 \~Japanese 分離処理保存値構造体ポインタ
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese 周波数ライン数
 \param iLocSize \~English It's the number of source location.
 \~Japanese 音源位置数
 \param iMicSize \~English It's the number of microphone .
 \~Japanese マイク数
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese 確保する要素数(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese 確保する要素数(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese 確保する要素数(Mic)
*/
void SavedSeparationAlloc_r(MICARY_SavedSeparation * SSP ,int iFreqSize, int iLocSize, int iMicSize,
						  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) ;
#endif

/**
 \brief \~English It frees saved separation.(1:true, 0:false)
 \brief \~Japanese MICARY_SavedSeparation構造体を開放する（1:正常終了，0:エラー）
 \param SSP \~English It's the saved separation struct pointer .
 \~Japanese 分離処理保存値体ポインタ
*/
void SavedSeparationFree(MICARY_SavedSeparation * SSP) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/*
 \brief \~English It frees saved separation.(1:true, 0:false)
 \brief \~Japanese MICARY_SavedSeparation構造体を開放する（1:正常終了，0:エラー）
 \param SSP \~English It's the saved separation struct pointer .
 \~Japanese 分離処理保存値体ポインタ
*/
void SavedSeparationClear(MICARY_SavedSeparation * SSP) ;
#endif

/**
 \brief \~English It allocates GHDSS Location ID .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSSLocationId構造体の確保（1:正常終了，0:エラー）
 \param array \~English It's the location ID(for GHDSS) struct pointer .
 \~Japanese GHDSS座標ID配列構造体ポインタ
 \param iArraySize \~English It's the number of Location.
 \~Japanese 位置座標数
*/
void LocationIdAlloc(MICARY_GHDSSLocationId * array, int iArraySize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/*
 \brief \~English It allocates GHDSS Location ID .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSSLocationId構造体の確保（1:正常終了，0:エラー）
 \param array \~English It's the location ID(for GHDSS) struct pointer .
 \~Japanese GHDSS座標ID配列構造体ポインタ
 \param iArraySize \~English It's the number of Location.
 \~Japanese 位置座標数
 \param iReservedSize \~English It's the number of Location.
 \~Japanese 確保する位置座標数
*/
void LocationIdAlloc_r(MICARY_GHDSSLocationId * array, int iArraySize, int iReservedSize) ;
#endif

/**
 \brief \~English It frees GHDSS Location ID .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSSLocationId構造体を開放する（1:正常終了，0:エラー）
 \param array \~English It's the location ID(for GHDSS) struct pointer .
 \~Japanese GHDSS座標ID配列構造体ポインタ
 */
void LocationIdFree(MICARY_GHDSSLocationId * array) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees GHDSS Location ID .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSSLocationId構造体の使用数をクリアする（1:正常終了，0:エラー）
 \param array \~English It's the location ID(for GHDSS) struct pointer .
 \~Japanese GHDSS座標ID配列構造体ポインタ
 */
void LocationIdClear(MICARY_GHDSSLocationId * array) ;
#endif

/**
 \brief \~English It allocates GHDSS frequency line struct .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSS周波数ライン構造体の確保（1:正常終了，0:エラー）
 \param array \~English It's the frequency line struct pointer .
 \~Japanese 周波数ライン構造体ポインタ
 \param iLowerFreqLine \~English It's the lower frequency line number .
 \~Japanese 下限周波数ライン番号
 \param iUpperFreqLine \~English It's the upper frequency line number .
 \~Japanese 上限周波数ライン番号
*/
void FreqLineAlloc(MICARY_GHDSSFreqLine * array, int iLowerFreqLine , int iUpperFreqLine) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates GHDSS frequency line struct .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSS周波数ライン構造体の確保（1:正常終了，0:エラー）
 \param array \~English It's the frequency line struct pointer .
 \~Japanese 周波数ライン構造体ポインタ
 \param iLowerFreqLine \~English It's the lower frequency line number .
 \~Japanese 下限周波数ライン番号
 \param iUpperFreqLine \~English It's the upper frequency line number .
 \~Japanese 上限周波数ライン番号
 \param iReservedSize \~English It's size of array .
 \~Japanese 確保する要素数
*/
void FreqLineAlloc_r(MICARY_GHDSSFreqLine * array, int iLowerFreqLine , int iUpperFreqLine, int iReservedSize) ;
#endif

/*
 \brief \~English It frees frequency line struct .(1:true, 0:false)
 \brief \~Japanese 周波数ライン構造体を開放する（1:正常終了，0:エラー）
 \param array \~English It's the frequency line struct pointer .
 \~Japanese 周波数ライン構造体ポインタ
 */
void FreqLineFree(MICARY_GHDSSFreqLine * array) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/*
 \brief \~English It frees frequency line struct .(1:true, 0:false)
 \brief \~Japanese 周波数ライン構造体の使用数をクリアする（1:正常終了，0:エラー）
 \param array \~English It's the frequency line struct pointer .
 \~Japanese 周波数ライン構造体ポインタ
 */
void FreqLineClear(MICARY_GHDSSFreqLine * array) ;
#endif


/**
 Database of Separation matrix
 \author	Sachiko Suzuki
 \date	10 Jan 2008
*/

typedef struct{
	MICARY_SeparationMat micary_smSeparation ;	///<\~Japanese 分離行列	
	MICARY_LocationArray	micary_locaSource; ///<\~Japanese 伝達関数の音源推定位置座標配列
	MICARY_LocationArray	micary_locaMic; ///<\~Japanese 伝達関数のマイクロホン位置座標配列
} MICARY_SMDatabase;

/**
 \brief \~English It allocates database of separation matrix
 \brief \~Japanese 分離行列を格納する配列を確保する
 \param smD \~English It's Database of separation matrix
 \~Japanese 分離行列を格納する配列を含む構造体
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese 伝達関数の周波数ライン数
 \param iLocSize \~English It's the number of location
 \~Japanese 伝達関数の音源位置数
 \param iMicSize \~English It's the number of microphone
 \~Japanese 伝達関数のマイクロホン位置数
*/
void SMDatabaseAlloc(MICARY_SMDatabase * SMD, int iFreqSize, int iLocSize, int iMicSize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates database of separation matrix
 \brief \~Japanese 分離行列を格納する配列を確保する
 \param smD \~English It's Database of separation matrix
 \~Japanese 分離行列を格納する配列を含む構造体
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese 伝達関数の周波数ライン数
 \param iLocSize \~English It's the number of location
 \~Japanese 伝達関数の音源位置数
 \param iMicSize \~English It's the number of microphone
 \~Japanese 伝達関数のマイクロホン位置数
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese 確保する要素数(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese 確保する要素数(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese 確保する要素数(Mic)
*/
void SMDatabaseAlloc_r(MICARY_SMDatabase * SMD, int iFreqSize, int iLocSize, int iMicSize,
											  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) ;
#endif

/**
 \brief \~English It frees database of separation matrix
 \brief \~Japanese 分離行列を格納する配列を開放する
 \param SMD \~English It's Database of separation matrix
 \~Japanese 分離行列を格納する配列を含む構造体
*/
void SMDatabaseFree(MICARY_SMDatabase * SMD) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees database of separation matrix
 \brief \~Japanese 分離行列を格納する配列の使用数をクリアする
 \param SMD \~English It's Database of separation matrix
 \~Japanese 分離行列を格納する配列を含む構造体
*/
void SMDatabaseClear(MICARY_SMDatabase * SMD) ;
#endif

/**
 \brief \~English It initializes complex array
 \brief \~Japanese Complex配列の初期化(0埋め)
 \param array \~English It's complex array
 \~Japanese Complex配列を含む構造体
*/

void ComplexArrayInit(MICARY_ComplexArray * array) ;

/**
The structure contains Update Separation matrix.
\author Sachiko Suzuki
\date 6 Feb 2009
*/
typedef struct{
	MICARY_IntArray micary_iaUpdate ;	///<\~Japanese 更新履歴フラグ
	MICARY_SMDatabase micary_dbSeparation ;	///<\~Japanese 分離行列
} MICARY_UpdateSeparationMat ;

/**
 \brief \~English It allocates update separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_UpdateSeparationMat構造体の確保（1:正常終了，0:エラー）
 \param SPM \~English It's the update separation matrix struct pointer .
 \~Japanese 更新用分離行列構造体ポインタ
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese 周波数ライン数
 \param iLocSize \~English It's the number of source location.
 \~Japanese 音源位置数
 \param iMicSize \~English It's the number of source location.
 \~Japanese マイク数
 */
void UpdateSeparationMatAlloc(MICARY_UpdateSeparationMat * SPM, int iFreqSize, int iLocSize,int iMicSize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates update separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_UpdateSeparationMat構造体の確保（1:正常終了，0:エラー）
 \param SPM \~English It's the update separation matrix struct pointer .
 \~Japanese 更新用分離行列構造体ポインタ
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese 周波数ライン数
 \param iLocSize \~English It's the number of source location.
 \~Japanese 音源位置数
 \param iMicSize \~English It's the number of source location.
 \~Japanese マイク数
 */
void UpdateSeparationMatAlloc_r(MICARY_UpdateSeparationMat * SPM, int iFreqSize, int iLocSize,int iMicSize,
							  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) ;
#endif

/**
 \brief \~English It frees update separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_UpdateSeparationMat構造体を開放する（1:正常終了，0:エラー）
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese 更新用分離行列構造体ポインタ
*/
 void UpdateSeparationMatFree(MICARY_UpdateSeparationMat * SPM) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees update separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_UpdateSeparationMat構造体を使用数をクリアする（1:正常終了，0:エラー）
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese 更新用分離行列構造体ポインタ
*/
void UpdateSeparationMatClear(MICARY_UpdateSeparationMat * SPM) ;
#endif

//
#ifdef __cplusplus
}
#endif
// -------------------------------------------------------------------------------------
#endif // MICARY_GHDSS_struct_h_defined

