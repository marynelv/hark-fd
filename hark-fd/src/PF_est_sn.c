// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_sn.c
 \brief \~English Estimate Optimam Gain of speech present 
 \brief \~Japanese SN比・ゲイン推定処理関数
 \author Masatoshi Tsutsumi
 \date 23 Nov 2006
 \version $Id: PF_est_sn.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include "ArrayInit.h"
#include "PF_est_sn.h"
#include "PF_hypergeom_gain.h"
// ---------------------------------------------------------------------
/*
function [GH1, xi, v] = Est_GH1_valin_v2(yp, lamda_total, Shatp, xi0, alp_pmin, alp_pmag, a, OP)
%
% Estimate Optimam Gain of speech present 
% based on Jean-Marc VALIN, "Auditory System for a Mobile Robot", 2 juin 2005
% 
% Input:
%   yp          : Input power spectrum (N, M)
%   lamda_total : Total Estimated Noise spectrum (N, M)
%   Shatp        : Estimated signal power (N, M) <> abs(Shat).^2 !!!
%               ( N:#Freqeucy Lines, M:#Channels )
%   xi0         : Estimated priori ~= (SNR - 1) in previous frame (N, M) [gamma - 1]
%   alp_pmin    : Minimum Update late of xi [0.05] for valin
%   alp_pmag    : magnitude parameter of xi [0.9] for valin (new)
%   a           : parameter for preori SNR [0.8] for valin (new)
%   OP          : optional parameters
%    .xi_max    :xsi uppter limit (100)
%    .theta_max :theta upper limit (20)
%    .theta_min :theta lower limit (6)
%    ... (detail is described at 'Postfilter_Init_valin_v2')
%
% Output:
%   GH1    : Optimal amplitude gain (N, M)
%   xi     : Estimated postteriori ~= (SNR - 1)  (N, M)
%   v      : Variable for optimum gain estimator


gammaM1 = yp./(lamda_total+1) - 1;
gammaM1C = gammaM1;
gammaM1C(gammaM1C < 0) = 0; % max(gamma-1, 0) in Eq.(3.23)

alp_pg = alp_pmag*(xi0 ./ (xi0 + 1)).^2 + alp_pmin; % Est_GH1v3c, for valin

tmp = a * Shatp./(lamda_total + 1) + (1 - a) * xi0; % new valin
xi = (1 - alp_pg) .* tmp + alp_pg .* gammaM1C; % Eq.(3.23)
xi(xi > OP.xi_max) = OP.xi_max; % New valin

v = (gammaM1 + 1) .* xi ./ (xi + 1); % note of Eq. (3.22)
v(v > OP.theta_max) = OP.theta_max;
GH1 = xi ./ (1+xi) .* hypergeom_gain(v); %Eq. (20) in Ephiraims' [Valin's Eq. (3.22), excluded 'exp']
GH1(v >= OP.theta_min) = 1;
GH1(GH1 > 1) = 1;
*/
//
int PF_EstSN(
			MICARY_FloatSpecArray fsInputPSpec,
			MICARY_FloatSpecArray fsEstNoisePSpec,
			MICARY_SavedEstSN *psSavedSN,
			MICARY_PrmEstSN sPrmSN,
			MICARY_FloatSpecArray *pfsGH1,
			MICARY_FloatSpecArray *pfsSN,
			MICARY_FloatSpecArray *pfsEstVal)
{
	MICARY_FloatSpecArray	*pfsGammaM1, *pfsGammaM1C, *pfsAlp_pg, *pfsTmp;
	HA_Float	hfTmp;
	int	i, rc, iErr = 0;
	int	iArrayLen;

	/* 作業用gammaM1とgammaM1Cの確保 */
	pfsGammaM1 = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	pfsGammaM1C = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));

	// modified 2008.10.31
	//Initialize
	AllocFloatSpecArray(pfsGammaM1,0,0,0);
	AllocFloatSpecArray(pfsGammaM1C,0,0,0);

	AllocFloatSpecArray(pfsGammaM1, fsInputPSpec.ha_numCh, fsInputPSpec.ha_iLowerFreq, fsInputPSpec.ha_iUpperFreq);
	AllocFloatSpecArray(pfsGammaM1C, fsInputPSpec.ha_numCh, fsInputPSpec.ha_iLowerFreq, fsInputPSpec.ha_iUpperFreq);

	iArrayLen = fsInputPSpec.ha_numCh * (fsInputPSpec.ha_iUpperFreq - fsInputPSpec.ha_iLowerFreq + 1);

	/* gammaM1, gammaM1Cの計算 */
	for (i = 0; i < iArrayLen; i++) {
	  //printf("i = %d \n" ,i) ;
	  //printf("G1 = %f , In = %f , Ns = %f \n",pfsGammaM1->micary_faSpec.ha_fValue[i],fsInputPSpec.micary_faSpec.ha_fValue[i],fsEstNoisePSpec.micary_faSpec.ha_fValue[i]) ;
pfsGammaM1->micary_faSpec.ha_fValue[i] = fsInputPSpec.micary_faSpec.ha_fValue[i] / (fsEstNoisePSpec.micary_faSpec.ha_fValue[i] + 1.0) - 1.0;
		pfsGammaM1C->micary_faSpec.ha_fValue[i] = pfsGammaM1->micary_faSpec.ha_fValue[i];
		if (pfsGammaM1C->micary_faSpec.ha_fValue[i] < 0.0) {
			pfsGammaM1C->micary_faSpec.ha_fValue[i] = 0.0;
		}
	}

	/* 作業用alp_pgの確保 */
	pfsAlp_pg = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	// modified 2008.10.31
	AllocFloatSpecArray(pfsAlp_pg,0,0,0);

	AllocFloatSpecArray(pfsAlp_pg, psSavedSN->micary_fsSN.ha_numCh, psSavedSN->micary_fsSN.ha_iLowerFreq, psSavedSN->micary_fsSN.ha_iUpperFreq);

	iArrayLen = psSavedSN->micary_fsSN.ha_numCh * (psSavedSN->micary_fsSN.ha_iUpperFreq - psSavedSN->micary_fsSN.ha_iLowerFreq + 1);

	/* alp_pgの計算 */
	for (i = 0; i < iArrayLen; i++) {
		pfsAlp_pg->micary_faSpec.ha_fValue[i] = sPrmSN.ha_fAlp_pmag
			* pow((double)(psSavedSN->micary_fsSN.micary_faSpec.ha_fValue[i] / (psSavedSN->micary_fsSN.micary_faSpec.ha_fValue[i] + 1.0)), 2.0)
			+ sPrmSN.ha_fAlp_pmin;
	}

	/* 作業用tmpの確保 */
	pfsTmp = (MICARY_FloatSpecArray *)calloc((size_t)1, sizeof(MICARY_FloatSpecArray));
	// modified 2008.10.31
	AllocFloatSpecArray(pfsTmp,0,0,0);

	AllocFloatSpecArray(pfsTmp, psSavedSN->micary_fsInputPSpec.ha_numCh, psSavedSN->micary_fsInputPSpec.ha_iLowerFreq, psSavedSN->micary_fsInputPSpec.ha_iUpperFreq);

	iArrayLen = psSavedSN->micary_fsInputPSpec.ha_numCh * (psSavedSN->micary_fsInputPSpec.ha_iUpperFreq - psSavedSN->micary_fsInputPSpec.ha_iLowerFreq + 1);

	/* tmpの計算 */
	for (i = 0; i < iArrayLen; i++) {
		pfsTmp->micary_faSpec.ha_fValue[i] = sPrmSN.ha_fA * psSavedSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[i] / (fsEstNoisePSpec.micary_faSpec.ha_fValue[i] + 1.0)
			+ (1.0 - sPrmSN.ha_fA) * psSavedSN->micary_fsSN.micary_faSpec.ha_fValue[i];
	}

	/* xiの計算 */
	for (i = 0; i < iArrayLen; i++) {
		pfsSN->micary_faSpec.ha_fValue[i] = (1.0 - pfsAlp_pg->micary_faSpec.ha_fValue[i]) * pfsTmp->micary_faSpec.ha_fValue[i]
			+ pfsAlp_pg->micary_faSpec.ha_fValue[i] * pfsGammaM1C->micary_faSpec.ha_fValue[i];
		if (pfsSN->micary_faSpec.ha_fValue[i] > sPrmSN.ha_fXi_max) {
			pfsSN->micary_faSpec.ha_fValue[i] = sPrmSN.ha_fXi_max;
		}
	}

	/* vの計算 */
	for (i = 0; i < iArrayLen; i++) {
		pfsEstVal->micary_faSpec.ha_fValue[i] = (pfsGammaM1->micary_faSpec.ha_fValue[i] + 1.0) * pfsSN->micary_faSpec.ha_fValue[i] / (pfsSN->micary_faSpec.ha_fValue[i] + 1.0);
		if (pfsEstVal->micary_faSpec.ha_fValue[i] > sPrmSN.ha_fTheta_max) {
			pfsEstVal->micary_faSpec.ha_fValue[i] = sPrmSN.ha_fTheta_max;
		}
	}

	/* GH1の計算 */
	for (i = 0; i < iArrayLen; i++) {
		if (pfsEstVal->micary_faSpec.ha_fValue[i] >= sPrmSN.ha_fTheta_min) {
			pfsGH1->micary_faSpec.ha_fValue[i] = 1.0;
		}
		else {
			rc = PF_HyperGeomGain(pfsEstVal->micary_faSpec.ha_fValue[i], &hfTmp);
			if (rc == 0) {
				hfTmp = 0.0 ;
			//	iErr = 1;
			//	break;
			}
			pfsGH1->micary_faSpec.ha_fValue[i] = pfsSN->micary_faSpec.ha_fValue[i] / (1.0 + pfsSN->micary_faSpec.ha_fValue[i]) * hfTmp;
			if (pfsGH1->micary_faSpec.ha_fValue[i] > 1.0) {
				pfsGH1->micary_faSpec.ha_fValue[i] = 1.0;
			}
		}
	}

	/* 作業用tmpのエリア開放 */
	FreeFloatSpecArray(pfsTmp);
	free(pfsTmp);

	/* 作業用alp_pgのエリア開放 */
	FreeFloatSpecArray(pfsAlp_pg);
	free(pfsAlp_pg);

	/* 作業用gammaM1とgammaM1Cのエリア開放 */
	FreeFloatSpecArray(pfsGammaM1C);
	FreeFloatSpecArray(pfsGammaM1);
	free(pfsGammaM1C);
	free(pfsGammaM1);

	if (iErr == 1) {
		return 0;
	}

	return 1;
}

