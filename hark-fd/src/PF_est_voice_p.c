// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_voice_p.c
 \brief \~English Estimate speech present probability
 \brief \~Japanese 音声存在確率の推定
 \author Masatoshi Tsutsumi
 \date 1 Dec 2006
 \version $Id: PF_est_voice_p.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <math.h>

#include "ArrayInit.h"
#include "PF_est_voice_p.h"
#include "PF_est_zeta.h"
// ---------------------------------------------------------------------
/*
function [p, zeta, zetaF, zpeak] = Est_prosp_valin(xi, v, zeta0, b, zetaF0, zpeak0, OP)
% [p, zeta, zetaF, zpeak] = Est_prosp_valin(xi, v, zeta0, b, zetaF0, zpeak0, OP)
%
% Estimate speech present probability 
% based on Jean-Marc VALIN, "Auditory System for a Mobile Robot", 2 juin 2005
% 
% Input:
%   xi      : Priori SNR (Estimated) (N, M)
%             ( N:#Freqeucy Lines, M:#Channels )
%   v       : Variable for optimum gain estimator (output of Est_GH1)
%   zeta0   : Smoothed priori SNR by time & freqnency of previous frame (N, M) [xi]
%   b       : smoothind parameter, new valin
%   zetaF0  : zeta_frame of previous frame (M, 1)
%   zpeak0  : zeta_frame peak of previous frame (M, 1)
%   OP      : optional parameters
%    .zminF : zeta minimum for frame
%    .zmaxF : zeta maximum for frame
%    .zminG : zeta minimum for global
%    .zmaxG : zeta maximum for global
%    .zminL : zeta minimum for local
%    .zmaxL : zeta maximum for local
%
% Output:
%   p       : Probability of speech present (N, M)
%   zeta    : Smoothed priori SNR by time & freqnency (N, M) [xi]
%   zetaF   : zeta_frame frame (M, 1)
%   zpeak   : zeta_frame peak (M, 1)


[N, M] = size(xi);
    
%Estimate zeta local, global, frame in valin
[zeta, zetaL, zetaG, zetaF] = Est_zeta_valin_v2(xi, zeta0, b, OP);

%for P_frame
zpeak = zpeak0;
PF = zeros(M, 1);
for m = 1:M
    if zetaF(m) < OP.zminF
        PF(m) = 0;
    elseif zetaF(m) > OP.zthF * zetaF0(m)
        PF(m) = 1;
        zpeak(m) = zetaF(m);
        if zpeak(m) < OP.zpmin, zpeak(m) = OP.zpmin;, end
        if zpeak(m) > OP.zpmax, zpeak(m) = OP.zpmax;, end
    elseif zetaF(m) < zpeak(m) * OP.zminF
        PF(m) = 0;
    elseif zetaF(m) > zpeak(m) * OP.zmaxF
        PF(m) = 1;
    else
            PF(m) = log(zetaF(m)/(zpeak(m) * OP.zminF)) / log(OP.zmaxF / OP.zminF);
    end
end

% for P_local
PL = zeros(N, M);
PL(zetaL > OP.zmaxL) = 1;
PL(zetaL < OP.zminL) = 0;
ind = (zetaL <= OP.zmaxL) & (zetaL >= OP.zminL);
PL(ind) = log(zetaL(ind) / OP.zminL)/log(OP.zmaxL / OP.zminL);

% for P_global
PG = zeros(N, M);
PG(zetaG > OP.zmaxG) = 1;
PG(zetaG < OP.zminG) = 0;
ind = (zetaG <= OP.zmaxG) & (zetaG >= OP.zminG);
PG(ind) = log(zetaG(ind) / OP.zminG)/log(OP.zmaxG / OP.zminG);

%Estimate Speech absence probability in Eq. 3.30
q = 1 - (1-OP.ampL + OP.ampL * PL) .* (1-OP.ampG + OP.ampG * PG) .* (1-OP.ampF + OP.ampG * (PF*ones(1,N)).');
q(q > OP.qmax) = OP.qmax;
q(q < OP.qmin) = OP.qmin;

%Estimate Speech present probability in Eq. 3.29
p = 1./(1 + q./(1-q).*(1 + xi).*exp(-v) );
*/
//
int PF_EstVoiceP(
		MICARY_FloatSpecArray fsPreSN,
		MICARY_FloatSpecArray fsEstVal,
		MICARY_PrmVoiceP sPrmVoiceP,
		MICARY_SavedVoiceP *psSavedVoiceP,
		MICARY_FloatSpecArray *pfsVoiceP,
		MICARY_FloatSpecArray *pfsSmSNR,
		MICARY_FloatArray *pfaSmSNRF,
		MICARY_FloatArray *pfaSmSNRP)
{
	MICARY_PrmZeta	sPrmZeta;
	MICARY_FloatSpecArray	*pfsSmSNRG, *pfsSmSNRL, *pfsPL, *pfsPG, *pfsQ;
	MICARY_FloatArray	*pfaPF;
	HA_Float	**pha_fZetaL, **pha_fZetaG;
	HA_Float	**pha_fPL, **pha_fPG, **pha_fQ, **pha_fVoiceP, **pha_fPreSN, **pha_fEstVal;
	
	//09/1/27追加 Kenichi Morio 出力用バッファ（他に影響を与えないように修正するため）
	MICARY_FloatSpecArray *pfsSmSNRbuf;
	MICARY_FloatArray *pfaSmSNRFbuf, *pfaSmSNRPbuf;
    //

	int	i, j;
	int	iFreq, iCh;
	int	rc;

	iCh = fsPreSN.ha_numCh;
	iFreq = fsPreSN.ha_iUpperFreq - fsPreSN.ha_iLowerFreq + 1;

	/* 平滑化 */
	sPrmZeta.ha_fB = sPrmVoiceP.ha_fB;			///\~Japanese 時間平滑化係数(b)
	sPrmZeta.ha_fBwFst = sPrmVoiceP.ha_fBwFst;	///\~Japanese フレーム平均で矩形窓を適用する下限周波数ビン(bwFst)
	sPrmZeta.ha_fBwFen = sPrmVoiceP.ha_fBwFen;	///\~Japanese フレーム平均で矩形窓を適用する上限周波数ビン(bwFen)
	sPrmZeta.ha_fBwG = sPrmVoiceP.ha_fBwG;		///\~Japanese 広帯域平均幅(bwG)
	sPrmZeta.ha_fBwL = sPrmVoiceP.ha_fBwL;		///\~Japanese 狭帯域平均で矩形窓幅(bwL)

	/* ポインタ変数 zetaG, zetaL, PF, PG, PL, Qの確保 */
	pfsSmSNRG = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	pfsSmSNRL = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	pfaPF = (MICARY_FloatArray *)calloc((size_t)1, sizeof(MICARY_FloatArray));
	pfsPL = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	pfsPG = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	pfsQ = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));

	AllocFloatSpecArray(pfsSmSNRG, iCh, fsPreSN.ha_iLowerFreq, fsPreSN.ha_iUpperFreq);
	AllocFloatSpecArray(pfsSmSNRL, iCh, fsPreSN.ha_iLowerFreq, fsPreSN.ha_iUpperFreq);

	/* 出力用バッファ*/
	pfsSmSNRbuf=(MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	pfaSmSNRFbuf=(MICARY_FloatArray *)calloc((size_t)1, sizeof(MICARY_FloatArray));
	pfaSmSNRPbuf=(MICARY_FloatArray *)calloc((size_t)1, sizeof(MICARY_FloatArray));
	AllocFloatSpecArray(pfsSmSNRbuf, iCh, fsPreSN.ha_iLowerFreq, fsPreSN.ha_iUpperFreq);
	FloatArrayAlloc(pfaSmSNRFbuf,iCh);
	FloatArrayAlloc(pfaSmSNRPbuf,iCh);
	/* 09/1/27 変更 Kenichi Morio */

	
	/* rc = PF_EstZeta(fsPreSN, sPrmZeta, psSavedVoiceP->micary_fsSmSNR, pfsSmSNR, pfsSmSNRL, pfsSmSNRG, pfaSmSNRF); */
	rc = PF_EstZeta(fsPreSN, sPrmZeta, psSavedVoiceP->micary_fsSmSNR, pfsSmSNRbuf, pfsSmSNRL, pfsSmSNRG, pfaSmSNRFbuf);
	/* 09/1/27 変更 Kenichi Morio */
	
	
	/* PFのエリアの確保 */
	FloatArrayAlloc(pfaPF, iCh);

	/* PLのエリアの確保 */
	AllocFloatSpecArray(pfsPL, iCh, fsPreSN.ha_iLowerFreq, fsPreSN.ha_iUpperFreq);

	/* PGのエリアの確保 */
	AllocFloatSpecArray(pfsPG, iCh, fsPreSN.ha_iLowerFreq, fsPreSN.ha_iUpperFreq);

	/* qのエリアの確保 */
	AllocFloatSpecArray(pfsQ, iCh, fsPreSN.ha_iLowerFreq, fsPreSN.ha_iUpperFreq);

	/* zetaGを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fZetaG = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fZetaG[i] = (HA_Float *)(&(pfsSmSNRG->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* zetaLを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fZetaL = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fZetaL[i] = (HA_Float *)(&(pfsSmSNRL->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* PLを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fPL = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fPL[i] = (HA_Float *)(&(pfsPL->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* PGを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fPG = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fPG[i] = (HA_Float *)(&(pfsPG->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* qを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fQ = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fQ[i] = (HA_Float *)(&(pfsQ->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* pを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fVoiceP = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fVoiceP[i] = (HA_Float *)(&(pfsVoiceP->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* xiを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fPreSN = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fPreSN[i] = (HA_Float *)(&(fsPreSN.micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* xiを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fEstVal = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fEstVal[i] = (HA_Float *)(&(fsEstVal.micary_faSpec.ha_fValue[iFreq * i]));
	}

	// pfaSmSNRPが更新されない場合もあるため追加 09/1/27 Kenichi Morio
	memcpy(pfaSmSNRPbuf->ha_fValue,psSavedVoiceP->micary_faSmSNRP.ha_fValue,sizeof(HA_Float)*psSavedVoiceP->micary_faSmSNRP.ha_numValue);

	// 以下，バッファを使用して変更 09/1/27 Kenichi Morio

	// zpeakの追加(sachiko.suzuki::SavedVoicePの値は昔)
	for (i = 0; i < iCh; i++) {
		
		if (pfaSmSNRFbuf->ha_fValue[i] < sPrmVoiceP.ha_fZminF) {
			pfaPF->ha_fValue[i] = 0.0;
		}
		else if (pfaSmSNRFbuf->ha_fValue[i] > (sPrmVoiceP.ha_fZthF * psSavedVoiceP->micary_faSmSNRF.ha_fValue[i])) {
			pfaPF->ha_fValue[i] = 1.0;
			//psSavedVoiceP->micary_faSmSNRP.ha_fValue[i] = pfaSmSNRFbuf->ha_fValue[i];
			pfaSmSNRPbuf->ha_fValue[i] = pfaSmSNRFbuf ->ha_fValue[i] ;
			//if (psSavedVoiceP->micary_faSmSNRP.ha_fValue[i] < sPrmVoiceP.ha_fZpmin) {
			//	psSavedVoiceP->micary_faSmSNRP.ha_fValue[i] = sPrmVoiceP.ha_fZpmin;
			if (pfaSmSNRPbuf->ha_fValue[i] < sPrmVoiceP.ha_fZpmin) {
				pfaSmSNRPbuf->ha_fValue[i] = sPrmVoiceP.ha_fZpmin ;
			}
			//if (psSavedVoiceP->micary_faSmSNRP.ha_fValue[i] > sPrmVoiceP.ha_fZpmax) {
			//	psSavedVoiceP->micary_faSmSNRP.ha_fValue[i] = sPrmVoiceP.ha_fZpmax;
			if (pfaSmSNRPbuf->ha_fValue[i] > sPrmVoiceP.ha_fZpmax) {
				pfaSmSNRPbuf ->ha_fValue[i] = sPrmVoiceP.ha_fZpmax ;
			}
		}
		else if (pfaSmSNRFbuf->ha_fValue[i] < (psSavedVoiceP->micary_faSmSNRP.ha_fValue[i] * sPrmVoiceP.ha_fZminF)) {
			pfaPF->ha_fValue[i] = 0.0;
		}
		else if (pfaSmSNRFbuf->ha_fValue[i] > (psSavedVoiceP->micary_faSmSNRP.ha_fValue[i] * sPrmVoiceP.ha_fZmaxF)) {
			pfaPF->ha_fValue[i] = 1.0;
		}
		else {
			pfaPF->ha_fValue[i] = (HA_Float)(log10(pfaSmSNRFbuf->ha_fValue[i] / (psSavedVoiceP->micary_faSmSNRP.ha_fValue[i] * sPrmVoiceP.ha_fZminF)) / log10(sPrmVoiceP.ha_fZmaxF / sPrmVoiceP.ha_fZminF));
		}
		
		/* >>>>> Frameの音声確率の計算 */

		for (j = 0; j < iFreq; j++) {
			/* Localの音声確率の計算 >>>>> */
			if (pha_fZetaL[i][j] > sPrmVoiceP.ha_fZmaxL) {
				pha_fPL[i][j] = 1.0;
			}
			else if (pha_fZetaL[i][j] < sPrmVoiceP.ha_fZminL) {
				pha_fPL[i][j] = 0.0;
			}
			else {
				pha_fPL[i][j] = (HA_Float)(log10(pha_fZetaL[i][j] / sPrmVoiceP.ha_fZminL) / log10(sPrmVoiceP.ha_fZmaxL / sPrmVoiceP.ha_fZminL));
			}
			/* >>>>> Localの音声確率の計算 */

			/* Globalの音声確率の計算 >>>>> */
			if (pha_fZetaG[i][j] > sPrmVoiceP.ha_fZmaxG) {
				pha_fPG[i][j] = 1.0;
			}
			else if (pha_fZetaG[i][j] < sPrmVoiceP.ha_fZminG) {
				pha_fPG[i][j] = 0.0;
			}
			else {
				pha_fPG[i][j] = (HA_Float)(log10(pha_fZetaG[i][j] / sPrmVoiceP.ha_fZminG) / log10(sPrmVoiceP.ha_fZmaxG / sPrmVoiceP.ha_fZminG));
			}
			/* >>>>> Globalの音声確率の計算 */

			/* >>>>> 音声休止確率の計算 */
			pha_fQ[i][j] = (HA_Float)(1.0 - (1.0 - sPrmVoiceP.ha_fAmpL + sPrmVoiceP.ha_fAmpL * pha_fPL[i][j])
						* (1.0 - sPrmVoiceP.ha_fAmpG + sPrmVoiceP.ha_fAmpG * pha_fPG[i][j])
						* (1.0 - sPrmVoiceP.ha_fAmpF + sPrmVoiceP.ha_fAmpF * pfaPF->ha_fValue[i]));
			if (pha_fQ[i][j] > sPrmVoiceP.ha_fQmax) {
				pha_fQ[i][j] = sPrmVoiceP.ha_fQmax;
			}
			if (pha_fQ[i][j] < sPrmVoiceP.ha_fQmin) {
				pha_fQ[i][j] = sPrmVoiceP.ha_fQmin;
			}
			/* 音声休止確率の計算 >>>>> */

			/* 音声存在確率の計算 >>>>> */
			pha_fVoiceP[i][j] = (HA_Float)(1.0 / (1.0 + pha_fQ[i][j] / (1.0 - pha_fQ[i][j]) * (1.0 + pha_fPreSN[i][j]) * exp(-1.0 * pha_fEstVal[i][j])));
			/* >>>>> 音声存在確率の計算 */
		}
	}

	memcpy(pfsSmSNR->micary_faSpec.ha_fValue,pfsSmSNRbuf->micary_faSpec.ha_fValue,sizeof(HA_Float)*pfsSmSNRbuf->micary_faSpec.ha_numValue);
	memcpy(pfaSmSNRF->ha_fValue,pfaSmSNRFbuf->ha_fValue,sizeof(HA_Float)*iCh);
	memcpy(pfaSmSNRP->ha_fValue,pfaSmSNRPbuf->ha_fValue,sizeof(HA_Float)*iCh);
	memcpy(psSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_fValue,pfsSmSNRbuf->micary_faSpec.ha_fValue,sizeof(HA_Float)*pfsSmSNRbuf->micary_faSpec.ha_numValue);
	memcpy(psSavedVoiceP->micary_faSmSNRF.ha_fValue,pfaSmSNRFbuf->ha_fValue,sizeof(HA_Float)*iCh);
	memcpy(psSavedVoiceP->micary_faSmSNRP.ha_fValue,pfaSmSNRPbuf->ha_fValue,sizeof(HA_Float)*iCh);

	/* vを2次元配列として扱うためのポインタの開放 */
	free(pha_fEstVal);

	/* xiを2次元配列として扱うためのポインタの開放 */
	free(pha_fPreSN);

	/* pを2次元配列として扱うためのポインタの開放 */
	free(pha_fVoiceP);

	/* qを2次元配列として扱うためのポインタの開放 */
	free(pha_fQ);

	/* PGを2次元配列として扱うためのポインタの開放 */
	free(pha_fPG);

	/* PLを2次元配列として扱うためのポインタの開放 */
	free(pha_fPL);

	/* zetaLを2次元配列として扱うためのポインタの開放 */
	free(pha_fZetaL);

	/* zetaGを2次元配列として扱うためのポインタの開放 */
	free(pha_fZetaG);

	/* qのエリアの確保 */
	FreeFloatSpecArray(pfsQ);

	/* PGのエリアの確保 */
	FreeFloatSpecArray(pfsPG);

	/* PLのエリアの確保 */
	FreeFloatSpecArray(pfsPL);

	/* PFのエリアの開放 */
	FloatArrayFree(pfaPF);

	/* ポインタ変数 zetaG, zetaL, PF, PG, PL, Qの開放 */
	free(pfsQ);
	free(pfsPL);
	free(pfsPG);
	free(pfaPF);
	FreeFloatSpecArray(pfsSmSNRL);
	free(pfsSmSNRL);
	FreeFloatSpecArray(pfsSmSNRG);
	free(pfsSmSNRG);

	/* 追加 09/01/27 Kenichi Morio */
	FreeFloatSpecArray(pfsSmSNRbuf);
	free(pfsSmSNRbuf);
	FloatArrayFree(pfaSmSNRFbuf);
	free(pfaSmSNRFbuf);
	FloatArrayFree(pfaSmSNRPbuf);
	free(pfaSmSNRPbuf);

	return 1;
}
