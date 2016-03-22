// ---------------------------------------------------------------------
/**
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.

 \file CPF_exec.h
 \brief \~English Header file for execute Post-Filter.
 \brief \~Japanese Post-Filter�̎��s�N���X�̃w�b�_(CPF)
 \author Sachiko Suzuki
 \date 1 Aug 2007
 \version $Id: CPF_exec.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	CPF_exec_h_defined
#define	CPF_exec_h_defined
// ---------------------------------------------------------------------
#include "ArrayInit.h"
//#include "ArrayUtil.h"
#include "PF_exec.h"
#include "PF_Init.h"
#include "MICARY_PF_Struct.h"

#include <string>
#include <iostream>
#include <vector>
// ---------------------------------------------------------------------
/**
 \class CPF_exec
 \brief \~English Class for executing Post-Filter process
 \brief \~Japanese Post-Filter�̎��s�N���X
*/
class CPF_exec{
private:
  //MICARY_FloatSpecArray *m_pfsNoiseSpec ;	//�w�i�G��Spectra
	MICARY_SavedMCRA *m_psSavedMCRA ;	//MCRA�ۑ��l(current frame)
	MICARY_SavedEstRev *m_psSavedRev ;	//�c�������ۑ��l(current frame)
	MICARY_SavedEstSN *m_psSavedEstSN ;	//SN�䐄��ۑ��l
	MICARY_SavedVoiceP *m_psSavedVoiceP ;	//�������݊m���ۑ��l
	MICARY_SavedMCRA *m_psSavedMCRATmp ;	//MCRA�ۑ��lTmp(1�O�t���[��)
	MICARY_FloatSpecArray *m_pfsSavedLeak ;	//�R��ۑ��l
	MICARY_SavedEstRev *m_psSavedRevTmp ;	//�c�������ۑ��lTmp(1�O�t���[��)
	MICARY_FloatSpecArray *m_pfsLamda_total ; //Total noise

	// Temporary array for change source(�|�C���^�ɕύX�@2008/2/15)
	MICARY_SavedMCRA *tmp_sSavedMCRA,*tmp_sSavedMCRATmp ;//MCRA�ۑ��l(temporary)
	MICARY_SavedEstRev *tmp_sSavedRev,*tmp_sSavedRevTmp ;//�c�������ۑ��l(temporary)
	MICARY_SavedEstSN *tmp_sSavedEstSN ;	//SN�䐄��ۑ��l(temporary)
	MICARY_SavedVoiceP *tmp_sSavedVoiceP ;	//�������݊m���ۑ��l(temporary)
	MICARY_FloatSpecArray *tmp_fsSavedLeak ;	//�R��ۑ��l(temporary)
	MICARY_FloatSpecArray *tmp_fsLamda_total ; // total lamda(temporary)
	
	
public:
	virtual int ChangeSrcPos(MICARY_FloatSpecArray faInStatNoise,MICARY_IntArray iaSrcCur,MICARY_IntArray iaSrcIni,HA_Float ha_fNoise_Compense);
	virtual int InitSavedArray(MICARY_FloatSpecArray faInStatNoise,int iLower_freq,int iUpper_freq,int iNpos,int iNch,HA_Float ha_fNoise_compense);
	/**
	 \brief \~Japanese �R���X�g���N�^
	*/
	CPF_exec() ;
	/**
	 \brief \~Japanese �f�X�g���N�^
	*/
	~CPF_exec() ;
	/**
	 \brief \~Japanese �N���A
	*/
	int Clear() ;
	MICARY_FloatSpecArray *m_pfsNoiseSpec ;	//�w�i�G��Spectra

	// PF�pParameter�\����
	MICARY_PrmVoiceP sPrmVoiceP ;	//����power�p�����[�^
	MICARY_PrmEstSN sPrmEstSN ;	//S/N�䐄��p�����[�^
	MICARY_PrmRevNoise sPrmRev ;	//�c�������p�����[�^
	MICARY_PrmLeakNoise sPrmLeak ;	//�R�ꐬ���p�����[�^
	MICARY_PrmMCRA sPrmMCRA ;	//MCRA�p�����[�^	
	MICARY_PrmGain sPrmGain ;	//�œKGain�p�����[�^

	/**
	 \brief \~Japanese Post-Filter�p�ۑ��l�z��̊m��
	 \param iLower_freq \~English Index of lower bound frequency
	 \~Japanese �������g���C���f�b�N�X
	 \param iUpper_freq \~English Index of upper bound frequency
	 \~Japanese ������g���C���f�b�N�X
	 \param iNpos \~English Size of sources
	 \~Japanese ������
	 \param iNch \~English Size of mic channels
	 \~Japanese �}�C�N�`�����l����
	 \param iNproc \~English Number of process
	 \~Japanese ������
	*/
	int Initialize(int iLower_freq,int iUpper_freq,int iNch,int iNproc) ;
	/**
	 \brief \~Japanese Post-Filter�p�ۑ��l�z��̏�����
	 \param faInStatNoise \~English Power spectra estimated the state noise of each channel
	 \~Japanese ���肳�ꂽMCRA�p�����m�C�Y�̃p���[�X�y�N�g��
	 \param iLower_freq \~English Index of lower bound frequency
	 \~Japanese �������g���C���f�b�N�X
	 \param iUpper_freq \~English Index of upper bound frequency
	 \~Japanese ������g���C���f�b�N�X
	 \param iNpos \~English Size of sources
	 \~Japanese ������
	 \param iNch \~English Size of mic channels
	 \~Japanese �}�C�N�`�����l����
	*/
	int InitSavedArray(MICARY_FloatSpecArray faInStatNoise,int iLower_freq,int iUpper_freq,int iNpos,int iNch);
	/**
	 \brief \~Japanese MCAR�p�p�����[�^�̐ݒ�
	 \param fNoise_compens \~English Noise compens
	 \~Japanese �G���␳�l
	 \param fSmooth_spec_factor \~English Smoothing factor for input spectra
	 \~Japanese ���̓X�y�N�g���̕������W��
	 \param fAmp_leak_factor \~English Leak factor of amplitude
	 \~Japanese �U���R��W��
	 \param fBacknoise_est_factor \~English Smoothing factor for estimated BGN
	 \~Japanese ����w�i�G���̕������W��
	 \param fLeak_factor \~English Leak factor
	 \~Japanese �R��W��
	 \param fSs_floor \~English Floor factor
	 \~Japanese floor�W��
	 \param iL \~English Time width for detection
	 \~Japanese ���o���ԕ�
	 \param fDelta \~English Threshold level of power ratio
	 \~Japanese �p���[��臒l
	*/
	int Set_PF_PrmMCRA(float fNoise_compens,float fSmooth_spec_factor,float fAmp_leak_factor,
		float fBacknoise_est_factor, float fLeak_factor,float fSs_floor,int iL,float fDelta) ;
	/**
	 \brief \~Japanese �R�ꐄ��p�p�����[�^�̐ݒ�
	 \param fLeak_factor \~English Leak factor
	 \~Japanese �R��W��
	 \param fOver_cancel \~English Over cancel value
	 \~Japanese �I�[�o�[�L�����Z���l
	*/
	int Set_PF_PrmLeak(float fLeak_factor,float fOver_cancel) ;
	/**
	 \brief \~Japanese �c����������p�p�����[�^�̐ݒ�
	 \param fReverb_decay \~English Reverb decay value
	 \~Japanese �c�������萔
	 \param fReverb_level \~English Direct component ratio
	 \~Japanese ���ڐ�����
	*/
	int Set_PF_PrmRev(float fReverb_decay,float fReverb_level) ;
	/**
	 \brief \~Japanese SN�䐄��p�p�����[�^�̐ݒ�
	 \param fPrior_factor \~English Time SN ratio
	 \~Japanese ����SN��
	 \param fAlp_pmag \~English Amplitude factor of voice inclusion rate
	 \~Japanese �����ܗL���̐U���W��
	 \param fAlp_pmin \~English Minimum voice inclusion rate
	 \~Japanese �ŏ������ܗL��
	 \param fXi_max \~English Max value of posterior SN ratio
	 \~Japanese ����SN��̍ő�l
	 \param fTheta_max \~English Max value of optimum gain mid variable(v)
	 \~Japanese �œK�Q�C�����ԕϐ�(v)�̍ő�l
	 \param fTheta_min \~English Minimum value of optimum gain mid variable(v)
	 \~Japanese �œK�Q�C�����ԕϐ�(v)�̍ŏ��l
	*/
	int Set_PF_PrmSN(float fPrior_factor,float fAlp_pmag,float fAlp_pmin,float fXi_max,
		float fTheta_max,float fTheta_min) ;
	/**
	 \brief \~Japanese �����m������p�p�����[�^�̐ݒ�
	 \param fSmooth_prior_factor \~English Smoothing factor of prior SN ratio
	 \~Japanese ���OSN��̕������W��
	 \param fZminF \~English Minimum value of smoothing SN ratio(frame)
	 \~Japanese ������SN��̍ŏ��l(frame)
	 \param fZmaxF \~English Max value of smoothing SN ratio(frame)
	 \~Japanese ������SN��̍ő�l(frame)
	 \param fZminG \~English Minimum value of smoothing SN ratio(global)
	 \~Japanese ������SN��̍ŏ��l(global)
	 \param fZmaxG \~English Max value of smoothing SN ratio(global)
	 \~Japanese ������SN��̍ő�l(global)
	 \param fZminL \~English Minimum value of smoothing SN ratio(local)
	 \~Japanese ������SN��̍ŏ��l(local)
	 \param fZmaxL \~English Max value of smoothing SN ratio(local)
	 \~Japanese ������SN��̍ő�l(local)
     \param fBwFen \~English Max value of smoothing interval(frame)
	 \~Japanese ��������Ԃ̍ő�l(frame)
     \param fBwFst \~English Minimum value of smoothing interval(frame)
	 \~Japanese ��������Ԃ̍ŏ��l(frame)
     \param fBwG \~English Width of smoothing interval(global)
	 \~Japanese ��������ԕ�(global)
     \param fBwL \~English Width of smoothing interval(local)
	 \~Japanese ��������ԕ�(local)
	 \param fZthF \~English Threshold of smoothing SN ratio(frame)
	 \~Japanese ������SN���臒l(frame)
	 \param fZpmin \~English Minimum peak value of smoothing SN ratio
	 \~Japanese ������SN��s�[�N�̍ŏ��l
	 \param fZpmax \~English Max peak value of smoothing SN ratio
	 \~Japanese ������SN��s�[�N�̍ő�l
	 \param fAmpF \~English Smoothing factor of voice probability(frame)
	 \~Japanese �����m���������W��(frame)
	 \param fAmpG \~English Smoothing factor of voice probability(global)
	 \~Japanese �����m���������W��(global)
	 \param fAmpL \~English Smoothing factor of voice probability(local)
	 \~Japanese �����m���������W��(local)
	 \param fQmin \~English Minimum value of break probability
	 \~Japanese �x�~�m���̍ŏ��l
	 \param fQmax \~English Max value of break probability
	 \~Japanese �x�~�m���̍ő�l
	*/
	int Set_PF_PrmVoiceP(float fSmooth_prior_factor,float fZminF,float fZmaxF,float fZminG,float fZmaxG,
		float fZminL,float fZmaxL,int iBwFen,int iBwFst,int iBwG,int iBwL,float fZthF,
		float fZpmin,float fZpmax,float fAmpF,float fAmpG,float fAmpL,float fQmin,float fQmax) ;
	/**
	 \brief \~Japanese �ŏ��Q�C���p�����[�^�̐ݒ�
	 \param fMinGain \~English Minimum gain while without speeching
	 \~Japanese ���������̍ŏ��Q�C��
	*/
	int Set_PF_PrmGain(float fMinGain) ;
	/**
	 \brief \~Japanese Post-Filter�̎��s
	 \param csInputCSpec \~English Input signal spectra(complex)
	 \~Japanese ���͐M���X�y�N�g��(complex)
	 \param pcsOutputCSpec \~English Output signal spectra(complex)
	 \~Japanese �o�͐M���X�y�N�g��(complex)
	 \param pfsEstNoise \~English Estimated noise power spectra
	 \~Japanese ����G���p���[�X�y�N�g��
	*/
	int ExecutePF(MICARY_ComplexSpecArray csInputCSpec,MICARY_ComplexSpecArray *pcsOutputCSpec,
		MICARY_FloatSpecArray *pfsEstNoise) ;
	/**
	 \brief \~Japanese �����ύX���̏���
	 \param faInStatNoise \~English Power spectra estimated the state noise of each channel
	 \~Japanese ���肳�ꂽMCRA�p�����m�C�Y�̃p���[�X�y�N�g��
	 \param iaSrcCur \~English Source ID array for current frame
	 \~Japanese ���t���[���̉���ID�z��
	 \param iaSrcIni \~English Source ID array in front of frame
	 \~Japanese 1�t���[���O�̉���ID�z��
	*/
	int ChangeSrcPos(MICARY_FloatSpecArray faInStatNoise,MICARY_IntArray iaSrcCur,MICARY_IntArray iaSrcIni) ;
		
};
// ---------------------------------------------------------------------
#endif // CPF_exec_h_defined
