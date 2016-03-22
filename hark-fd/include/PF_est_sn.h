// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_sn.h
 \brief \~English Header file for Estimate Optimam Gain of speech present
 \brief \~Japanese SN比・ゲイン推定関数のヘッダファイル
 \author Masatoshi Tsutsumi
 \date 23 Nov 2006
 \version $Id: PF_est_sn.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_SN_h_defined
#define	PF_EST_SN_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------

/**
 \brief \~Japanese SN比推定(1:正常終了, 0:エラー)
 \param fsInputPSpec \~English Input power spectrum (N, M)
 \~Japanese 入力パワー
 \param fsEstNoisePSpec \~English Total Estimated Noise spectrum (N, M)
 \~Japanese 妨害音パワー
 \param psSavedSN \~English Saved Valiables for S/N Calc.
 \~Japanese SN比保存値
 \param sPrmSN \~English parameters for S/N Calc.
 \~Japanese SN比計算パラメータ
 \param pfsGH1 \~English Optimal amplitude gain (N, M)
 \~Japanese 最適増幅係数
 \param pfsSN \~English Estimated postteriori ~= (SNR - 1)  (N, M)
 \~Japanese SN比推定値
 \param pfsEstVal \~English Variable for optimum gain estimator  (N, M)
 \~Japanese 最適ゲイン推定値算出のための変数
*/
int PF_EstSN(
			MICARY_FloatSpecArray fsInputPSpec,
			MICARY_FloatSpecArray fsEstNoisePSpec,
			MICARY_SavedEstSN *psSavedSN,
			MICARY_PrmEstSN sPrmSN,
			MICARY_FloatSpecArray *pfsGH1,
			MICARY_FloatSpecArray *pfsSN,
			MICARY_FloatSpecArray *pfsEstVal);

#ifdef __cplusplus
}
#endif

#endif	// PF_EST_SN_h_defined
