// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file MICARY_TimeWin.h
 \brief \~English Header file for calculating time window function. 
 \brief \~Japanese 時間窓作成関数のヘッダファイル
 \author Sachiko Suzuki
 \date 17 Nov 2006
 \version $Id: MICARY_TimeWin.h 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#ifndef MICARY_TimeWin_h_defined
#define MICARY_TimeWin_h_defined
// ---------------------------------------------------------------------
#include "MICARY_struct.h"
#include "ArrayInit.h"
#include "MICARY_GHDSS_struct.h"

#ifndef M_PI
#define M_PI (3.141592653589793238)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------
/**
 \brief \~English It makes time window function . (1: true, 0: false)
 \brief \~Japanese 時間窓の作成関数（1:正常終了，0:エラー）
 \param pfaTWin \~English It's the window function .
 \~Japanese 窓関数
 \param iNwin \~English It's the length of the window function.
 \~Japanese 窓長
 \param facoef \~English It's the coefficient value 
 \~Japanese 係数
 \param enumWin \~English It's the veriety of time window. (1: hanning window , 0: Special window)
 \~Japanese 時間窓の種類(1:Hanning窓，0:Special窓)
*/
int MakeTimeWin(MICARY_FloatArray * pfaTWin,
					 HA_Integer iNwin,float facoef,MICARY_WindowEnum enumWin);
/**
 \brief \~English It products data window function . 
 \brief \~Japanese 時系列データに時間窓をかける
 \param pfaTWin \~English It's the window function .
 \~Japanese 窓関数
 \param iaData \~English It's data.
 \~Japanese データ[チャンネル数][サンプル]
 */
MICARY_IntArray2 *ProductTimeWin(MICARY_FloatArray * pfaTWin,	MICARY_IntArray2 iaData) ;

/**
 \brief \~English It products float data window function . 
 \brief \~Japanese 時系列floatデータに時間窓をかける
 \param pfaTWin \~English It's the window function .
 \~Japanese 窓関数
 \param faData \~English It's float data.
 \~Japanese データ[チャンネル数*サンプル]
 \param FrameLen \~English It's data length by one frame.
 \~Japanese フレーム長

 */
MICARY_FloatArray *ProductTimeWinF(MICARY_FloatArray * pfaTWin,
								MICARY_FloatArray faData,HA_Num FrameLen) ;

/**
 \brief \~English It products data(float) window function . 
 \brief \~Japanese 時系列(float)データに時間窓をかける
 \param pfaTWin \~English It's the window function .
 \~Japanese 窓関数
 \param iaData \~English It's (float) data.
 \~Japanese  データ[チャンネル数][サンプル]
 */
MICARY_FloatArray2 *ProductTimeWinFloat2(MICARY_FloatArray * pfaTWin,MICARY_IntArray2 iaData);

/**
 \brief \~English It products data(float) window function . 
 \brief \~Japanese 時系列(float)データに時間窓をかける
 \param faReturn \~English It's the windowed  signal .
 \~Japanese 窓関数をかけた時系列データ（2次元配列）
 \param pfaTWin \~English It's the window function .
 \~Japanese 窓関数
 \param iaData \~English It's (float) data.
 \~Japanese  データ[チャンネル数][サンプル]
 */
int ProductTimeWinFloat3(MICARY_FloatArray2 *faReturn,MICARY_FloatArray * pfaTWin,
				MICARY_IntArray2 iaData) ;
/**
 \brief \~English It products float data window function . 
 \brief \~Japanese 時系列floatデータに時間窓をかける
 \param faReturn \~English It's the windowed signal .
 \~Japanese 窓関数をかけた時系列データ（1次元配列）
 \param pfaTWin \~English It's the window function .
 \~Japanese 窓関数
 \param faData \~English It's float data.
 \~Japanese データ[チャンネル数*サンプル]
 \param FrameLen \~English It's data length by one frame.
 \~Japanese フレーム長

 */
int ProductTimeWinF3(MICARY_FloatArray *faReturn,MICARY_FloatArray * pfaTWin,
				MICARY_FloatArray faData,HA_Num FrameLen) ;


#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------
#endif
