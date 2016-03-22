// ---------------------------------------------------------------------
/**
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.

 \file CPF_init_state_noise.h
 \brief \~English Header file for estimating the state noise of the each channnel
 \brief \~Japanese �e�`�����l���̒��G������N���X�̃w�b�_(Post Filter)
 \author Natsu Tanaka
 \date 6 Jun 2007
 \version $Id: CPF_init_state_noise.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	CPF_init_state_noise_h_defined
#define	CPF_init_state_noise_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
#include "PF_values.h"
#include "ArrayInit.h"
// ---------------------------------------------------------------------
/**
 \class CPF_init_state_noise
 \brief \~English Class for estimating the state noise of the each channnel
 \brief \~Japanese MCRA�p�����m�C�Y����N���X
*/
class CPF_init_state_noise{
private:
	MICARY_SavedInitMCRA	*m_psSavedInitMCRA;	// ���̓`�����l��MCRA�O�t���[���ۑ��l
	MICARY_PrmInitMCRA	m_sPrmInitMCRA;	// MCRA�p�����[�^
	
public:
	/**
	 \brief \~Japanese �R���X�g���N�^
	*/
	CPF_init_state_noise();
	/**
	 \brief \~Japanese �f�X�g���N�^
	*/
	~CPF_init_state_noise();
	
	/**
	 \brief \~Japanese �N���A
	*/
	int Clear();

	/**
	 \brief \~Japanese MCRA�p�����m�C�Y����N���X�̏�����
	 \param iLowerFreq \~English Lower frequency[line]
	 \~Japanese �������g�����C��
	 \param iUpperFreq \~English Upper frequency[line]
	 \~Japanese ������g�����C��
	 \param iNCh \~English Number of channel
	 \~Japanese �`�����l����
	*/
	int Initialize(int iLowerFreq, int iUpperFreq, int iNCh);


	/**
	 \brief \~Japanese MCRA�p�����m�C�Y�̎擾
	 \param faInputPSpec \~English Input power spectrum (N, M)
	 \~Japanese ���̓p���[�X�y�N�g��
	 \param pfsInitNoisePSpec \~English MCRA values
	 \~Japanese MCRA�����m�C�Y
	*/
	int InitStateNoise(MICARY_FloatSpecArray faInputPSpec,
                       MICARY_FloatSpecArray *pfsInitNoisePSpec);

	/**
	 \brief \~Japanese MCRA�p�����m�C�Y�p�p�����[�^�̐ݒ�
	 \param sPrmInitMCRA \~English Parameters for Initial noise of MCRA calc.
	 \~Japanese MCRA�����m�C�Y�v�Z�p�����[�^
	*/
	int SetPrmInitMCRA(MICARY_PrmInitMCRA sPrmInitMCRA);
};
// ---------------------------------------------------------------------
#endif // CPF_init_state_noise_h_defined
