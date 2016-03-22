/**
Copywright (c)2006 Nittobo Acoustic Engineering,Co.,Ltd.

\file MICARY_GHDSS_struct.h
\brief\~Japanese GHDSS�p�\����
\author Sachiko Suzuki
\date 5 Oct 2006
\Version $Id: MICARY_GHDSS_struct.h 3538 2012-02-29 07:25:35Z tanaka $
*/
// ---------------------------------------------------------------------
#ifndef	MICARY_GHDSS_struct_h_defined
#define	MICARY_GHDSS_struct_h_defined
// ---------------------------------------------------------------------
#include	<stdio.h>
#include	"MICARY_def.h"
#include	"MICARY_struct.h"
#include	"ArrayInit.h"
// ---------------------------------------------------------------------
// 2007/07/09 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif

// CmdLinesArgs�̏�����

// --------------------------------------------------------------------
/**
The enum contains Command TF Referense form .
\author Sachiko Suzuki
\date 5 Oct 2006
*/

typedef enum{
	ntrCalcFree,
	ntrDB
} MICARY_TFRefEnum ;

/**
The enum contains Command separation matrix initialize form .
\author Sachiko Suzuki
\date 8 Dec 2006
*/

typedef enum{
	nsmInit,
	nsmLib
} MICARY_SepMatInitEnum ;

//----------------------------------------------------------------------

/**
The structure contains Command Line Args.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct {
	HA_String cOriginalWavFile ;	///<\~Japanese ���̓t�@�C����
	HA_Float ha_fStartTime ;	///<\~Japanese �J�n����[sec]
	HA_Float ha_fEndTime ;		///<\~Japanese �I������[sec]
	HA_String cSeparateWavFile ;	///<\~Japanese �����t�@�C����
	HA_String cOriginalSpecFile ;	///<\~Japanese ���̓X�y�N�g���t�@�C����
	HA_Integer ha_iStartFrame ;	///<\~Japanese ���͊J�n�t���[��[frame]
	HA_Integer ha_iEndFrame ;	///<\~Japanese ���͏I���t���[��[frame]
	HA_String cSeparateSpecFile ;	///<\~Japanese �o�̓X�y�N�g���t�@�C����
	HA_String cSrcLocFile ;			///<\~Japanese �����ʒu�t�@�C����
	HA_String dMicLocFile ;			///<\~Japanese �󉹓_�ʒu�t�@�C����
	HA_String cTFDBFile ;			///<\~Japanese TF�f�[�^�x�[�X��
	MICARY_TFRefEnum ntrTFRef ;	///<\~Japanese TF�Q�ƕ��@
	HA_Integer ha_iNFFT ;		///<\~Japanese FFT��[point]
	HA_Integer ha_iWindow ;		///<\~Japanese ����[sample]
	MICARY_WindowEnum micary_nwnWindow ;	///<\~Japanese ��
	HA_Integer ha_iShift ;		///<\~Japanese �V�t�g��[sample]
	HA_Float ha_fUpperFreq ;	///<\~Japanese ���͏�����g��[Hz]
	HA_Float ha_fStepSizePrm ;	///<\~Japanese �X�e�b�v�T�C�Y�p�����[�^
	HA_Float ha_fTemperature ;	///<\~Japanese �C��[��]
	MICARY_SepMatInitEnum  ntrSepMat ;	///<\~Japanese �����s��̏����� 
	HA_Float ha_fOutMag ;           ///<\~Japanese �o�̓Q�C�� 

} MICARY_GHDSSCmdArgs ;

// --------------------------------------------------------------------
/**
The structure contains Separation matrix.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct{
	MICARY_IntArray micary_iaFreqLinex ;	///<\~Japanese ���g���C���f�b�N�X
	HA_Num ha_numInput ;			///<\~Japanese ���͐M����
	HA_Num ha_numOutput ;			///<\~Japanese �o�͐M����
	MICARY_ComplexArray micary_cmpaSeparation ;	///<\~Japanese �����s��
	// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num ha_numReservedInput;	///<\~ ���͐M���̊m�ې�
	HA_Num ha_numReservedOutput;	///<\~ �o�͐M���̊m�ې�
#endif
} MICARY_SeparationMat ;

//----------------------------------------------------------------------
/**
The structure contains Saved Separation.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct{
	MICARY_TFDatabase micary_tdTFDB ;			///<\~Japanese TF�ǂݏo�������ۑ��l
	MICARY_SeparationMat micary_smSeparation ;	///<\~Japanese �����s��
} MICARY_SavedSeparation ;

// ---------------------------------------------------------------------
/**
The structure contains Parameter Separation.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct{
	HA_Float ha_fStepSizePrm ;	///<\~Japanese �X�e�b�v�T�C�Y�p�����[�^
} MICARY_PrmSeparation ;

//-----------------------------------------------------------------------
/**
The structure contains Parameter Separation for ASOCRA.
\author Sachiko Suzuki
\date 14 May 2008
*/
typedef struct{
	HA_Integer	ha_iSSMethod ; ///<\~Japanese SS���@
	HA_Float ha_fSSMyu ;	///<\~Japanese SS�p�X�e�b�v�T�C�Y�p�����[�^
	HA_Integer	ha_iLCMethod ;	///<~Japanese LC���@
	HA_Float	ha_fLCMyu ;	///<~Japanese	LC�p�X�e�b�v�T�C�Y�p�����[�^
	HA_Integer	ha_iAvg_Method ;	 ///<~Japanese	���ω����@
	HA_Float	ha_fAlpha ;	///<~Japanese ���ω��̏d�݌W��
	HA_Float	ha_fBeta ;	///<~Japanese �����W��
	HA_Integer	ha_iNmax ;	 ///<~Japanese �����̍ő�l
	HA_Float	ha_fTran ;	///<~Japanese trancation
} MICARY_PrmSeparationASOCRA ;

//-----------------------------------------------------------------------

// 2008/09/03 changed by N.Tanaka
// Version�ԍ��C�t�@�C���^�C�v�̒ǉ�
/*
/ **
The structure contains TFDataBase File header.
\author Sachiko Suzuki
\date 5 Oct 2006
* /
typedef struct{
	int lFs ;		///<\~Japanese �T���v�����O���g��[Hz]
	int lNFFT ;	///<\~Japanese FFT��[point]
	int lNMic ;	///<\~Japanese �}�C�N���z���ʒu��
	int lNSrc ;	///<\~Japanese �����ʒu��
	int lNFreq ;	///<\~Japanese ���g�����C����[line]
	int lMeshFlg ;	///<\~Japanese �����ʒu���i�q��ɂȂ��Ă��邩��flag
	int lMeshX ;	///<\~Japanese X�����̊i�q��
	int lMeshY ;	///<\~Japanese Y�����̊i�q��
	int lMeshZ ;	///<\~Japanese Z�����̊i�q��
	char cPad[100] ;	///<\~Japanese �p�b�h
} MICARY_TFDBFileHeader ;
*/
/**
The structure contains TFDataBase File header.
\author Sachiko Suzuki
\date 5 Oct 2006
*/
typedef struct{
	int lFs ;		///<\~Japanese �T���v�����O���g��[Hz]
	int lNFFT ;	///<\~Japanese FFT��[point]
	int lNMic ;	///<\~Japanese �}�C�N���z���ʒu��
	int lNSrc ;	///<\~Japanese �����ʒu��
	int lNFreq ;	///<\~Japanese ���g�����C����[line]
	int lMeshFlg ;	///<\~Japanese �����ʒu���i�q��ɂȂ��Ă��邩��flag
	int lMeshX ;	///<\~Japanese X�����̊i�q��
	int lMeshY ;	///<\~Japanese Y�����̊i�q��
	int lMeshZ ;	///<\~Japanese Z�����̊i�q��
	int iVersion;	///<\~Japanese �o�[�W�����ԍ�
	int iFileType;	///<\~Japanese �t�@�C���^�C�v�i0:TF_CONJ, 1:�����s��j
	char cPad[92] ;	///<\~Japanese �p�b�h
} MICARY_TFDBFileHeader ;


/**
 2Dimention array struct
 \author	Sachiko Suzuki
 \date	27 Nov 2006
*/
typedef struct{
	HA_Float **	ha_fValue; ///<\~Japanese HA_Float�z��
	HA_Num	ha_numValue1; ///<\~Japanese �P�����z��̃T�C�Y
	HA_Num ha_numValue2 ; ///<\~Japanese �Q�����z��̃T�C�Y
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved1; ///<\~Japanese �z��̎��ۂ̃T�C�Y
	HA_Num	ha_numReserved2; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_FloatArray2;

/**
 2Dimention complex array struct
 \author	Sachiko Suzuki
 \date	27 Nov 2006
*/
typedef struct{
	HA_Complex **	ha_cmpValue; ///<\~Japanese HA_Complex�z��
	HA_Num	ha_numValue1; ///<\~Japanese �P�����z��̃T�C�Y
	HA_Num ha_numValue2 ; ///<\~Japanese �Q�����z��̃T�C�Y
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved1; ///<\~Japanese �z��̎��ۂ̃T�C�Y
	HA_Num	ha_numReserved2; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_ComplexArray2;

/**
 GHDSS_Location Array struct
 \author	Sachiko Suzuki
 \date	19 Dec 2006
*/
typedef struct{
	HA_Id *	ha_idLocation; ///<\~Japanese �ʒu���WID�z��
	HA_Id *	ha_idExistence; ///<\~Japanese �ʒu���W�z��
	int micary_MoveFlg ;	///<\~Japanese �����ړ��t���O
	HA_Num ha_numLocation ;	///<\~Japanese ���W�ʒu��
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_GHDSSLocationId;

/**
 GHDSS_Frequency Array struct
 \author	Sachiko Suzuki
 \date	26 Feb 2007
*/
typedef struct{
	MICARY_IntArray *micary_iaFreqLine ;	///<\~Japanese ���g�����C���z��
	HA_Integer 	ha_iLowerFreq; ///<\~Japanese �������g�����C���ԍ�
	HA_Integer 	ha_iUpperFreq; ///<\~Japanese ������g�����C���ԍ�
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_GHDSSFreqLine;


// ---------------------------------------------------------------------
/**
The structure contains Parameter Separation.
\author Sachiko Suzuki
\date 12 May 2008
*/
typedef struct{
	HA_Integer	ha_iSSMethod ;	///<\~Japanese SS�X�e�b�v�T�C�Y�ƍX�V�ʂ̌v�Z���@(0:Fix,1:myu_LC,2:Adaptive)
	HA_Float	ha_fSSMyu ;	///<\~Japanese SS�p�X�e�b�v�T�C�Y�p�����[�^
	HA_Integer	ha_iOCRAMethod ;	///<\~Japanese OCRA���@(0:No,1:alpha->Fix,2:alpha->Adaptive)
	HA_Float	ha_fAlpha ;	///<\~Japanese ���ω��̏d�݌W��
	HA_Float	ha_fBeta ; ///<\~Japanese ��������̍ۂ̌W��
	HA_Integer	ha_iNmax ;	///<\~Japanese �����̍ő�l
	HA_Float	ha_fNoiseFloor ;	///<\~Japanese �m�C�Y�t���A�l
	HA_Integer	ha_iLCConst ;	///<\~Japanese LC_Const���@(0:Full ,1:Diag)
	HA_Integer	ha_iLCMethod ;	///<\~Japanese LC�X�e�b�v�T�C�Y�ƍX�V�ʂ̌v�Z���@(0:Fix,1:Adaptive)
	HA_Float	ha_fLCMyu ;	///<\~Japanese LC�p�X�e�b�v�T�C�Y�p�����[�^
} MICARY_PrmSeparation_ASOCRA ;

//----------------------------------------------------------------------
/**
The structure contains Parameter Separation for GHDSS.
\author Sachiko Suzuki
\date 24 Dec 2009
*/
typedef struct{
	HA_Integer	ha_iSSMethod ;	///<\~Japanese SS�X�e�b�v�T�C�Y�ƍX�V�ʂ̌v�Z���@(0:Fix,1:myu_LC,2:Adaptive)
	HA_Float	ha_fSSMyu ;	///<\~Japanese SS�p�X�e�b�v�T�C�Y�p�����[�^
	HA_Float	ha_fSSScal ;	///<\~Japanese SS�p�X�P�[�����O�t�@�N�^�[
	HA_Float	ha_fNoiseFloor ;	///<\~Japanese �m�C�Y�t���A�l
	HA_Integer	ha_iLCConst ;	///<\~Japanese LC_Const���@(0:Full ,1:Diag)
	HA_Integer	ha_iLCMethod ;	///<\~Japanese LC�X�e�b�v�T�C�Y�ƍX�V�ʂ̌v�Z���@(0:Fix,1:Adaptive)
	HA_Float	ha_fLCMyu ;	///<\~Japanese LC�p�X�e�b�v�T�C�Y�p�����[�^
} MICARY_PrmSeparation_GHDSS ;

// ---------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////////
/**
 \brief \~English It allocates float array(dim:2)
 \brief \~Japanese float�^�̓񎟌��z����m�ۂ���
 \param array \~English It's the array of float. (iArraySize1 * iArraySize2)
 \~Japanese float�^�̓񎟌��z��̃|�C���^
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese ����1�̔z��̌�
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese ����2�̔z��̌�
*/
void FloatArray2Alloc(MICARY_FloatArray2 * array, int iArraySize1, int iArraySize2) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates float array(dim:2)
 \brief \~Japanese float�^�̓񎟌��z����m�ۂ���
 \param array \~English It's the array of float. (iArraySize1 * iArraySize2)
 \~Japanese float�^�̓񎟌��z��̃|�C���^
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese ����1�̔z��̌�
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese ����2�̔z��̌�
 \param iReservedSize1 \~English It's the size of array (dim:1)
 \~Japanese ����1�̔z��̊m�ی�
 \param iReservedSize2 \~English It's the size of array (dim:2)
 \~Japanese ����2�̔z��̊m�ی�
*/
void FloatArray2Alloc_r(MICARY_FloatArray2 * array, int iArraySize1, int iArraySize2,
												  int iReservedSize1, int iReservedSize2) ;
#endif

/**
 \brief \~English It frees float array(dim:2)
 \brief \~Japanese float�^�̓񎟌��z����J������
 \param array \~English It's the array of float. (iArraySize1 * iArraySize2)
 \~Japanese int�^�̓񎟌��z��̃|�C���^
*/
void FloatArray2Free(MICARY_FloatArray2 * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees float array(dim:2)
 \brief \~Japanese float�^�̓񎟌��z��̎g�p�T�C�Y���N���A����
 \param array \~English It's the array of float. (iArraySize1 * iArraySize2)
 \~Japanese int�^�̓񎟌��z��̃|�C���^
*/
void FloatArray2Clear(MICARY_FloatArray2 * array);
#endif

/**
 \brief \~English It allocates complex array(dim:2)
 \brief \~Japanese complex�^�̓񎟌��z����m�ۂ���
 \param array \~English It's the array of complex. (iArraySize1 * iArraySize2)
 \~Japanese complex�^�̓񎟌��z��̃|�C���^
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese ����1�̔z��̌�
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese ����2�̔z��̌�
*/
void ComplexArray2Alloc(MICARY_ComplexArray2 * array, int iArraySize1, int iArraySize2) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates complex array(dim:2)
 \brief \~Japanese complex�^�̓񎟌��z����m�ۂ���
 \param array \~English It's the array of complex. (iArraySize1 * iArraySize2)
 \~Japanese complex�^�̓񎟌��z��̃|�C���^
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese ����1�̔z��̌�
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese ����2�̔z��̌�
 \param iReservedSize1 \~English It's the size of array (dim:1)
 \~Japanese ����1�̔z��̊m�ی�
 \param iReservedSize2 \~English It's the size of array (dim:2)
 \~Japanese ����2�̔z��̊m�ی�
*/
void ComplexArray2Alloc_r(MICARY_ComplexArray2 * array, int iArraySize1, int iArraySize2,
													  int iReservedSize1, int iReservedSize2) ;
#endif

/**
 \brief \~English It frees complex array(dim:2)
 \brief \~Japanese complex�^�̓񎟌��z����J������
 \param array \~English It's the array of complex. (iArraySize1 * iArraySize2)
 \~Japanese complex�^�̓񎟌��z��̃|�C���^
*/
void ComplexArray2Free(MICARY_ComplexArray2 * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex array(dim:2)
 \brief \~Japanese complex�^�̓񎟌��z��̎g�p�����N���A����
 \param array \~English It's the array of complex. (iArraySize1 * iArraySize2)
 \~Japanese complex�^�̓񎟌��z��̃|�C���^
*/
void ComplexArray2Clear(MICARY_ComplexArray2 * array);
#endif

/**
 \brief \~English It allocates separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_SeparationMat�\���̂̊m�ہi1:����I���C0:�G���[�j
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese �����s��\���̃|�C���^
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese ���g�����C����
 \param iLocSize \~English It's the number of source location.
 \~Japanese �����ʒu��
 \param iMicSize \~English It's the number of microphone .
 \~Japanese �}�C�N��
*/
void SeparationMatAlloc(MICARY_SeparationMat * SPM, int iFreqSize, int iLocSize, int iMicSize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_SeparationMat�\���̂̊m�ہi1:����I���C0:�G���[�j
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese �����s��\���̃|�C���^
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese ���g�����C����
 \param iLocSize \~English It's the number of source location.
 \~Japanese �����ʒu��
 \param iMicSize \~English It's the number of microphone .
 \~Japanese �}�C�N��
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Mic)
*/
void SeparationMatAlloc_r(MICARY_SeparationMat * SPM, int iFreqSize, int iLocSize, int iMicSize,
													int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) ;
#endif

/**
 \brief \~English It frees separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_SeparationMat�\���̂��J������i1:����I���C0:�G���[�j
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese �����s��\���̃|�C���^
*/
 void SeparationMatFree(MICARY_SeparationMat * SPM) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_SeparationMat�\���̂̎g�p�����N���A����i1:����I���C0:�G���[�j
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese �����s��\���̃|�C���^
*/
 void SeparationMatClear(MICARY_SeparationMat * SPM) ;
#endif

/**
 \brief \~English It allocates saved separation .(1:true, 0:false)
 \brief \~Japanese MICARY_SavedSeparation�\���̂̊m�ہi1:����I���C0:�G���[�j
 \param SSP \~English It's the saved separation struct pointer .
 \~Japanese ���������ۑ��l�\���̃|�C���^
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese ���g�����C����
 \param iLocSize \~English It's the number of source location.
 \~Japanese �����ʒu��
 \param iMicSize \~English It's the number of microphone .
 \~Japanese �}�C�N��
*/
void SavedSeparationAlloc(MICARY_SavedSeparation * SSP ,int iFreqSize, int iLocSize, int iMicSize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates saved separation .(1:true, 0:false)
 \brief \~Japanese MICARY_SavedSeparation�\���̂̊m�ہi1:����I���C0:�G���[�j
 \param SSP \~English It's the saved separation struct pointer .
 \~Japanese ���������ۑ��l�\���̃|�C���^
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese ���g�����C����
 \param iLocSize \~English It's the number of source location.
 \~Japanese �����ʒu��
 \param iMicSize \~English It's the number of microphone .
 \~Japanese �}�C�N��
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Mic)
*/
void SavedSeparationAlloc_r(MICARY_SavedSeparation * SSP ,int iFreqSize, int iLocSize, int iMicSize,
						  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) ;
#endif

/**
 \brief \~English It frees saved separation.(1:true, 0:false)
 \brief \~Japanese MICARY_SavedSeparation�\���̂��J������i1:����I���C0:�G���[�j
 \param SSP \~English It's the saved separation struct pointer .
 \~Japanese ���������ۑ��l�̃|�C���^
*/
void SavedSeparationFree(MICARY_SavedSeparation * SSP) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/*
 \brief \~English It frees saved separation.(1:true, 0:false)
 \brief \~Japanese MICARY_SavedSeparation�\���̂��J������i1:����I���C0:�G���[�j
 \param SSP \~English It's the saved separation struct pointer .
 \~Japanese ���������ۑ��l�̃|�C���^
*/
void SavedSeparationClear(MICARY_SavedSeparation * SSP) ;
#endif

/**
 \brief \~English It allocates GHDSS Location ID .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSSLocationId�\���̂̊m�ہi1:����I���C0:�G���[�j
 \param array \~English It's the location ID(for GHDSS) struct pointer .
 \~Japanese GHDSS���WID�z��\���̃|�C���^
 \param iArraySize \~English It's the number of Location.
 \~Japanese �ʒu���W��
*/
void LocationIdAlloc(MICARY_GHDSSLocationId * array, int iArraySize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/*
 \brief \~English It allocates GHDSS Location ID .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSSLocationId�\���̂̊m�ہi1:����I���C0:�G���[�j
 \param array \~English It's the location ID(for GHDSS) struct pointer .
 \~Japanese GHDSS���WID�z��\���̃|�C���^
 \param iArraySize \~English It's the number of Location.
 \~Japanese �ʒu���W��
 \param iReservedSize \~English It's the number of Location.
 \~Japanese �m�ۂ���ʒu���W��
*/
void LocationIdAlloc_r(MICARY_GHDSSLocationId * array, int iArraySize, int iReservedSize) ;
#endif

/**
 \brief \~English It frees GHDSS Location ID .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSSLocationId�\���̂��J������i1:����I���C0:�G���[�j
 \param array \~English It's the location ID(for GHDSS) struct pointer .
 \~Japanese GHDSS���WID�z��\���̃|�C���^
 */
void LocationIdFree(MICARY_GHDSSLocationId * array) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees GHDSS Location ID .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSSLocationId�\���̂̎g�p�����N���A����i1:����I���C0:�G���[�j
 \param array \~English It's the location ID(for GHDSS) struct pointer .
 \~Japanese GHDSS���WID�z��\���̃|�C���^
 */
void LocationIdClear(MICARY_GHDSSLocationId * array) ;
#endif

/**
 \brief \~English It allocates GHDSS frequency line struct .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSS���g�����C���\���̂̊m�ہi1:����I���C0:�G���[�j
 \param array \~English It's the frequency line struct pointer .
 \~Japanese ���g�����C���\���̃|�C���^
 \param iLowerFreqLine \~English It's the lower frequency line number .
 \~Japanese �������g�����C���ԍ�
 \param iUpperFreqLine \~English It's the upper frequency line number .
 \~Japanese ������g�����C���ԍ�
*/
void FreqLineAlloc(MICARY_GHDSSFreqLine * array, int iLowerFreqLine , int iUpperFreqLine) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates GHDSS frequency line struct .(1:true, 0:false)
 \brief \~Japanese MICARY_GHDSS���g�����C���\���̂̊m�ہi1:����I���C0:�G���[�j
 \param array \~English It's the frequency line struct pointer .
 \~Japanese ���g�����C���\���̃|�C���^
 \param iLowerFreqLine \~English It's the lower frequency line number .
 \~Japanese �������g�����C���ԍ�
 \param iUpperFreqLine \~English It's the upper frequency line number .
 \~Japanese ������g�����C���ԍ�
 \param iReservedSize \~English It's size of array .
 \~Japanese �m�ۂ���v�f��
*/
void FreqLineAlloc_r(MICARY_GHDSSFreqLine * array, int iLowerFreqLine , int iUpperFreqLine, int iReservedSize) ;
#endif

/*
 \brief \~English It frees frequency line struct .(1:true, 0:false)
 \brief \~Japanese ���g�����C���\���̂��J������i1:����I���C0:�G���[�j
 \param array \~English It's the frequency line struct pointer .
 \~Japanese ���g�����C���\���̃|�C���^
 */
void FreqLineFree(MICARY_GHDSSFreqLine * array) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/*
 \brief \~English It frees frequency line struct .(1:true, 0:false)
 \brief \~Japanese ���g�����C���\���̂̎g�p�����N���A����i1:����I���C0:�G���[�j
 \param array \~English It's the frequency line struct pointer .
 \~Japanese ���g�����C���\���̃|�C���^
 */
void FreqLineClear(MICARY_GHDSSFreqLine * array) ;
#endif


/**
 Database of Separation matrix
 \author	Sachiko Suzuki
 \date	10 Jan 2008
*/

typedef struct{
	MICARY_SeparationMat micary_smSeparation ;	///<\~Japanese �����s��	
	MICARY_LocationArray	micary_locaSource; ///<\~Japanese �`�B�֐��̉�������ʒu���W�z��
	MICARY_LocationArray	micary_locaMic; ///<\~Japanese �`�B�֐��̃}�C�N���z���ʒu���W�z��
} MICARY_SMDatabase;

/**
 \brief \~English It allocates database of separation matrix
 \brief \~Japanese �����s����i�[����z����m�ۂ���
 \param smD \~English It's Database of separation matrix
 \~Japanese �����s����i�[����z����܂ލ\����
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese �`�B�֐��̎��g�����C����
 \param iLocSize \~English It's the number of location
 \~Japanese �`�B�֐��̉����ʒu��
 \param iMicSize \~English It's the number of microphone
 \~Japanese �`�B�֐��̃}�C�N���z���ʒu��
*/
void SMDatabaseAlloc(MICARY_SMDatabase * SMD, int iFreqSize, int iLocSize, int iMicSize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates database of separation matrix
 \brief \~Japanese �����s����i�[����z����m�ۂ���
 \param smD \~English It's Database of separation matrix
 \~Japanese �����s����i�[����z����܂ލ\����
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese �`�B�֐��̎��g�����C����
 \param iLocSize \~English It's the number of location
 \~Japanese �`�B�֐��̉����ʒu��
 \param iMicSize \~English It's the number of microphone
 \~Japanese �`�B�֐��̃}�C�N���z���ʒu��
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Mic)
*/
void SMDatabaseAlloc_r(MICARY_SMDatabase * SMD, int iFreqSize, int iLocSize, int iMicSize,
											  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) ;
#endif

/**
 \brief \~English It frees database of separation matrix
 \brief \~Japanese �����s����i�[����z����J������
 \param SMD \~English It's Database of separation matrix
 \~Japanese �����s����i�[����z����܂ލ\����
*/
void SMDatabaseFree(MICARY_SMDatabase * SMD) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees database of separation matrix
 \brief \~Japanese �����s����i�[����z��̎g�p�����N���A����
 \param SMD \~English It's Database of separation matrix
 \~Japanese �����s����i�[����z����܂ލ\����
*/
void SMDatabaseClear(MICARY_SMDatabase * SMD) ;
#endif

/**
 \brief \~English It initializes complex array
 \brief \~Japanese Complex�z��̏�����(0����)
 \param array \~English It's complex array
 \~Japanese Complex�z����܂ލ\����
*/

void ComplexArrayInit(MICARY_ComplexArray * array) ;

/**
The structure contains Update Separation matrix.
\author Sachiko Suzuki
\date 6 Feb 2009
*/
typedef struct{
	MICARY_IntArray micary_iaUpdate ;	///<\~Japanese �X�V�����t���O
	MICARY_SMDatabase micary_dbSeparation ;	///<\~Japanese �����s��
} MICARY_UpdateSeparationMat ;

/**
 \brief \~English It allocates update separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_UpdateSeparationMat�\���̂̊m�ہi1:����I���C0:�G���[�j
 \param SPM \~English It's the update separation matrix struct pointer .
 \~Japanese �X�V�p�����s��\���̃|�C���^
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese ���g�����C����
 \param iLocSize \~English It's the number of source location.
 \~Japanese �����ʒu��
 \param iMicSize \~English It's the number of source location.
 \~Japanese �}�C�N��
 */
void UpdateSeparationMatAlloc(MICARY_UpdateSeparationMat * SPM, int iFreqSize, int iLocSize,int iMicSize) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates update separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_UpdateSeparationMat�\���̂̊m�ہi1:����I���C0:�G���[�j
 \param SPM \~English It's the update separation matrix struct pointer .
 \~Japanese �X�V�p�����s��\���̃|�C���^
 \param iFreqSize \~English It's the size of frequency.
 \~Japanese ���g�����C����
 \param iLocSize \~English It's the number of source location.
 \~Japanese �����ʒu��
 \param iMicSize \~English It's the number of source location.
 \~Japanese �}�C�N��
 */
void UpdateSeparationMatAlloc_r(MICARY_UpdateSeparationMat * SPM, int iFreqSize, int iLocSize,int iMicSize,
							  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) ;
#endif

/**
 \brief \~English It frees update separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_UpdateSeparationMat�\���̂��J������i1:����I���C0:�G���[�j
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese �X�V�p�����s��\���̃|�C���^
*/
 void UpdateSeparationMatFree(MICARY_UpdateSeparationMat * SPM) ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees update separation matrix.(1:true, 0:false)
 \brief \~Japanese MICARY_UpdateSeparationMat�\���̂��g�p�����N���A����i1:����I���C0:�G���[�j
 \param SPM \~English It's the separation matrix struct pointer .
 \~Japanese �X�V�p�����s��\���̃|�C���^
*/
void UpdateSeparationMatClear(MICARY_UpdateSeparationMat * SPM) ;
#endif

//
#ifdef __cplusplus
}
#endif
// -------------------------------------------------------------------------------------
#endif // MICARY_GHDSS_struct_h_defined

