// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_noise.c
 \brief \~English Estimate Noise
 \brief \~Japanese 妨害音推定処理関数
 \author Masatoshi Tsutsumi
 \date 27 Nov 2006
 \version $Id: PF_est_noise.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "ArrayInit.h"
#include "PF_est_noise.h"
// ---------------------------------------------------------------------
/*
function lamda_total = Est_lamda_valin(lamda_MCRA, lamda_leak, lamda_rev)
% lamda_total = Est_lamda_valin(lamda_MCRA, lamda_leak, lamda_rev)
%
%  Inputs:
%   lamda_MCRA: back ground noise power
%   lamda_leak: leak power
%   lamda_rev : reverbaration power 
%
%  Outputs:
%   total_ramda :  Total estimated noise spectrum (N, M)

[nfreq, nch] = size(lamda_MCRA);

% other rev power
sum_rev = sum(lamda_rev, 2);
other_rev = sum_rev*ones(1, nch) - lamda_rev;

% total lamda
lamda_total = lamda_MCRA + lamda_leak + other_rev;
*/
//
int PF_EstNoise(
		MICARY_FloatSpecArray *pfsStateNoisePSpec,
		MICARY_FloatSpecArray *pfsLeakPSpec,
		MICARY_FloatSpecArray *pfsRevPSpec,
		MICARY_FloatSpecArray *pfsEstNoisePSpec)
{
	MICARY_FloatSpecArray	*pfsOther_rev;
	HA_Float	*phfSum_rev;
	HA_Float	**phfTmp, **phfOther_rev;
	int	i, j;
	int	iFreq, iCh;
	int	iArrayLen;

	iCh = pfsStateNoisePSpec->ha_numCh;
	iFreq = pfsStateNoisePSpec->ha_iUpperFreq - pfsStateNoisePSpec->ha_iLowerFreq + 1;

	/* lamda_revを2次元配列として扱うためのポインタの確保と初期化 */
	phfTmp = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh;  i++) {
		phfTmp[i] = (HA_Float *)(&(pfsRevPSpec->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* sum_revの確保 */
	phfSum_rev = (HA_Float *)calloc((size_t)iFreq, sizeof(HA_Float));

	/* sum_revの計算 */
	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			phfSum_rev[j] = phfSum_rev[j] + phfTmp[i][j];
		}
	}

	/* other_revの確保 */
	pfsOther_rev = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	// modified 2008.10.31 Sachiko.suzuki
	// Initialize
	AllocFloatSpecArray(pfsOther_rev,0,0,0);

	AllocFloatSpecArray(pfsOther_rev, pfsRevPSpec->ha_numCh, pfsRevPSpec->ha_iLowerFreq, pfsRevPSpec->ha_iUpperFreq);

	/* other_revを2次元配列として扱うためのポインタの確保と初期化 */
	phfOther_rev = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh;  i++) {
		phfOther_rev[i] = (HA_Float *)(&(pfsOther_rev->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* other_revの計算 */
	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			phfOther_rev[i][j] = phfSum_rev[j] - phfTmp[i][j];
		}
	}

	/* total lamdaの計算 */
	iArrayLen = pfsEstNoisePSpec->ha_numCh * (pfsEstNoisePSpec->ha_iUpperFreq - pfsEstNoisePSpec->ha_iLowerFreq + 1);
	for (i = 0; i < iArrayLen; i++) {
		pfsEstNoisePSpec->micary_faSpec.ha_fValue[i] = pfsStateNoisePSpec->micary_faSpec.ha_fValue[i]
				+ pfsLeakPSpec->micary_faSpec.ha_fValue[i]
				+ pfsOther_rev->micary_faSpec.ha_fValue[i];
	}

	/* 作業用2次元配列ポインタのエリア開放 */
	free(phfOther_rev);

	/* other_revの開放 */
	FreeFloatSpecArray(pfsOther_rev);
	free(pfsOther_rev);

	/* sum_revのエリア開放 */
	free(phfSum_rev);

	/* 作業用2次元配列ポインタのエリア開放 */
	free(phfTmp);

	return 1;
}
