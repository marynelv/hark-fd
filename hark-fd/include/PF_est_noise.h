// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_noise.h
 \brief \~English Header file for Estimate Noise function
 \brief \~Japanese 妨害音推定関数のヘッダファイル
 \author Masatoshi Tsutsumi
 \date 27 Nov 2006
 \version $Id: PF_est_noise.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_NOISE_h_defined
#define	PF_EST_NOISE_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------

/**
 \brief \~Japanese 妨害音推定(1:正常終了, 0:エラー)
 \param pfsStateNoisePSpec \~English Back ground noise power spectrum (N, M)
 \~Japanese MCRAパワー
 \param pfsLeakPSpec \~English Leak power spectrum (N, M)
 \~Japanese 漏れパワー
 \param pfsRevPSpec \~English Reverberation power (N ,M)
 \~Japanese 残響パワー
 \param pfsEstNoisePSpec \~English Total estimated noise spectrum (N, M)
 \~Japanese 推定した妨害音パワー
*/
int PF_EstNoise(
		MICARY_FloatSpecArray *pfsStateNoisePSpec,
		MICARY_FloatSpecArray *pfsLeakPSpec,
		MICARY_FloatSpecArray *pfsRevPSpec,
		MICARY_FloatSpecArray *pfsEstNoisePSpec);

#ifdef __cplusplus
}
#endif

#endif	// PF_EST_NOISE_h_defined
