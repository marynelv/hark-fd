// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_leak.c
 \brief \~English Estimation leak spectrum function
 \brief \~Japanese 漏れ推定処理関数
 \author Masatoshi Tsutsumi
 \date 28 Nov 2006
 \version $Id: PF_est_leak.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "ArrayInit.h"
#include "PF_est_leak.h"
// ---------------------------------------------------------------------
/*
function lamda_leak = Est_leak_valin(S, alp_leak)
% lamda_leak = Est_leak_valin(S, alp_leak)
% Estimation leak spectrum function
% based on Jean-Marc VALIN, "Auditory System for a Mobile Robot", 2 juin 2005
%
% Inputs:
%   S :Smoothed Input power spectrums (N, M) by MCRA
%      ( N:#Freqeucy Lines, M:#Channels )
% alp_leak: Leak parameter (used like 'eta')
%
% Note:
%   Some parameters can be set to [], and initialized
%
% Outputs:
%   lamda_leak: Estimated leak power spectrum (N, M)

if isempty(alp_leak), alp_leak = 0.25*1;, end

[N, M] = size(S);

S_sum = sum(S, 2); % Sum power of channel direction
S_leak = S_sum*ones(1, M) - S; % power sum of other channels

% Eq. analized from Yamamoto
beta = -alp_leak / (1 - alp_leak^2 + alp_leak*(1 - alp_leak)*(M - 2) );
alpha = 1 - (M - 1) * alp_leak * beta;
Z = alpha * S + beta * S_leak;
Z(Z < 1) = 1; % Clipping

% Calculation Z sum of other channels
Z_sum = sum(Z, 2); % Sum power of channel direction
lamda_leak = alp_leak * (Z_sum*ones(1, M) - Z); % power sum of other channels with coefficients
*/
//
int PF_EstLeak(
		MICARY_FloatSpecArray *pfsSmoothedPSpec,
		MICARY_PrmLeakNoise sPrmLeak,
		MICARY_FloatSpecArray *pfsEstLeakPSpec)
{
	MICARY_FloatSpecArray	*pfsS_leak, *pfsZ;
	HA_Float	**pha_fS, **pha_fS_leak, **pha_fZ, **pha_fLamda_leak;
	HA_Float	*pha_fS_sum, *pha_fZ_sum;
	HA_Float	ha_fAlpha, ha_fBeta;
	int	i, j;
	int	iFreq, iCh;
	int	iArrayLen;

	iCh = pfsSmoothedPSpec->ha_numCh;
	iFreq = pfsSmoothedPSpec->ha_iUpperFreq - pfsSmoothedPSpec->ha_iLowerFreq + 1;

	/* Sを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fS = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh;  i++) {
		pha_fS[i] = (HA_Float *)(&(pfsSmoothedPSpec->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* S_sumの確保 */
	pha_fS_sum = (HA_Float *)calloc((size_t)iFreq, sizeof(HA_Float *));

	/* S_sumの計算 */
	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			pha_fS_sum[j] = pha_fS_sum[j] + pha_fS[i][j];
		}
	}

	/* S_leakの確保 */
	pfsS_leak = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	// modified 2008.10.31 sachiko.suzuki
	// Initialize
        AllocFloatSpecArray(pfsS_leak,0,0,0);

	AllocFloatSpecArray(pfsS_leak, pfsSmoothedPSpec->ha_numCh, pfsSmoothedPSpec->ha_iLowerFreq, pfsSmoothedPSpec->ha_iUpperFreq);

	/* S_leakを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fS_leak = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh;  i++) {
		pha_fS_leak[i] = (HA_Float *)(&(pfsS_leak->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* S_leakの計算 */
	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			pha_fS_leak[i][j] = pha_fS_sum[j] - pha_fS[i][j];
		}
	}

	/* betaの計算 */
	ha_fBeta = (HA_Float)(-1.0 * sPrmLeak.ha_fAlp_leak / (1.0 - sPrmLeak.ha_fAlp_leak * sPrmLeak.ha_fAlp_leak + sPrmLeak.ha_fAlp_leak * (1.0 - sPrmLeak.ha_fAlp_leak) * ((HA_Float)(iCh - 2))));
	ha_fAlpha = (HA_Float)(1.0 - (HA_Float)(iCh - 1) * sPrmLeak.ha_fAlp_leak * ha_fBeta);

	/* Zの確保 */
	pfsZ = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	// modified 2008.10.31
	// Initialize
	AllocFloatSpecArray(pfsZ,0,0,0);

	AllocFloatSpecArray(pfsZ, pfsSmoothedPSpec->ha_numCh, pfsSmoothedPSpec->ha_iLowerFreq, pfsSmoothedPSpec->ha_iUpperFreq);

	iArrayLen = pfsSmoothedPSpec->ha_numCh * (pfsSmoothedPSpec->ha_iUpperFreq - pfsSmoothedPSpec->ha_iLowerFreq + 1);

	/* Zの計算 */
	for (i = 0; i < iArrayLen; i++) {
		pfsZ->micary_faSpec.ha_fValue[i] = ha_fAlpha * pfsSmoothedPSpec->micary_faSpec.ha_fValue[i] + ha_fBeta * pfsS_leak->micary_faSpec.ha_fValue[i];
		if (pfsZ->micary_faSpec.ha_fValue[i] < 1.0) {
			pfsZ->micary_faSpec.ha_fValue[i] = 1.0;
		}
	}

	/* Zを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fZ = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh;  i++) {
		pha_fZ[i] = (HA_Float *)(&(pfsZ->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Z_sumの確保 */
	pha_fZ_sum = (HA_Float *)calloc((size_t)iFreq, sizeof(HA_Float));

	/* Z_sumの計算 */
	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			pha_fZ_sum[j] = pha_fZ_sum[j] + pha_fZ[i][j];
		}
	}

	/* lamda_leakを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fLamda_leak = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh;  i++) {
		pha_fLamda_leak[i] = (HA_Float *)(&(pfsEstLeakPSpec->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* lamda_leakの計算 */
	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			pha_fLamda_leak[i][j] = sPrmLeak.ha_fAlp_leak * (pha_fZ_sum[j] - pha_fZ[i][j]);
		}
	}

	/* lamda_leakを2次元配列として扱うためのポインタの開放 */
	free(pha_fLamda_leak);

	/* Z_sumの確保 */
	free(pha_fZ_sum);

	/* Zを2次元配列として扱うためのポインタの開放 */
	free(pha_fZ);

	/* Zの開放 */
	FreeFloatSpecArray(pfsZ);
	free(pfsZ);

	/* S_leakを2次元配列として扱うためのポインタの確保と初期化 */
	free(pha_fS);

	/* S_leakの開放 */
	FreeFloatSpecArray(pfsS_leak);
	free(pfsS_leak);

	/* S_sumの開放 */
	free(pha_fS_sum);

	/* Sを2次元配列として扱うためのポインタの開放 */
	free(pha_fS_leak);
	
	
	return 1;
}
