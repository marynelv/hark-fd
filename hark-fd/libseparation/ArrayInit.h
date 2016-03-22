// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file ArrayInit.h
 \brief \~English Header file for initializing arrays (Microphone Array System)
 \brief \~Japanese 配列を含む構造体を初期化するヘッダファイル(Microphone Array System)
 \author Natsu Tanaka
 \date 20 Jul 2005
 \version $Id: ArrayInit.h 3538 2012-02-29 07:25:35Z tanaka $
*/

// ---------------------------------------------------------------------
#ifndef	ArrayInit_h_defined
#define	ArrayInit_h_defined
// ---------------------------------------------------------------------
#include	"MICARY_struct.h"
// ---------------------------------------------------------------------

// 2007/06/11 added by N.Tanaka C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif

/**
 \brief \~English It allocates float array
 \brief \~Japanese HA_Float配列の領域を確保する
 \param array \English It's floatArray
 \~Japanese HA_Float配列を含む構造体
 \param iArraySize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void FloatArrayAlloc(MICARY_FloatArray * array, int iArraySize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates float array
 \brief \~Japanese HA_Float配列の領域を確保する
 \param array \English It's floatArray
 \~Japanese HA_Float配列を含む構造体
 \param iArraySize \~English It's the size of array in use
 \~Japanese 使用要素数
 \param iReservedSize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void FloatArrayAlloc_r(MICARY_FloatArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief \~English It frees float array
 \brief \~Japanese HA_Float配列を解放する 
 \param array \~English FloatArray
 \~Japanese HA_Float配列を含む構造体
*/
void FloatArrayFree(MICARY_FloatArray * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees float array
 \brief \~Japanese HA_Float配列の使用サイズをクリアする 
 \param array \~English FloatArray
 \~Japanese HA_Float配列を含む構造体
*/
void FloatArrayClear(MICARY_FloatArray * array);
#endif

/**
 \brief \~English It allocates int array
 \brief \~Japanese HA_Integer配列の領域を確保する
 \param array \~English It's intArray
 \~Japanese HA_Integer配列を含む構造体
 \param iArraySize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void IntArrayAlloc(MICARY_IntArray * array, int iArraySize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates int array
 \brief \~Japanese HA_Int配列の領域を確保する
 \param array \English It's intArray
 \~Japanese HA_Float配列を含む構造体
 \param iArraySize \~English It's the size of array in use
 \~Japanese 使用要素数
 \param iReservedSize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void IntArrayAlloc_r(MICARY_IntArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief It frees int array
 \brief \~Japanese HA_Integer配列を解放する 
 \param array \~English IntArray
 \~Japanese HA_Integer配列を含む構造体
*/
void IntArrayFree(MICARY_IntArray * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees int array
 \brief \~Japanese HA_Int配列の使用サイズをクリアする 
 \param array \~English IntArray
 \~Japanese HA_Int配列を含む構造体
*/
void IntArrayClear(MICARY_IntArray * array);
#endif

/**
 \brief \~English It allocates complex array
 \brief \~Japanese HA_Complex配列の領域を確保する
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex配列を含む構造体
 \param iArraySize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void ComplexArrayAlloc(MICARY_ComplexArray * array, int iArraySize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates complex array
 \brief \~Japanese HA_Complex配列の領域を確保する
 \param array \English It's complexArray
 \~Japanese HA_Complex配列を含む構造体
 \param iArraySize \~English It's the size of array in use
 \~Japanese 使用要素数
 \param iReservedSize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void ComplexArrayAlloc_r(MICARY_ComplexArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex配列を解放する 
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex配列を含む構造体
*/
void ComplexArrayFree(MICARY_ComplexArray * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex配列の使用サイズをクリアする 
 \param array \~English ComplexArray
 \~Japanese HA_Complex配列を含む構造体
*/
void ComplexArrayClear(MICARY_ComplexArray * array);
#endif

/**
 \brief \~English It allocates location array
 \brief \~Japanese MICARY_Location配列を確保する
 \param array \~English It's LocationArray
 \~Japanese MICARY_Location配列を含む構造体
 \param iArraySize It's the size of array
 \~Japanese 確保する要素数
*/
void LocationArrayAlloc(MICARY_LocationArray * array, int iArraySize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates location array
 \brief \~Japanese MICARY_Location配列を確保する
 \param array \~English It's LocationArray
 \~Japanese MICARY_Location配列を含む構造体
 \param iArraySize \~English It's the size of array in use
 \~Japanese 使用要素数
 \param iReservedSize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void LocationArrayAlloc_r(MICARY_LocationArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief It frees location array
 \brief \~Japanese MICARY_Location配列を解放する
 \param array It's LocationArray
 \~Japanese MICARY_Location配列を含む構造体
*/
void LocationArrayFree(MICARY_LocationArray * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees location array
 \brief \~Japanese MICARY_Location配列の使用サイズをクリアする 
 \param array \~English LocationArray
 \~Japanese MICARY_Location配列を含む構造体
*/
void LocationArrayClear(MICARY_LocationArray * array);
#endif

/**
 \brief \~English It allocates database of beamforming
 \brief \~Japanese BF係数を格納する配列を確保する
 \param BFD \~English It's Database of beamforming
 \~Japanese BF係数を格納する配列を含む構造体\n
 [音源位置[マイクロホン位置数[周波数ライン数]]の順に格納する\n
 例(音源位置数s, マイク位置数m, 周波数ライン数fの場合):\n
 BF(S1M1F1), BF(S1M1F2), ..., BF(S1M1Ff), BF(S1M2F1), ..., BF(S1MmFf), BF(S2M1F1), ..., BF(SsMmFf)
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese BF係数の周波数ライン数
 \param iLocSize \~English It's the number of location
 \~Japanese BF係数の音源位置数
 \param iMicSize \~English It's the number of microphone
 \~Japanese BF係数のマイクロホン位置数
*/
void BFDatabaseAlloc(MICARY_BFDatabase * BFD, int iFreqSize, int iLocSize, int iMicSize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates database of beamforming
 \brief \~Japanese BF係数を格納する配列を確保する
 \param BFD \~English It's Database of beamforming
 \~Japanese BF係数を格納する配列を含む構造体\n
 [音源位置[マイクロホン位置数[周波数ライン数]]の順に格納する\n
 例(音源位置数s, マイク位置数m, 周波数ライン数fの場合):\n
 BF(S1M1F1), BF(S1M1F2), ..., BF(S1M1Ff), BF(S1M2F1), ..., BF(S1MmFf), BF(S2M1F1), ..., BF(SsMmFf)
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese BF係数の周波数ライン数
 \param iLocSize \~English It's the number of location
 \~Japanese BF係数の音源位置数
 \param iMicSize \~English It's the number of microphone
 \~Japanese BF係数のマイクロホン位置数
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese 確保する要素数(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese 確保する要素数(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese 確保する要素数(Mic)
*/
void BFDatabaseAlloc_r(MICARY_BFDatabase * BFD, int iFreqSize, int iLocSize, int iMicSize, 
											  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize);
#endif

/**
 \brief \~English It frees database of beamforming
 \brief \~Japanese BF係数を格納する配列を開放する
 \param BFD \English It's Database of beamforming
 \~Japanese BF係数を格納する配列を含む構造体\n
*/
void BFDatabaseFree(MICARY_BFDatabase * BFD);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees database of beamforming
 \brief \~Japanese BF係数を格納する配列の使用サイズをクリアする 
 \param BFD \English It's Database of beamforming
 \~Japanese BF係数を格納する配列を含む構造体\n
*/
void BFDatabaseClear(MICARY_BFDatabase * BFD);
#endif

/**
 \brief \~English It allocates database of transfer function
 \brief \~Japanese 伝達関数を格納する配列を確保する
 \param TFD \~English It's Database of transfer function
 \~Japanese 伝達関数を格納する配列を含む構造体
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese 伝達関数の周波数ライン数
 \param iLocSize \~English It's the number of location
 \~Japanese 伝達関数の音源位置数
 \param iMicSize \~English It's the number of microphone
 \~Japanese 伝達関数のマイクロホン位置数
*/
void TFDatabaseAlloc(MICARY_TFDatabase * TFD, int iFreqSize, int iLocSize, int iMicSize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates database of transfer function
 \brief \~Japanese 伝達関数を格納する配列を確保する
 \param TFD \~English It's Database of transfer function
 \~Japanese 伝達関数を格納する配列を含む構造体
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese 伝達関数の周波数ライン数
 \param iLocSize \~English It's the number of location
 \~Japanese 伝達関数の音源位置数
 \param iMicSize \~English It's the number of microphone
 \~Japanese 伝達関数のマイクロホン位置数
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese 確保する要素数(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese 確保する要素数(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese 確保する要素数(Mic)
*/
void TFDatabaseAlloc_r(MICARY_TFDatabase * TFD, int iFreqSize, int iLocSize, int iMicSize,
											  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize);
#endif

/**
 \brief \~English It frees database of transfer function
 \brief \~Japanese 伝達関数を格納する配列を開放する
 \param TFD \~English It's Database of transfer function
 \~Japanese 伝達関数を格納する配列を含む構造体
*/
void TFDatabaseFree(MICARY_TFDatabase * TFD);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees database of transfer function
 \brief \~Japanese 伝達関数を格納する配列の使用サイズをクリアする
 \param TFD \~English It's Database of transfer function
 \~Japanese 伝達関数を格納する配列を含む構造体
*/
void TFDatabaseClear(MICARY_TFDatabase * TFD);
#endif

/**
 \brief \~English It allocates int array(dim:2)
 \brief \~Japanese int型の二次元配列を確保する
 \param array \~English It's the array of integer. (iArraySize1 * iArraySize2)
 \~Japanese int型の二次元配列のポインタ
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese 次元1の配列の個数
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese 次元2の配列の個数
*/
void IntArray2Alloc(MICARY_IntArray2 * array, int iArraySize1, int iArraySize2);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates int array(dim:2)
 \brief \~Japanese int型の二次元配列を確保する
 \param array \~English It's the array of integer. (iArraySize1 * iArraySize2)
 \~Japanese int型の二次元配列のポインタ
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese 次元1の配列の個数
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese 次元2の配列の個数
 \param iReservedArraySize1 \~English It's the size of array (dim:1)
 \~Japanese 次元1の配列の確保個数
 \param iReservedArraySize2 \~English It's the size of array (dim:2)
 \~Japanese 次元2の配列の確保個数
*/
void IntArray2Alloc_r(MICARY_IntArray2 * array, int iArraySize1, int iArraySize2,
											  int iReservedSize1, int iReservedSize2);
#endif

/**
 \brief \~English It frees int array(dim:2)
 \brief \~Japanese int型の二次元配列を開放する
 \param array \~English It's the array of integer. (iArraySize1 * iArraySize2)
 \~Japanese int型の二次元配列のポインタ
*/
void IntArray2Free(MICARY_IntArray2 * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees int array(dim:2)
 \brief \~Japanese int型の二次元配列の使用サイズをクリアする
 \param array \~English It's the array of integer. (iArraySize1 * iArraySize2)
 \~Japanese int型の二次元配列のポインタ
*/
void IntArray2Clear(MICARY_IntArray2 * array);
#endif

// 2006/10/12 added by N.Tanaka
/**
 \brief \~English It allocates complex spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Complex配列の領域を確保する
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complexのスペクトル用構造体
 \param numCh \~English It's the number of channel.
 \~Japanese チャンネル数
 \param iLowerIndex \~English It's the lower frequency index.
 \~Japanese 下限周波数インデックス
 \param iUpperIndex \~English It's the upper frequency index.
 \~Japanese 上限周波数インデックス
*/
void AllocComplexSpecArray(MICARY_ComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates complex spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Complex配列の領域を確保する
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complexのスペクトル用構造体
 \param numCh \~English It's the number of channel.
 \~Japanese チャンネル数
 \param iLowerIndex \~English It's the lower frequency index.
 \~Japanese 下限周波数インデックス
 \param iUpperIndex \~English It's the upper frequency index.
 \~Japanese 上限周波数インデックス
 \param iReservedChSize \~English It's the size of array
 \~Japanese 配列の確保個数(ch)
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese 配列の確保個数(Freq)
*/
void AllocComplexSpecArray_r(MICARY_ComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex,
						  int iReservedChSize,
						  int iReservedFreqSize);
#endif

// 2006/10/12 added by N.Tanaka
/**
 \brief \~English It frees complex spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Complex配列の領域を開放する
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complexのスペクトル用構造体
*/
void FreeComplexSpecArray(MICARY_ComplexSpecArray *pcsSpec);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Complex配列の使用サイズクリア
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complexのスペクトル用構造体
*/
void ClearComplexSpecArray(MICARY_ComplexSpecArray *pcsSpec);
#endif

// 2006/10/12 added by N.Tanaka
/**
 \brief \~English It allocates float spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Float配列の領域を確保する
 \param pfsSpec \~English It's float spectrum array
 \~Japanese HA_Floatのスペクトル用構造体
 \param numCh \~English It's the number of channel.
 \~Japanese チャンネル数
 \param iLowerIndex \~English It's the lower frequency index.
 \~Japanese 下限周波数インデックス
 \param iUpperIndex \~English It's the upper frequency index.
 \~Japanese 上限周波数インデックス
*/
void AllocFloatSpecArray(MICARY_FloatSpecArray *pfsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates freq spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Float配列の領域を確保する
 \param pfsSpec \~English It's float spectrum array
 \~Japanese HA_Floatのスペクトル用構造体
 \param numCh \~English It's the number of channel.
 \~Japanese チャンネル数
 \param iLowerIndex \~English It's the lower frequency index.
 \~Japanese 下限周波数インデックス
 \param iUpperIndex \~English It's the upper frequency index.
 \~Japanese 上限周波数インデックス
 \param iReservedChSize \~English It's the size of array
 \~Japanese 配列の確保個数(ch)
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese 配列の確保個数(Freq)
*/
void AllocFloatSpecArray_r(MICARY_FloatSpecArray *pfsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex,
						  int iReservedChSize,
						  int iReservedFreqSize);
#endif


// 2006/10/12 added by N.Tanaka
/**
 \brief \~English It frees float spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Float配列の領域を開放する
 \param pfsSpec \~English It's float spectrum array
 \~Japanese HA_Floatのスペクトル用構造体
*/
void FreeFloatSpecArray(MICARY_FloatSpecArray *pfsSpec);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees float spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Float配列の使用サイズをクリアする
 \param pfsSpec \~English It's float spectrum array
 \~Japanese HA_Floatのスペクトル用構造体
*/
void ClearFloatSpecArray(MICARY_FloatSpecArray *pfsSpec);
#endif

// 2007/08/22 added by N.Tanaka
/**
 \brief \~English It allocates char array(string)
 \brief \~Japanese char配列(string)の領域を確保する
 \param array \English It's CharArray(string)
 \~Japanese char配列(string)を含む構造体
 \param iArraySize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void StringAlloc(HA_String * array, int iArraySize);

// 2007/08/22 added by N.Tanaka
/**
 \brief \~English It frees char array(string)
 \brief \~Japanese char配列(string)を解放する 
 \param array \~English CharArray(string)
 \~Japanese char配列(string)を含む構造体
*/
void StringFree(HA_String * array);


#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------
#endif	// ArrayInit_h_defined
