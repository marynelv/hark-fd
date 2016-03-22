// HarkFileIO_NH.cpp: CHarkFileIO クラスのインプリメンテーション
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
// 構築/消滅
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
// tfファイルのオープン(cFilenameが既に開いている場合はスルーする)
bool CHarkFileIO::OpenTFfile(const char* cFilename)
{
	if (m_pcTFfilename){
		if (strcmp(m_pcTFfilename, cFilename) == 0){
			// すでに開いているので何もしない
			return true;
		}
	}
	
	ReopenTFfile(cFilename);

	return true;
}
// --------------------------------------------------------------------
// tfファイルの再オープン
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
// tfファイルのクローズ
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

	// 初期化
	m_pcTFfilename = NULL;
	m_ptf = NULL;
	m_iFileType = -1;

	m_pPrinbrs = NULL;
	m_fThresh = 0.0;
}
// --------------------------------------------------------------------
//ファイル読み込み
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
//ヘッダとその他周辺データの読込み
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
		
	    // Mic数の確認
		if(plocaMicPos->ha_numLocation != psTFDBHead->lNMic){
			cout << "Mic number is mismatch !" << endl ;
			return false ;
		}
	    // Src数の確認
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
//データの読込み
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
			// 伝達関数データの読み込み
			// micary_cmpaData->ha_cmpValueは(周波数(音源(マイク)))の順に入力
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
			// 分離行列データの読み込み
			// micary_cmpaData->ha_cmpValueは(周波数(マイク(音源)))の順に入力
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
// テキストファイル(from PositionsXML)
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
// テキストファイル(from tfzipファイル)
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
// Neighborsリスト計算
bool CHarkFileIO::CalcNeighbors(const char* cFilename,
					float thresh)
{
	try{
		int i, j;
	    vector<pair<float, int> > vifclosestIDs; // 距離, id
		
		if (!OpenTFfile(cFilename)){
			return false;
		}
		
		harkio_TransferFunction* tf = m_ptf;

		if (m_pPrinbrs){
			harkio_Neighbors_delete(&m_pPrinbrs);
		}

		m_pPrinbrs = harkio_Neighbors_new(tf->nbrs->poses);	// 注) m_pPrinbrsを使用する間はTFファイルを閉じないこと
		
		// 内部でNeighbor計算する
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
			// 初期化
			vifclosestIDs.clear();
			
			// threshould以内を検索
			for(j=0; j<m_pPrinbrs->size; j++)
    	    {
				float tmpdist = euclidDistance(m_pPrinbrs->poses->pos[i], m_pPrinbrs->poses->pos[j], 3);
				if (tmpdist < thresh){
					vifclosestIDs.push_back(std::pair<float, int>(tmpdist, m_pPrinbrs->poses->pos[j]->id));
				}
    	    }
		
			// sort
			sort(vifclosestIDs.begin(), vifclosestIDs.end());

			// Neighbor構造体内に保存
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
		
		m_pPrinbrs->alg = NearestNeighbor;	// harkio_Neighbors_deleteでfreeするために指定
		m_fThresh = thresh;
	}
	catch(std::exception &ex){
		return false ;
	}
	//cout << "Success Read Hark Data." << endl ;
	return true ;
}
// --------------------------------------------------------------------
// 現在のTFからidを指定してposindexを取得
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
// ユークリッド距離の算出
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
// 現在のTFから座標を指定してidを取得
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
// 分離行列データの書き出し
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
		// Wとして保存する
		int i, j, k, index;
		int nMic,nSrc,nFrq,nFFT,nFs ;
		float  coord[3];
		COMPLEX_TYPE cvalue;
		
		// 開いている場合は一度クローズする
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
		// 以下ダミーで入力
//		tf->cfg->synchronousAverage = 1;

		harkio_Position *pos;

		// 音源位置の保存
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

		// Neighborsの設定
		// 設定の必要あり
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
		
		
		// マイク位置の保存
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
		
		// septfsに保存
		tf->loctfs = NULL;	// ダミーで入力する必要あり？
//		tf->loctfs = (harkio_Matrix **) malloc(sizeof(harkio_Matrix *) * tf->poses->size);	// for Dummy
		tf->septfs = (harkio_Matrix **) malloc(sizeof(harkio_Matrix *) * tf->poses->size);
		for (j = 0 ; j < tf->poses->size ; j++){
//			tf->loctfs[j] = harkio_Matrix_new(HARKFILEIO_COMPLEX_TYPESTR, 1, 1);	// for Dummy
			tf->septfs[j] = harkio_Matrix_new(HARKFILEIO_COMPLEX_TYPESTR, tf->mics->size, nFFT/2+1);
		}
		// micary_cmpaData->ha_cmpValueは(周波数(マイク(音源)))の順に入力
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
