// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_rev.h
 \brief \~English Header file for Estimation reverberation noise power spectrum
 \brief \~Japanese 残響成分推定関数のヘッダファイル
 \author Masatoshi Tsutsumi
 \date 29 Nov 2006
 \version $$
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_REV_h_defined
#define	PF_EST_REV_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------

/**
 \brief \~Japanese 残響推定(1:正常終了, 0:エラー)
 \param fsInputPSpec \~English Input power spectrum (N, M)
 \~Japanese 入力パワー
 \param sPrmRev \~English Parameters for Estimation reverberation noise calc.
 \~Japanese 残響推定計算のためのパラメータ
 \param psSavedRev \~English Estimated reverberation noise power spectrum of previous frame (N ,M)
 \~Japanese 前フレームの残響パワー
 \param pfsEstRevPSpec \~English Total estimated reverberation power spectrum (N, M)
 \~Japanese 推定した残響成分パワー
*/
int PF_EstRev(
		MICARY_FloatSpecArray fsInputPSpec,
		MICARY_PrmRevNoise sPrmRev,
		MICARY_SavedEstRev *psSavedRev,
		MICARY_FloatSpecArray *pfsEstRevPSpec);

#ifdef __cplusplus
}
#endif

#endif	// PF_EST_REV_h_defined
