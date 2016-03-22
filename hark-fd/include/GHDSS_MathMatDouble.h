// ---------------------------------------------------------------------
/*
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_MathMatDouble.h
 \brief	\~English Header file for matrix operation[type double]. (GHDSS System)
 \brief	\~Japanese 行列演算のためのヘッダファイル(GHDSS System)
 \author	Sachiko Suzuki
 \date	27 Jun 2008
 \version	$Id: GHDSS_MathMatDouble.h 456 2009-04-09 02:24:03Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef GHDSS_MathMatDouble_h_defined
#define GHDSS_MathMatDouble_h_defined
// ---------------------------------------------------------------------
#include	"MICARYHA_struct.h"
#include "MICARY_struct.h"
#include "MICARY_GHDSS_struct.h"
#include <stdlib.h>

#define __MAX_FLOAT_VALUE 3.0428E+038
#define __MIN_FLOAT_VALUE 1.1754E-038
#define __MAX_DOUBLE_VALUE 1.79769313E+308
#define __MIN_DOUBLE_VALUE 2.22507385E-308
// ---------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

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
} MICARY_dComplexSpecArray;


// ---------------------------------------------------------------------
/**
 \brief \~English It allocates complex array
 \brief \~Japanese HA_Complex配列の領域を確保する
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex配列を含む構造体
 \param iArraySize \~English It's the size of array
 \~Japanese 確保する要素数
*/
void dComplexArrayAlloc(MICARY_dComplexArray * array, int iArraySize);

/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex配列を解放する 
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex配列を含む構造体
*/
void dComplexArrayFree(MICARY_dComplexArray * array);

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
void AllocdComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex);

/**
 \brief \~English It frees complex spectrum array
 \brief \~Japanese スペクトルデータ用のHA_Complex配列の領域を開放する
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complexのスペクトル用構造体
*/
void FreedComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec);

/*
 \brief \~English It products complex matrix.
 \brief \~Japanese 複素行列の掛け算
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/
int ProductdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It products complex matrix.
 \brief \~Japanese 複素行列の掛け算
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/
int ProductdfCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It products complex matrix.
 \brief \~Japanese 複素行列の掛け算
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/
int ProductfdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It products complex matrix.
 \brief \~Japanese 複素行列の掛け算
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/
int ProductdffCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It products complex matrix.
 \brief \~Japanese 複素行列の掛け算
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/
int ProductfddCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2) ;

// ---------------------------------------------------------------------

/*
 \brief \~English It conjugates complex matrix.
 \brief \~Japanese 共役複素数
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese 入力複素行列
*/
int ConjdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat);

// ---------------------------------------------------------------------
/*
 \brief \~English It makes diagonal complex matrix.
 \brief \~Japanese 対角行列(正方行列の場合)
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力複素行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int DiagdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat, int g , int r) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It makes diagonal complex matrix.
 \brief \~Japanese 対角行列(正方行列の場合)
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力複素行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int DiagdfCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat, int g , int r) ;

// ---------------------------------------------------------------------
/*
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
int ConjTransdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,int g,int r) ;
// ---------------------------------------------------------------------
/*
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
int ConjTransdfCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r) ;
// ---------------------------------------------------------------------

/*
 \brief \~English It makes unit matrix.
 \brief \~Japanese 単位行列の作成
 \param g \~English It's the complex matrix struct
 \~Japanese 出力複素数行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int EyedCompMat1(MICARY_dComplexArray *pcaOutMat,int g,int r);

int EyedCompMat1v2(MICARY_dComplexArray *caOutMat,int g,int r) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It transrates complex matrix.
 \brief \~Japanese 複素数の転置
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese 入力複素行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/

int TransdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,int g,int r) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It adds complex matrix.
 \brief \~Japanese 複素行列のたし算
 \param pcaOutMat1 \~English It's the complex array struct.
 \~Japanese 出力複素行列1
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/

int SumdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It adds complex matrix.
 \brief \~Japanese 複素行列のたし算
 \param pcaOutMat1 \~English It's the complex array struct.
 \~Japanese 出力複素行列1
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/

int SumffdCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2) ;

// ---------------------------------------------------------------------
/*
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
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/

int SubdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
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
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/

int SubfdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It multiplicates complex matrix.
 \brief \~Japanese 複素行列の定数倍
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese 入力複素行列
 \param a \~English It's the coeficient value.
 \~Japanese 係数値
 
*/

int MultipldCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,double a) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It multiplicates complex matrix[float * double].
 \brief \~Japanese 複素行列の定数倍
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese 入力複素行列
 \param a \~English It's the coeficient value.
 \~Japanese 係数値
 
*/
int MultiplfdCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,double a) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It computes a dot product of a conjugated vector with another vector(0:false 1:true).
 \brief \~Japanese dot掛け算(0:エラー 1:正常終了)
 \param pcaOut \~English It's the complex  vector.
 \~Japanese 出力複素数ベクトル
 \param caIn1 \~English It's the input vector1.
 \~Japanese 入力複素数ベクトル1
 \param caIn2 \~English It's the input vector2.
 \~Japanese 入力複素数ベクトル2
*/
int DotcdCompMat1(MICARY_dComplexArray *pcaOut ,MICARY_dComplexArray caIn1 ,MICARY_dComplexArray caIn2) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It calculates abs complex matrix.
 \brief \~Japanese 行列の絶対値の計算
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese 出力複素数行列
 \param caInMat \~English It's the complex  matrix.
 \~Japanese 入力複素数行列
*/
int AbsdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It calculates abs complex matrix.
 \brief \~Japanese 行列の絶対値の計算
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese 出力複素数行列
 \param caInMat \~English It's the complex  matrix.
 \~Japanese 入力複素数行列
*/
int AbsdfCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It calculates max value.
 \brief \~Japanese 行列(real part)の最大値の計算
 \param caInMat \~English It's the complex  matrix.
 \~Japanese 入力複素数行列
*/
double MaxdCompMat1(MICARY_dComplexArray caInMat) ;

/*
 \brief \~English It devids complex matrix.
 \brief \~Japanese 複素行列の割り算
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力複素行列1の列サイズ
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese 入力複素行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力複素行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力複素行列2の列サイズ
 
*/
int DividedCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2) ;

/*
 \brief \~English It devids complex matrix.
 \brief \~Japanese 複素行列の平方根
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese 出力複素行列
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力複素行列
*/
int SqrtdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1) ;

#ifdef __cplusplus
}
#endif
#endif


