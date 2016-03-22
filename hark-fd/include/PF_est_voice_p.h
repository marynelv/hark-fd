// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_voice_p.h
 \brief \~English Header file for Estimate speech present probability
 \brief \~Japanese 音声存在確率関数のヘッダファイル
 \author Masatoshi Tsutsumi
 \date 1 Dec 2006
 \version $Id: PF_est_voice_p.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_VOICE_P_h_defined
#define	PF_EST_VOICE_P_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------

/**
 \brief \~Japanese 音声存在確率の推定(1:正常終了, 0:エラー)
 \param fsPreSN \~English Priori SNR (N, M)
 \~Japanese 事前SN比
 \param fsEstVal \~English Variable for optimum gain estimator  (N, M)
 \~Japanese 最適ゲイン推定値算出のための変数
 \param sPrmZeta \~English Parameters for Estimate speech present probability
 \~Japanese 音声存在確率計算のパラメータ
 \param psSavedVoiceP \~English Speech present probability of previous frame (N ,M)
 \~Japanese 前フレームの音声存在確率計算の結果
 \param pfsVoiceP \~English Probability of speech present (p) (N, M)
 \~Japanese 音声存在確率
 \param pfsSmSNR \~English Smoothed priori SNR (zeta) (N, M)
 \~Japanese 平滑化された事前SN比
 \param pfaSmSNRF \~English zeta_frame frame (M, 1)
 \~Japanese 平滑化された事前SN比(Frame)
 \param pfaSmSNRP \~English zeta_frame peak (M, 1)
 \~Japanese 平滑化された事前SN比(Peak)
*/
int PF_EstVoiceP(
		MICARY_FloatSpecArray fsPreSN,
		MICARY_FloatSpecArray fsEstVal,
		MICARY_PrmVoiceP sPrmVoiceP,
		MICARY_SavedVoiceP *psSavedVoiceP,
		MICARY_FloatSpecArray *pfsVoiceP,
		MICARY_FloatSpecArray *pfsSmSNR,
		MICARY_FloatArray *pfaSmSNRF,
		MICARY_FloatArray *pfaSmSNRP);

#ifdef __cplusplus
}
#endif
// --------------------------------------------------------------------

#endif	// PF_EST_VOICE_P_h_defined
