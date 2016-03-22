// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_state_noise.h
 \brief \~English Header file for Estimate MCRA
 \brief \~Japanese 定常雑音推定関数のヘッダファイル
 \author Masatoshi Tsutsumi
 \date 5 Dec 2006
 \version $Id: PF_est_state_noise.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_STATE_NOISE_h_defined
#define	PF_EST_STATE_NOISE_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"
#include "PF_Init.h"

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------

/**
 \brief \~Japanese 定常雑音の推定(1:正常終了, 0:エラー)
 \param fsInputPSpec \~English Input power spectrum (N, M)
 \~Japanese 入力パワースペクトル
 \param sPrmMCRA \~English Parameters for MCRA calc.
 \~Japanese MCRA計算パラメータ
 \param psSavedMCRA \~English Saved MCRA values
 \~Japanese 前フレームのMCRA計算値
 \param pfsEstNoisePSpec \~English MCRA values
 \~Japanese MCRA計算値
 \param pfsSmin \~English Minimum Spectrum of yp
 \~Japanese 最小入力パワー
 \param pfsStmp \~English Temporary Spectrum of yp
 \~Japanese 作業用入力パワー
 \param pfaS \~English Smoothed spectrum of frame
 \~Japanese 平滑化された雑音パワー
*/
int PF_EstStateNoise(
		MICARY_FloatSpecArray fsInputPSpec,
		MICARY_PrmMCRA sPrmMCRA,
		MICARY_SavedMCRA *psSavedMCRA,
		MICARY_FloatSpecArray *pfsEstNoisePSpec,
		MICARY_FloatSpecArray *pfsSmin,
		MICARY_FloatSpecArray *pfsStmp,
		MICARY_FloatSpecArray *pfsS);

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
}
#endif
// ----------------------------------------------------------------------
#endif	// PF_EST_STATE_NOISE_h_defined
