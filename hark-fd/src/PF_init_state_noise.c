// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_init_state_noise.c
 \brief \~English Initial noise of MCRA
 \brief \~Japanese MCRA用初期ノイズ関数
 \author Masatoshi Tsutsumi
 \date 13 Dec 2006
 \version $Id: PF_init_state_noise.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>

#include "ArrayInit.h"
#include "PF_Init.h"
#include "PF_init_state_noise.h"
// ---------------------------------------------------------------------

/*
function [In_lamda, Tmp] = MCRA_In_Valin_v2(yp, Tmp, Prm)
% [In_lamda, Tmp] = MCRA_In_Valin_v2(yp, Tmp, Prm)
% MCRA function for input signal channel
%
% Inputs:
%   yp :Input power spectrum (N, M)
%   Prm : Parameter structure
%    .In_delta         : minimum clipping value of alp_d (1 ,1) [0.05]
%    .In_L             : Frame length of Smin update (1, 1) [150]
%    .In_alp_s         : Smoothing coefficient of input spectrum (1, 1) [0.7]
%    .In_noise_compense: valin spetical parameter (1,1) [1?]
%    .In_alp_dmin      : Minimum speech probability (1,1) [0.05]
%    .In_init_frame    : Initialize update frams (1,1) [100]
%   Tmp : Temporaly value structure saved by previous frame
%    .In_lamda         : Noise power specrum of previous frame (N, M) [0v]
%    .In_Smin0         : Minimum Spectrum of y of previous frame (N, M) [S0]
%    .In_Stmp0         : Temporary Spectrum of y of previous frame (N, M) [S0]
%    .In_S             : Smoothed power spectrum of previous frame (N, M) [0?]
%    .In_Sf            : Frequency smoothed spectrum (N, M) [0?] for debug output
%    .In_l             : Frame number (1, 1) [0]
%   
% Outputs:
%   In_lamda: Estimated noise power spectrum for input channels (N, M)
%   Tmp : Temporaly value structure used for next frame

[N_freq, N_ch] = size(yp);

Tmp.In_S = Prm.In_alp_s * Tmp.In_S + (1 - Prm.In_alp_s) * yp; % Eq. (33)
Tmp.In_Sf = 0.25*[zeros(1, N_ch) ; Tmp.In_S(1:end-1, :)] + 0.5 * Tmp.In_S + 0.25*[Tmp.In_S(2:end, :) ; zeros(1, N_ch)]; % Frequency bin smoothing
if rem(Tmp.In_l, Prm.In_L) == 0
    Tmp.In_Smin = minmat(Tmp.In_Stmp, Tmp.In_Sf);  % Eq. (36)
    Tmp.In_Stmp = Tmp.In_Sf; % Eq. (37)
else
    Tmp.In_Smin = minmat(Tmp.In_Smin, Tmp.In_Sf);
    Tmp.In_Stmp = minmat(Tmp.In_Stmp, Tmp.In_Sf);
end
I = ((Tmp.In_Sf < Tmp.In_Smin * Prm.In_delta) | Tmp.In_l < Prm.In_init_frame | (Tmp.In_lamda > Tmp.In_S)); % Eq. (37)+, (39) form Valin
alp_d = 1 / (Tmp.In_l + 1);
alp_d(alp_d < Prm.In_alp_dmin)  = Prm.In_alp_dmin;
alp_dc = alp_d * I; % Valin processing

In_lamda = (1 - alp_dc) .* Tmp.In_lamda+ alp_dc * Prm.In_noise_compense .* yp; % Eq. (30)
Tmp.In_lamda = In_lamda;

Tmp.In_l = Tmp.In_l + 1;
*/
//
int PF_InitStateNoise(
		MICARY_FloatSpecArray fsInputPSpec,
		MICARY_PrmInitMCRA sPrmInitMCRA,
		MICARY_SavedInitMCRA *psSavedInitMCRA,
		MICARY_FloatSpecArray *pfsInitNoisePSpec)
{
	HA_Float	fAlp_d, fAlp_dc;
	HA_Float	**pha_fInputPSpec, **pha_fS;
	HA_Float	**pha_fSf, **pha_fSmin, **pha_fStmp, **pha_fLamda;
	int	iCh, iFreq;
	int	i ,k;
        int     j;

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
		pha_fS[i] = (HA_Float *)(&(psSavedInitMCRA->micary_fsS.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* 時間軸方向のスムージング */
	for (i= 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			pha_fS[i][j] = (HA_Float)(sPrmInitMCRA.ha_fAlp_s * pha_fS[i][j] + (1.0 - sPrmInitMCRA.ha_fAlp_s) * pha_fInputPSpec[i][j]);
		}
	}

	/* 更新係数の計算 >>>>> */
	fAlp_d = (HA_Float)(1.0 / (HA_Float)(psSavedInitMCRA->ha_numProc + 1));
	if (fAlp_d < sPrmInitMCRA.ha_fAlp_dmin) {
		fAlp_d = sPrmInitMCRA.ha_fAlp_dmin;
	}
	/* >>>>> 更新係数の計算 */

	/* Sfを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fSf = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fSf[i] = (HA_Float *)(&(psSavedInitMCRA->micary_fsSf.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* >>>>> 周波数方向のスムージング */
	for (i = 0; i < iCh; i++) {
		pha_fSf[i][0] = (HA_Float)(0.5 * pha_fS[i][0] + 0.25 * pha_fS[i][1]);
		for (j = 1; j < (iFreq - 1); j++) {
			pha_fSf[i][j] = (HA_Float)(0.25 * pha_fS[i][j - 1] + 0.5 * pha_fS[i][j] + 0.25 * pha_fS[i][j + 1]);
		}
		pha_fSf[i][iFreq - 1] = (HA_Float)(0.25 * pha_fS[i][iFreq - 2] + 0.5 * pha_fS[i][iFreq - 1]);
	}
	/* 周波数方向のスムージング >>>>> */

	/* Sminを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fSmin = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fSmin[i] = (HA_Float *)(&(psSavedInitMCRA->micary_fsSmin.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* Stmpを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fStmp = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fStmp[i] = (HA_Float *)(&(psSavedInitMCRA->micary_fsStmp.micary_faSpec.ha_fValue[iFreq * i]));
	}
        
       	/* 最小値更新 >>>>> */
	if ((psSavedInitMCRA->ha_numProc% sPrmInitMCRA.ha_iL) == 0) {
		for (i = 0; i < iCh; i++) {
			for (j = 0; j < iFreq; j++) {
				// add minmat(Sachiko suzuki)
				pha_fSmin[i][j] = minmat(pha_fStmp[i][j] ,pha_fSf[i][j]) ;
			//	pha_fSmin[i][j] = min(pha_fStmp[i][j], pha_fSf[i][j]);
				pha_fStmp[i][j] = pha_fSf[i][j];
			}
		}
	}
	else {
		for (i = 0; i < iCh; i++) {
			for (j = 0; j < iFreq; j++) {
				pha_fSmin[i][j] = minmat(pha_fSmin[i][j], pha_fSf[i][j]);
				pha_fStmp[i][j] = minmat(pha_fStmp[i][j], pha_fSf[i][j]);
				//pha_fSmin[i][j] = min(pha_fSmin[i][j], pha_fSf[i][j]);
				//pha_fStmp[i][j] = min(pha_fStmp[i][j], pha_fSf[i][j]);
			}
		}
	}
	/* >>>>> 最小値更新 */

	
         /* In_lamdaを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fLamda = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	//	pha_fLamda = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fLamda[i] = (HA_Float *)(&(psSavedInitMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* 雑音計算 >>>>> */
	k = 0 ;
       	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			/* 音声有無判断 >>>>> */
			if ((pha_fSf[i][j] < pha_fSmin[i][j] * sPrmInitMCRA.ha_fDelta)
				|| (psSavedInitMCRA->ha_numProc < sPrmInitMCRA.ha_iInit_frame) 
					|| (pha_fLamda[i][j] > pha_fS[i][j])) {
				fAlp_dc = fAlp_d;
			}
			else {
				fAlp_dc = 0.0;
			}
			/* >>>>> 音声有無判断 */

			pha_fLamda[i][j] = (HA_Float)((1.0 - fAlp_dc) * pha_fLamda[i][j]
						+ fAlp_dc * sPrmInitMCRA.ha_fNoise_compense * pha_fInputPSpec[i][j]);
			//add by Sachiko suzuki(2007/02/26)
			pfsInitNoisePSpec->micary_faSpec.ha_fValue[k] = pha_fLamda[i][j] ;
			k ++ ;
		}
	}
	/* >>>>> 雑音計算 */
	psSavedInitMCRA->ha_numProc ++ ;
	//sPrmInitMCRA.ha_iL++;

	/* In_lamdaを2次元配列として扱うためのポインタの開放 */
	free(pha_fLamda);

	/* Stmpを2次元配列として扱うためのポインタの開放 */
	free(pha_fStmp);

	/* Sminを2次元配列として扱うためのポインタの開放 */
	free(pha_fSmin);

	/* Sfを2次元配列として扱うためのポインタの開放 */
	free(pha_fSf);

	/* Sを2次元配列として扱うためのポインタの開放 */
	free(pha_fS);

	/* ypを2次元配列として扱うためのポインタの開放 */
	free(pha_fInputPSpec);

	return 1;
}
