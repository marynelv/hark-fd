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

	// tfファイルのオープン(cFilenameが既に開いている場合はスルーする)
	bool OpenTFfile(const char* cFilename);

	// tfファイルの再オープン
	bool ReopenTFfile(const char* cFilename);
	
	// tfファイルのクローズ
	bool CloseTFfile();

	//ファイル読み込み
	bool ReadHeader(const char* cFilename,
					MICARY_TFDBFileHeader *psTFDBHead) ;

	//ヘッダとその他周辺データの読込み
	bool ReadInfo(const char* cFilename,
					MICARY_TFDBFileHeader *psTFDBHead,
					MICARY_LocationArray * plocaSrcPos , 
					MICARY_LocationArray * plocaMicPos ,
		                        MICARY_IntArray *piaFreqLine,
		                        bool bInfo = true  );

	//データの読込み
	bool ReadData(const char*cFilename,
					MICARY_ComplexArray *micary_cmpaData)  ;

	// テキストファイル(xml)
	bool LoadPositionData(const char* cFilename,
					MICARY_LocationArray * plocaPos) ;

	// テキストファイル
	bool LoadPositionData(const char* cFilename,
					MICARY_LocationArray * plocaPos,int iFileType) ;
				
	// Neighborsリスト計算
	bool CalcNeighbors(const char* cFilename,
						float thresh);

	// 現在のTFからidを指定してposindexを取得
	int GetPosIndex(int id);
	
	// 現在のTFから座標を指定して最近接位置のidを取得
	int GetNearestPosID(MICARY_Location locCur);
					
	// ファイル作成
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
					
	// メンバー
	char*	m_pcTFfilename;	///< TFファイル名
	harkio_TransferFunction	*m_ptf;	///< TFファイルポインタ
	int	m_iFileType;	///< ファイルタイプ(FORMAT_TYPE_TF or FORMAT_TYPE_W)
	
	// クラス内取得Neighborと取得条件
	// 注) m_pPrinbrsを使用する間はTFファイルを閉じないこと。poses配列はTFと共有している
	harkio_Neighbors *m_pPrinbrs;	///< クラス内で再計算したNeighbors
	float	m_fThresh;	///< スレッショルド(ユークリッド距離[m])
	
private:
	
	float _EuclidDistance(harkio_Position *pos, MICARY_Location locCur);
};

#endif // ENABLE_HARKIO_NH
#endif // CHarkFileIO_NH_h_defined
