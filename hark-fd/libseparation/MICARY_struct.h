// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file MICARY_struct.h
 \brief \~English Header file for Microphone Array System
 \brief \~Japanese �}�C�N�A���C�V�X�e���̋��ʍ\���̂̂��߂̃w�b�_�t�@�C��(Microphone Array System)
 \author Natsu Tanaka 
 \date 20 Jul 2005
 \version $Id: MICARY_struct.h 3538 2012-02-29 07:25:35Z tanaka $
*/

// ---------------------------------------------------------------------
#ifndef	MICARY_struct_h_defined
#define	MICARY_struct_h_defined
// ---------------------------------------------------------------------
#include "MICARY_def.h"	// 2011/11/16 added by N.Tanaka
#include "MICARYHA_struct.h"
// modified by S.Suzuki 2010/03/28
#include <stdio.h>
//#include "wave.h"	// 2006/09/29 added by N.Tanaka
//#include "SpectrumFile.h"	// 2006/09/29 added by N.Tanaka	// 2008/11/11 deleted by N.Tanaka

// 2008/11/11 added by N.Tanaka
// micarray_fdplugin�p�̑΍�
#ifndef HA_STRING_MAXLEN
#define HA_STRING_MAXLEN     1024
#endif
// ---------------------------------------------------------------------

/**
 TF Calclating Flag Enum
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef enum{
	ntcFreeField,
	ntcReflection1p,
	ntcReflection1m,
} MICARY_TFCalcEnum;

/**
 Frequency Scale Enum
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef enum{
	nfsLog,
	nfsLinear
} MICARY_FreqScaleEnum;

/**
 Kind of window Enum
 \author	Natsu Tanaka
 \date	19 Oct 2005
*/
typedef enum{
	nwnHanning,
	nwnSpecial
} MICARY_WindowEnum;

/**
 Location Flag
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef int	MICARY_LocationFlg;

/**
 Float array struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Float *	ha_fValue; ///<\~Japanese HA_Float�z��
	HA_Num	ha_numValue; ///<\~Japanese �z��̃T�C�Y
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_FloatArray;

/**
 Integer array struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Integer *	ha_iValue; ///<\~Japanese HA_Integer�z��
	HA_Num	ha_numValue; ///<\~Japanese �z��̃T�C�Y
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_IntArray;

/**
 Complex array struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Complex *	ha_cmpValue; ///<\~Japanese HA_Complex�z��
	HA_Num	ha_numValue; ///<\~Japanese �z��̃T�C�Y
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_ComplexArray;

/**
 Location struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Float	ha_fX; ///<\~Japanese X���W[m]
	HA_Float	ha_fY; ///<\~Japanese Y���W[m]
	HA_Float	ha_fZ; ///<\~Japanese Z���W[m]
	HA_Float	ha_fTheta; ///<\~Japanese ���ʊp[deg]
	HA_Float	ha_fPhi; ///<\~Japanese �p[deg]
	MICARY_LocationFlg	micary_LocationFlag; ///<\~Japanese �L���ȍ��W�������t���O
	HA_Id	ha_idPos; ///<\~Japanese �ʒuID // 2008/05/15 added by N.Tanaka
	HA_Id ha_idDir; ///<\~Japanese ����ID // 2008/05/15 added by N.Tanaka
	HA_Id tfindex[3]; ///<\~Japanese TFIndex // 2013/06/26 added by K.Morio
	HA_Id tfindex_max[3]; ///<\~Japanese TFIndex�̍ő�l // 2013/10/10 added by K.Morio
    HA_Id tfindex_min[3]; ///<\~Japanese TFIndex�̍ŏ��l // 2013/10/10 added by K.Morio
	HA_Id tfposid;	///<\~Japanese TFDB���̈ʒuID // 2014/8/27 added by N.Tanaka ha_idPos���قȂ�g���������Ă��邽�ߒǉ�

} MICARY_Location;

/**
 Location Array struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Id *	ha_idLocation; ///<\~Japanese �ʒu���WID�z��
	MICARY_Location *	micary_Location; ///<\~Japanese �ʒu���W�z��
	HA_Num	ha_numLocation; ///<\~Japanese �ʒu���W��
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_LocationArray;

/**
 Parameter of setting struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Float	ha_fFs; ///<\~Japanese �T���v�����O���g��[Hz]
	HA_Num		ha_numNFFT; ///<\~Japanese FFT��
	HA_Num		ha_numWindowLength; ///<\~Japanese ����
	MICARY_FreqScaleEnum	micary_FScale; ///<\~Japanese ���g����
	HA_Float	ha_fTemperature; ///<\~Japanese �C��[deg C]
} MICARY_SettingParameter;

/**
 Database of BF
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	MICARY_ComplexArray	micary_cmpaBF; ///<\~Japanese BF�W���z��
	MICARY_IntArray		micary_iaFreqLine; ///<\~Japanese BF�W���̎��g�����C���z��
	MICARY_LocationArray	micary_locaSource; ///<\~Japanese BF�W���̉�������ʒu���W�z��
	MICARY_LocationArray	micary_locaMic; ///<\~Japanese BF�W���̃}�C�N���z���ʒu���W�z��
	HA_Float		ha_fSearchMicR; ///<\~Japanese BF�v�Z�ΏۂƂȂ�}�C�N���z���Ɖ����T���ʒu�̋����̏��[m]
	//HA_Num			ha_numMic;
} MICARY_BFDatabase;

/**
 Database of TF
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	MICARY_ComplexArray	micary_cmpaTF; ///<\~Japanese �`�B�֐��z��
	MICARY_IntArray	micary_iaFreqLine; ///<\~Japanese �`�B�֐��̎��g�����C���z��
	MICARY_LocationArray	micary_locaSource; ///<\~Japanese �`�B�֐��̉�������ʒu���W�z��
	MICARY_LocationArray	micary_locaMic; ///<\~Japanese �`�B�֐��̃}�C�N���z���ʒu���W�z��
	//HA_Num			ha_numMic;
} MICARY_TFDatabase;

/**
 Integer array struct(dim:2)
 \author	Natsu Tanaka
 \date	12 Oct 2005
*/
typedef struct{
	HA_Integer **	ha_iValue; ///<\~Japanese HA_Integer�^2�����z��
	HA_Num	ha_numValue1; ///<\~Japanese �z��̃T�C�Y
	HA_Num	ha_numValue2; ///<\~Japanese �z��̃T�C�Y
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved1; ///<\~Japanese �z��̎��ۂ̃T�C�Y
	HA_Num	ha_numReserved2; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_IntArray2;

/**
 ADC data array struct
 \author	Natsu Tanaka
 \date 5 Dec 2005
*/
typedef struct{
	HA_Num	ha_numDataLength; ///<\~Japanese �L���Ȓl�̐�
	HA_Num	ha_numCh;	///<\~Japanese AD�`�����l���� // 2006/11/09 added by N.Tanaka
	MICARY_IntArray micary_iaData; ///<\~Japanese �m�ۍς݂�HA_Integer�̔z��
} MICARY_ADCDataArray;

// modified S.Suzuki 2010/03/28
/**
 Wavfile information struct
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
/*
typedef struct{
	//char	cFilename[256]; ///<\~Japanese �t�@�C����
	HA_String ha_stFileName; ///<\~Japanese �t�@�C����
	FILE *	pfinFile; ///<\~Japanese �t�@�C���|�C���^
	HA_Num	ha_numCurrentSample; ///<\~Japanese �ǂݏo���ς݃T���v����[sample]
	struct waveheader sWavHeader; ///<\~Japanese wav�t�@�C���w�b�_
} MICARY_WavFile;
*/
// 2008/11/11 moved from SpectrumFile.h
// 2008/11/11 changed by N.Tanaka
// X86_64bit�̏ꍇlong��8byte�ƂȂ邽�߁Cint�ɕύX
/**
 File Header struct
 \author	Natsu Tanaka
 \date	11 Sep 2006
*/
typedef struct {
	unsigned int ulVersion;	///<\~Japanese �o�[�W����
	float fFs;					///<\~Japanese �T���v�����O���g��[Hz]
	unsigned int ulNFFT;		///<\~Japanese FFT��[point]
	unsigned int ulWindow;		///<\~Japanese ����[sample]
	int lNFrame;				///<\~Japanese �t���[����(����̏ꍇ�͕���)
	int lShift;				///<\~Japanese �V�t�g��[sample]
	unsigned int ulNCh;		///<\~Japanese �`�����l����
	unsigned int ulLowerFreqLine;	///<\~Japanese �������g�����C���ԍ�
	unsigned int ulUpperFreqLine;	///<\~Japanese ������g�����C���ԍ�
	unsigned short usDataType;	///<\~Japanese �f�[�^�̃^�C�v(0:float, 1:complex(float))
	char cPad[22];
} MICARY_SpecFileHeader;

/**
 Spectrumfile information struct
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	//char	cFilename[256]; ///<\~Japanese �t�@�C����
	HA_String ha_stFileName; ///<\~Japanese �t�@�C����
	FILE *	pfinFile; ///<\~Japanese �t�@�C���|�C���^
	HA_Num	ha_numCurrentFrame; ///<\~Japanese �ǂݏo���ς݃t���[����[frame]
	MICARY_SpecFileHeader micary_sSpecHeader; ///<\~Japanese �X�y�N�g���t�@�C���w�b�_
} MICARY_SpecFile;

// modified S.Suzuki 2010/03/28
/**
 Wavfile struct for getting a frame data
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
/*
typedef struct{
	MICARY_WavFile	micary_sWavFile; ///<\~Japanese WAV�t�@�C�����
	HA_Integer	ha_iStartsample; ///<\~Japanese �J�n���ԃT���v��[sample]
	HA_Integer	ha_iEndSample; ///<\~Japanese �I�����ԃT���v��[sample]
} MICARY_SavedGetWavFrame;
*/
// modified S.Suzuki 2010/03/28
/**
 Wavfile struct for setting a frame data
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
/*
typedef struct{
	MICARY_WavFile	micary_sWavFile; ///<\~Japanese WAV�t�@�C�����
	MICARY_FloatArray	micary_faWave; ///<\~Japanese ���n��f�[�^
} MICARY_SavedSetWavFrame;
*/
/**
 Spectrumfile struct for getting a frame data
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	MICARY_SpecFile	micary_sSpecFile; ///<\~Japanese �X�y�N�g���t�@�C�����
	HA_Integer	ha_iStartFrame; ///<\~Japanese �J�n���ԃt���[��[frame]
	HA_Integer	ha_iEndFrame; ///<\~Japanese �I�����ԃt���[��[frame]
} MICARY_SavedGetSpecFrame;

/**
 Spectrumfile struct for setting a frame data
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	MICARY_SpecFile	micary_sSpecFile; ///<\~Japanese �X�y�N�g���t�@�C�����
	HA_Num	ha_numCurrentFrame; ///<\~Japanese �������ݍς݃t���[����[frame]
} MICARY_SavedSetSpecFrame;

/**
 Parameter struct for getting frame data
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	HA_Float	ha_fFs;	///<\~Japanese �T���v�����O���g��[Hz]
	HA_Num	ha_numNFFT; ///<\~Japanese FFT��[point]
	HA_Integer	ha_iShift; ///<\~Japanese �V�t�g��[sample](���t���[���̎n�[���O�t���[���̏I�[)
	HA_Num	ha_numWindow; ///<\~Japanese ����[sample]
	MICARY_WindowEnum	micary_nwnWindow; ///<\~Japanese ���֐�
} MICARY_PrmFrame;

/**
 Spectrum struct (Complex)
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	MICARY_ComplexArray	micary_cmpaSpec; ///<\~Japanese �X�y�N�g��(Complex) 
	HA_Num	ha_numCh; ///<\~Japanese �`�����l����
	HA_Integer	ha_iLowerFreq; ///<\~Japanese �������g��[Line]
	HA_Integer	ha_iUpperFreq; ///<\~Japanese ������g��[Line]
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReservedChSize; ///<\~Japanese �z��̎��ۂ̃T�C�Y
	HA_Num	ha_numReservedFreqSize; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_ComplexSpecArray;

/**
 Spectrum struct (Float)
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	MICARY_FloatArray	micary_faSpec; ///<\~Japanese �X�y�N�g��(Float) 
	HA_Num	ha_numCh; ///<\~Japanese �`�����l����
	HA_Integer	ha_iLowerFreq; ///<\~Japanese �������g��[Line]
	HA_Integer	ha_iUpperFreq; ///<\~Japanese ������g��[Line]
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReservedChSize; ///<\~Japanese �z��̎��ۂ̃T�C�Y
	HA_Num	ha_numReservedFreqSize; ///<\~Japanese �z��̎��ۂ̃T�C�Y
#endif
} MICARY_FloatSpecArray;

// ---------------------------------------------------------------------
#endif	// MICARY_struct_h_defined
