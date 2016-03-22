// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_leak.h
 \brief \~English Header file for Estimation leak spectrum function
 \brief \~Japanese 漏れ推定関数のヘッダファイル
 \author Masatoshi Tsutsumi
 \date 28 Nov 2006
 \version $Id: PF_est_leak.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_LEAK_h_defined
#define	PF_EST_LEAK_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------

/**
 \brief \~Japanese 漏れ推定(1:正常終了, 0:エラー)
 \param pfsSmoothedPSpec \~English Smoothed Input power spectrums (N, M) by MCRA
 \~Japanese MCRA時のスムージング結果
 \param sPrmLeak \~English Parameters for Leak calc.
 \~Japanese 漏れ推定計算用パラメータ構造体
 \param pfsEstLeakPSpec \~English Estimated leak spectrum (N, M)
 \~Japanese 推定した漏れパワー
*/
int PF_EstLeak(
		MICARY_FloatSpecArray *pfsSmoothedPSpec,
		MICARY_PrmLeakNoise sPrmLeak,
		MICARY_FloatSpecArray *pfsEstLeakPSpec);

#ifdef __cplusplus
}
#endif

#endif	// PF_EST_LEAK_h_defined.
