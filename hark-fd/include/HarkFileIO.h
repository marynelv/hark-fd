// ---------------------------------------------------------------------
/*
 * Copyright 2010 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#ifndef	CHarkFileIO_h_defined
#define	CHarkFileIO_h_defined
// ---------------------------------------------------------------------
#include "../config.h"

#ifndef ENABLE_HARKIO_NH

#include <iostream>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

#ifdef ENABLE_HARKIO2
	#include "harkio2/harkio.hpp"
#elif ENABLE_HARKIO1
	#include "libharkio/libharkio.h"
	#include "libharkio/textdata.h"
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

	// ファイル変換
	bool ConvertToGHDSS(const char* cOrgFilename,
						const char * cNewFilename) ;

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


	// テキストファイル // 
	bool LoadPositionData(const char* cFilename,
					MICARY_LocationArray * plocaPos,int iFileType) ;
					
#ifdef ENABLE_HARKIO1	
		// テキストファイル (libHarkIO format)// 
	bool LoadPositionData(const char* cFilename,
					MICARY_LocationArray * plocaPos) ;
#endif

private:
	
};

#endif // ENABLE_HARKIO_NH
#endif // CHarkFileIO_h_defined
