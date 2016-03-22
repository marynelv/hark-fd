// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file MICARY_PF_Struct.h
 \brief \~English Header file for Post Filter Struct
 \brief \~Japanese MICARY�|�X�g�t�B���^�̋��ʍ\���̂̂��߂̃w�b�_�t�@�C��(Post Filter)
 \author Masatoshi Tsutsumi
 \date 15 Nov 2006
 \version $Id: MICARY_PF_Struct.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	MICARY_PF_Struct_h_defined
#define	MICARY_PF_Struct_h_defined
// ---------------------------------------------------------------------
//#include <string>
#include "MICARYHA_struct.h"
#include "MICARY_struct.h"


// ---------------------------------------------------------------------

/**
 Gain Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_Float	ha_fMinGain;	///\~Japanese ���������̍ŏ��Q�C��
} MICARY_PrmGain;

/**
 Smoothing Priori SNR Parameters struct
 \author	Masatoshi Tsutsumi
 \date	29 Nov 2006
*/
typedef struct{
	HA_Float	ha_fB;	///\~Japanese ���ԕ������W��(b)
	HA_Integer	ha_fBwFst;	///\~Japanese �t���[�����ςŋ�`����K�p���鉺�����g���r��(bwFst)
	HA_Integer	ha_fBwFen;	///\~Japanese �t���[�����ςŋ�`����K�p���������g���r��(bwFen)
	HA_Integer	ha_fBwG;	///\~Japanese �L�ш敽�ϕ�(bwG)
	HA_Integer	ha_fBwL;	///\~Japanese ���ш敽�ςŋ�`����(bwL)
} MICARY_PrmZeta;

/**
 Voice Power Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_Float	ha_fB;	///\~Japanese ���ԕ������W��(b)
	HA_Integer	ha_fBwFst;	///\~Japanese �t���[�����ςŋ�`����K�p���鉺�����g���r��(bwFst)
	HA_Integer	ha_fBwFen;	///\~Japanese �t���[�����ςŋ�`����K�p���������g���r��(bwFen)
	HA_Integer	ha_fBwG;	///\~Japanese �L�ш敽�ϕ�(bwG)
	HA_Integer	ha_fBwL;	///\~Japanese ���ш敽�ςŋ�`����(bwL)
	HA_Float	ha_fZminF;	///\~Japanese ���OSN��̍ŏ��l (Frame)
	HA_Float	ha_fZmaxF;	///\~Japanese ���OSN��̍ő�l (Frame)
	HA_Float	ha_fZminG;	///\~Japanese ���OSN��̍ŏ��l (Global)
	HA_Float	ha_fZmaxG;	///\~Japanese ���OSN��̍ő�l (Global)
	HA_Float	ha_fZminL;	///\~Japanese ���OSN��̍ŏ��l (Local)
	HA_Float	ha_fZmaxL;	///\~Japanese ���OSN��̍ő�l (Local)
	HA_Float	ha_fZthF;	///\~Japanese zetaF0�̌W��
	HA_Float	ha_fZpmin;	///\~Japanese zpeak�̉����l
	HA_Float	ha_fZpmax;	///\~Japanese zpeak�̏���l
	HA_Float	ha_fAmpL;	///\~Japanese �������݊m���v�Z����Local�̑΂���W��
	HA_Float	ha_fAmpG;	///\~Japanese �������݊m���v�Z����Global�̑΂���W��
	HA_Float	ha_fAmpF;	///\~Japanese �������݊m���v�Z����Frame�̑΂���W��
	HA_Float	ha_fQmax;	///\~Japanese �������݊m���̏���l
	HA_Float	ha_fQmin;	///\~Japanese �������݊m���̉����l
} MICARY_PrmVoiceP;

/**
 Saved Voice Power Spectrum struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsSmSNR;	///\~Japanese �O�t���[���̕�����SN��
	MICARY_FloatArray	micary_faSmSNRF;	///\~Japanese �O�t���[���̕�����SN�� Frame
	MICARY_FloatArray	micary_faSmSNRP;	///\~Japanese �O�t���[���̕�����SN�� Peak
} MICARY_SavedVoiceP;

/**
 Estimate S/N Parameters struct
 \author	Masatoshi Tsutsumi
 \date	23 Nov 2006
*/
typedef struct{
	HA_Float	ha_fAlp_pmin;	///\~Japanese �ŏ������ܗL��(alp_pmin)
	HA_Float	ha_fAlp_pmag;	///\~Japanese �����ܗL���̌W��(alp_pmag)
	HA_Float	ha_fA;	///\~Japanese ���OSN��̍X�V��(a)
	HA_Float	ha_fXi_max;	///\~Japanese ���OSN��̃N���b�v(OP.xi_max)
	HA_Float	ha_fTheta_max;	///\~Japanese GH1�����߂�ۂ�v�̍ő�l(OP.theta_max)
	HA_Float	ha_fTheta_min;	///\~Japanese GH1�����߂�ۂ�v�̍ŏ��l(OP.theta_min)
} MICARY_PrmEstSN;

/**
 Saved Estimate S/N Values struct
 \author	Masatoshi Tsutsumi
 \date	23 Nov 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsInputPSpec;	///\~Japanese ���̓p���[(Shatp)
	MICARY_FloatSpecArray	micary_fsSN;	///\~Japanese ���OSN��(xi0)
} MICARY_SavedEstSN;

/**
 reverberation Noise Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_Float	ha_fGamma;	///\~Japanese �c�������萔(gamma)
	HA_Float	ha_fDelta;	///\~Japanese ���ډ�������(SRR)(delta)
} MICARY_PrmRevNoise;

/**
 Saved Reverberation Noise Values struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsRevPSpec;	///\~Japanese �O�t���[���̎c�������p���[�X�y�N�g��
} MICARY_SavedEstRev;

/**
 Leak Noise Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_Float	ha_fAlp_leak;	///\~Japanese Leak parameter (used like 'eta')
} MICARY_PrmLeakNoise;

/**
 Initial noise of MCRA Parameters struct
 \author	Masatoshi Tsutsumi
 \date	13 Dec 2006
*/
typedef struct{
	HA_Float	ha_fDelta;	///\~Japanese �����m��臒l
	HA_Integer	ha_iL;	///\~Japanese �X�V�̃t���[����
	HA_Float	ha_fAlp_s;	///\~Japanese ���̓X�y�N�g���̕������W��
	HA_Float	ha_fNoise_compense;	///\~Japanese valin spetical parameter (1,1) [1?]
	HA_Float	ha_fAlp_dmin;	///~\Japanese Minimum speech probability (1,1) [0.05]
	HA_Integer	ha_iInit_frame;	///~\Japanese Initialize update frams (1,1) [100]
} MICARY_PrmInitMCRA;

/**
 Saved Initial noise of MCRA Values struct
 \author	Masatoshi Tsutsumi
 \date	13 Dec 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsStateNoisePSpec;	///\~Japanese ���G���p���[�X�y�N�g��
	MICARY_FloatSpecArray	micary_fsSmin;	///\~Japanese ���͐M���̃p���[�X�y�N�g���̍ŏ��l
	MICARY_FloatSpecArray	micary_fsStmp;	///\~Japanese ���͐M���̃p���[�X�y�N�g���̈ꎞ�ŏ��l
	MICARY_FloatSpecArray	micary_fsS;	///\~Japanese ���͐M���̕������p���[�X�y�N�g��
	MICARY_FloatSpecArray	micary_fsSf;	///\~Japanese Frequency smoothed spectrum (N, M) [0?] for debug output
	HA_Num	ha_numProc;	///\~Japanese ������
} MICARY_SavedInitMCRA;

/**
 MCRA Parameters struct
 \author	Masatoshi Tsutsumi
 \date	5 Dec 2006
*/
typedef struct{
	HA_Float	ha_fAlp_d;				///\~Japanese �G���̕������W��
	HA_Float	ha_fDelta;				///\~Japanese �����m��臒l
	HA_Integer	ha_iL;					///\~Japanese �X�V�̃t���[����
	HA_Float	ha_fAlp_s;				///\~Japanese ���̓X�y�N�g���̕������W��
	HA_Float	ha_fLeak_ampleak_factor;	///\~Japanese �R��G���U���W��
	HA_Float	ha_fSs_floor;			///\~Japanese�@Floor�W��
	HA_Float	ha_fNoise_compense;		///\~Japanese �G���␳�l
	HA_Float	ha_fIn_Noise_compense;	///\~Japanese ���͐M���̒��G���������B�ǉ��X��2008.12.3
	HA_Integer	ha_iResetL;				///\~Japanese �����ύX���ɏ����񐔂����Z�b�g���邩 1:�ύX����C0:�ύX���Ȃ� �ǉ��X��2009.3.12
} MICARY_PrmMCRA;



typedef struct{///\~Japanese �ǉ��i�X���j2008.11.14
	HA_Char		*ha_pcSavename;			///\~Japanese �f�[�^�ۑ��� 
	HA_Char		*ha_iInitFromfilename;	///\~Japanese �����l��ǂݏo���t�@�C���̖��O
	HA_Integer	ha_iInitFromfile_ID;	///\~Japanese �����l��ǂݏo���t�@�C��ID�̏��
	HA_Integer	ha_iFileSave;			///\~Japanese �t�@�C����ۑ����邩�ǂ����H1:�ۑ�����C0:�ۑ����Ȃ�
	HA_Integer  ha_iKeepPeriod;			///\~Japanese �f�[�^�ێ��t���[����
	
} MICARY_PrmSave;





typedef struct{
	HA_Integer iID;							//����ID
	HA_Integer iSize;							//�ۑ��l�̔z��̃T�C�Y
	HA_Integer ha_iLowerFreq;					//�������g��
	HA_Integer ha_iUpperFreq;					//������g��
	HA_Integer	ha_numCh;						//����������
    HA_Char cPad[100];						//�p�b�h
}MICARY_SavedFileHeader;///\~Japanese �ǉ��i�X���j2008.11.14


/**
 Saved MCRA Values struct
 \author	Masatoshi Tsutsumi
 \date	5 Dec 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsStateNoisePSpec;	///\~Japanese ���G���p���[�X�y�N�g��
	MICARY_FloatSpecArray	micary_fsSmin;	///\~Japanese ���͐M���̃p���[�X�y�N�g���̍ŏ��l
	MICARY_FloatSpecArray	micary_fsStmp;	///\~Japanese ���͐M���̃p���[�X�y�N�g���̈ꎞ�ŏ��l
	HA_Num	ha_numProc;	///\~Japanese ������
	MICARY_FloatSpecArray	micary_fsS;	///\~Japanese ���͐M���̕������p���[�X�y�N�g��
} MICARY_SavedMCRA;



/**
 Post Filter Parameters struct
 \author	Masatoshi Tsutsumi
 \date	27 Dec 2006
*/
typedef struct{
	MICARY_PrmMCRA sPrmMCRA;	///\~Japanese MCRA�v�Z�p�����[�^
	HA_Float	ha_fAlp_d;	///\~Japanese �G���̕������W��
	HA_Float	ha_fDelta;	///\~Japanese �����m��臒l
	HA_Integer	ha_iL;	///\~Japanese �X�V�̃t���[����
	HA_Float	ha_fAlp_s;	///\~Japanese ���̓X�y�N�g���̕������W��
	HA_Float	ha_fLeak_ampleak_factor;	///\~Japanese
	HA_Float	ha_fSs_floor;	///\~Japanese
	HA_Float	ha_fNoise_compense;	///\~Japanese 
	
} MICARY_PrmPFExec;

/**
 Saved Post Filter Values struct
 \author	Masatoshi Tsutsumi
 \date	27 Dec 2006
*/
typedef struct{
	MICARY_SavedMCRA	*psSavedMCRA;	///\~Japanese ���G���v�Z�̕ۑ��l
} MICARY_SavedPFExec;

/**
 Command Line Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_String		cOriginalWavFile;	///<\~Japanese ���̓t�@�C����
	HA_Float		ha_fStartTime;	///<\~Japanes �J�n����[sec]
	HA_Float		ha_fEndTime;	///<\~Japanes �I������[sec]
	HA_String		cSeparateWavFile;	///<\~Japanese �����t�@�C����
	HA_String		cOriginalSpecFile;	///<\~Japanese ���̓X�y�N�g���t�@�C����
	HA_Integer		ha_iStartFrame;	///<\~Japanese ���͊J�n�t���[��[frame]
	HA_Integer		ha_iEndFrame;	///<\~Japanese ���͏I���t���[��[frame]
	HA_String		cSeparateSpecFile;	///<\~Japanese �o�̓X�y�N�g���t�@�C����
	HA_String		cNoiseSpecFile;	///<\~Japanese �G���X�y�N�g���t�@�C����
	HA_Integer		ha_iNFFT;			///<\~Japanese FFT��[point]
	HA_Integer		ha_iWindow;			///<\~Japanese ����[sample]
	MICARY_WindowEnum	micary_nwnWindow;	///<\~Japanese ��
	HA_Integer		ha_iShift;			///<\~Japanese �V�t�g��[sample]
	HA_Float		ha_fUpperFreq;	///<\~Japanes ���͏�����g��[Hz]
	MICARY_PrmMCRA		micary_sPrmMCRA;	///<\~Japanes MCRA�ݒ�p�����[�^
	MICARY_PrmLeakNoise		micary_sPrmLeakNoise;	///<\~Japanes �R��ݒ�p�����[�^
	MICARY_PrmRevNoise		micary_sPrmRevNoise;	///<\~Japanes �c�������p�����[�^
	MICARY_PrmEstSN		micary_sPrmEstSN;	///<\~Japanes SN����ݒ�p�����[�^
	MICARY_PrmVoiceP		micary_sPrmVoiceP;	///<\~Japanes �������݊m���ݒ�p�����[�^
	MICARY_PrmGain		micary_sPrmGain;	///<\~Japanes �œK�Q�C������p�����[�^
} MICARY_PFCmdArgs;
// ---------------------------------------------------------------------
#endif	// MICARY_PF_Struct_h_defined
