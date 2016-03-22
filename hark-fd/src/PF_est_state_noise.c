// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_state_noise.c
 \brief \~English Estimate MCRA
 \brief \~Japanese 定常雑音の推定
 \author Masatoshi Tsutsumi
 \date 5 Dec 2006
 \version $Id: PF_est_state_noise.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>

#include "ArrayInit.h"
#include "PF_est_state_noise.h"
// ---------------------------------------------------------------------

/*
function [lamda, Smin, Stmp, S] = MCRA_Valin_v1(yp, lamda0, alp_d, delta, Smin0, Stmp0, l, L, alp_s, S0, leak_ampleak_factor, ss_floor, noise_compense)
% [lamda, Smin, Stmp, S] = MCRA_Valin_v1(yp, lamda0, alp_d, delta, Smin0, Stmp0, l, L, alp_s, S0, leak_ampleak_factor, ss_floor, noise_compense)
% MCRA function (with fix coefficient frequency axis direction smoothing )
% based on I.Cohen et., al "Speech enhancement for non-stationary
% environments", and Valins'
%
% Inputs:
%   yp :Input power spectrum (N, M)
%   lamda0:Noise power specrum of previous frame (N, M) [0v]
%   alp_d:Smoothing coefficient of noise (1, 1) [0.98]
%   delta:Speech present threshold (1,1 ) [3]
%   Smin0:Minimum Spectrum of y of previous frame (N, M) [S0]
%   Stmp0:Temporary Spectrum of y of previous frame (N, M) [S0]
%   l: Frame number (1, 1) [0]
%   L: Frame length of Smin update (1, 1) [125]
%   alp_s: Smoothing coefficient of input spectrum (1, 1) [0.5]
%   S0:Smoothed power spectrum of previous frame (N, M) [yp]
%   leak_ampleak_factor: (1, 1) [0.25 * 1.5], leak amount factor
%   ss_floor: (1, 1) [0.1], spectrum subtration floor value
%   noise_compense (1,1) [1.2], valin spetical parameter
% Note:
%   Some parameters can be set to [], and initialized
%
% Outputs:
%   lamda: Estimated noise power spectrum (N, M)
%   Smin:Minimum Spectrum of yp
%   Stmp:Temporary Spectrum of yp
%   S: Smoothed spectrum of frame
%   

[N_freq, N_ch] = size(yp);
if isempty(lamda0), lamda0 = zeros(size(yp));, end
if isempty(alp_d), alp_d = 0.98;, end % from Valin value
if isempty(delta), delta = 3;, end % from valin value
if isempty(Smin0), Smin0 = zeros(size(yp));, end % from valin value
if isempty(Stmp0), Stmp0 = zeros(size(yp));, end % from valin value
if isempty(l), l = 0;, end % from Valin value
if isempty(L), L = 80;, end % from Valin value
if isempty(alp_s), alp_s = 0.5;, end % from Valin value
if isempty(S0), S0 = yp;, end % different from Valin value, Difficult to match
if isempty(leak_ampleak_factor), leak_ampleak_factor = 0.25 * 1.5;, end % from Valin value
if isempty(ss_floor), ss_floor = 0.1;, end % from Valin value
if isempty(noise_compense), noise_compense = 1.2;, end % from Valin value


S = alp_s*S0 + (1-alp_s)*yp; % Eq. (33)
Sf = 0.25*[zeros(1, N_ch) ; S(1:end-1, :)] + 0.5*S + 0.25*[S(2:end, :) ; zeros(1, N_ch)]; % Frequency bin smoothing
if rem(l, L) == 0
    Smin = minmat(Stmp0, Sf);  % Eq. (36)
    Stmp = Sf; % Eq. (37)
else
    Smin = minmat(Smin0, Sf);
    Stmp = minmat(Stmp0, Sf);
end
I = ((Sf < Smin*delta) | (Sf < lamda0)); % Eq. (37)+, (39) form Valin

alp_dc = (alp_d - 1)*I + 1; % Valin processing

%leak estimation & subtraction
yh_sum = sum(yp, 2); % Sum power of channel direction
yh_leak = yh_sum*ones(1, N_ch) - yp; % power sum of other channels
yh = yp - yh_leak * leak_ampleak_factor; % leak subtraction
yh(yh < ss_floor) = ss_floor; % floor clipping

lamda = alp_dc.*lamda0+ (1 - alp_dc)*noise_compense.*yh; % Eq. (30)
*/
//
int PF_EstStateNoise(
		MICARY_FloatSpecArray fsInputPSpec,
		MICARY_PrmMCRA sPrmMCRA,
		MICARY_SavedMCRA *psSavedMCRA,
		MICARY_FloatSpecArray *pfsEstNoisePSpec,
		MICARY_FloatSpecArray *pfsSmin,
		MICARY_FloatSpecArray *pfsStmp,
		MICARY_FloatSpecArray *pfsS)
{
	MICARY_FloatSpecArray	*pfsSf, *pfsI;
	MICARY_FloatArray	*pfaYh_sum;
	HA_Float	**pha_fInputPSpec, **pha_fS, **pha_fSf, **pha_fSmin, **pha_fStmp, **pha_fI, **pha_fLamda;
	HA_Float	**pha_fS0, **pha_fSmin0, **pha_fStmp0, **pha_fLamda0;
	HA_Float	fAlp_dc, fYh_leak, fYh;
	int	iCh, iFreq;
	int	i, j;

	iCh = fsInputPSpec.ha_numCh;
	iFreq = fsInputPSpec.ha_iUpperFreq - fsInputPSpec.ha_iLowerFreq + 1;

	/* ypを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fInputPSpec = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fInputPSpec[i] = (HA_Float *)(&(fsInputPSpec.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Sを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fS = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fS[i] = (HA_Float *)(&(pfsS->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* S0を2次元配列として扱うためのポインタの確保と初期化 */
	pha_fS0 = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fS0[i] = (HA_Float *)(&(psSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* 時間軸方向oのスムージング */
	for (i= 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			pha_fS[i][j] = (HA_Float)(sPrmMCRA.ha_fAlp_s * pha_fS0[i][j] + (1.0 - sPrmMCRA.ha_fAlp_s) * pha_fInputPSpec[i][j]);
			pha_fS0[i][j] = pha_fS[i][j];
		}
	}

	/* yh_sumのエリアの確保 */
	pfaYh_sum = (MICARY_FloatArray *)calloc((size_t)1, sizeof(MICARY_FloatArray));
	// modified 2008.10.31
	FloatArrayAlloc(pfaYh_sum,0);

	FloatArrayAlloc(pfaYh_sum, iFreq);

	/* 前chの入力パワーをFreq毎に加算する */
	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			pfaYh_sum->ha_fValue[j] = (HA_Float)(pfaYh_sum->ha_fValue[j] + pha_fInputPSpec[i][j]);
		}
	}

	/* Sfのエリアの確保(N,M) */
	pfsSf = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	// modified 2008.10.31
	AllocFloatSpecArray(pfsSf,0,0,0);

	AllocFloatSpecArray(pfsSf, iCh, fsInputPSpec.ha_iLowerFreq, fsInputPSpec.ha_iUpperFreq);

	/* Sfを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fSf = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fSf[i] = (HA_Float *)(&(pfsSf->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Sminを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fSmin = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fSmin[i] = (HA_Float *)(&(pfsSmin->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Stmpを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fStmp = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fStmp[i] = (HA_Float *)(&(pfsStmp->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Smin0を2次元配列として扱うためのポインタの確保と初期化 */
	pha_fSmin0 = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fSmin0[i] = (HA_Float *)(&(psSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Stmp0を2次元配列として扱うためのポインタの確保と初期化 */
	pha_fStmp0 = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fStmp0[i] = (HA_Float *)(&(psSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Iのエリアの確保(N,M) */
	pfsI = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	// modified 2008.10.31

	AllocFloatSpecArray(pfsI,0,0,0);

	AllocFloatSpecArray(pfsI, iCh, fsInputPSpec.ha_iLowerFreq, fsInputPSpec.ha_iUpperFreq);

	/* Iを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fI = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fI[i] = (HA_Float *)(&(pfsI->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Lamda0を2次元配列として扱うためのポインタの確保と初期化 */
	pha_fLamda0 = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fLamda0[i] = (HA_Float *)(&(psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Lamdaを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fLamda = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fLamda[i] = (HA_Float *)(&(pfsEstNoisePSpec->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* 周波数軸方向のスムージング >>>>> */
	for (i = 0; i < iCh; i++) {
		pha_fSf[i][0] = (HA_Float)(0.5 * pha_fS[i][0] + 0.25 * pha_fS[i][1]);
		for (j = 1; j < (iFreq - 1); j++) {
			pha_fSf[i][j] = (HA_Float)(0.25 * pha_fS[i][j - 1] + 0.5 * pha_fS[i][j] + 0.25 * pha_fS[i][j + 1]);
		}
		pha_fSf[i][iFreq - 1] = (HA_Float)(0.25 * pha_fS[i][iFreq - 2] + 0.5 * pha_fS[i][iFreq - 1]);
	}
	/* >>>>> 周波数軸方向のスムージング */

	/* 最小値更新 >>>>> */
	if (((psSavedMCRA->ha_numProc+1) % sPrmMCRA.ha_iL) == 0) {
		for (i = 0; i < iCh; i++) {
			for (j = 0; j < iFreq; j++) {
				//pha_fSmin[i][j] = min(pha_fStmp0[i][j], pha_fSf[i][j]);
				pha_fSmin[i][j] = minmat(pha_fStmp0[i][j], pha_fSf[i][j]);
				pha_fStmp[i][j] = pha_fSf[i][j];
			}
		}
	}
	else {
		for (i = 0; i < iCh; i++) {
			for (j = 0; j < iFreq; j++) {
				pha_fSmin[i][j] = minmat(pha_fSmin0[i][j], pha_fSf[i][j]);
				pha_fStmp[i][j] = minmat(pha_fStmp0[i][j], pha_fSf[i][j]);
				//pha_fSmin[i][j] = min(pha_fSmin0[i][j], pha_fSf[i][j]);
				//pha_fStmp[i][j] = min(pha_fStmp0[i][j], pha_fSf[i][j]);
			}
		}
	}
	/* >>>>> 最小値更新 */

	/* 雑音計算 >>>>> */
	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			/* 音声有無と雑音更新率の計算 >>>>> */
			if ((pha_fSf[i][j] < (pha_fSmin[i][j] * sPrmMCRA.ha_fDelta)) || (pha_fSf[i][j] < pha_fLamda0[i][j])) {
				fAlp_dc = (HA_Float)((sPrmMCRA.ha_fAlp_d - 1.0) + 1.0);
			}
			else {
				fAlp_dc = 1.0;
			}
			/* >>>>> 音声有無と雑音更新率の計算 */

			/* 他chの漏れ >>>>> */
			fYh_leak = pfaYh_sum->ha_fValue[j] - pha_fInputPSpec[i][j];
			fYh = pha_fInputPSpec[i][j] - fYh_leak * sPrmMCRA.ha_fLeak_ampleak_factor;
			if (fYh < sPrmMCRA.ha_fSs_floor) {
				fYh = sPrmMCRA.ha_fSs_floor;
			}
			/* >>>>> 他chの漏れ */

			pha_fLamda[i][j] = (HA_Float)(fAlp_dc * pha_fLamda0[i][j] + (1.0 - fAlp_dc) * sPrmMCRA.ha_fNoise_compense * fYh);
		}
	}
	/* >>>>> 雑音計算 */

	/* Lamdaを2次元配列として扱うためのポインタの開放 */
	free(pha_fLamda);

	/* Lamda0を2次元配列として扱うためのポインタの開放 */
	free(pha_fLamda0);

	/* Iを2次元配列として扱うためのポインタの開放 */
	free(pha_fI);

	/* Iのエリアの開放 */
	FreeFloatSpecArray(pfsI);
	free(pfsI);

	/* Stmp0を2次元配列として扱うためのポインタの開放 */
	free(pha_fStmp0);

	/* Smin0を2次元配列として扱うためのポインタの開放 */
	free(pha_fSmin0);

	/* Stmpを2次元配列として扱うためのポインタの開放 */
	free(pha_fStmp);

	/* Sminを2次元配列として扱うためのポインタの開放 */
	free(pha_fSmin);

	/* Sfを2次元配列として扱うためのポインタの開放 */
	free(pha_fSf);

	/* Sfのエリアの開放 */
	FreeFloatSpecArray(pfsSf);
	free(pfsSf);

	/* yh_sumのエリアの開放 */
	FloatArrayFree(pfaYh_sum);
	free(pfaYh_sum);

	/* S0を2次元配列として扱うためのポインタの開放 */
	free(pha_fS0);

	/* Sを2次元配列として扱うためのポインタの開放 */
	free(pha_fS);

	/* ypを2次元配列として扱うためのポインタの開放 */
	free(pha_fInputPSpec);

	return 1;
}
