// ---------------------------------------------------------------------
/*
 * Copyright 2010 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#ifndef	CHarkFileIO_NH_h_defined
#define	CHarkFileIO_NH_h_defined
// ---------------------------------------------------------------------
#include "../config.h"

#ifdef ENABLE_HARKIO_NH

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

#ifdef ENABLE_HARKIO3
extern "C"{
	#include "libharkio3.h"
}
#endif	

#include "MICARY_struct.h"
#include "MICARY_GHDSS_struct.h"

#define	FORMAT_TYPE_TF	2
#define FORMAT_TYPE_W	3

#define __FILE_FORMAT_MICARY	(0)
#define __FILE_FORMAT_HARK	(1)

#define __FILE_TYPE_MIC		(0)
#define __FILE_TYPE_NOISE	(1)

class CHarkFileIO 
{
public:
	CHarkFileIO();
	~CHarkFileIO();

	// tf�t�@�C���̃I�[�v��(cFilename�����ɊJ���Ă���ꍇ�̓X���[����)
	bool OpenTFfile(const char* cFilename);

	// tf�t�@�C���̍ăI�[�v��
	bool ReopenTFfile(const char* cFilename);
	
	// tf�t�@�C���̃N���[�Y
	bool CloseTFfile();

	//�t�@�C���ǂݍ���
	bool ReadHeader(const char* cFilename,
					MICARY_TFDBFileHeader *psTFDBHead) ;

	//�w�b�_�Ƃ��̑����Ӄf�[�^�̓Ǎ���
	bool ReadInfo(const char* cFilename,
					MICARY_TFDBFileHeader *psTFDBHead,
					MICARY_LocationArray * plocaSrcPos , 
					MICARY_LocationArray * plocaMicPos ,
		                        MICARY_IntArray *piaFreqLine,
		                        bool bInfo = true  );

	//�f�[�^�̓Ǎ���
	bool ReadData(const char*cFilename,
					MICARY_ComplexArray *micary_cmpaData)  ;

	// �e�L�X�g�t�@�C��(xml)
	bool LoadPositionData(const char* cFilename,
					MICARY_LocationArray * plocaPos) ;

	// �e�L�X�g�t�@�C��
	bool LoadPositionData(const char* cFilename,
					MICARY_LocationArray * plocaPos,int iFileType) ;
				
	// Neighbors���X�g�v�Z
	bool CalcNeighbors(const char* cFilename,
						float thresh);

	// ���݂�TF����id���w�肵��posindex���擾
	int GetPosIndex(int id);
	
	// ���݂�TF������W���w�肵�čŋߐڈʒu��id���擾
	int GetNearestPosID(MICARY_Location locCur);
					
	// �t�@�C���쐬
	bool WriteData(const char* cFilename,
						MICARY_TFDBFileHeader *psTFDBHead,
						MICARY_ComplexArray micary_cmpaData,
						MICARY_IntArray	micary_iaFreqLine,
						MICARY_LocationArray micary_locaSource,
						MICARY_LocationArray micary_locaMic,
						const char* cDate = "",
						const char* cLocation = "",
						const char* cRobot = "",
						const char* cArrayType = "",
						const char * cCreator = "",
						const char* cMemo = "") ;
					
	// �����o�[
	char*	m_pcTFfilename;	///< TF�t�@�C����
	harkio_TransferFunction	*m_ptf;	///< TF�t�@�C���|�C���^
	int	m_iFileType;	///< �t�@�C���^�C�v(FORMAT_TYPE_TF or FORMAT_TYPE_W)
	
	// �N���X���擾Neighbor�Ǝ擾����
	// ��) m_pPrinbrs���g�p����Ԃ�TF�t�@�C������Ȃ����ƁBposes�z���TF�Ƌ��L���Ă���
	harkio_Neighbors *m_pPrinbrs;	///< �N���X���ōČv�Z����Neighbors
	float	m_fThresh;	///< �X���b�V�����h(���[�N���b�h����[m])
	
private:
	
	float _EuclidDistance(harkio_Position *pos, MICARY_Location locCur);
};

#endif // ENABLE_HARKIO_NH
#endif // CHarkFileIO_NH_h_defined
