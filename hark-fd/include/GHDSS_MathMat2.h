// ---------------------------------------------------------------------
/*
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_MathMat2.h
 \brief	\~English Header file for matrix operation. (GHDSS System)
 \brief	\~Japanese 行列演算のためのヘッダファイル(GHDSS System)
 \author	Sachiko Suzuki
 \date	23 Oct 2007
 \version	$Id$
*/

// ---------------------------------------------------------------------
#ifndef GHDSS_MathMat2_h_defined
#define GHDSS_MathMat2_h_defined
// ---------------------------------------------------------------------
#include "MICARY_struct.h"
#include "MICARY_GHDSS_struct.h"

// ---------------------------------------------------------------------

/*
 \brief \~English It products complex matrix.
 \brief \~Japanese 複素行列の掛け算
 \param pcaOutMat \~English It's output matrix struct.
 \~Japanese 出力複素行列
 \param caInMat1 \~English It's the complex matrix struct.
 \~Japanese 入力複素行列1
 \param caInMat2 \~English It's the complex matrix struct.
 \~Japanese 入力複素行列2
*/
int ProductCompMat(MICARY_ComplexArray2 *pcaOutMat,MICARY_ComplexArray2 caInMat1,
								 MICARY_ComplexArray2 caInMat2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It products float matrix.
 \brief \~Japanese 行列の掛け算
 \param pcaOutMat \~English It's the float matrix struct.
 \~Japanese 出力行列
 \param caInMat1 \~English It's the float matrix struct.
 \~Japanese 入力行列1
 \param caInMat2 \~English It's the float matrix struct.
 \~Japanese 入力行列2
*/
int ProductFloatMat(MICARY_FloatArray2 *pcaOutMat,MICARY_FloatArray2 caInMat1,
								MICARY_FloatArray2 caInMat2);

// ---------------------------------------------------------------------
/*
 \brief \~English It conjugattes complex matrix.(0:false 1:true)
 \brief \~Japanese 共役複素数(0:エラー　1:正常終了)
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese 入力複素行列
*/
int ConjCompMat(MICARY_ComplexArray2 *pcaOutMat ,MICARY_ComplexArray2 caInMat);

// ---------------------------------------------------------------------
/*
 \brief \~English It makes diagonal complex matrix.
 \brief \~Japanese 対角行列(正方行列の場合)
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
   \param caInMat \~English It's the complex matrix struct.
 \~Japanese 入力複素行列
*/
int DiagCompMat(MICARY_ComplexArray2 *pcaOutMat,MICARY_ComplexArray2 caInMat) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It makes diagonal float matrix.
 \brief \~Japanese 対角行列(正方行列の場合)
 \param pcaOutMat \~English It's the float matrix struct.
 \~Japanese 出力行列
 \param caInMat \~English It's the float matrix struct.
 \~Japanese 入力行列
*/
int DiagFloatMat(MICARY_FloatArray2 *pcaOutMat,MICARY_FloatArray2 caInMat) ;
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
int ProductCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It products float matrix.
 \brief \~Japanese 行列の掛け算
 \param pcaOutMat \~English It's the float array struct.
 \~Japanese 出力行列
 \param caInMat1 \~English It's the float array struct.
 \~Japanese 入力行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力行列1の列サイズ
 \param caInMat2 \~English It's the float array struct.
 \~Japanese 入力行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力行列2の列サイズ
 
*/
int ProductFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat1,int g1,int r1,
								MICARY_FloatArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It conjugates complex matrix.
 \brief \~Japanese 共役複素数
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese 出力複素行列
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese 入力複素行列
*/
int ConjCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat);

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
int DiagCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat, int g , int r) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It makes diagonal float matrix.
 \brief \~Japanese 対角行列(正方行列の場合)
 \param pcaOutMat \~English It's the float matrix struct.
 \~Japanese 出力行列
   \param caInMat \~English It's the float matrix struct.
 \~Japanese 入力行列
*/
int DiagFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat,int g,int r);
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
int ConjTransCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r) ;
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
int EyeCompMat1(MICARY_ComplexArray *pcaOutMat,int g,int r);

int EyeCompMat1v2(MICARY_ComplexArray *caOutMat,int g,int r) ;
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

int TransCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r) ;
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

int SumCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_ComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It adds float matrix.
 \brief \~Japanese 行列のたし算
 \param pcaOutMat \~English It's the float array struct.
 \~Japanese 出力行列
 \param caInMat1 \~English It's the float array struct.
 \~Japanese 入力行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力行列1の列サイズ
 \param caInMat2 \~English It's the float array struct.
 \~Japanese 入力行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力行列2の列サイズ
 
*/

int SumFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat1,int g1,int r1,
									MICARY_FloatArray caInMat2,int g2,int r2) ;
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

int SubCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_ComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It subtracts float matrix.
 \brief \~Japanese 行列のひき算
 \param pcaOutMat \~English It's the float array struct.
 \~Japanese 出力行列
 \param caInMat1 \~English It's the float array struct.
 \~Japanese 入力行列1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese 入力行列1の行サイズ
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese 入力行列1の列サイズ
 \param caInMat2 \~English It's the float array struct.
 \~Japanese 入力行列2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese 入力行列2の行サイズ
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese 入力行列2の列サイズ
 
*/

int SubFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat1,int g1,int r1,
									MICARY_FloatArray caInMat2,int g2,int r2) ;
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

int MultiplCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,float a) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It multiplicates float matrix.
 \brief \~Japanese 行列配列の定数倍
 \param pcaOutMat \~English It's the float array struct.
 \~Japanese 出力行列
 \param caInMat \~English It's the float array struct.
 \~Japanese 入力行列
 \param a \~English It's the coefficient value.
 \~Japanese 係数値
 
*/

int MultiplFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat,float a) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It makes unit float matrix.
 \brief \~Japanese 単位行列の作成
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese 出力複素数行列
 \param g \~English It's the number of row for input matrix.
 \~Japanese 入力行列の行サイズ
 \param r \~English It's the number of column for input matrix.
 \~Japanese 入力行列の列サイズ
*/
int EyeFloatMat1(MICARY_FloatArray *pcaOutMat ,int g,int r) ;

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
int DotcCompMat1(MICARY_ComplexArray *pcaOut ,MICARY_ComplexArray caIn1 ,MICARY_ComplexArray caIn2) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It calculates abs complex matrix.
 \brief \~Japanese 行列の絶対値の計算
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese 出力複素数行列
 \param caInMat \~English It's the complex  matrix.
 \~Japanese 入力複素数行列
*/
int AbsCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It calculates max value.
 \brief \~Japanese 行列(real part)の最大値の計算
 \param caInMat \~English It's the complex  matrix.
 \~Japanese 入力複素数行列
*/
HA_Float MaxCompMat1(MICARY_ComplexArray caInMat) ;

// ---------------------------------------------------------------------

#endif


