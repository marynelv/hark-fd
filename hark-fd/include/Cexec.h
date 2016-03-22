// ---------------------------------------------------------------------
/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#ifndef	Cexec_h_defined
#define	Cexec_h_defined
// ---------------------------------------------------------------------

#include "../config.h"
#ifndef ENABLE_HARKIO_NH

///#include "ArrayInit.h"
///#include "MICARY_GHDSS_struct.h"
#include "GHDSS_SeparateMat.h"
///#include "GHDSS_MathMat2.h"
//#include "GHDSS_TFData.h"

#include <string>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "MICARYHA_struct.h"	// ClassView �ɂ���Ēǉ�����܂����B
#include "HarkFileIO.h"
// 2014/8/25 added by N.Tanaka libharkio3�Ή�
#ifdef ENABLE_HARKIO3
extern "C"{
	#include "libharkio3.h"
}
#endif


#define __MICPOSITION_MOVE_ORIGINALPOINT	(1) // 0 : shift position center / 1 : fix

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define __SAVED_LOC_THETA_PITCH	(5)	//5[deg]�s�b�`
#define __UPDATE_METHOD_ID	(0)
#define __UPDATE_METHOD_POS (1)
#define __UPDATE_METHOD_ID_POS (2)
#define __INVALID_SRC_ID (-32768)

#define __SRCID_FRONT_CREATE	(1)
#define __SRCID_FRONT_EXIST		(0)
#define __SRCID_FRONT_REMOVE	(-1)

#define __FILE_FORMAT_MICARY	(0)
#define __FILE_FORMAT_HARK	(1)

// Added by morio 2013/06/19
#define __COMPARE_MODE_DEG	(0)
#define __COMPARE_MODE_TFINDEX (1)

// ---------------------------------------------------------------------
/**
 \class Cexec
 \brief \~English Class for executing GHDSSprocess 
 \brief \~Japanese GHDSS�������s�N���X
*/
class Cexec{
public:
	
	/**
	 \brief \~Japanese �R���X�g���N�^
	*/
	Cexec() ;
	
      /**
	 \brief \~Japanese �f�X�g���N�^
	*/
	~Cexec() ;

	/**
	 \brief \~Japanese �N���A
	 \return \~English error code (0 is error ,1 is success)	
	*/
	int Clear() ;
      
	MICARY_SavedSeparation *m_psSepMat ;	//�����s��ۑ��l
	
	
	/**
	 \brief \~Japanese ������
	 \return \~English error code (0 is error ,1 is success)	
	 \param iNch \~English Size of microphone channels.
	 \~Japanese �}�C�N�`�����l����
	 \param iNpos \~English Size of sources. 
	 \~Japanese ������
	 \param iLower_freq \~English Index of lower bound frequency.
	 \~Japanese �������g���C���f�b�N�X
	 \param iUpper_freq \~English Index of upper bound frequency.
	 \~Japanese ������g���C���f�b�N�X
	 \param filename \~English mic position filename.
	 \~Japanese �}�C�N�ʒu���W�t�@�C��
       \param fspeed_sound \~English speed of sound
	 \~Japanese ����
	 \param filename_tf \~English Transfer function data file name.
       \~Japanese �`�B�֐��f�[�^�t�@�C����
	 \param bFlgFixedNoise \~English The flag for fixed noise source separation.(true is separate ,false is no separate)
       \~Japanese �Œ�G�����̕����t���O(true ���� ,false �����Ȃ�)
	 \param filename_FixedNoise \~English Fixed noise source position file name.
       \~Japanese �Œ�G�����ʒu���W�t�@�C����
	 \param filename_sm \~English Initial separate matrix file name.
	 \~Japanese �����s�񏉊��l�f�[�^�t�@�C����     
    */
	int Initialize(const char *filename,float fspeed_sound,int iFlgTf,const char *filename_tf ,
		bool bFlgFixedNoise,const char *filename_FixedNoise, int iFlgSm,const char *filename_sm) ;
	
	/**
	 \brief \~Japanese �t���[���p�����[�^�̐ݒ�
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese �t���[���p�����[�^
	 \param iFs \~English Sampling rate[Hz]
	 \~Japanese �T���v�����O���g��[Hz]
	 \param iLength \~English The length of a frame
	 \~Japanese �t���[����
	 \param iOverlap \~English Size of overlap sample
	 \~Japanese �I�[�o�[���b�v�T���v����
	 \param enumWin \~English The form of time window(0 is Hanning window ,1 is CONJ window)
     �@\~Japanese ���ԑ� (0 Hanning�� ,1 CONJ��)
	*/
	int SetPrmFrame(MICARY_PrmFrame *psPrmFrame,int iFs,int iLength,int iOverlap,int enumWin) ;
	
	/**
	 \brief \~Japanese �`�B�֐��̏�����
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter pointer
	 \~Japanese �t���[���p�����[�^�̃|�C���^
	*/
	int InitTFMatrix(MICARY_PrmFrame *psPrmFrame) ;

	// 2012/01/13 added
	/**
	 \brief \~Japanese TF�f�[�^�x�[�X�A�����s�񏉊��l�f�[�^�x�[�X�̏����l�ɒl���X�V
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter pointer
	 \~Japanese �t���[���p�����[�^�̃|�C���^
	 \param iSrcIndex \~English Index of the source to load
	 \~Japanese �ǂ݂��������ԍ�
	*/
	int LoadInitialTFMatrix(MICARY_PrmFrame *psPrmFrame, int iSrcIndex);

	/**
	 \breif \~Japanese �����ʒu�f�[�^���N���X�����ۑ��l�ɑ������
 	 \return \~English error code (0 is error ,1 is success)	
	 \param locaSrcPos \~English Source position array 
	 \~Japanese �����ʒu���W�f�[�^
	*/
	int AssignSrcPos(MICARY_LocationArray locaSrcPos) ;

	/**
	 \brief \~Japanese �Œ�G�����ʒu���W�̍쐬
 	 \return \~English error code (0 is error ,1 is success)	
	 \param locaSrc \~English source position array with size of includ fixed noise source position
	 \~Japanese �Œ�G�������܂߂��T�C�Y���������ʒu���W�f�[�^
	 */
	int MakeFixedNoise_Pos(MICARY_LocationArray &locaSrc);

	/**
	 \brief \~Japanese �Œ�G�����̌��o�`�F�b�N
	 \return \~English error code (true is include ,false is no include)	
	 \param locaSrc \~English Source position array 
	 \~Japanese �����ʒu���W�f�[�^
	*/
	bool CheckFixedNoise_noise(MICARY_LocationArray locaSrc);

	/**
	 \brief \~Japanese �����s��̏�����
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese �t���[���p�����[�^
	*/
	int InitSepMatrix(MICARY_PrmFrame *psPrmFrame) ;

	// 2012/01/13 added
	/**
	 \brief \~Japanese �����C���f�b�N�X�ŕ����s�񏉊��l��ǂݍ���
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese �t���[���p�����[�^
	 \param iSrcIndex \~English Index of the source to load
	 \~Japanese �ǂݍ��މ���ID�C���f�b�N�X
	*/
	int LoadInitialSepMatrix(MICARY_PrmFrame *psPrmFrame, int iSrcIndex);

	/**
	 \breif \~Japanese �ŋߖT�ʒu�̃f�[�^�C���f�b�N�X���擾
 	 \return \~English Output index 	
	 \param micary_loCur \~English Current source position
	 \~Japanese ���݂̉������W
	 \param micary_laDB \~English Database source position array
	 \~Japanese �����������ʒu���W�f�[�^�x�[�X

	*/
	HA_Integer GetNearestPos(MICARY_Location micary_loCur, MICARY_LocationArray *micary_laDB) ;
	
	/**
	 \breif \~Japanese �f�[�^�x�[�X��S�������čŋߖT�ʒu�̃f�[�^�C���f�b�N�X���擾
 	 \return \~English Output index 	
	 \param micary_loCur \~English Current source position
	 \~Japanese ���݂̉������W
	 \param micary_laDB \~English Database source position array
	 \~Japanese �����������ʒu���W�f�[�^�x�[�X

	*/
	HA_Integer GetNearestPos_ALLSearch(MICARY_Location micary_loCur, MICARY_LocationArray *micary_laDB) ;

	/**
	 \brief \~Japanese �`�B�֐��t�@�C���t�H�[�}�b�g�p�����[�^�̐ݒ�
	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese �t�@�C���t�H�[�}�b�g�^�C�v(__FILE_FORMAT_MICARY �܂��� __FILE_FORMAT_HARK)
	*/	
	void SetTF_CONJ_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese �����s�񏉊��l�t�@�C���t�H�[�}�b�g�p�����[�^�̐ݒ�
	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese �t�@�C���t�H�[�}�b�g�^�C�v(__FILE_FORMAT_MICARY �܂��� __FILE_FORMAT_HARK)
	*/	
	void SetInitW_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese �����s��o�̓t�@�C���t�H�[�}�b�g�p�����[�^�̐ݒ�
	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese �t�@�C���t�H�[�}�b�g�^�C�v(__FILE_FORMAT_MICARY �܂��� __FILE_FORMAT_HARK)
	*/	
	void SetExportW_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese �}�C�N�ʒu���W�t�@�C���t�H�[�}�b�g�p�����[�^�̐ݒ�
 	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese �t�@�C���t�H�[�}�b�g�^�C�v(__FILE_FORMAT_MICARY �܂��� __FILE_FORMAT_HARK)
	*/	
	void SetMicPos_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese �Œ�G�����t�@�C���t�H�[�}�b�g�p�����[�^�̐ݒ�
	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese �t�@�C���t�H�[�}�b�g�^�C�v(__FILE_FORMAT_MICARY �܂��� __FILE_FORMAT_HARK)
	*/	
	void SetFixedNoisePos_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese �X�V���ꂽ�S�Ă̕����s����t�@�C���ɏo��
	 \return \~English error code (true is success ,false is error)
	 \~Japanese �G���[�R�[�h(true ���� ,false�@�G���[)
	 \param cFilename \~English Export file name
	 \~Japanese �o�̓t�@�C����
	*/	
	bool ExportAllUpdateW(MICARY_PrmFrame *psPrmFrame,const char *cFilename);

	/**
	 \brief \~Japanese �X�V���ꂽ�����s����t�@�C���ɒǉ��o��
	 \return \~English error code (true is success ,false is error)
	 \~Japanese �G���[�R�[�h(true ���� ,false �G���[)
	 \param cFilename \~English Export file name
	 \~Japanese �o�̓t�@�C����
	*/	
	bool AddUpdateW(MICARY_PrmFrame *psPrmFrame,const char *cFilename,bool bExpW);

 	/**
	 \brief \~Japanese �X�V�p�����s��ۑ��l�̍X�V
	*/
	void SetUpdateW();

	/**
	 \brief \~Japanese �X�V���ꂽ�����s����t�@�C���ɏo��
	�@\return \~English error code (true is success ,false is error)
	 \~Japanese �G���[�R�[�h(true / false)
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese �t���[���p�����[�^
	 \param cFilename \~English Export file name
	 \~Japanese �o�̓t�@�C����
	*/
	bool ExportUpdateW(MICARY_PrmFrame *psPrmFrame,const char *cFilename);
		
	// 2012/01/13 added
	/**
	 \brief \~Japanese TF_CONJ��W�̈����p��
	 \return \~English check code(0 is no change ,1 is change)
	 \param plaSrcPosCur \~English The durrent source position array
	 \~Japanese ���݂̉����ʒu���W�f�[�^
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese �t���[���p�����[�^
	*/
	// TF_CONJ��W�̈����p��
	int ChangeSrcPos_TF_CONJ_W(MICARY_LocationArray plaSrcPosCur, MICARY_PrmFrame psPrmFrame);

	/**
	 \brief \~Japanese POS�ɂ�镪���s��̕ύX�m�F
	 \return \~English check code(0 is no change ,1 is change)
	 \param plaSrcPosCur \~English The durrent source position array
	 \~Japanese ���݂̉����ʒu���W�f�[�^
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese �t���[���p�����[�^
	*/
	int ChangeSrcPos_W(MICARY_LocationArray plaSrcPosCur,MICARY_PrmFrame psPrmFrame);
	
	/**
	 \brief \~Japanese POS�ɂ��`�B�֐��̕ύX�m�F
	 \return \~English check code (0 is no change ,1 is change)
	 \param plaSrcPosCur \~English Source location array for current frame
	 \~Japanese ���t���[���̉������W�z��
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese �t���[���p�����[�^
	*/
	int ChangeSrcPos_TF_CONJ(MICARY_LocationArray plaSrcPosCur,MICARY_PrmFrame psPrmFrame);
	
	/**
	 \brief \~Japanese �X�V�p�����s��ۑ��l�̏�����
	 \return \~English error code (0 is error ,1 is success)
	*/
	int InitUpdateW();

	/**
	 \brief \~Japanese �����s��o�̓t�@�C���̏�����
	 \return \~English error code (0 is error ,1 is success)	
	 \param filename \~English Export separate matrix filename 
	 \~Japanese �����s��o�̓t�@�C����
	*/
	int InitExportW(const char *cFilename,MICARY_PrmFrame *psPrmFrame);


	//Changed by Morio 2013/01/20
	/**
	 \brief \~Japanese �X�V�p�p�����[�^�̐ݒ�
	 \return \~English error code (true is success ,false is error)
	 \param iUpdateMTF_CONJ \~English Update method for transfer function(ID os POS).
	 \~Japanese �`�B�֐��̍X�V���@(ID �܂��� POS)
	 \param iUpdateMW \~English Update method for separate matrix(ID or POS ,ID_POS).
	 \~Japanese �����s��X�V���@(ID �܂��́@POS �CID_POS)
	 \param iSrc_Compare_mode \~English Compare method for source position(DEG or TFIndex).
	 \~Japanese �����ʒu��r���@(DEG �܂��� TFIndex)
	 \param fThred_angle \~English The threshold angle[deg.] for check source position.
	 \~Japanese �����ʒu�m�F�̂��߂̋��e�p�x[deg.]
	 \param fThred_angle \~English The search azimuth angle[deg.] for check source position.
	 \~Japanese �����ʒu�m�F�̂��߂̌������ʊp[deg.]
	 \param fThred_angle \~English The search elevation angle[deg.] for check source position.
	 \~Japanese �����ʒu�m�F�̂��߂̌����p[deg.]
	 \param bCheck_distance \~English Check source distance when using TFINDEX.
	 \~Japanese TFINDEX�g�p����Source�̋��������ꂷ���Ă��Ȃ����m�F����C臒l��fThred_angle
	*/
	bool SetPrmUpdate(int iUpdateMTF_CONJ,int iUpdateMW,int iSrc_Compare_mode, float fThred_angle, int iUpdate_tfindex_distance, float fSearch_azimuth,float fSearch_elevation, bool bCheck_distance);

	/**
	 \brief \~Japanese �}�C�N���W�̃V�t�g�p�����[�^�ݒ�
	 \param ivalue \~English Shift parameter for microphone position
	 \~Japanese shift�̎��s�t���O
	*/
	void SetiMicPosShift(int ivalue);
	
	/**
	 \brief \~Japanese ������
	  \return \~English error code (true is success ,false is error)
	 \param iNch \~English Size of microphone channels.
	 \~Japanese �}�C�N�`�����l����
	 \param iNpos \~English Size of sources.
	 \~Japanese ������
	 \param iLower_f \~English Index of lower bound frequency.
	 \~Japanese �������g���C���f�b�N�X
	 \param iUpper_f \~English Index of upper bound frequency.
	 \~Japanese ������g���C���f�b�N�X
	*/
	bool Initialize_Change(int iNch,int iNpos,int iLower_f,int iUpper_f);

private:
	// HarkFileFormat Class
	CHarkFileIO cHark ;
	
	/**
	 \brief \~Japanese �����̏o���E���݁E���ł��m�F
	 \return \~English Check current source if it creats, exists or remove. 
	 \param lcCurLoc \~English Source location.
	 \~Japanese �����ʒu
	 \param iUpdateMethodW \~English Update method(ID / POS / ID_POS).
	 \~Japanese �X�V���@(ID / POS / ID_POS).
	**/	
	int _CheckFrontSrcLoc(MICARY_Location lcCurLoc,int iUpdateMethodW);

	/**
	 \brief \~Japanese ���ł��������̊m�F
	 **/	
	void _SetRemoveSrc();

	/**
	 \brief \~Japanese �X�V�p�����s��ۑ��l�̂����C�X�V�����̂���L���T�C�Y���擾
	 \return \~English Valid source number
	 **/	
	int _GetNvalidUpdateW();

	/**
	 \brief \~Japanese �ʒu���W������ς��v�Z
	 \return \~English Result value
	 \param micary_Loc1 \~English location position1[x,y,z]
	 \~Japanese �ʒu���W1[x,y,z�n]
	 \param micary_Loc2 \~English location position2[x,y,z]
	 \~Japanese �ʒu���W2[x,y,z�n]
	**/	
	HA_Float _InnerProductLoc(MICARY_Location micary_Loc1,MICARY_Location micary_Loc2);

	/**
	 \brief \~Japanese ���ς̌v�Z
	 \return \~English Result value.
	 \param micary_faVec1 \~English input vector 1
	 \~Japanese �x�N�g��1
	 \param micary_faVec2 \~English input vector 2
	 \~Japanese �x�N�g��2
	*/
	HA_Float _InnerProductVec(MICARY_FloatArray micary_faVec1,MICARY_FloatArray micary_faVec2);
	
	// 2011/11/16 added by N.Tanaka
	/**
	 \brief \~Japanese ���ς̌v�Z
	 \return \~English Result value.
	 \param fVec1 \~English input vector 1
	 \~Japanese �x�N�g��1
	 \param fVec2 \~English input vector 2
	 \~Japanese �x�N�g��2
	*/
	HA_Float _InnerProductVec(float fVec1[3],float fVec2[3]);
	
	/**
	 \brief \~Japanese �p�x[angle]����Position[x,y,z]�ɕϊ�
	 \return \~English location position[x,y,z]
	 \param plocPos \~English location position[x,y,z,angle]
	 \~Japanese �ʒu���W[�����p�x���ݒ肳��Ă���]
�@ �@ **/	
	void _Ang2Pos(MICARY_Location *plocPos);
	
	/**
	 \brief \~Japanese 2�_��IFIndex���������߂�
	 \return \~English Calicurate TFIndex distance
	 \param laRefPos \~English Source location (MICARY_Location type)
	 \~Japanese �����x�N�g��
	 \param laObjPos \~English Source location (MICARY_Location type)
	 \~Japanese �����x�N�g��
	 **/	
	HA_Integer _tfindexDistance(MICARY_Location laRefPos,MICARY_Location laObjPos);

	/**
	 \brief \~Japanese 2�_�̈ʒu��Threshold_angle�Őݒ肳�ꂽ�͈͓��ł��邩�̔���
	 \return \~English check code(true is in range ,false is out of range)
	 \param laRefPos \~English reference(saved) position
	 \~Japanese �Q�ƈʒu
�@ �@  \param laObjPos \~English objective position
	 \~Japanese �ړI�ʒu
	*/	
	bool _IsThreshPos(MICARY_Location laRefPos,MICARY_Location laObjPos);
	
	/**
	 \brief \~Japanese �����s��̉����ʒu�ɂ��ύX�̊m�F�ƈ��p��
	 \return \~English check code(true is no change ,false is change)
	 \param iIdcur \~English The objective index of current source position array.    
	 \~Japanese �ΏۂƂȂ錻�݂̉����ʒu�f�[�^�̃C���f�b�N�X
	*/
	bool _CheckSrcPos_W(int iIdcur);
	
	/**
	 \brief \~Japanese �����s��̉���ID�ɂ��ύX�̊m�F�ƈ��p��
	 \return \~English check code(true is no change ,false is change)
	 \param iIdcur \~English The objective index of current source position array.  
	 \~Japanese �ΏۂƂȂ錻�݂̉����ʒu�f�[�^�̃C���f�b�N�X
	*/
	bool _CheckSrcID_W(int iIdcur);
	
	/**
	 \brief \~Japanese �`�B�֐��̉���ID�ɂ��ύX�̊m�F�ƈ��p��
	 \return \~English check code(true is no change ,false is change)
	 \param caTFDBInit \~English The previous TF_CONJ data. 
	 \~Japanese 1�t���[���O��TF_CONJ�f�[�^
	 \param laSrcPosInit \~English The previous source position array.
	 \~Japanese 1�t���[���O�̉����ʒu�f�[�^
	*/
	bool _CheckSrcID_TF_CONJ(MICARY_ComplexArray caTFDBInit,MICARY_LocationArray laSrcPosInit);
	
	/**
	 \brief \~Japanese (�����`)�`�B�֐������l�̌v�Z
	 \return \~English error code(0 is error ,1 is success)
	 \param iUpper_freq \~English The index of upper boundary frequency
	 \~Japanese ������g���C���f�b�N�X
	 \param psPrmFrame \~English Parameter struct pointer for current frame
	 \~Japanese ���t���[���̃p�����[�^�\���̃|�C���^
	*/
	int MakeTFMatrix(int iUpper_freq,MICARY_PrmFrame *psPrmFrame ) ;

	// 2012/01/13 added
	/**
	 \brief \~Japanese (�����`)�`�B�֐������l�̌v�Z
	 \return \~English error code(0 is error ,1 is success)
	 \param psPrmFrame \~English Parameter struct pointer for current frame
	 \~Japanese ���t���[���̃p�����[�^�\���̃|�C���^
	 \param idsrc \~English the index of the source id
	 \~Japanese TF�f�[�^���쐬���鉹��ID�C���f�b�N�X
	*/
	int MakeTFMatrix2(MICARY_PrmFrame *psPrmFrame, int idsrc) ;

	/**
	 \brief \~Japanese �`�B�֐��̃t�@�C���ǂݍ���
	 \return \~English error code(0 is error, 1 is success)
	 \param psPrmFrame \~English Parameter struct for current frame
	 \~Japanese ���t���[���̃p�����[�^�\����
	*/
	int ReadTFMatrix(MICARY_PrmFrame psPrmFrame) ;

	/**
	 \brief \~Japanese �}�C�N�ʒu���W�̃t�@�C���ǂݍ���
	 \return \~English error code(0 is error ,1 is success)
	*/
	int ReadMicPos() ;	

	/**
	 \brief \~Japanese �����s��̃t�@�C���ǂݍ���
	�@\return \~English error code(0 is error, 1 is success)
	 \param psPrmFrame \~English Parameter struct for current frame
	 \~Japanese ���t���[���̃p�����[�^�\����
	*/
	int ReadSepMatrix(MICARY_PrmFrame psPrmFrame);
	
	
	/**
	 \brief \~Japanese �ۑ��l�`�B�֐��f�[�^�x�[�X����ŋߖT�ʒu�����f�[�^�̓ǂݍ���
	 \return \~English The saved data index
	 \param OutTF \~English Read transfer function data
	 \~Japanese �ǂݍ��܂ꂽ�`�B�֐��f�[�^
	 \param TFDB \~English The saved transfer function data base
	 \~Japanese �ۑ�����Ă���`�B�֐��f�[�^�x�[�X
	*/
	int GetNearestTFMatrixFromDB(MICARY_TFDatabase *OutTF,MICARY_TFDatabase *TFDB);

	// 
	/**
	 \brief \~Japanese DB����f�[�^�̎擾(�ł��߂����W��ǂ�)
	 \return \~English The saved data index
	 \param OutTF \~English Read transfer function data
	 \~Japanese �ǂݍ��܂ꂽ�`�B�֐��f�[�^
	 \param TFDB \~English The saved transfer function data base
	 \~Japanese �ۑ�����Ă���`�B�֐��f�[�^�x�[�X
	 \param idsrc \~English Index of the source to load
	 \~Japanese �ǂݍ��މ����C���f�b�N�X
	*/
	int GetNearestTFMatrixFromDB2(MICARY_TFDatabase *OutTF,MICARY_TFDatabase *TFDB, int idsrc);

	/**
	 \brief \~Japanese �ۑ��l�����s��f�[�^�x�[�X����ŋߖT�ʒu�����f�[�^�̓ǂݍ���
	 \return \~English The saved data index
	 \param OutSM \~English Read separate matrix
	 \~Japanese �ǂݍ��܂ꂽ�����s��
	 \param SMDB \~English The saved separate matrix data base
	 \~Japanese �ۑ�����Ă��镪���s��f�[�^�x�[�X
	*/
	int GetNearestSepMatrixFromDB(MICARY_SavedSeparation *OutSM,MICARY_SMDatabase *SMDB);

	/**
	 \brief \~Japanese �ۑ��l�����s��f�[�^�x�[�X����ŋߖT�ʒu�����f�[�^�̓ǂݍ���
	 \return \~English The saved data index
	 \param OutSM \~English Read separate matrix
	 \~Japanese �ǂݍ��܂ꂽ�����s��
	 \param SMDB \~English The saved separate matrix data base
	 \~Japanese �ۑ�����Ă��镪���s��f�[�^�x�[�X
	 \param idsrc \~English Index of the source to load
	 \~Japanese �ǂݍ��މ����C���f�b�N�X
	*/
	int GetNearestSepMatrixFromDB2(MICARY_SavedSeparation *OutSM, MICARY_SMDatabase *SMDB, int idsrc);

	/**
	\brief \~Japanese POS�ɂ��m�F���̋��e�͈� (DEG)
	\~English�@The threshold angle when it checks source change by POS. 
	*/
	HA_Float m_fThred_angle;	 
	
	/**
	\brief \~Japanese POS�ɂ��m�F���̋��e�͈� (TFIndex)
	\~English�@The threshold angle when it checks source change by POS. (TFIndex)
	*/
	HA_Integer m_fThred_TFIndex;

	/**
	\brief \~Japanese �f�[�^�x�[�X�������̌����͈͕��ʊp
	\~English�@The search azimuth angle when it checks source change. 
	*/
	HA_Float m_fSearch_azimuth;

	/**
	\brief \~Japanese �f�[�^�x�[�X�������̌����͈͋p
	\~English�@The search elevation angle when it checks source change. 
	*/
	HA_Float m_fSearch_elevation;

	/**
	\brief \~Japanese �X�V�p�����s��ۑ��l 
	\~English�@The saved separate matrix for update. 
	*/
	MICARY_UpdateSeparationMat *m_updateW;	

	/**
	\brief \~Japanese �`�B�֐��̍X�V���@ 
	\~English�@The update method for the (conjugated)transfer function. 
	*/
	int m_iUpdateMethodTF_CONJ;

	/**
	\brief \~Japanese �����s��̍X�V���@ 
	\~English�@The update method for the separate matrix. 
	*/
	int m_iUpdateMethodW;
	
	/**
	\brief \~Japanese Position�̔�r���@ 
	\~English The compare method for source position. 
	*/
	int m_iSrc_Compare_mode;
	
	/**
	\brief \~Japanese TFIndex�g�p����Source�����m�F�t���O
	\~English Check flag for confirm source distace when using TFIndex. 
	*/
	
	bool m_bCheck_distance;
	
	/**
	\brief \~Japanese 1�t���[���O�̉����ʒu�f�[�^ 
	\~English�@The previous source position array. 
	*/
	MICARY_LocationArray *m_laFrontSrcLoc;	

	/**
	\brief \~Japanese �����s��̎��ۂ̍X�V���@ 
	\~English�@The real update method for the separate matrix. 
	*/
	MICARY_IntArray *m_iUpdateMethodWex ;

	/**
	\brief \~Japanese �}�C�N�ʒu���W�t�@�C���� 
	\~English�@The microphone position file name. 
	*/
	char *m_file ;

	/**
	\brief \~Japanese (�����`)�`�B�֐��t�@�C���� 
	\~English�@The (conjugated) transfer function file name. 
	*/
	char *m_file_tf ;

	/**
	\brief \~Japanese ���� 
	\~English�@The speed of sound. 
	*/
	float m_fspeed_sound ;	

	/**
	\brief \~Japanese �`�B�֐����������@�t���O(0 ���_�l ,1 �t�@�C���ǂݍ���)
	\~English�@The initialize method flag for (conjugated) transfer function. (0 is calculate ,1 is read data file)
	*/
	int m_iFlgtf ;

	/**
	\brief \~Japanese �Œ�G�����̕����t���O(true ���� , false �����Ȃ�) 
	\~English�@The separate flag for fixed noise source.(true is separate ,false is no separate) 
	*/
	bool m_bFlgFixedNoise ; 

	/**
	\brief \~Japanese �Œ�G�����ʒu 
	\~English�@The fixed noise source position. 
	*/ 
	MICARY_LocationArray locaSrcFixedNoise ; 

	/**
	\brief \~Japanese �t�@�C������ǂݍ��܂ꂽ(�����`)�`�B�֐��f�[�^ 
	\~English�@The read (conjugated) transfer function from file. 
	*/
	MICARY_TFDatabase  *m_psTFDB ;

	/**
	\brief \~Japanese �t�@�C������ǂݍ��܂ꂽ�����s��f�[�^ 
	\~English�@The read separate matrix from file. 
	*/
	MICARY_SMDatabase  *m_psSMDB ;

	/**
	\brief \~Japanese �����s�񏉊������@�t���O(0 ���_�l ,1 �t�@�C���ǂݍ���) 
	\~English�@The initialize method for separate matrix.(0 is calculate ,1 is read data file) 
	*/
	int m_iFlgsm ;	

	/**
	\brief \~Japanese �����s��t�@�C���� 
	\~English�@The separate matrix file name. 
	*/
	char *m_file_sm ;	

	/**
	\brief \~Japanese �Œ�G�����ʒu���W�t�@�C���� 
	\~English�@The fixed noise source position file name. 
	*/
	char *m_file_FixedNoise ;

	/**
	\brief \~Japanese �}�C�N�ʒu 
	\~English�@The microphone position array. 
	*/
	MICARY_LocationArray locaMic ; 

	/**
	\brief \~Japanese �}�C�N�ʒu�̏d�S�V�t�g(0 �V�t�g ,1 �Œ�) 
	\~English�@The shift flag for the center of microphone position.(0 is shift ,1 is fix) 
	*/
	int iMicPosShift;

	/**
	\brief \~Japanese �`�B�֐��t�@�C���̃t�H�[�}�b�g�^�C�v 
	\~English�@The transfer function file format. 
	*/
	int m_iFormat_TF_CONJ ;

	/**
	\brief \~Japanese �����s�񏉊��l�t�@�C���̃t�H�[�}�b�g�^�C�v 
	\~English�@The initial separate matrix file format. 
	*/
	int m_iFormat_InitW;	

	/**
	\brief \~Japanese �����s��o�̓t�@�C���̃t�H�[�}�b�g�^�C�v 
	\~English�@The export separate matrix file format. 
	*/
	int m_iFormat_ExportW ;	
	
	/**
	\brief \~Japanese �}�C�N�ʒu���W�t�@�C���̃t�H�[�}�b�g�^�C�v 
	\~English�@The microphone position file format. 
	*/
	int m_iFormat_MicPos ;
	
	/**
	\brief \~Japanese �Œ�G�����ʒu���W�t�@�C���̃t�H�[�}�b�g�^�C�v 
	\~English�@The fixed noise source position file format. 
	*/
	int m_iFormat_FixedNoise ;  

	
	// 2012/01/19 added
	/**
	\brief \~Japanese �`�B�֐��f�[�^�e���|�����o�b�t�@
	\~English Tmporary buffer of TF database
	*/
	MICARY_ComplexArray m_tmp_cmpaTFDB ;
	
	/**
	\brief \~Japanese �����ʒu�ۑ��p�e���|�����o�b�t�@
	\~English Tmporary buffer of the source position
	*/
	MICARY_LocationArray m_plaSrcPosIni ;
	
		
};
// ---------------------------------------------------------------------
#endif // ENABLE_HARKIO_NH
#endif // Cexec_h_defined

