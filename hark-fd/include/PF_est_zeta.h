// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_est_zeta.h
 \brief \~English Header file for Estimate smoothed SNR for speech on/off judgement
 \brief \~Japanese ���OSN��̕������֐��̃w�b�_�t�@�C��
 \author Masatoshi Tsutsumi
 \date 29 Nov 2006
 \version $Id: PF_est_zeta.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EST_ZETA_h_defined
#define	PF_EST_ZETA_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_PF_Struct.h"

// 2007/07/13 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------

/**
 \brief \~Japanese ���OSN��̕�����(1:����I��, 0:�G���[)
 \param fsPreSN \~English Priori SNR (N, M)
 \~Japanese ���OSN��
 \param sPrmZeta \~English Parameters for smoothing calc.
 \~Japanese �������v�Z�̃p�����[�^
 \param pfsSavedSmSNR \~English Smoothed SNR of previous frame (N ,M)
 \~Japanese �O�t���[���̕�����SN��
 \param pfsSmSNR \~English Smoothed priori SNR (zeta) (N, M)
 \~Japanese ���������ꂽ���OSN��
 \param pfsSmSNRLocal \~English Smoothed priori SNR (zetaL) (Local) (N, M)
 \~Japanese ���������ꂽ���OSN��(Local)
 \param pfsSmSNRGlobal \~English Smoothed priori SNR (zetaG) (Global) (N, M)
 \~Japanese ���������ꂽ���OSN��(Global)
 \param pfaSmSNRFrame \~English Smoothed priori SNR (zetaF) (Frame) (M, 1)
 \~Japanese ���������ꂽ���OSN��(Frame)
*/
int PF_EstZeta(
		MICARY_FloatSpecArray fsPreSN,
		MICARY_PrmZeta sPrmZeta,
		MICARY_FloatSpecArray fsSavedSmSNR,
		MICARY_FloatSpecArray *pfsSmSNR,
		MICARY_FloatSpecArray *pfsSmSNRL,
		MICARY_FloatSpecArray *pfsSmSNRG,
		MICARY_FloatArray *pfaSmSNRF);

#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------

#endif	// PF_EST_ZETA_h_defined
