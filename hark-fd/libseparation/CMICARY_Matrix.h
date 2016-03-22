/**
 Copyright (c) 2004-2010 Honda Research Institute Japan, Co., Ltd.

 \file CMICARY_Matrix.h
 \brief \~English Header file for matrix
 \brief \~Japanese 行列演算ヘッダファイル(GDHSS program)
 \author Aki Yamamoto
 \date 22 Feb 2011
 \version $Id: CMICARY_Matrix.h $
*/
// ---------------------------------------------------------------------
#ifndef	CMICARY_Matrix_h_defined
#define	CMICARY_Matrix_h_defined
// ---------------------------------------------------------------------
#include <boost/numeric/ublas/matrix.hpp>
#include <complex>
#include <iostream>
#include <boost/numeric/ublas/io.hpp>      // Matrix ストリーム入出力用ヘッダ
#include <stdio.h>

#include "ArrayInit.h"
#include "MICARY_struct.h"
#include "MICARY_GHDSS_struct.h"

//#include "GHDSS_MathMatDouble.h"
// ---------------------------------------------------------------------
using namespace std;
using namespace boost;
using namespace boost::numeric;
using namespace boost::numeric::ublas;
// ---------------------------------------------------------------------
typedef ublas::matrix<complex<double> > cdMat;


// ---------------------------------------------------------------------
#define __MAX_FLOAT_VALUE 3.0428E+038
#define __MIN_FLOAT_VALUE 1.1754E-038
#define __MAX_DOUBLE_VALUE 1.79769313E+308
#define __MIN_DOUBLE_VALUE 2.22507385E-308

#define __FALSE__ 0
#define __TRUE__ 1
// ---------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// ----------- Add 2011.01.19 -----------------
typedef struct 
{
    /// real part
    double re;
    /// imaginary part
    double im;
} DA_Complex;
/**
 Complex array struct
 \author	Natsu Tanaka
 \date	20 Jul 2005
*/
typedef struct{
	DA_Complex *	da_cmpValue; ///<\~Japanese HA_Complex配列
	HA_Num	ha_numValue; ///<\~Japanese 配列のサイズ
// 2012/01/13 added
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num ha_numReserved; ///<\~Japanese 確保要素数
#endif
} MICARY_dComplexArray;

/**
 Spectrum struct (Double Complex)
 \author	Sachiko Suzuki
 \date 01 Jul 2008
*/
typedef struct{
	MICARY_dComplexArray	micary_cmpaSpec; ///<\~Japanese スペクトル(Complex) 
	HA_Num	ha_numCh; ///<\~Japanese チャンネル数
	HA_Integer	ha_iLowerFreq; ///<\~Japanese 下限周波数[Line]
	HA_Integer	ha_iUpperFreq; ///<\~Japanese 上限周波数[Line]
// 2012/01/13 added
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num ha_numReservedChSize; ///<\~Japanese 確保要素数 (ch)
	HA_Num ha_numReservedFreqSize; ///<\~Japanese 確保要素数 (freq)
#endif
} MICARY_dComplexSpecArray;


// ----------- Add 2011.01.19 -----------------
/**
 \brief \~English It allocates complex array
 \brief \~Japanese HA_Complex配列の領域を確保する
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex配列を含む構造体
 \param iArraySize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void dComplexArrayAlloc(MICARY_dComplexArray * array, int iArraySize);

// 2012/01/13 added by N.Tanaka
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
void dComplexArrayAlloc_r(MICARY_dComplexArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex配列を解放する 
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex配列を含む構造体
*/
void dComplexArrayFree(MICARY_dComplexArray * array);

// 2012/01/13 added
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex配列の使用サイズをクリアする 
 \param array \~English ComplexArray
 \~Japanese HA_Complex配列を含む構造体
*/
void dComplexArrayClear(MICARY_dComplexArray * array);
#endif

	
/**
 \brief \~English It initializes complex array
 \brief \~Japanese Complex配列の初期化(0埋め)
 \param array \~English It's complex array
 \~Japanese Complex配列を含む構造体
*/

void dComplexArrayInit(MICARY_dComplexArray * array) ;

// 2008/07/01 added by S.Suzuki
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
void AllocdComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec,HA_Num numCh, HA_Integer iLowerIndex, HA_Integer iUpperIndex);

// 2012/01/13 added by N.Tanaka
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
void AllocdComplexSpecArray_r(MICARY_dComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex,
						  int iReservedChSize,
						  int iReservedFreqSize);
#endif

/**
 \brief \~English It frees complex spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Complex配列の領域を開放する
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complexのスペクトル用構造体
*/
void FreedComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec);

// 2012/01/13 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Complex配列の使用サイズクリア
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complexのスペクトル用構造体
*/
void CleardComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec);
#endif


// ****************** boost/numeric/ublas/ を使用しない *******************
// -----  double  ------

/**
 \brief \~English It products complex matrix.(double)
 \brief \~Japanese 複素行列の掛け算(double)
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param caInMat1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
*/
int MICARY_Product_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It devids complex matrix.
 \brief \~Japanese 複素行列の割り算
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param caInMat1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
*/
int MICARY_Divide_CompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It adds complex matrix.
 \brief \~Japanese 複素行列のたし算
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
*/
int MICARY_Sum_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It substracts complex matrix.
 \brief \~Japanese 複素行列のひき算
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
*/
int MICARY_Sub_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2);


/**
 \brief \~English It translates conjugate complex matrix.
 \brief \~Japanese 共役転置複素数
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese 入力複素行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int MICARY_ConjTrans_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,int g,int r);

/**
 \brief \~English It makes diagonal complex matrix.
 \brief \~Japanese 対角行列(正方行列の場合)
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
 \param caInMat \~English It's the number of row for input matrix.
 \~Japanese 入力複素行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int MICARY_Diag_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat, int g , int r);

/**
 \brief \~English It conjugates complex matrix.
 \brief \~Japanese 共役複素数
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese 入力複素行列
*/
int MICARY_Conj_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat);

/**
 \brief \~English It computes a dot product of a conjugated vector with another vector(0:false 1:true).
 \brief \~Japanese dot掛け算(0:エラー 1:正常終了)
 \param pcaOut \~English It's the complex  vector.
 \~Japanese 出力複素数ベクトル
 \param caIn1 \~English It's the input vector1.
 \~Japanese 入力複素数ベクトル1
 \param caIn2 \~English It's the input vector2.
 \~Japanese 入力複素数ベクトル2
*/
int MICARY_Dot_CompMat(MICARY_dComplexArray *pcaOut, MICARY_dComplexArray caIn1, MICARY_dComplexArray caIn2);

/**
 \brief \~English It makes unit matrix.
 \brief \~Japanese 単位行列の作成
 \param pcaOutMat \~English It's the complex matrix struct
 \~Japanese 出力複素数行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int MICARY_Eye_CompMat(MICARY_dComplexArray *pcaOutMat,int g,int r);


/**
 \brief \~English It multiplicates complex matrix.
 \brief \~Japanese 複素行列の定数倍
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param caInMat \~English It's the complex array struct.
 \~Japanese 入力複素行列
 \param a \~English It's the coeficient value.
 \~Japanese 係数値
*/
int MICARY_Multipl_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat, double a);

/*
 \brief \~English It devids complex matrix.
 \brief \~Japanese 複素行列の平方根
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param caInMat1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列
*/
int MICARY_Sqrt_CompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1) ;


/**
 \brief \~English It calculates abs complex matrix.(double)
 \brief \~Japanese 行列の絶対値の計算(double)
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese 出力複素数行列
 \param caInMat \~English It's the complex  matrix.
 \~Japanese 入力複素数行列
*/
int MICARY_Abs_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat) ;


// -----  float  ------

/**
 \brief \~English It substracts complex matrix.
 \brief \~Japanese 複素行列のひき算
 \param pcaOutMat \~English It's the complex array struct.(float)
 \~Japanese 出力複素行列(float)
 \param caInMat1 \~English It's the complex array struct.(float)
 \~Japanese 入力複素行列1(float)
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.(float)
 \~Japanese 入力複素行列2(float)
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
*/
int MICARY_Sub_fCompMat(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
										MICARY_ComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It products complex matrix.
 \brief \~Japanese 複素行列の掛け算
 \param pcaOutMat \~English It's the complex array struct.(double)
 \~Japanese 出力複素行列(double)
 \param caInMat1 \~English It's the number of row for input matrix1.(float)
 \~Japanese 入力複素行列1(float)
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.(double)
 \~Japanese 入力複素行列2(double)
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
*/
int MICARY_Product_fdCompMat(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It products complex matrix.
 \brief \~Japanese 複素行列の掛け算
 \param pcaOutMat \~English It's the complex array struct.(double)
 \~Japanese 出力複素行列(double)
 \param caInMat1 \~English It's the number of row for input matrix1.(double)
 \~Japanese 入力複素行列1(double)
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.(float)
 \~Japanese 入力複素行列2(float)
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
*/
int MICARY_Product_dfCompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2);


//int MICARY_Product_ddfCompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
//									MICARY_ComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It products complex matrix.
 \brief \~Japanese 複素行列の掛け算
 \param pcaOutMat \~English It's the complex array struct.(float)
 \~Japanese 出力複素行列(float)
 \param caInMat1 \~English It's the number of row for input matrix1.(float)
 \~Japanese 入力複素行列1(float)
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.(float)
 \~Japanese 入力複素行列2(float)
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
*/
int MICARY_Product_fCompMat(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2);


/**
 \brief \~English It translates conjugate complex matrix.
 \brief \~Japanese 共役転置複素数
 \param pcaOutMat \~English It's the complex matrix struct.(double)
 \~Japanese 出力複素行列(double)
 \param caInMat \~English It's the complex matrix struct.(float)
 \~Japanese 入力複素行列(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int MICARY_ConjTrans_dfCompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r);


/**
 \brief \~English It translates conjugate complex matrix.
 \brief \~Japanese 共役転置複素数
 \param pcaOutMat \~English It's the complex matrix struct.(float)
 \~Japanese 出力複素行列(float)
 \param caInMat \~English It's the complex matrix struct.(float)
 \~Japanese 入力複素行列(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int MICARY_ConjTrans_fCompMat(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r);

/**
 \brief \~English It makes diagonal complex matrix.
 \brief \~Japanese 対角行列(正方行列の場合)
 \param pcaOutMat \~English It's the complex matrix struct.(float)
 \~Japanese 出力複素行列(float)
 \param caInMat \~English It's the number of row for input matrix.(float)
 \~Japanese 入力複素行列(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int MICARY_Diag_fCompMat(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat, int g , int r);


/**
 \brief \~English It multiplicates complex matrix.
 \brief \~Japanese 複素行列の定数倍
 \param pcaOutMat \~English It's the complex array struct.(float)
 \~Japanese 出力複素行列(float)
 \param caInMat \~English It's the complex array struct.(float)
 \~Japanese 入力複素行列(float)
 \param a \~English It's the coeficient value.
 \~Japanese 係数値
*/
int MICARY_Multipl_fCompMat(MICARY_ComplexArray *pcaOutMat, MICARY_ComplexArray caInMat, float a);

/**
 \brief \~English It conjugates complex matrix.
 \brief \~Japanese 共役複素数
 \param pcaOutMat \~English It's the complex matrix struct.(float)
 \~Japanese 出力複素行列(float)
 \param caInMat \~English It's the complex matrix struct.(float)
 \~Japanese 入力複素行列(float)
*/
int MICARY_Conj_fCompMat(MICARY_ComplexArray *pcaOutMat, MICARY_ComplexArray caInMat);

/**
 \brief \~English It makes unit matrix.
 \brief \~Japanese 単位行列の作成
 \param pcaOutMat \~English It's the complex matrix struct
 \~Japanese 出力複素数行列(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int MICARY_Eye_fCompMat(MICARY_ComplexArray *pcaOutMat,int g,int r);

/**
 \brief \~English It transrates complex matrix.
 \brief \~Japanese 複素数の転置
 \param pcaOutMat \~English It's the complex matrix struct.(float)
 \~Japanese 出力複素行列(float)
 \param caInMat \~English It's the complex matrix struct.(float)
 \~Japanese 入力複素行列(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int MICARY_Trans_fCompMat(MICARY_ComplexArray *pcaOutMat, MICARY_ComplexArray caInMat,int g,int r);

/**
 \brief \~English It calculates abs complex matrix.
 \brief \~Japanese 行列の絶対値の計算
 \param pcaOutMat \~English It's the complex  matrix.(float)
 \~Japanese 出力複素数行列(float)
 \param caInMat \~English It's the complex  matrix.(float)
 \~Japanese 入力複素数行列(float)
*/
int MICARY_Abs_dfCompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat); 

/**
 \brief \~English It calculates max value.
 \brief \~Japanese 行列(real part)の最大値の計算
 \param caInMat \~English It's the complex  matrix.
 \~Japanese 入力複素数行列
*/
double MICARY_Max_CompMat(MICARY_dComplexArray caInMat);



//   ******************* boost/numeric/ublas/ を使用 *******************
// 2011.02.16時点では未使用

bool _InitCompMat(cdMat *pcaOutMat);

bool _DiagCompMat(cdMat *pcaOutMat, cdMat caInMat);

bool _EyeCompMat(cdMat *pcaOutMat);

bool _AbsCompMat(cdMat *pcaOutMat ,cdMat caInMat);


double _MaxCompMat(cdMat caInMat);

bool _SqrtCompMat(cdMat *pcaOutMat ,cdMat caInMat);

bool _MultiplCompMat(cdMat *pcaOutMat ,cdMat caInMat, double a);

//--- uBlas::matrix型への変換 ---
bool _Convert_ComplexSpec(cdMat *pcaOutMat, MICARY_ComplexArray caIn, int Nrow, int Ncol, int iFreqN);

bool _Convert_Complex_InputSpec(cdMat *pcaOutMat ,MICARY_ComplexArray caIn, int Nfreq, int iFreq);

bool _Convert_dComplexSpec(cdMat *pcaOutMat ,MICARY_dComplexArray caIn,   int Nrow, int Nmic, int iFreqN);
// ---

//--- uBlas::matrix型からの変換
bool _Update_ComplexSpec(MICARY_ComplexArray *caOut, cdMat pcaInMat, int Nrow, int Ncol, int iFreq);

bool _Update_dComplexSpec(MICARY_dComplexArray *caOut, cdMat pcaInMat, int Nrow, int Ncol, int iFreq);

//bool _Update_Complex_InputSpec(MICARY_ComplexArray *caOut, cdMat pcaInMat,   int Nfreq, int iFreq);
//---
//   ********************************************************************

#ifdef __cplusplus
}
#endif
// -------------------------------------------------------------------------------------
#endif // CMICARY_Matrix_h_defined
