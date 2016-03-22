// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_state_noise.h
 \brief \~English Header file for Estimate MCRA
 \brief \~Japanese ���G������֐��̃w�b�_�t�@�C��
 \author Masatoshi Tsutsumi
 \date 5 Dec 2006
 \version $Id: PF_est_state_noise.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_STATE_NOISE_h_defined
#define	PF_EST_STATE_NOISE_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"
#include "PF_Init.h"

// 2007/07/13 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------

/**
 \brief \~Japanese ���G���̐���(1:����I��, 0:�G���[)
 \param fsInputPSpec \~English Input power spectrum (N, M)
 \~Japanese ���̓p���[�X�y�N�g��
 \param sPrmMCRA \~English Parameters for MCRA calc.
 \~Japanese MCRA�v�Z�p�����[�^
 \param psSavedMCRA \~English Saved MCRA values
 \~Japanese �O�t���[����MCRA�v�Z�l
 \param pfsEstNoisePSpec \~English MCRA values
 \~Japanese MCRA�v�Z�l
 \param pfsSmin \~English Minimum Spectrum of yp
 \~Japanese �ŏ����̓p���[
 \param pfsStmp \~English Temporary Spectrum of yp
 \~Japanese ��Ɨp���̓p���[
 \param pfaS \~English Smoothed spectrum of frame
 \~Japanese ���������ꂽ�G���p���[
*/
int PF_EstStateNoise(
		MICARY_FloatSpecArray fsInputPSpec,
		MICARY_PrmMCRA sPrmMCRA,
		MICARY_SavedMCRA *psSavedMCRA,
		MICARY_FloatSpecArray *pfsEstNoisePSpec,
		MICARY_FloatSpecArray *pfsSmin,
		MICARY_FloatSpecArray *pfsStmp,
		MICARY_FloatSpecArray *pfsS);

// 2007/07/13 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
}
#endif
// ----------------------------------------------------------------------
#endif	// PF_EST_STATE_NOISE_h_defined
