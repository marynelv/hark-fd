// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_rev.h
 \brief \~English Header file for Estimation reverberation noise power spectrum
 \brief \~Japanese �c����������֐��̃w�b�_�t�@�C��
 \author Masatoshi Tsutsumi
 \date 29 Nov 2006
 \version $$
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_REV_h_defined
#define	PF_EST_REV_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"

// 2007/07/13 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------

/**
 \brief \~Japanese �c������(1:����I��, 0:�G���[)
 \param fsInputPSpec \~English Input power spectrum (N, M)
 \~Japanese ���̓p���[
 \param sPrmRev \~English Parameters for Estimation reverberation noise calc.
 \~Japanese �c������v�Z�̂��߂̃p�����[�^
 \param psSavedRev \~English Estimated reverberation noise power spectrum of previous frame (N ,M)
 \~Japanese �O�t���[���̎c���p���[
 \param pfsEstRevPSpec \~English Total estimated reverberation power spectrum (N, M)
 \~Japanese ���肵���c�������p���[
*/
int PF_EstRev(
		MICARY_FloatSpecArray fsInputPSpec,
		MICARY_PrmRevNoise sPrmRev,
		MICARY_SavedEstRev *psSavedRev,
		MICARY_FloatSpecArray *pfsEstRevPSpec);

#ifdef __cplusplus
}
#endif

#endif	// PF_EST_REV_h_defined
