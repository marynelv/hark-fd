// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_rev.c
 \brief \~English Estimate Reverberation Noise power
 \brief \~Japanese écãøêÑíËèàóùä÷êî
 \author Masatoshi Tsutsumi
 \date 29 Nov 2006
 \version $Id: PF_est_rev.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include "ArrayInit.h"
#include "PF_est_rev.h"
// ---------------------------------------------------------------------
/*
function [lamda_rev] = Est_rev_valin(Shatp, gamma, delta, lamda_rev0)
% [lamda_rev] = Est_rev_valin(Shatp, gamma, delta, lamda_rev0)
% Estimation reverbaration spectrum function
% based on Jean-Marc VALIN, "Auditory System for a Mobile Robot", 2 juin 2005
%
% Inputs:
%   Shatp :Input Power spectrums of Direct component (N, 1)
%      NOTE: Input must be set to estimated output signal 'S hat' = gain^2*abs(y)^2 
%   gamma: Reverbaration fator (1, 1) [0.06] ( = 10^(-6/T60), T60: Rev. Time [Frame] )
%   delta: SRR factor (Signal to reverbatation Ratio)^-1 (1, 1) [1]
%   lamda_rev0 :Reverberation poewer spectrums of previous frame (N, 1) [zeros(N, 1)]
%
% Note:
%   Some parameters can be set to [], and initialized
%
% Outputs:
%   lamda_rev: Estimated reverbaration power spectrum (N, M)

NFreq = size(Shatp, 1); % #Freq
if isempty(gamma), gamma = 0.5;, end %from valin
if isempty(delta), delta = 0.2;, end %from valin
if isempty(lamda_rev0), lamda_rev0 = zeros(NFreq, 1);, end %from valin

% Eq. from yamamoto
lamda_rev = gamma * lamda_rev0 + gamma * delta * Shatp;
*/
//
int PF_EstRev(
		MICARY_FloatSpecArray fsInputPSpec,
		MICARY_PrmRevNoise sPrmRev,
		MICARY_SavedEstRev *psSavedRev,
		MICARY_FloatSpecArray *pfsEstRevPSpec)
{
	int	i, iArrayLen;

	iArrayLen = fsInputPSpec.ha_numCh * (fsInputPSpec.ha_iUpperFreq - fsInputPSpec.ha_iLowerFreq + 1);
	for (i = 0; i < iArrayLen; i++) {
		pfsEstRevPSpec->micary_faSpec.ha_fValue[i] = sPrmRev.ha_fGamma * psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[i]
				+ sPrmRev.ha_fGamma * sPrmRev.ha_fDelta * fsInputPSpec.micary_faSpec.ha_fValue[i];
	}

	return 1;
}
