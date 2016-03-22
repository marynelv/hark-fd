// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_init_state_noise.h
 \brief \~English Header file for Estimate MCRA
 \brief \~Japanese MCRA�p�����m�C�Y�֐��̃w�b�_�t�@�C��
 \author Masatoshi Tsutsumi
 \date 13 Dec 2006
 \version $Id: PF_init_state_noise.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_INIT_STATE_NOISE_h_defined
#define	PF_INIT_STATE_NOISE_h_defined
// ---------------------------------------------------------------------
//#include "MICARYHA_struct.h"	// 2008/11/11 deleted by N.Tanaka MICARY_struct�o�R�ŌĂ�
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
#include "PF_Init.h"

// 2007/07/13 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------

/**
 \brief \~Japanese MCRA�p�����m�C�Y(1:����I��, 0:�G���[)
 \param fsInputPSpec \~English Input power spectrum (N, M)
 \~Japanese ���̓p���[�X�y�N�g��
 \param sPrmInitMCRA \~English Parameters for Initial noise of MCRA calc.
 \~Japanese MCRA�����m�C�Y�v�Z�p�����[�^
 \param psSavedInitMCRA \~English Saved Initial noise of MCRA values
 \~Japanese �O�t���[����MCRA�����m�C�Y
 \param pfsInitNoisePSpec \~English MCRA values
 \~Japanese MCRA�����m�C�Y
*/
int PF_InitStateNoise(
		MICARY_FloatSpecArray fsInputPSpec,
		MICARY_PrmInitMCRA sPrmMCRA,
		MICARY_SavedInitMCRA *psSavedMCRA,
		MICARY_FloatSpecArray *pfsInitNoisePSpec);

#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------
#endif	// PF_INIT_STATE_NOISE_h_defined
