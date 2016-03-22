// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_exec.h
 \brief \~English Header file for Execute PostFilter function
 \brief \~Japanese PostFilter���s�֐��̃w�b�_�t�@�C��
 \author Sachiko Suzuki
 \date 30 Jan 2007
 \version $Id: PF_exec.h 504 2009-09-09 09:05:29Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EXEC_h_defined
#define	PF_EXEC_h_defined
// ---------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "MICARYHA_struct.h"
#include "ArrayInit.h"
#include "MICARY_PF_Struct.h"
//#include "MICARY_SpecFile.h"
#include "PF_Init.h"
#include "PF_est_leak.h"
#include "PF_est_noise.h"
#include "PF_est_rev.h"
#include "PF_est_sn.h"
#include "PF_est_state_noise.h"
#include "PF_est_voice_p.h"

// 2007/07/13 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif


// ---------------------------------------------------------------------

/**
 \brief \~Japanese PF�̎��s(1:����I��, 0:�G���[)
 \param csInputSpec \~English Input spectrum
 \~Japanese ���̓X�y�N�g��
 \param pcsOnputSpec \~English Output spectrum 
 \~Japanese �o�̓X�y�N�g��
 \param pSavedMCRA \~English saved data struct for MCRA
 \~Japanese MCRA�ۑ��l�\����
 \param pSavedMCRA \~English saved data struct for MCRA(previous frame)
 \~Japanese �O�t���[��MCRA�ۑ��l
 \param prmMCRA \~English parameter struct for MCRA
 \~Japanese MCRA�p�����[�^
 \param pSavedLeak \~English saved data struct for Leak noise
 \~Japanese �R��m�C�Y����ۑ��l
 \param prmLeak \~English parameter struct for Leak noise
 \~Japanese �R��m�C�Y����p�����[�^
 \param pSavedRev \~English saved data struct for Reverbaration
 \~Japanese �c����������ۑ��l
 \param pSavedRevtmp \~English saved data struct for Reverbration(previous frame)
 \~Japanese �O�t���[���c����������ۑ��l
 \param prmRev \~English parameter struct for Reverbraion
 \~Japanese �c����������p�����[�^
 \param pSavedSN \~English saved data struct for SN ratio
 \~Japanese SN�䐄��ۑ��l
 \param prmSN \~English parameter struct for SN ratio
 \~Japanese SN�䐄��p�����[�^
 \param pSavedVoiceP \~English saved data struct for Speech probability
 \~Japanese �������݊m���ۑ��l
 \param prmVoiceP \~English parameter struct for Speech probability
 \~Japanese �������݊m���p�����[�^
 \param PrmGain \~English parameter optimum gain
 \~Japanese �œK�Q�C���p�����[�^
 \param pSavedLamda \~English saved data estimated back ground noise
 \~Japanese ����w�i�m�C�Y�f�[�^
 */

int PF_ExecutePostFilter(MICARY_ComplexSpecArray csInputSpec , MICARY_ComplexSpecArray *pcsOutputSpec ,
							MICARY_SavedMCRA *pSavedMCRA ,MICARY_SavedMCRA *pSavedMCRAtmp ,MICARY_PrmMCRA prmMCRA ,
							MICARY_FloatSpecArray *pSavedLeak ,MICARY_PrmLeakNoise prmLeak ,
							MICARY_SavedEstRev *pSavedRev ,MICARY_SavedEstRev *pSavedRevtmp ,
							MICARY_PrmRevNoise prmRev , 
							MICARY_SavedEstSN *pSavedSN , MICARY_PrmEstSN prmSN,
							MICARY_SavedVoiceP *pSavedVoiceP , MICARY_PrmVoiceP prmVoiceP ,
			 /*MICARY_PrmGain PrmGain ,*/ MICARY_FloatSpecArray *pSavedLamda) ;

/**
 \brief \~Japanese MCRA�w�i�m�C�Y�̏�����(1:����I��, 0:�G���[)
 \param csInputSpec \~English Input spectrum
 \~Japanese ���̓X�y�N�g��
 \param pfsOutputSpec \~English Output Noise spectrum
 \~Japanese �o�̓m�C�Y�X�y�N�g��
 \param ha_fNoiseCoef \~English Noise compensation value 
 \~Japanese �m�C�Y�␳�p�����[�^
*/

int PF_InitMCRA(MICARY_ComplexSpecArray csInputSpec ,
				MICARY_FloatSpecArray *pfsOutputSpec ,HA_Float ha_fNoiseCoef) ;

// ---------------------------------------------------------------------
/**
 \brief \~English It calculate power spectrum .
 \brief \~Japanese �p���[�X�y�N�g���̍쐬(1:����I��, 0:�G���[)
 \param pfsPowerSpec \English Power spectrum array struct pointer
 \~Japanese �p���[�X�y�N�g���A���C�|�C���^
 \param csSpec \English Spectrum array struct pointer
 \~Japanese �X�y�N�g���A���C�|�C���^
*/
int MakePowerSpec(MICARY_FloatSpecArray * pfsPowerSpec ,
					MICARY_ComplexSpecArray csSpec ) ;

#ifdef __cplusplus
}
#endif
// ----------------------------------------------------------------------------
#endif	// PF_EXEC_h_defined
