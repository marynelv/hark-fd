// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file MICARY_struct.h
 \brief \~English Header file for Microphone Array System
 \brief \~Japanese マイクアレイシステムの共通構造体のためのヘッダファイル(Microphone Array System)
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
// micarray_fdplugin用の対策
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
	HA_Float *	ha_fValue; ///<\~Japanese HA_Float配列
	HA_Num	ha_numValue; ///<\~Japanese 配列のサイズ
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_FloatArray;

/**
 Integer array struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Integer *	ha_iValue; ///<\~Japanese HA_Integer配列
	HA_Num	ha_numValue; ///<\~Japanese 配列のサイズ
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_IntArray;

/**
 Complex array struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Complex *	ha_cmpValue; ///<\~Japanese HA_Complex配列
	HA_Num	ha_numValue; ///<\~Japanese 配列のサイズ
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_ComplexArray;

/**
 Location struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Float	ha_fX; ///<\~Japanese X座標[m]
	HA_Float	ha_fY; ///<\~Japanese Y座標[m]
	HA_Float	ha_fZ; ///<\~Japanese Z座標[m]
	HA_Float	ha_fTheta; ///<\~Japanese 方位角[deg]
	HA_Float	ha_fPhi; ///<\~Japanese 仰角[deg]
	MICARY_LocationFlg	micary_LocationFlag; ///<\~Japanese 有効な座標を示すフラグ
	HA_Id	ha_idPos; ///<\~Japanese 位置ID // 2008/05/15 added by N.Tanaka
	HA_Id ha_idDir; ///<\~Japanese 方向ID // 2008/05/15 added by N.Tanaka
	HA_Id tfindex[3]; ///<\~Japanese TFIndex // 2013/06/26 added by K.Morio
	HA_Id tfindex_max[3]; ///<\~Japanese TFIndexの最大値 // 2013/10/10 added by K.Morio
    HA_Id tfindex_min[3]; ///<\~Japanese TFIndexの最小値 // 2013/10/10 added by K.Morio
	HA_Id tfposid;	///<\~Japanese TFDB中の位置ID // 2014/8/27 added by N.Tanaka ha_idPosが異なる使われ方をしているため追加

} MICARY_Location;

/**
 Location Array struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Id *	ha_idLocation; ///<\~Japanese 位置座標ID配列
	MICARY_Location *	micary_Location; ///<\~Japanese 位置座標配列
	HA_Num	ha_numLocation; ///<\~Japanese 位置座標数
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_LocationArray;

/**
 Parameter of setting struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	HA_Float	ha_fFs; ///<\~Japanese サンプリング周波数[Hz]
	HA_Num		ha_numNFFT; ///<\~Japanese FFT長
	HA_Num		ha_numWindowLength; ///<\~Japanese 窓長
	MICARY_FreqScaleEnum	micary_FScale; ///<\~Japanese 周波数軸
	HA_Float	ha_fTemperature; ///<\~Japanese 気温[deg C]
} MICARY_SettingParameter;

/**
 Database of BF
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	MICARY_ComplexArray	micary_cmpaBF; ///<\~Japanese BF係数配列
	MICARY_IntArray		micary_iaFreqLine; ///<\~Japanese BF係数の周波数ライン配列
	MICARY_LocationArray	micary_locaSource; ///<\~Japanese BF係数の音源推定位置座標配列
	MICARY_LocationArray	micary_locaMic; ///<\~Japanese BF係数のマイクロホン位置座標配列
	HA_Float		ha_fSearchMicR; ///<\~Japanese BF計算対象となるマイクロホンと音源探査位置の距離の上限[m]
	//HA_Num			ha_numMic;
} MICARY_BFDatabase;

/**
 Database of TF
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	MICARY_ComplexArray	micary_cmpaTF; ///<\~Japanese 伝達関数配列
	MICARY_IntArray	micary_iaFreqLine; ///<\~Japanese 伝達関数の周波数ライン配列
	MICARY_LocationArray	micary_locaSource; ///<\~Japanese 伝達関数の音源推定位置座標配列
	MICARY_LocationArray	micary_locaMic; ///<\~Japanese 伝達関数のマイクロホン位置座標配列
	//HA_Num			ha_numMic;
} MICARY_TFDatabase;

/**
 Integer array struct(dim:2)
 \author	Natsu Tanaka
 \date	12 Oct 2005
*/
typedef struct{
	HA_Integer **	ha_iValue; ///<\~Japanese HA_Integer型2次元配列
	HA_Num	ha_numValue1; ///<\~Japanese 配列のサイズ
	HA_Num	ha_numValue2; ///<\~Japanese 配列のサイズ
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReserved1; ///<\~Japanese 配列の実際のサイズ
	HA_Num	ha_numReserved2; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_IntArray2;

/**
 ADC data array struct
 \author	Natsu Tanaka
 \date 5 Dec 2005
*/
typedef struct{
	HA_Num	ha_numDataLength; ///<\~Japanese 有効な値の数
	HA_Num	ha_numCh;	///<\~Japanese ADチャンネル数 // 2006/11/09 added by N.Tanaka
	MICARY_IntArray micary_iaData; ///<\~Japanese 確保済みのHA_Integerの配列
} MICARY_ADCDataArray;

// modified S.Suzuki 2010/03/28
/**
 Wavfile information struct
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
/*
typedef struct{
	//char	cFilename[256]; ///<\~Japanese ファイル名
	HA_String ha_stFileName; ///<\~Japanese ファイル名
	FILE *	pfinFile; ///<\~Japanese ファイルポインタ
	HA_Num	ha_numCurrentSample; ///<\~Japanese 読み出し済みサンプル数[sample]
	struct waveheader sWavHeader; ///<\~Japanese wavファイルヘッダ
} MICARY_WavFile;
*/
// 2008/11/11 moved from SpectrumFile.h
// 2008/11/11 changed by N.Tanaka
// X86_64bitの場合longは8byteとなるため，intに変更
/**
 File Header struct
 \author	Natsu Tanaka
 \date	11 Sep 2006
*/
typedef struct {
	unsigned int ulVersion;	///<\~Japanese バージョン
	float fFs;					///<\~Japanese サンプリング周波数[Hz]
	unsigned int ulNFFT;		///<\~Japanese FFT長[point]
	unsigned int ulWindow;		///<\~Japanese 窓長[sample]
	int lNFrame;				///<\~Japanese フレーム数(未定の場合は負数)
	int lShift;				///<\~Japanese シフト長[sample]
	unsigned int ulNCh;		///<\~Japanese チャンネル数
	unsigned int ulLowerFreqLine;	///<\~Japanese 下限周波数ライン番号
	unsigned int ulUpperFreqLine;	///<\~Japanese 上限周波数ライン番号
	unsigned short usDataType;	///<\~Japanese データのタイプ(0:float, 1:complex(float))
	char cPad[22];
} MICARY_SpecFileHeader;

/**
 Spectrumfile information struct
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	//char	cFilename[256]; ///<\~Japanese ファイル名
	HA_String ha_stFileName; ///<\~Japanese ファイル名
	FILE *	pfinFile; ///<\~Japanese ファイルポインタ
	HA_Num	ha_numCurrentFrame; ///<\~Japanese 読み出し済みフレーム数[frame]
	MICARY_SpecFileHeader micary_sSpecHeader; ///<\~Japanese スペクトルファイルヘッダ
} MICARY_SpecFile;

// modified S.Suzuki 2010/03/28
/**
 Wavfile struct for getting a frame data
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
/*
typedef struct{
	MICARY_WavFile	micary_sWavFile; ///<\~Japanese WAVファイル情報
	HA_Integer	ha_iStartsample; ///<\~Japanese 開始時間サンプル[sample]
	HA_Integer	ha_iEndSample; ///<\~Japanese 終了時間サンプル[sample]
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
	MICARY_WavFile	micary_sWavFile; ///<\~Japanese WAVファイル情報
	MICARY_FloatArray	micary_faWave; ///<\~Japanese 時系列データ
} MICARY_SavedSetWavFrame;
*/
/**
 Spectrumfile struct for getting a frame data
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	MICARY_SpecFile	micary_sSpecFile; ///<\~Japanese スペクトルファイル情報
	HA_Integer	ha_iStartFrame; ///<\~Japanese 開始時間フレーム[frame]
	HA_Integer	ha_iEndFrame; ///<\~Japanese 終了時間フレーム[frame]
} MICARY_SavedGetSpecFrame;

/**
 Spectrumfile struct for setting a frame data
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	MICARY_SpecFile	micary_sSpecFile; ///<\~Japanese スペクトルファイル情報
	HA_Num	ha_numCurrentFrame; ///<\~Japanese 書き込み済みフレーム数[frame]
} MICARY_SavedSetSpecFrame;

/**
 Parameter struct for getting frame data
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	HA_Float	ha_fFs;	///<\~Japanese サンプリング周波数[Hz]
	HA_Num	ha_numNFFT; ///<\~Japanese FFT長[point]
	HA_Integer	ha_iShift; ///<\~Japanese シフト長[sample](次フレームの始端→前フレームの終端)
	HA_Num	ha_numWindow; ///<\~Japanese 窓長[sample]
	MICARY_WindowEnum	micary_nwnWindow; ///<\~Japanese 窓関数
} MICARY_PrmFrame;

/**
 Spectrum struct (Complex)
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	MICARY_ComplexArray	micary_cmpaSpec; ///<\~Japanese スペクトル(Complex) 
	HA_Num	ha_numCh; ///<\~Japanese チャンネル数
	HA_Integer	ha_iLowerFreq; ///<\~Japanese 下限周波数[Line]
	HA_Integer	ha_iUpperFreq; ///<\~Japanese 上限周波数[Line]
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReservedChSize; ///<\~Japanese 配列の実際のサイズ
	HA_Num	ha_numReservedFreqSize; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_ComplexSpecArray;

/**
 Spectrum struct (Float)
 \author	Natsu Tanaka
 \date 29 Sep 2006
*/
typedef struct{
	MICARY_FloatArray	micary_faSpec; ///<\~Japanese スペクトル(Float) 
	HA_Num	ha_numCh; ///<\~Japanese チャンネル数
	HA_Integer	ha_iLowerFreq; ///<\~Japanese 下限周波数[Line]
	HA_Integer	ha_iUpperFreq; ///<\~Japanese 上限周波数[Line]
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num	ha_numReservedChSize; ///<\~Japanese 配列の実際のサイズ
	HA_Num	ha_numReservedFreqSize; ///<\~Japanese 配列の実際のサイズ
#endif
} MICARY_FloatSpecArray;

// ---------------------------------------------------------------------
#endif	// MICARY_struct_h_defined
