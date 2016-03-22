/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

\file PF_Init.h
\brief \~Japanese Post filter �ϐ��A�f�[�^�̏�����
\author Sachiko Suzuki
\date 24 Jan 2007
\Version $Id: PF_Init.h 453 2009-03-17 04:44:42Z tanaka $
*/
// ---------------------------------------------------------------------
#ifndef	PFInit_h_defined
#define	PFInit_h_defined
// ---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include "MICARYHA_struct.h"	// 2008/11/11 deleted by N.Tanaka MICARY_struct.h�o�R�ŌĂ�
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
#include "PF_values.h"
#include "ArrayInit.h"

// 2007/07/13 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------

/**
 \brief \~English It initialize the parameter for Post filter.
 \brief \~Japanese �ϐ��̏�����(1:����I��, 0:�G���[)
 \param psPrmMCRA \English Setting parameter for MCRA
 \~Japanese MCRA�ݒ�p�����[�^
 \param psPrmLeak \English Setting parameter for Leak noise
 \~Japanese �R��ݒ�p�����[�^
 \param psPrmRev \English Setting parameter for Reverb factor
 \~Japanese �c�������ݒ�p�����[�^
 \param psPrmSN \English Setting parameter for estimate SN ratio
 \~Japanese SN�䐄��ݒ�p�����[�^
 \param psPrmVoiceP \English Setting parameter for Voice Probability
 \~Japanese �������݊m���ݒ�p�����[�^
 \param psPrmGain \English Setting parameter for estimate gain
 \~Japanese �œK�Q�C������ݒ�p�����[�^ 
*/
int InitPFsPrm(MICARY_PrmMCRA *psPrmMCRA , MICARY_PrmLeakNoise *psPrmLeak ,
			   MICARY_PrmRevNoise *psPrmRev , MICARY_PrmEstSN *psPrmSN ,
			   MICARY_PrmVoiceP *psPrmVoiceP , MICARY_PrmGain *psPrmGain) ;

// ---------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(MCRA) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(MCRA)�̊m��(1:����I��, 0:�G���[)
 \param psSavedMCRA \English Saved data for MCRA
 \~Japanese MCRA�ۑ��l
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese ������g�����C��
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese �������g�����C��
 \param Nch \English Number of source position
 \~Japanese ������
*/
int AllocSavedMCRA(MICARY_SavedMCRA *psSavedMCRA ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) ;

// ---------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(MCRA) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(MCRA)�̊J��(1:����I��, 0:�G���[)
 \param psSavedMCRA \English Saved data for MCRA
 \~Japanese MCRA�ۑ��l
*/
int FreeSavedMCRA(MICARY_SavedMCRA *psSavedMCRA) ;
//
//// ---------------------------------------------------------------------
///**
// \brief \~English It frees the saved data(EstSN) for Post filter.
// \brief \~Japanese �f�[�^�ۑ��l(EstSN)�̊J��(1:����I��, 0:�G���[)
// \param psSavedEstSN \English Saved data for EstSN
// \~Japanese EstSN�ۑ��l
//*/
//int FreeSavedEstSN(MICARY_SavedEstSN *psSavedEstSN) ;


// ---------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(estimate Rev) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(Rev)�̊m��(1:����I��, 0:�G���[)
 \param psSavedRev \English saved data for Reverb factor
 \~Japanese �c�������ۑ��l
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese ������g�����C��
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese �������g�����C��
 \param Nch \English Number of source position
 \~Japanese ������
*/
int AllocSavedRev(MICARY_SavedEstRev *psSavedRev ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) ;

// ---------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(estimate rev) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(Rev)�̊J��(1:����I��, 0:�G���[)
 \param psSavedRev \English Saved data for Reverb factor
 \~Japanese �c�������ۑ��l
*/
int FreeSavedRev(MICARY_SavedEstRev *psSavedRev) ;

// ----------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(estimate SN ratio) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(SN ratio)�̊m��(1:����I��, 0:�G���[)
 \param psSavedSN \English saved data for SN ratio
 \~Japanese SN��ۑ��l
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese ������g�����C��
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese �������g�����C��
 \param Nch \English Number of source position
 \~Japanese ������
*/
int AllocSavedSN(MICARY_SavedEstSN *psSavedSN ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) ;
// ----------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(estimate SN ratio) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(SN ratio)�̊J��(1:����I��, 0:�G���[)
 \param psSavedSN \English saved data for SN ratio
 \~Japanese SN��ۑ��l
*/
int FreeSavedSN(MICARY_SavedEstSN *psSavedSN ) ;
// ----------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(Voice Probability) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(Voice Probability)�̊m��(1:����I��, 0:�G���[)
 \param psSavedVoiceP \English saved data for Voice Probability
 \~Japanese �������݊m���ۑ��l
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese ������g�����C��
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese �������g�����C��
 \param Nch \English Number of source position
 \~Japanese ������
*/
int AllocSavedVoiceP(MICARY_SavedVoiceP *psSavedVoiceP ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) ;
// ----------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(Voice Probability) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(Voice Probability)�̊J��(1:����I��, 0:�G���[)
 \param psSavedVoiceP \English saved data for Voice Probability
 \~Japanese �������݊m���ۑ��l
*/
int FreeSavedVoiceP(MICARY_SavedVoiceP *psSavedVoiceP ) ;
// ----------------------------------------------------------------------
// ---------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(Initialized MCRA) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(MCRA�̏����l)�̊m��(1:����I��, 0:�G���[)
 \param psSavedInitMCRA \English Saved data for MCRA
 \~Japanese MCRA�����ۑ��l
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese ������g�����C��
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese �������g�����C��
 \param Nch \English Number of source position
 \~Japanese ������
*/
int AllocSavedInitMCRA(MICARY_SavedInitMCRA *psSavedInitMCRA ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch)  ;

// ---------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(Initialized MCRA) for Post filter.
 \brief \~Japanese �f�[�^�ۑ��l(MCRA�̏����l)�̊J��(1:����I��, 0:�G���[)
 \param psSavedInitMCRA \English Saved data for MCRA
 \~Japanese MCRA�����ۑ��l
*/
int FreeSavedInitMCRA(MICARY_SavedInitMCRA *psSavedInitMCRA)  ;

// ---------------------------------------------------------------------
/**
 \brief \~English It assigns the initialized MCRA parameter  for Post filter.
 \brief \~Japanese MCRA�̏����l�p�p�����[�^�̐ݒ�(1:����I��, 0:�G���[)
 \param psPrmInitMCRA \English Parameter structure for Initialized MCRA
 \~Japanese MCRA�����p�����[�^
*/
int InitPrmInMCRA(MICARY_PrmInitMCRA *psPrmInitMCRA) ;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
/**
 \brief \~English It initializes temporary data for Post filter.
 \brief \~Japanese �ۑ��l�f�[�^�̏�����(1:����I��, 0:�G���[)
 \param psSavedMCRA \English Saved data structure for Initialized MCRA
 \~Japanese MCRA�������f�[�^�\����
 \param psSavedRev \English Saved data structure for Initialized REV
 \~Japanese REV����p�������f�[�^�\����
 \param psSavedSN \English Saved data structure for Initialized SN
 \~Japanese SN����p�������f�[�^�\����
 \param psSavedVoiceP \English Saved data structure for Initialized VoiceP
 \~Japanese �����m���������f�[�^�\����
 \param faNoise \English Estimated noise power data array
 \~Japanese ����m�C�Y�p���[
 \param noise_compense \English parameter of noise_compense
 \~Japanese ���G���p���[�p�����[�^
 \param iLowerFreq \English Lower Frequency line index
 \~Japanese �������g�����C���ԍ�
 \param iUpperFreq \English Upper frequency line index
 \~Japanese ������g�����C���ԍ�
 \param Nsrc \English Number of Source
 \~Japanese ������
 \param Nmic \English Number of microphone
 \~Japanese �}�C�N�`�����l����
*/
int PFInitTmp(MICARY_SavedMCRA *psSavedMCRA ,MICARY_SavedEstRev *psSavedRev,MICARY_SavedEstSN *psSavedSN ,
			  MICARY_SavedVoiceP *psSavedVoiceP,MICARY_FloatArray *faNoise ,HA_Float noise_compense,
			  HA_Integer iLowerFreq ,HA_Integer iUpperFreq,HA_Integer Nsrc,HA_Integer Nmic) ;



// 2008/12/5 added by K.Morio
/**
 \brief \~English It initializes temporary data for Post filter.
 \brief \~Japanese �ۑ��l�f�[�^�̏�����(1:����I��, 0:�G���[)
 \param psSavedMCRA \English Saved data structure for Initialized MCRA
 \~Japanese MCRA�������f�[�^�\����
 \param psSavedRev \English Saved data structure for Initialized REV
 \~Japanese REV����p�������f�[�^�\����
 \param psSavedSN \English Saved data structure for Initialized SN
 \~Japanese SN����p�������f�[�^�\����
 \param psSavedVoiceP \English Saved data structure for Initialized VoiceP
 \~Japanese �����m���������f�[�^�\����
 \param faNoise \English Initial State Noise data array
 \~Japanese ���G�������l
 \param iLowerFreq \English Lower Frequency line index
 \~Japanese �������g�����C���ԍ�
 \param iUpperFreq \English Upper frequency line index
 \~Japanese ������g�����C���ԍ�


*/
int PFInitTmp2(MICARY_SavedMCRA *psSavedMCRA ,MICARY_SavedEstRev *psSavedRev,MICARY_SavedEstSN *psSavedSN ,
			  MICARY_SavedVoiceP *psSavedVoiceP,MICARY_FloatArray *pfCalcStateNoise,
			  HA_Integer iLowerFreq ,HA_Integer iUpperFreq);

// 2008/12/5 added by K.Morio
/**
 \brief \~English It calcurates Initial state noise power.
 \brief \~Japanese ���G�������l�̎Z�o(1:����I��, 0:�G���[)
 \param faNoise \English Estimated noise power data array
 \~Japanese ����m�C�Y�p���[
 \param noise_compense \English parameter of noise_compense
 \~Japanese ���G���p���[�p�����[�^
  \param faNoise \English Initial State Noise data array
 \~Japanese ���G�������l
 \param iLowerFreq \English Lower Frequency line index
 \~Japanese �������g�����C���ԍ�
 \param iUpperFreq \English Upper frequency line index
 \~Japanese ������g�����C���ԍ�
 \param Nsrc \English Number of Source
 \~Japanese ������
 \param Nmic \English Number of microphone
 \~Japanese �}�C�N�`�����l����


*/
int CalcInitStateNoise(MICARY_FloatArray faNoise ,HA_Float noise_compense,MICARY_FloatArray *pfCalcStateNoise,
			  HA_Integer iLowerFreq ,HA_Integer iUpperFreq,HA_Integer Nsrc,HA_Integer Nmic);



/**
 \brief \~English It calculates minimum sum (PF_init_state_noise.c/PF_est_state_noise.c).
 \brief \~Japanese �ϐ�A�AB�̍ŏ��l�̍����Z�o(PF_init_state_coise.c/PF_est_state_noise.c).
 \param A \English Variable A
 \~Japanese �ϐ�A
 \param B \English Variable B
 \~Japanese �ϐ�B
*/
HA_Float minmat(HA_Float A ,HA_Float B) ;






// 2008/08/06 added by N.Tanaka
/**
 \brief \~Japanese �R�ꐄ��p�p�����[�^�̐ݒ�
 \param psPrmLeakNoise \~English Parameter struct for estimating leak noise
 \~Japanese �R�ꐄ��p�p�����[�^�̐ݒ�
 \param fLeak_factor \~English Leak factor
 \~Japanese �R��W��
 \param fOver_cancel \~English Over cancel value
 \~Japanese �I�[�o�[�L�����Z���l
*/
int Input_PF_PrmLeak(MICARY_PrmLeakNoise *psPrmLeakNoise, float fLeak_factor,float fOver_cancel) ;


#ifdef __cplusplus
}
#endif
// ----------------------------------------------------------------------------------------

#endif // PF_CmdLine_h_defined

