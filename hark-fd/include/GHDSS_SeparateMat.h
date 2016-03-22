// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_SeparateMat.h
 \brief	\~English Header file for making Separation matrix (GHDSS program).
 \brief	\~Japanese �����s��̂��߂̃w�b�_�t�@�C��(GHDSS program) 
 \author	Sachiko Suzuki, Aki Yamamoto(2010)
 \date	24 Dec 2009
 \version	$Id: GHDSS_SeparateMat.h 3538 2012-02-29 07:25:35Z tanaka $
*/

// ---------------------------------------------------------------------
#ifndef GHDSS_SeparateMat_h_defined
#define GHDSS_SeparateMat_h_defined
// ---------------------------------------------------------------------

///#include "MICARY_struct.h"
///#include "MICARY_GHDSS_struct.h"
///#include "ArrayInit.h"
//#include "GHDSS_MathMatDouble.h"
//#include "GHDSS_MathMat2.h"

//2011.01.19
#include "CMICARY_Matrix.h"

// 2007/09/04
#ifdef __cplusplus
extern "C"{
#endif

#define _MINIMUM_ZERO_VALUE 0.0000001
#define _OCRA_METHOD_NONE	0
#define	_OCRA_METHOD_FIX	1
#define _OCRA_METHOD_ADAPTIVE	2

#define _SS_METHOD_FIX	0
#define _SS_METHOD_LCMYU	1
#define _SS_METHOD_ADAPTIVE	2

#define _LC_CONST_FULL	0
#define _LC_CONST_DIAG	1

#define _LC_METHOD_FIX	0
#define _LC_METHOD_ADAPTIVE	1

#ifndef M_pi
#define M_pi (3.141592653589793238)
#endif
//
// ---------------------------------------------------------------------
/**
 \brief \~English It calculats TF value in free field .(1:true, 0:false)
 \brief \~Japanese ���R����ł�TF�̌v�Z�i1:����I���C0:�G���[�j
 \param psTFDB \~English It's the TF value database struct pointer.\n
 The elements of this database is (the number of source positions * the number of microphones * the number of frequency lines)
 \~Japanese TF�l���i�[����z����܂ލ\���̃|�C���^\n
 ���̃f�[�^�x�[�X�̗v�f��(�����ʒu��*�}�C�N���z���ʒu��*���g�����C����)
 \param iUpperFreq \~English It's the upper frequency line
 \~Japanese ������g�����C���ԍ�
 \param locaSrcPos \~English It's the source position struct
 \~Japanese �����ʒu���W
 \param locaMicPos \~English It's the mic position struct
 \~Japanese �}�C�N�ʒu���W
 \param fTemperature \~English It's the speed of sound[m/sec]
 \~Japanese ����[m/sec]
*/
int CalcTFFreeH(MICARY_TFDatabase * psTFDB , 
					 HA_Integer iUpperFreq ,
					 MICARY_PrmFrame sPrmFrame ,
					 MICARY_LocationArray locaSrcPos ,
					 MICARY_LocationArray locaMicPos ,
					 HA_Float fSpeedSound) ;

// 2012/01/13 added
/**
 \brief \~English It calculats TF value in free field .(1:true, 0:false)
\brief \~Japanese ���R����ł�TF�̌v�Z(SrcPos�̃C���f�b�N�X���w��)�i1:����I���C0:�G���[�j
 \param psTFDB \~English It's the TF value database struct pointer.\n
 The elements of this database is (the number of source positions * the number of microphones * the number of frequency lines)
 \~Japanese TF�l���i�[����z����܂ލ\���̃|�C���^\n
 ���̃f�[�^�x�[�X�̗v�f��(�����ʒu��*�}�C�N���z���ʒu��*���g�����C����)
 \param fTemperature \~English It's the speed of sound[m/sec]
 \~Japanese ����[m/sec]
*/
int CalcTFFreeH2(MICARY_TFDatabase * psTFDB , 
					 MICARY_PrmFrame sPrmFrame ,
					 int idsrc,
					 HA_Float fSpeedSound) ;

/**
 \brief \~English It makes separation matrix.(1:true, 0:false)
 \brief \~Japanese �����s��̏����l���쐬�i1:����I���C0:�G���[�j
 \param sTFDB \~English It's the Transfer function struct.
 \~Japanese TF�f�[�^�x�[�X
 \param pcsSeparationMat \~English It's the separated spectrum struct pointer.
 \~Japanese �����s��
 */
int MakeWMatGHDSS(MICARY_TFDatabase sTFDB,
						MICARY_SeparationMat * psSeparationMat);

// 2012/01/13 added
/**
 \brief \~English It makes separation matrix.(1:true, 0:false)
 \brief \~Japanese �����s��̏����l�������C���f�b�N�X�w��œǂݍ��݁i1:����I���C0:�G���[�j
 \param sTFDB \~English It's the Transfer function struct.
 \~Japanese TF�f�[�^�x�[�X
 \param pcsSeparationMat \~English It's the separated spectrum struct pointer.
 \~Japanese �����s��
 \param idsrc \~English Index of the source to load
 \~Japanese �ǂݍ��މ����C���f�b�N�X
 */
int MakeWMatGHDSS2(MICARY_TFDatabase sTFDB,
						MICARY_SeparationMat * psSeparationMat,
						int idsrc);

/**
 \brief \~English It calcurates py ,qy for Ryy(GHDSS).
 \brief \~Japanese Ryy�v�Z�ppy,qy�̎Z�o[GHDSS]
 \param caPY \~English It's calculated PY.
 \~Japanese PY�v�Z����
 \param caQY \~English It's calculated QY.
 \~Japanese QY�v�Z����
 \param caY \~English It's separated spectrum of a freq line.
 \~Japanese �����X�y�N�g��
 \param ha_fSSScal \~English It's scaling factor.
 \~Japanese �X�P�[�����O�t�@�N�^�[
*/
void _dGetPQY(MICARY_ComplexArray *caPY,MICARY_ComplexArray *caQY,
							 MICARY_ComplexArray caY,HA_Float ha_fSSScal) ;

//void _dGetPQY(MICARY_dComplexArray *caPY,MICARY_dComplexArray *caQY,
//							 MICARY_dComplexArray caY,HA_Float ha_fSSScal) ;

/**
 \brief \~English It calcurates step size parameter for SS(GHDSS).(1:true, 0:false)
 \brief \~Japanese SS�p�X�e�b�v�T�C�Y�X�V�ʂ̌v�Z[GHDSS]�i1:����I���C0:�G���[�j
 \param caW \~English It's the Separation matrix array of a freq line.
 \~Japanese �����s��
 \param caA \~English It's the Transfer function matrix array of a freq line.
 \~Japanese �`�B�֐��s��
 \param caY \~English It's separated spectrum of a freq line .
 \~Japanese �����X�y�N�g��
 \param caX \~English It's input spectrum of a freq line.
 \~Japanese ���̓X�y�N�g��
 \param iSSMethod \~English It's stepsize method(0:Fix, 1:same as LC_myu, 2:Adaptive).
 \~Japanese �X�e�b�v�T�C�Y�X�V�ʂ̌v�Z���@ (0:�Œ�, 1:LC_myu�Ƌ���, 2:Adaptive)
 \param fSSMyu \~English It's step size parameter.
 \~Japanese SS�p�X�e�b�v�T�C�Y�p�����[�^
 \param fSSScal \~English It's scaling parameter.
 \~Japanese SS�p�X�P�[�����O�p�����[�^
 \param fNoiseFloor \~English It's trancation value.
 \~Japanese �m�C�Y�t���A���x���l
 \param pcamyu_J_SS \~English It's separate matrix updated matrix.
 \~Japanese �����s��X�V�l
*/
int _dCalcSSStepSize_GHDSS(MICARY_ComplexArray caW ,
							MICARY_ComplexArray caA ,
							MICARY_ComplexArray caY ,
							MICARY_ComplexArray caX , 
							HA_Integer iSSMethod ,
							HA_Float fSSMyu ,
							HA_Float fSSScal ,
							HA_Float fNoiseFloor ,
							MICARY_ComplexArray *pcamyu_J_SS ) ;
//int _dCalcSSStepSize_GHDSS(MICARY_dComplexArray caW ,
//							MICARY_dComplexArray caA ,
//							MICARY_dComplexArray caY ,
//							MICARY_dComplexArray caX , 
//							HA_Integer iSSMethod ,
//							HA_Float fSSMyu ,
//							HA_Float fSSScal ,
//							HA_Float fNoiseFloor ,
//							MICARY_dComplexArray *pcamyu_J_SS ) ;


/**
 \brief \~English It calcurates step size parameter for LC(GHDSS).(1:true, 0:false)
 \brief \~Japanese LC�p�X�e�b�v�T�C�Y�X�V�ʂ̌v�Z[GHDSS]�i1:����I���C0:�G���[�j
 \param caW \~English It's the Separation matrix.
 \~Japanese �����s��
 \param caA \~English It's the Transfer Function matrix.
 \~Japanese �`�B�֐��s��
 \param iNsrc \~English It's the source number.
 \~Japanese ������
 \param iNmic \~English It's the microphone number.
 \~Japanese �}�C�N��
 \param iLCConst \~English It's the LC method(0:FULL ,1:DIAG).
 \~Japanese LC�v�Z���@(0:FULL, 1:DIAG)
 \param iLCMethod \~English It's the LC_myu method(0:Fix ,1:Adaptive).
 \~Japanese LC�p�X�e�b�v�T�C�Y�X�V�ʂ̌v�Z���@(0:�Œ�,1:�œK��)
 \param fLCMyu \~English It's step size parameter.
 \~Japanese LC�p�X�e�b�v�T�C�Y�p�����[�^
 \param pcamyu_J_LC \~English It's separate matrix updated value.
 \~Japanese �����s��X�V�l
*/

int _dCalcLCStepSize_GHDSS(MICARY_ComplexArray caW ,
							MICARY_ComplexArray caA ,
							HA_Integer iNsrc ,
							HA_Integer iNmic ,
							HA_Integer	iLCConst ,
							HA_Integer	iLCMethod ,
							HA_Float fLCMyu ,
							MICARY_ComplexArray *pcamyu_J_LC ) ;


/**
 \brief \~English It updates separation matrix for GHDSS.(1:true, 0:false)
 \brief \~Japanese �����s��̍X�V[GHDSS] �i1:����I���C0:�G���[�j
 \param psSavedSeparation \~English It's the saved value for executing GHDSS struct pointer.
 \~Japanese ���������ۑ��l
 \param csSeparationSpec \~English It's separated spectrum .
 \~Japanese �����X�y�N�g��
 \param csInputSpec \~English It's input spectrum .
 \~Japanese ���̓X�y�N�g��
 \param pcsRxx \~English It's input correlation matrix(Rxx) spectrum[double] .
 \~Japanese 1�t���[���O�̓��͐M�����֍s��
 \param pcsRyy \~English It's output crrelation matrix(Ryy) spectrum[couble] .
 \~Japanese 1�t���[���O�̏o�͐M�����֍s��
 \param sPrmSeparationASOCRA \~English It's setting parameter for GSS_AS_OCRA .
 \~Japanese ���������ݒ�p�����[�^ 
 \param HA_fWmyu \~English It's stepsize parameter for update .
 \~Japanese �X�V�p�X�e�b�v�T�C�Y�p�����[�^
 \param HA_fThreshold \~English It's update thereshold level[dB] .
 \~Japanese �X�V�pThreshold[dB]
 \param HA_iUpdate \~English It's update method .
 \~Japanese �X�V���@

*/

int dUpdateSeparationMatGHDSS(MICARY_SavedSeparation * psSavedSeparation ,
							MICARY_ComplexSpecArray csSeparationSpec ,
							MICARY_ComplexSpecArray csInputSpec , 
							MICARY_dComplexSpecArray *pcsRxx ,
							MICARY_dComplexSpecArray *pcsRyy ,
							MICARY_PrmSeparation_GHDSS sPrmSeparation,
							HA_Float ha_fWmyu,
							HA_Float ha_fThreshold,
							HA_Integer ha_iUpdate) ;

// added 2012/03/07
	// ���W���[�����ϐ��̏�����
void __SepMat_InitializeValues();
	
// added 2012/03/07
	// ���W���[�����ϐ���Alloc (Nsrc, Nmic�ɂ��ω�������̂̂�)
void __SepMat_AllocValues(int Nsrc, int Nmic);
	
// added 2012/03/07
	// ���W���[�����ϐ���Free
void __SepMat_FreeValues(bool bFreeSrc, bool bFreeMic);


#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------

#endif
