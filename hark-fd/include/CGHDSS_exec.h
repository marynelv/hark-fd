// ---------------------------------------------------------------------
/**
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.

 \file CGHDSS_exec.h
 \brief \~English Header file for execute GHDSS.
 \brief \~Japanese GHDSS���s�N���X�̃w�b�_(CGHDSS)
 \author Sachiko Suzuki
 \date 24 Dec 2009
 \version $Id$
*/

// ---------------------------------------------------------------------
#ifndef	CGHDSS_exec_h_defined
#define	CGHDSS_exec_h_defined
// ---------------------------------------------------------------------
///#include "MICARYHA_struct.h"	// ClassView �ɂ���Ēǉ�����܂����B
///#include "ArrayInit.h"
//#include "ArrayUtil.h"
///#include "MICARY_GHDSS_struct.h"
///#include "GHDSS_SeparateMat.h"
//#include "GHDSS_MathMat2.h"		2011.01.26
//#include "GHDSS_ReadLocationFile.h"
//#include "GHDSS_TFData.h"
// 2014/8/27 changed by N.Tanaka
#ifdef ENABLE_HARKIO_NH
#include "Cexec_NH.h"
#else
#include "Cexec.h"
#endif
//#include "GHDSS_MathMatDouble.h"  2011.01.26

#include <string>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

//#define __MICPOSITION_MOVE_ORIGINALPOINT	(0)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------------------------------------------------------------
/**
 \class CGHDSS_exec
 \brief \~English Class for executing GHDSS process 
 \brief \~Japanese GHDSS���s�N���X
*/
class CGHDSS_exec : public Cexec{
private:
	bool m_bIni;
	
	// modified 2008/07/01
	//MICARY_ComplexSpecArray *m_pcsRxx ,*m_pcsRyy ;
	MICARY_dComplexSpecArray *m_pcsRxx ,*m_pcsRyy ;
	MICARY_PrmSeparation_GHDSS m_prmSep ;

	// ���U�h�~�̂��߂̃p�����[�^
	HA_Float m_fWMyu;
	HA_Float m_fThreshold;
	HA_Integer m_iUpdate;
	
public:
	void SetbIni(bool bIni);
	/**
	 \breif \~Japanese �p�����[�^�̐ݒ�
	 \param ha_fWmyu \~English Stepsize parameter for update Separate matrix
	 \~Japanese �����s��X�V�̂��߂̃X�e�b�v�T�C�Y�p�����[�^
	 \param ha_fThreshold\~English Threshold for update separate matrix
	 \~Japanese �����s��X�V�̂��߂�threshold[dB]
	 \param ha_iUpdate \~English update method
	 \~Japanese �����s��X�V���@(0:�i�K�I�C1:�����I)
	*/
	void SetPrmGHDSS2(HA_Float ha_fWmyu,HA_Float ha_fThreshold,HA_Integer ha_iUpdate);

	void Initialize_GHDSS(HA_Integer iNch,HA_Integer iNpos,HA_Integer iLower_freq,HA_Integer iUpper_freq );
	/**
	 \brief \~Japanese �R���X�g���N�^
	*/
	CGHDSS_exec() ;
	/**
	 \brief \~Japanese �f�X�g���N�^
	*/
	~CGHDSS_exec() ;

	/**
	 \breif \~Japanese �p�����[�^�̐ݒ�
	 \param ha_iSSMethod \~English SS stepsize calculation method(0:Fix,1:lc_myu,2:Adaptive)
	 \~Japanese SS�X�e�b�v�T�C�Y�v�Z���@(0:Fix,1:lc_myu,2:Adaptive)
	 \param ha_fSSMyu \~English SS stepsize parameter
	 \~Japanese SS�X�e�b�v�T�C�Y�p�����[�^
	 \param ha_fNoiseFloor \~English Noise floor value  
	 \~Japanese �m�C�Y�t���A�l
	 \param ha_iLCConst \~English LC_Const calculation method(0:Full,1:Diag)  
	 \~Japanese LCConst�v�Z���@(0:Full,1:Diag)
	 \param ha_iLCMethod \~English LC stepsize calculation method(0:Fix,1:Adaptive)  
	 \~Japanese LC�X�e�b�v�T�C�Y�p�����[�^�v�Z���@(0:Fix,1:Adaptive)
	 \param ha_fLCMyu \~English LC stepsize parameter  
	 \~Japanese LC�X�e�b�v�T�C�Y�p�����[�^
	*/
	void SetPrmGHDSS(HA_Integer ha_iSSMethod,HA_Float ha_fSSMyu ,HA_Float ha_fSSScal,
								HA_Float ha_fNoiseFloor , HA_Integer ha_iLCConst ,
								HA_Integer ha_iLCMethod , HA_Float ha_fLCMyu) ;
	

	/**
	 \breif \~Japanese GHDSS�̎��s
	 \param pcsInputCSpec \~English Input spectra(complex)
	 \~Japanese ���̓X�y�N�g��(complex)
	 \param pcsOutputCSpec \~English Output spectra(complex)
	 \~Japanese �o�̓X�y�N�g��(complex)
	*/
	int ExecuteGHDSS(MICARY_ComplexSpecArray pcsInputCSpec,MICARY_ComplexSpecArray *pcsOutputCSpec) ;
	

		
};
// ---------------------------------------------------------------------
#endif // CGHDSS_exec_h_defined
