// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_voice_p.h
 \brief \~English Header file for Estimate speech present probability
 \brief \~Japanese �������݊m���֐��̃w�b�_�t�@�C��
 \author Masatoshi Tsutsumi
 \date 1 Dec 2006
 \version $Id: PF_est_voice_p.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_VOICE_P_h_defined
#define	PF_EST_VOICE_P_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"

// 2007/07/13 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------

/**
 \brief \~Japanese �������݊m���̐���(1:����I��, 0:�G���[)
 \param fsPreSN \~English Priori SNR (N, M)
 \~Japanese ���OSN��
 \param fsEstVal \~English Variable for optimum gain estimator  (N, M)
 \~Japanese �œK�Q�C������l�Z�o�̂��߂̕ϐ�
 \param sPrmZeta \~English Parameters for Estimate speech present probability
 \~Japanese �������݊m���v�Z�̃p�����[�^
 \param psSavedVoiceP \~English Speech present probability of previous frame (N ,M)
 \~Japanese �O�t���[���̉������݊m���v�Z�̌���
 \param pfsVoiceP \~English Probability of speech present (p) (N, M)
 \~Japanese �������݊m��
 \param pfsSmSNR \~English Smoothed priori SNR (zeta) (N, M)
 \~Japanese ���������ꂽ���OSN��
 \param pfaSmSNRF \~English zeta_frame frame (M, 1)
 \~Japanese ���������ꂽ���OSN��(Frame)
 \param pfaSmSNRP \~English zeta_frame peak (M, 1)
 \~Japanese ���������ꂽ���OSN��(Peak)
*/
int PF_EstVoiceP(
		MICARY_FloatSpecArray fsPreSN,
		MICARY_FloatSpecArray fsEstVal,
		MICARY_PrmVoiceP sPrmVoiceP,
		MICARY_SavedVoiceP *psSavedVoiceP,
		MICARY_FloatSpecArray *pfsVoiceP,
		MICARY_FloatSpecArray *pfsSmSNR,
		MICARY_FloatArray *pfaSmSNRF,
		MICARY_FloatArray *pfaSmSNRP);

#ifdef __cplusplus
}
#endif
// --------------------------------------------------------------------

#endif	// PF_EST_VOICE_P_h_defined
