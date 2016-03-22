// HarkFileIO_NH.cpp: CHarkFileIO �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "HarkFileIO_NH.h"

#ifdef ENABLE_HARKIO_NH

#include <math.h>
#include <vector>
using namespace std ;

char HARKFILEIO_SIGNATURE_TFMAT[] = "transfer function";
char HARKFILEIO_SIGNATURE_SPMAT[] = "separation matrix";
char HARKFILEIO_COMPLEX_TYPESTR[] = "complex";

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CHarkFileIO::CHarkFileIO()
{
	m_pcTFfilename = NULL;
	m_ptf = NULL;
	m_iFileType = -1;
	
	m_pPrinbrs = NULL;
	m_fThresh = 0.0;
}

CHarkFileIO::~CHarkFileIO()
{
	CloseTFfile();
}

// --------------------------------------------------------------------
// tf�t�@�C���̃I�[�v��(cFilename�����ɊJ���Ă���ꍇ�̓X���[����)
bool CHarkFileIO::OpenTFfile(const char* cFilename)
{
	if (m_pcTFfilename){
		if (strcmp(m_pcTFfilename, cFilename) == 0){
			// ���łɊJ���Ă���̂ŉ������Ȃ�
			return true;
		}
	}
	
	ReopenTFfile(cFilename);

	return true;
}
// --------------------------------------------------------------------
// tf�t�@�C���̍ăI�[�v��
bool CHarkFileIO::ReopenTFfile(const char* cFilename)
{
	int i;
	
	if (m_pcTFfilename){
		CloseTFfile();
	}
	
	m_pcTFfilename = (char *)malloc(strlen(cFilename)+1);
	strcpy(m_pcTFfilename, cFilename);
	
	m_ptf = harkio_TransferFunction_fromFile(m_pcTFfilename, HARKFILEIO_SIGNATURE_TFMAT);
	m_iFileType = FORMAT_TYPE_TF;
	if (!m_ptf){
		m_ptf = harkio_TransferFunction_fromFile(m_pcTFfilename, HARKFILEIO_SIGNATURE_SPMAT);
		if (!m_ptf){
			free(m_pcTFfilename);
			m_pcTFfilename = NULL;
			cerr << "Failed to open " << m_pcTFfilename << "@CHarkFileIO::ReopenTFfile" << endl;
			return false;
		}
		m_iFileType = FORMAT_TYPE_W;
	}
	
	return true;
}
// --------------------------------------------------------------------
// tf�t�@�C���̃N���[�Y
bool CHarkFileIO::CloseTFfile()
{
	if (m_pcTFfilename){
		free(m_pcTFfilename);
	}
	if (m_pPrinbrs){
		harkio_Neighbors_delete(&m_pPrinbrs);
	}
	if (m_ptf){
		harkio_TransferFunction_delete(&m_ptf);
	}	

	// ������
	m_pcTFfilename = NULL;
	m_ptf = NULL;
	m_iFileType = -1;

	m_pPrinbrs = NULL;
	m_fThresh = 0.0;
}
// --------------------------------------------------------------------
//�t�@�C���ǂݍ���
bool CHarkFileIO::ReadHeader(const char* cFilename,
				MICARY_TFDBFileHeader *psTFDBHead) 
{
	try {
		int i;

		if (!OpenTFfile(cFilename)){
			return false;
		}
		
		harkio_TransferFunction* tf = m_ptf;
		
		psTFDBHead->lFs = tf->cfg->samplingRate;
		psTFDBHead->lNFFT = tf->cfg->nfft;
		psTFDBHead->lNSrc = tf->poses->size;
		psTFDBHead->lNFreq = tf->cfg->nfft/2+1;
		psTFDBHead->lNMic = tf->mics->size;
		psTFDBHead->iFileType = m_iFileType;
		
	}
	catch(std::exception &ex){
	
		return false ;
	
	}

	//	cout << "Success Write Hark file" << endl ;
	return true ;

}
// --------------------------------------------------------------------
//�w�b�_�Ƃ��̑����Ӄf�[�^�̓Ǎ���
bool CHarkFileIO::ReadInfo(const char* cFilename,
				MICARY_TFDBFileHeader *psTFDBHead,
				MICARY_LocationArray * plocaSrcPos , 
				MICARY_LocationArray * plocaMicPos ,
                MICARY_IntArray *piaFreqLine,
                bool bInfo)
{
	try {
		int i;
		float fcoord[3];
		float *pfc;

		if (!OpenTFfile(cFilename)){
			return false;
		}
		
		harkio_TransferFunction* tf = m_ptf;
		
		psTFDBHead->lFs = tf->cfg->samplingRate;
		psTFDBHead->lNFFT = tf->cfg->nfft;
		psTFDBHead->lNSrc = tf->poses->size;
		psTFDBHead->lNFreq = tf->cfg->nfft/2+1;
		psTFDBHead->lNMic = tf->mics->size;
		psTFDBHead->iFileType = m_iFileType;
		
	    // Mic���̊m�F
		if(plocaMicPos->ha_numLocation != psTFDBHead->lNMic){
			cout << "Mic number is mismatch !" << endl ;
			return false ;
		}
	    // Src���̊m�F
		if(plocaSrcPos->ha_numLocation != psTFDBHead->lNSrc){
			cout << "Source number is mismatch !" << endl ;
			return false ;
		}

		for(i = 0; i < tf->mics->size ; i++){
			if (tf->mics->pos[i]->sys != Cartesian){
				harkio_Position_Convert(tf->mics->pos[i], fcoord, Cartesian);
				pfc = fcoord;
			}
			else {
				pfc = tf->mics->pos[i]->coord;
			}
			plocaMicPos->micary_Location[i].ha_fX = pfc[0];
			plocaMicPos->micary_Location[i].ha_fY = pfc[1];
			plocaMicPos->micary_Location[i].ha_fZ = pfc[2];
			plocaMicPos->micary_Location[i].ha_idPos = tf->mics->pos[i]->id;
			plocaMicPos->micary_Location[i].tfposid = tf->mics->pos[i]->id;
			plocaMicPos->ha_idLocation[i] = tf->mics->pos[i]->id;
			// TEST
//			if (m_iFileType == FORMAT_TYPE_W){
//				cout<<"mic"<<i<<","<< pfc[0] <<","<< pfc[1] <<","<<pfc[2] <<endl;
//			}
		}
		
		for(i = 0; i < tf->poses->size ; i++){
			if (tf->poses->pos[i]->sys != Cartesian){
				harkio_Position_Convert(tf->poses->pos[i], fcoord, Cartesian);
				pfc = fcoord;
			}
			else {
				pfc = tf->poses->pos[i]->coord;
			}
			plocaSrcPos->micary_Location[i].ha_fX = pfc[0];
			plocaSrcPos->micary_Location[i].ha_fY = pfc[1];
			plocaSrcPos->micary_Location[i].ha_fZ = pfc[2];
			plocaSrcPos->micary_Location[i].ha_idPos = tf->poses->pos[i]->id;
			plocaSrcPos->micary_Location[i].tfposid = tf->poses->pos[i]->id;
			plocaSrcPos->ha_idLocation[i] = tf->poses->pos[i]->id;
			// TEST
//			if (m_iFileType == FORMAT_TYPE_W){
//				cout<<"src"<<i<<","<< pfc[0] <<","<< pfc[1] <<","<<pfc[2] <<endl;
//			}
		}
		
		for(i = 0 ; i < psTFDBHead->lNFreq ; i++){
			piaFreqLine->ha_iValue[i] = i;
		}
		
	}
	catch(std::exception &ex){
	
		return false ;
	
	}

	//	cout << "Success Write Hark file" << endl ;
	return true ;
}
// --------------------------------------------------------------------
//�f�[�^�̓Ǎ���
bool CHarkFileIO::ReadData(const char*cFilename,
			MICARY_ComplexArray *micary_cmpaData)
{
	try{
		COMPLEX_TYPE value;
		int index;
		int i, j, k;
		
		if (!OpenTFfile(cFilename)){
			return false;
		}
		
		harkio_TransferFunction* tf = m_ptf;
		
		if(micary_cmpaData->ha_numValue != (tf->cfg->nfft/2+1)*tf->poses->size*tf->mics->size){
			cout << "TF Data size is mismatch! Nbuf = " << micary_cmpaData->ha_numValue << " Nfile = " << (tf->cfg->nfft/2+1)*tf->poses->size*tf->mics->size << endl ;
			return false ;
		}

		if (m_iFileType == FORMAT_TYPE_TF){
			//FORMAT_TYPE_TF
			// �`�B�֐��f�[�^�̓ǂݍ���
			// micary_cmpaData->ha_cmpValue��(���g��(����(�}�C�N)))�̏��ɓ���
			for(k = 0 ; k < (tf->cfg->nfft/2+1) ; k++){
				for(j = 0 ; j < tf->poses->size ; j ++){
					index = tf->poses->size*tf->mics->size*k + tf->mics->size*j;
					for(i = 0 ; i < tf->mics->size ; i++){
						harkio_Matrix_getValueComplex(tf->septfs[j], i, k, &value);
						micary_cmpaData->ha_cmpValue[index].re = crealf(value);
						micary_cmpaData->ha_cmpValue[index].im = cimagf(value);
						//cout << "index = " << index << micary_cmpaData->ha_cmpValue[index].re<< " , " << micary_cmpaData->ha_cmpValue[index].im ;
						index++;
					}
				}
			}
		}
		else if (m_iFileType == FORMAT_TYPE_W){
			//FORMAT_TYPE_W
			// �����s��f�[�^�̓ǂݍ���
			// micary_cmpaData->ha_cmpValue��(���g��(�}�C�N(����)))�̏��ɓ���
			for(k = 0 ; k < (tf->cfg->nfft/2+1) ; k++){
				for(i = 0 ; i < tf->mics->size ; i++){
					index = tf->poses->size*tf->mics->size*k + tf->poses->size*i;
					for(j = 0 ; j < tf->poses->size ; j ++){
						harkio_Matrix_getValueComplex(tf->septfs[j], i, k, &value);
						micary_cmpaData->ha_cmpValue[index].re = crealf(value);
						micary_cmpaData->ha_cmpValue[index].im = cimagf(value);
						//cout << "index = " << index << micary_cmpaData->ha_cmpValue[index].re<< " , " << micary_cmpaData->ha_cmpValue[index].im ;
						index++;
					}
				}
			}
		}
		else {
			cout << "File type is unknown" << endl ;
			return false ;
		}
		
	}
	catch(std::exception &ex){
		return false ;
	}
	//cout << "Success Read Hark Data." << endl ;
	return true ;
}
// --------------------------------------------------------------------
// �e�L�X�g�t�@�C��(from PositionsXML)
bool CHarkFileIO::LoadPositionData(const char* cFilename,
				MICARY_LocationArray * plocaPos)
{
	try {
		int i;
		
		char *cName = (char*)malloc((strlen(cFilename)+1)*sizeof(char));
		strcpy(cName, cFilename);
		harkio_XML* xml = harkio_XML_newFromXML(cName, NULL);
		free(cName);
		if (!xml){
			cerr << "Failed to read " << cFilename << "@CHarkFileIO::LoadPositionData" << endl;
			return false;
		}
		
		float *pfc;
		pfc = (float *) malloc(sizeof(float) * 3);
		
		for(i = 0 ; i < xml->poses->size ; i++){
			harkio_Position_Convert(xml->poses->pos[i], pfc, Cartesian);
			plocaPos->micary_Location[i].ha_fX = pfc[0];
			plocaPos->micary_Location[i].ha_fY = pfc[1];
			plocaPos->micary_Location[i].ha_fZ = pfc[2];
			plocaPos->micary_Location[i].ha_idPos = xml->poses->pos[i]->id;
			plocaPos->micary_Location[i].tfposid = xml->poses->pos[i]->id;
			plocaPos->ha_idLocation[i] = i;
			//cout<<"poses"<<i<<","<<plocaPos->micary_Location[i].ha_fX <<","<<plocaPos->micary_Location[i].ha_fY <<","<<plocaPos->micary_Location[i].ha_fZ <<endl;
		}
		
		free(pfc);
		
		harkio_XML_delete(&xml);
	}
	catch(std::exception &ex){
	
		return false ;
	
	}

	//	cout << "Success Write Hark file" << endl ;
	return true ;
}
// --------------------------------------------------------------------
// �e�L�X�g�t�@�C��(from tfzip�t�@�C��)
bool CHarkFileIO::LoadPositionData(const char* cFilename,
				MICARY_LocationArray * plocaPos, int iFileType)
{
	try{
		int i;

		if (!OpenTFfile(cFilename)){
			return false;
		}
		
		harkio_TransferFunction* tf = m_ptf;
		
		if (iFileType == __FILE_TYPE_NOISE){
			return false;
		}
		else if (iFileType == __FILE_TYPE_MIC){
			float *pfc;
			pfc = (float *) malloc(sizeof(float) * 3);
			for(i = 0 ; i < tf->mics->size ; i++){
				harkio_Position_Convert(tf->mics->pos[i], pfc, Cartesian);
				plocaPos->micary_Location[i].ha_fX = pfc[0];
				plocaPos->micary_Location[i].ha_fY = pfc[1];
				plocaPos->micary_Location[i].ha_fZ = pfc[2];
				plocaPos->micary_Location[i].ha_idPos = tf->mics->pos[i]->id;
				plocaPos->micary_Location[i].tfposid = tf->mics->pos[i]->id;
				plocaPos->ha_idLocation[i] = i;
				//cout<<"mic"<<i<<","<<plocaMicPos->micary_Location[i].ha_fX <<","<<plocaMicPos->micary_Location[i].ha_fY <<","<<plocaMicPos->micary_Location[i].ha_fZ <<endl;
			}
			free(pfc);
		}
		
	}
	catch(std::exception &ex){
		return false ;
	}
	//cout << "Success Read Hark Data." << endl ;
	return true ;
}
// --------------------------------------------------------------------
// Neighbors���X�g�v�Z
bool CHarkFileIO::CalcNeighbors(const char* cFilename,
					float thresh)
{
	try{
		int i, j;
	    vector<pair<float, int> > vifclosestIDs; // ����, id
		
		if (!OpenTFfile(cFilename)){
			return false;
		}
		
		harkio_TransferFunction* tf = m_ptf;

		if (m_pPrinbrs){
			harkio_Neighbors_delete(&m_pPrinbrs);
		}

		m_pPrinbrs = harkio_Neighbors_new(tf->nbrs->poses);	// ��) m_pPrinbrs���g�p����Ԃ�TF�t�@�C������Ȃ�����
		
		// ������Neighbor�v�Z����
//		if (harkio_Neighbors_calcNeighbors(m_pPrinbrs, NearestNeighbor, nth, thresh) != EXIT_SUCCESS){
//			cerr << "Failed harkio_Neighbors_calcNeighbors@CHarkFileIO::CalcNeighbors" << endl;
//			if (m_pPrinbrs){
//				harkio_Neighbors_delete(&m_pPrinbrs);
//				m_pPrinbrs = NULL;
//			}
//			return false;
//		}
	    for(i=0; i<m_pPrinbrs->size; i++)
    	{
			// ������
			vifclosestIDs.clear();
			
			// threshould�ȓ�������
			for(j=0; j<m_pPrinbrs->size; j++)
    	    {
				float tmpdist = euclidDistance(m_pPrinbrs->poses->pos[i], m_pPrinbrs->poses->pos[j], 3);
				if (tmpdist < thresh){
					vifclosestIDs.push_back(std::pair<float, int>(tmpdist, m_pPrinbrs->poses->pos[j]->id));
				}
    	    }
		
			// sort
			sort(vifclosestIDs.begin(), vifclosestIDs.end());

			// Neighbor�\���̓��ɕۑ�
        	m_pPrinbrs->numNeighbors[i] = vifclosestIDs.size();
	        m_pPrinbrs->neighbors[i] = (int *) malloc(sizeof(int) * m_pPrinbrs->numNeighbors[i]);
			for (j=0; j<m_pPrinbrs->numNeighbors[i] ; j++){
				m_pPrinbrs->neighbors[i][j] = vifclosestIDs[j].second;
			}
			
			// TEST
//			for (j=0; j<m_pPrinbrs->numNeighbors[i] ; j++){
//				cout << m_pPrinbrs->neighbors[i][j] << "(" << vifclosestIDs[j].second << "," << vifclosestIDs[j].first << ")," ;
//			}
//			cout << endl;	
    	}
		
		m_pPrinbrs->alg = NearestNeighbor;	// harkio_Neighbors_delete��free���邽�߂Ɏw��
		m_fThresh = thresh;
	}
	catch(std::exception &ex){
		return false ;
	}
	//cout << "Success Read Hark Data." << endl ;
	return true ;
}
// --------------------------------------------------------------------
// ���݂�TF����id���w�肵��posindex���擾
int CHarkFileIO::GetPosIndex(int id)
{
	try{
		int i;
		if (!m_ptf){
			cerr << "Not found TF file! @CHarkFileIO::GetPosIndex" << endl;
			return -1;
		}
		for (i=0 ; i < m_ptf->poses->size ; i++){
			if (m_ptf->poses->pos[i]->id == id){
				return i;
			}
		}
	}
	catch(std::exception &ex){
		return -1 ;
	}
	//cout << "Success Read Hark Data." << endl ;
	return -1 ;
}
// --------------------------------------------------------------------
// ���[�N���b�h�����̎Z�o
float CHarkFileIO::_EuclidDistance(harkio_Position *pos, MICARY_Location locCur)
{
	float fcoord[3];
	float *pfcoord;
	
	if (pos->sys == Cartesian){
		pfcoord = pos->coord;
	}
	else {
		harkio_Position_Convert(pos, fcoord, Cartesian);
		pfcoord = fcoord;
	}
	
	float fdist = pow(pfcoord[0] - locCur.ha_fX, 2.0)
				+ pow(pfcoord[1] - locCur.ha_fY, 2.0)
				+ pow(pfcoord[2] - locCur.ha_fZ, 2.0);
	fdist = sqrt(fdist);
	return fdist;
}
// --------------------------------------------------------------------
// ���݂�TF������W���w�肵��id���擾
int CHarkFileIO::GetNearestPosID(MICARY_Location locCur)
{
	try{
		int i;
		int id;
		float fmin, ftmp;
		
		if (!m_ptf){
			cerr << "Not found TF file! @CHarkFileIO::GetPosIndex" << endl;
			return -1;
		}
		if (m_ptf->poses->size < 1){
			return -1;
		}
		
		fmin = _EuclidDistance(m_ptf->poses->pos[0], locCur);
		id = m_ptf->poses->pos[0]->id;
		
		for (i=0 ; i < m_ptf->poses->size ; i++){
			ftmp = _EuclidDistance(m_ptf->poses->pos[i], locCur);
			if (ftmp < fmin){
				fmin = ftmp;
				id = m_ptf->poses->pos[i]->id;
			}
		}
		
		return id;
	}
	catch(std::exception &ex){
		return -1 ;
	}
	//cout << "Success Read Hark Data." << endl ;
	return -1 ;
}
// --------------------------------------------------------------------
// �����s��f�[�^�̏����o��
bool CHarkFileIO::WriteData(const char* cFilename,
						MICARY_TFDBFileHeader *psTFDBHead,
						MICARY_ComplexArray micary_cmpaData,
						MICARY_IntArray	micary_iaFreqLine,
						MICARY_LocationArray micary_locaSource,
						MICARY_LocationArray micary_locaMic,
						const char* cDate,
						const char* cLocation,
						const char* cRobot,
						const char* cArrayType,
						const char * cCreator,
						const char* cMemo)
{	
	
	try{
		// W�Ƃ��ĕۑ�����
		int i, j, k, index;
		int nMic,nSrc,nFrq,nFFT,nFs ;
		float  coord[3];
		COMPLEX_TYPE cvalue;
		
		// �J���Ă���ꍇ�͈�x�N���[�Y����
		this->CloseTFfile();
		
		nMic = psTFDBHead->lNMic ;
		nSrc = psTFDBHead->lNSrc ;
		nFFT = psTFDBHead->lNFFT ;
		nFs = psTFDBHead->lFs ;
		nFrq = psTFDBHead->lNFreq ;
		
//		cout << "CHarkFileIO::WriteData nMic" << nMic << ", nSrc" << nSrc << ", nFFT" << nFFT << ", nFrq" << nFrq << endl;	// TEST

		harkio_TransferFunction *tf = harkio_TransferFunction_new();
		if (!tf){
			cerr << "Failed to create harkio_Transferfunction @CHarkFileIO::WriteData" << endl;
			return false;
		}
		
		// config
		tf->cfg->samplingRate = nFs;
		tf->cfg->nfft = psTFDBHead->lNFFT;
		// �ȉ��_�~�[�œ���
//		tf->cfg->synchronousAverage = 1;

		harkio_Position *pos;

		// �����ʒu�̕ۑ�
//		if (tf->poses->pos){
//			free(tf->poses->pos);
//			tf->poses->pos = NULL;
//		}
	        tf->poses->type = (char*) malloc(sizeof(char) * (strlen("soundsource") + 1));
		strcpy(tf->poses->type, "soundsource");
//		tf->poses->frame = 1;
//		tf->poses->size = nSrc;
//		tf->poses->pos = (harkio_Position**)malloc(sizeof(harkio_Position*)*nSrc);
		for (k = 0 ; k < nSrc ; k++){
			coord[0] = micary_locaSource.micary_Location[k].ha_fX;
			coord[1] = micary_locaSource.micary_Location[k].ha_fY;
			coord[2] = micary_locaSource.micary_Location[k].ha_fZ;
//			tf->poses->pos[k] = harkio_Position_new(micary_locaSource.micary_Location[k].tfposid, Cartesian, coord, NULL);
			pos = harkio_Position_new(micary_locaSource.micary_Location[k].tfposid, Cartesian, coord, NULL);
			harkio_Positions_append(tf->poses, pos);
//			cout << "SrcPos" << k << ": " << coord[0] << ", " << coord[1] << ", "<< coord[2] << endl;	// TEST
		}

		// Neighbors�̐ݒ�
		// �ݒ�̕K�v����
		// if (tf->nbrs){
		// 	harkio_Neighbors_delete(&tf->nbrs);
		// 	tf->nbrs = NULL;
		// }
		if (tf->nbrs){
			harkio_Neighbors_delete(&tf->nbrs);
			tf->nbrs = NULL;
		}
		tf->nbrs = harkio_Neighbors_new(tf->poses);
		harkio_Neighbors_calcNeighbors(tf->nbrs, NearestNeighbor, 1, 300.0);
		
		
		// �}�C�N�ʒu�̕ۑ�
//		if (tf->mics->pos){
//			free(tf->mics->pos);
//			tf->mics->pos = NULL;
//		}
//		tf->mics->frame = 1;
	        tf->mics->type = (char*) malloc(sizeof(char) * (strlen("microphone") + 1));
		strcpy(tf->mics->type, "microphone");
//		tf->mics->size = nMic;
//		tf->mics->pos = (harkio_Position**)malloc(sizeof(harkio_Position*)*nMic);
		for (k = 0 ; k < nMic ; k++){
			coord[0] = micary_locaMic.micary_Location[k].ha_fX;
			coord[1] = micary_locaMic.micary_Location[k].ha_fY;
			coord[2] = micary_locaMic.micary_Location[k].ha_fZ;
//			tf->mics->pos[k] = harkio_Position_new(micary_locaMic.micary_Location[k].tfposid, Cartesian, coord, NULL);
			pos = harkio_Position_new(micary_locaMic.micary_Location[k].tfposid, Cartesian, coord, NULL);
			harkio_Positions_append(tf->mics, pos);
//			cout << "MicPos" << k << ": " << coord[0] << ", " << coord[1] << ", "<< coord[2] << endl;	// TEST
		}
		
		// septfs�ɕۑ�
		tf->loctfs = NULL;	// �_�~�[�œ��͂���K�v����H
//		tf->loctfs = (harkio_Matrix **) malloc(sizeof(harkio_Matrix *) * tf->poses->size);	// for Dummy
		tf->septfs = (harkio_Matrix **) malloc(sizeof(harkio_Matrix *) * tf->poses->size);
		for (j = 0 ; j < tf->poses->size ; j++){
//			tf->loctfs[j] = harkio_Matrix_new(HARKFILEIO_COMPLEX_TYPESTR, 1, 1);	// for Dummy
			tf->septfs[j] = harkio_Matrix_new(HARKFILEIO_COMPLEX_TYPESTR, tf->mics->size, nFFT/2+1);
		}
		// micary_cmpaData->ha_cmpValue��(���g��(�}�C�N(����)))�̏��ɓ���
		int isrcindex, imicindex;
		for (k=0 ; k < micary_iaFreqLine.ha_numValue ; k++){
			for (i = 0, imicindex=0 ; i < tf->mics->size ; i++){
				index = nMic*nSrc*k + nSrc*i;
				for (j = 0, isrcindex=0 ; j < tf->poses->size ; j++){
					cvalue = micary_cmpaData.ha_cmpValue[index].re + micary_cmpaData.ha_cmpValue[index].im*_Complex_I;
					harkio_Matrix_setValueComplex(tf->septfs[j], i, micary_iaFreqLine.ha_iValue[k], cvalue);
					index++;
				}
			}
		}
		
		char *cName = (char*)malloc((strlen(cFilename)+1)*sizeof(char));
		strcpy(cName, cFilename);
		harkio_TransferFunction_write(tf, cName, HARKFILEIO_SIGNATURE_SPMAT);
		free(cName);
		
		harkio_TransferFunction_delete(&tf);
	}
	catch(std::exception &ex){
	
		return false ;
	
	}

	//	cout << "Success Write Hark file" << endl ;
	return true ;
	

}

#endif // ENABLE_HARKIO_NH
