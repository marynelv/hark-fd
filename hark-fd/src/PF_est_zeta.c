// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_zeta.c
 \brief \~English Estimate smoothed SNR for speech on/off judgement
 \brief \~Japanese 事前SN比の平滑化関数
 \author Masatoshi Tsutsumi
 \date 29 Nov 2006
 \version $Id: PF_est_zeta.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>

#include "ArrayInit.h"
#include "PF_est_zeta.h"
#include "MICARY_TimeWin.h"
#include "MICARY_GHDSS_struct.h"
// ---------------------------------------------------------------------
/*
function [zeta, zetaL, zetaG, zetaF] = Est_zeta_valin(xi, zeta0, b, OP)
% [zeta, zetaL, zetaG, zetaF] = Est_zeta_valin(xi, zeta0, b, OP)
% Estimate smoothed SNR for speech on/off judgement 
% based on Jean-Marc VALIN, "Auditory System for a Mobile Robot", 2 juin 2005
% 
% Input:
%   xi       : Priori SNR (Estimated) (N, M)
%             ( N:#Freqeucy Lines, M:#Channels )
%   zeta0    : Smoothed priori SNR by time & freqnency of previous frame (N, M) [xi]
%   b        : smoothind parameter, new valin
%   OP          : optional parameters
%    ... (detail is described at 'Postfilter_Init_valin_v2')
%
% Output:
%   zeta     : Smoothed priori SNR by time & freqnency of previous frame [xi]


N = size(xi, 1);
M = size(xi, 2);

zeta = b*zeta0 + (1-b)*xi;

% zeta_Frame
bwF = (OP.bwFen - OP.bwFst + 1);
zetaF = sum(zeta(OP.bwFst:OP.bwFen, :)) / bwF; % Flat for frame

% zeta_Global
bwG2 = floor(OP.bwG/2);
windG = hanning(OP.bwG); % hanning for global
windG = windG./sum(windG);
zetaEX = zeros(N+OP.bwG, M);
zetaEX(1+bwG2:N+bwG2, :) = zeta; 
for n = 1:N
    zetaG(n, :) = (zetaEX(n:n+OP.bwG-1, :).'* windG).';
end

% zeta_Local
bwL2 = floor(OP.bwL/2);
windL = ones(OP.bwL, 1); % rectangular for local
windL = windL./sum(windL);
zetaL = zeros(N, M);
for n = 1+bwL2:N-bwL2
    zetaL(n, :) = (zeta(n-bwL2:n+bwL2, :).'* windL).';
end
for n = 2:1+bwL2-1
    zetaL(n, :) = (zeta(n-1:n+1, :).'* [0.25; 0.5; 0.25]).'; 
end
for n = N-bwL2+1:N-1
    zetaL(n, :) = (zeta(n-1:n+1, :).'* [0.25; 0.5; 0.25]).'; 
end
*/
//
int PF_EstZeta(
		MICARY_FloatSpecArray fsPreSN,
		MICARY_PrmZeta sPrmZeta,
		MICARY_FloatSpecArray fsSavedSmSNR,
		MICARY_FloatSpecArray *pfsSmSNR,
		MICARY_FloatSpecArray *pfsSmSNRL,
		MICARY_FloatSpecArray *pfsSmSNRG,
		MICARY_FloatArray *pfaSmSNRF)
{
	MICARY_FloatArray	*pfaHanning ;
	int	i, j, k,l;
	int	iFreq, iCh, iArrayLen;
	int	iBwG2, iBwL2;
	HA_Float	ha_fBwF, ha_fSumWind ;
	HA_Float	**pha_fZeta, **pha_fZetaG, **pha_fZetaL ;
	MICARY_FloatArray2 *pfa_ZetaEx ;


	/* 与えられた事前SN比行列の大きさ(N, M)を求める */
	iCh = fsPreSN.ha_numCh;
	iFreq = fsPreSN.ha_iUpperFreq - fsPreSN.ha_iLowerFreq + 1;

	/* zetaの計算 */
	iArrayLen = iCh * iFreq;
	for (i = 0; i < iArrayLen; i++) {
		pfsSmSNR->micary_faSpec.ha_fValue[i] = (HA_Float)(sPrmZeta.ha_fB * fsSavedSmSNR.micary_faSpec.ha_fValue[i]
				+ (1.0 - sPrmZeta.ha_fB) * fsPreSN.micary_faSpec.ha_fValue[i]);
		//pfsSavedSmSNR->micary_faSpec.ha_fValue[i] = pfsSmSNR->micary_faSpec.ha_fValue[i];
	}

	/* zetaを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fZeta = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fZeta[i] = (HA_Float *)(&(pfsSmSNR->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/* zeta_Frameの計算 >>>>> */
		/* zetaFの初期化 */
	iArrayLen = pfaSmSNRF->ha_numValue;
	for (i = 0; i < iArrayLen; i++) {
		pfaSmSNRF->ha_fValue[i] = 0.0;
	}

		/* bwFの計算 */
	ha_fBwF = (HA_Float)(sPrmZeta.ha_fBwFen - sPrmZeta.ha_fBwFst + 1);

	for (i = 0; i < iCh; i++) {
		for (j = sPrmZeta.ha_fBwFst; j <= sPrmZeta.ha_fBwFen; j++) {
			pfaSmSNRF->ha_fValue[i] = pfaSmSNRF->ha_fValue[i] + pha_fZeta[i][j];
		}
		pfaSmSNRF->ha_fValue[i] = pfaSmSNRF->ha_fValue[i] / ha_fBwF;
	}

	/* >>>>> zeta_Frameの計算 */

	/* zeta_Globalの計算 >>>>> */
	iBwG2 = (int)(sPrmZeta.ha_fBwG / 2);
		/* ハニング窓の作成 */
	pfaHanning = (MICARY_FloatArray *)calloc((size_t)1, sizeof(MICARY_FloatArray));
	// modified 2008.10.31
	FloatArrayAlloc(pfaHanning,0);
	FloatArrayAlloc(pfaHanning, (int)sPrmZeta.ha_fBwG);

	MakeTimeWin(pfaHanning, (HA_Integer)sPrmZeta.ha_fBwG, 0.0, nwnHanning);
		/* windGの計算 */
	ha_fSumWind = 0.0;
	for (i = 0; i < pfaHanning->ha_numValue; i++) {
		ha_fSumWind = ha_fSumWind + pfaHanning->ha_fValue[i];
	}
	for (i = 0; i < pfaHanning->ha_numValue; i++) {
		pfaHanning->ha_fValue[i] = pfaHanning->ha_fValue[i] / ha_fSumWind;
	}

		/* zetaGの初期化 */
	iArrayLen = pfsSmSNRG->ha_numCh * (pfsSmSNRG->ha_iUpperFreq - pfsSmSNRG->ha_iLowerFreq +1);

	for (i = 0; i < iArrayLen; i++) {
		pfsSmSNRG->micary_faSpec.ha_fValue[i] = 0.0;
	}
		/* zetaGを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fZetaG = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fZetaG[i] = (HA_Float *)(&(pfsSmSNRG->micary_faSpec.ha_fValue[iFreq * i]));
	}

	/// zetaGの計算(2007/02/01 Sachiko.suzuki)
	 //zetaExの確保
	pfa_ZetaEx = (MICARY_FloatArray2 *)calloc((size_t)1, sizeof(MICARY_FloatArray2));
	// modified 2008.10.31
	FloatArray2Alloc(pfa_ZetaEx,0,0) ;
	FloatArray2Alloc(pfa_ZetaEx,iCh,iArrayLen + sPrmZeta.ha_fBwG) ;
	
	for(i = 0 ; i < iCh ; i++) {
	  for(j = 0 ; j < iFreq;j++){
	    pfa_ZetaEx->ha_fValue[i][iBwG2+j] = pha_fZeta[i][j] ;
	  }
	  //memcpy(&pfa_ZetaEx->ha_fValue[i][iBwG2],&pha_fZeta[i][0],sizeof(HA_Float)*iFreq) ;
	}


  
	/* zetaGの計算 */
	l = 0 ;
	for (i = 0 ; i < iCh ; i++){
		for (j = 0 ; j < iFreq ; j ++){
			for(k = 0 ; k < (int)sPrmZeta.ha_fBwG ; k++){
			  

				pha_fZetaG[i][j] = pha_fZetaG[i][j] + pfa_ZetaEx->ha_fValue[i][j+k]*pfaHanning ->ha_fValue[k] ;
			}
			pfsSmSNRG->micary_faSpec.ha_fValue[l] = pha_fZetaG[i][j] ;
			l ++ ;
		}
	}

		/* zetaGの計算 */
	//l = 0 ;
/*	for (i = 0; i < iCh; i++) {
		for (j = 0; j < iFreq; j++) {
			for (k = 0; k < (int)sPrmZeta.ha_fBwG; k++) {
				if ((j - iBwG2) < 0) {
					continue;
				}
				pha_fZetaG[i][j] = pha_fZetaG[i][j] + pha_fZeta[i][j - iBwG2] * pfaHanning->ha_fValue[k];
			}
			//pfsSmSNRG->micary_faSpec.ha_fValue[l] = pha_fZetaG[i][j] ;
			//l ++ ;
		}
	}
*/
		/* zetaGを2次元配列として扱うためのポインタの開放 */
	free(pha_fZetaG);
	// add 2007/10/18
	FloatArray2Free(pfa_ZetaEx) ;
	free(pfa_ZetaEx) ;

		/* ハニング窓エリアの開放 */
	FloatArrayFree(pfaHanning);
	free(pfaHanning);
	/* >>>>> zeta_Globalの計算 */

	/* zeta_Localの計算 >>>>> */
	iBwL2 = (int)(sPrmZeta.ha_fBwL / 2);

		/* zetaLの初期化 */
	iArrayLen = pfsSmSNRL->ha_numCh * (pfsSmSNRL->ha_iUpperFreq - pfsSmSNRL->ha_iLowerFreq +1);
	for (i = 0; i < iArrayLen; i++) {
		pfsSmSNRL->micary_faSpec.ha_fValue[i] = 0.0;
	}

		/* zetaLを2次元配列として扱うためのポインタの確保と初期化 */
	pha_fZetaL = (HA_Float **)calloc((size_t)iCh, sizeof(HA_Float *));
	for (i = 0; i < iCh; i++) {
		pha_fZetaL[i] = (HA_Float *)(&(pfsSmSNRL->micary_faSpec.ha_fValue[iFreq * i]));
	}

		/* zetaLの計算 */
	for (i = 0; i < iCh; i++) {
		for (j = 1; j < iBwL2; j++) {
			pha_fZetaL[i][j] = (HA_Float)(0.25 * pha_fZeta[i][j - 1] + 0.5 * pha_fZeta[i][j] + 0.25 * pha_fZeta[i][j + 1]);
		}
		for (j = iBwL2; j < (iFreq - iBwL2); j++) {
			for (k = 0; k < sPrmZeta.ha_fBwL; k++) {
				pha_fZetaL[i][j] = pha_fZetaL[i][j] + pha_fZeta[i][j - iBwL2 + k] / (HA_Float)sPrmZeta.ha_fBwL;
			}
		}
		//for (j = (iFreq - iBwL2); j < iFreq; j++) {
		for (j = (iFreq - iBwL2); j < iFreq-1; j++) {
			pha_fZetaL[i][j] = (HA_Float)(0.25 * pha_fZeta[i][j - 1] + 0.5 * pha_fZeta[i][j] + 0.25 * pha_fZeta[i][j + 1]);
		}
	}

		/* zetaLを2次元配列として扱うためのポインタの開放 */
	free(pha_fZetaL);
	/* >>>>> zeta_Localの計算 */


	/* zetaを2次元配列として扱うためのポインタの開放 */
	free(pha_fZeta);

	return 1;
}
