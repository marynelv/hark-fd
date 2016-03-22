// ---------------------------------------------------------------------
/*
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_MathMat2.h
 \brief	\~English Header file for matrix operation. (GHDSS System)
 \brief	\~Japanese �s�񉉎Z�̂��߂̃w�b�_�t�@�C��(GHDSS System)
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
 \brief \~Japanese ���f�s��̊|���Z
 \param pcaOutMat \~English It's output matrix struct.
 \~Japanese �o�͕��f�s��
 \param caInMat1 \~English It's the complex matrix struct.
 \~Japanese ���͕��f�s��1
 \param caInMat2 \~English It's the complex matrix struct.
 \~Japanese ���͕��f�s��2
*/
int ProductCompMat(MICARY_ComplexArray2 *pcaOutMat,MICARY_ComplexArray2 caInMat1,
								 MICARY_ComplexArray2 caInMat2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It products float matrix.
 \brief \~Japanese �s��̊|���Z
 \param pcaOutMat \~English It's the float matrix struct.
 \~Japanese �o�͍s��
 \param caInMat1 \~English It's the float matrix struct.
 \~Japanese ���͍s��1
 \param caInMat2 \~English It's the float matrix struct.
 \~Japanese ���͍s��2
*/
int ProductFloatMat(MICARY_FloatArray2 *pcaOutMat,MICARY_FloatArray2 caInMat1,
								MICARY_FloatArray2 caInMat2);

// ---------------------------------------------------------------------
/*
 \brief \~English It conjugattes complex matrix.(0:false 1:true)
 \brief \~Japanese ���𕡑f��(0:�G���[�@1:����I��)
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese �o�͕��f�s��
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese ���͕��f�s��
*/
int ConjCompMat(MICARY_ComplexArray2 *pcaOutMat ,MICARY_ComplexArray2 caInMat);

// ---------------------------------------------------------------------
/*
 \brief \~English It makes diagonal complex matrix.
 \brief \~Japanese �Ίp�s��(�����s��̏ꍇ)
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese �o�͕��f�s��
   \param caInMat \~English It's the complex matrix struct.
 \~Japanese ���͕��f�s��
*/
int DiagCompMat(MICARY_ComplexArray2 *pcaOutMat,MICARY_ComplexArray2 caInMat) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It makes diagonal float matrix.
 \brief \~Japanese �Ίp�s��(�����s��̏ꍇ)
 \param pcaOutMat \~English It's the float matrix struct.
 \~Japanese �o�͍s��
 \param caInMat \~English It's the float matrix struct.
 \~Japanese ���͍s��
*/
int DiagFloatMat(MICARY_FloatArray2 *pcaOutMat,MICARY_FloatArray2 caInMat) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It products complex matrix.
 \brief \~Japanese ���f�s��̊|���Z
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͕��f�s��1�̗�T�C�Y
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese ���͕��f�s��2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
 
*/
int ProductCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It products float matrix.
 \brief \~Japanese �s��̊|���Z
 \param pcaOutMat \~English It's the float array struct.
 \~Japanese �o�͍s��
 \param caInMat1 \~English It's the float array struct.
 \~Japanese ���͍s��1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͍s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͍s��1�̗�T�C�Y
 \param caInMat2 \~English It's the float array struct.
 \~Japanese ���͍s��2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese ���͍s��2�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese ���͍s��2�̗�T�C�Y
 
*/
int ProductFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat1,int g1,int r1,
								MICARY_FloatArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It conjugates complex matrix.
 \brief \~Japanese ���𕡑f��
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese �o�͕��f�s��
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese ���͕��f�s��
*/
int ConjCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat);

// ---------------------------------------------------------------------
/*
 \brief \~English It makes diagonal complex matrix.
 \brief \~Japanese �Ίp�s��(�����s��̏ꍇ)
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese �o�͕��f�s��
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͕��f�s��
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int DiagCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat, int g , int r) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It makes diagonal float matrix.
 \brief \~Japanese �Ίp�s��(�����s��̏ꍇ)
 \param pcaOutMat \~English It's the float matrix struct.
 \~Japanese �o�͍s��
   \param caInMat \~English It's the float matrix struct.
 \~Japanese ���͍s��
*/
int DiagFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat,int g,int r);
// ---------------------------------------------------------------------
/*
 \brief \~English It translates conjugate complex matrix.
 \brief \~Japanese ����]�u���f��
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese �o�͕��f�s��
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese ���͕��f�s��
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int ConjTransCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It makes unit matrix.
 \brief \~Japanese �P�ʍs��̍쐬
 \param g \~English It's the complex matrix struct
 \~Japanese �o�͕��f���s��
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int EyeCompMat1(MICARY_ComplexArray *pcaOutMat,int g,int r);

int EyeCompMat1v2(MICARY_ComplexArray *caOutMat,int g,int r) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It transrates complex matrix.
 \brief \~Japanese ���f���̓]�u
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese �o�͕��f�s��
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese ���͕��f�s��
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/

int TransCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It adds complex matrix.
 \brief \~Japanese ���f�s��̂����Z
 \param pcaOutMat1 \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��1
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese ���͕��f�s��1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͕��f�s��1�̗�T�C�Y
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese ���͕��f�s��2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
 
*/

int SumCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_ComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It adds float matrix.
 \brief \~Japanese �s��̂����Z
 \param pcaOutMat \~English It's the float array struct.
 \~Japanese �o�͍s��
 \param caInMat1 \~English It's the float array struct.
 \~Japanese ���͍s��1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͍s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͍s��1�̗�T�C�Y
 \param caInMat2 \~English It's the float array struct.
 \~Japanese ���͍s��2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese ���͍s��2�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese ���͍s��2�̗�T�C�Y
 
*/

int SumFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat1,int g1,int r1,
									MICARY_FloatArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It substracts complex matrix.
 \brief \~Japanese ���f�s��̂Ђ��Z
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese ���͕��f�s��1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͕��f�s��1�̗�T�C�Y
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese ���͕��f�s��2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
 
*/

int SubCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_ComplexArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It subtracts float matrix.
 \brief \~Japanese �s��̂Ђ��Z
 \param pcaOutMat \~English It's the float array struct.
 \~Japanese �o�͍s��
 \param caInMat1 \~English It's the float array struct.
 \~Japanese ���͍s��1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͍s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͍s��1�̗�T�C�Y
 \param caInMat2 \~English It's the float array struct.
 \~Japanese ���͍s��2
 \param g1 \~English It's the number of row for input matrix2.
 \~Japanese ���͍s��2�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix2.
 \~Japanese ���͍s��2�̗�T�C�Y
 
*/

int SubFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat1,int g1,int r1,
									MICARY_FloatArray caInMat2,int g2,int r2) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It multiplicates complex matrix.
 \brief \~Japanese ���f�s��̒萔�{
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese ���͕��f�s��
 \param a \~English It's the coeficient value.
 \~Japanese �W���l
 
*/

int MultiplCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,float a) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It multiplicates float matrix.
 \brief \~Japanese �s��z��̒萔�{
 \param pcaOutMat \~English It's the float array struct.
 \~Japanese �o�͍s��
 \param caInMat \~English It's the float array struct.
 \~Japanese ���͍s��
 \param a \~English It's the coefficient value.
 \~Japanese �W���l
 
*/

int MultiplFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat,float a) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It makes unit float matrix.
 \brief \~Japanese �P�ʍs��̍쐬
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese �o�͕��f���s��
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int EyeFloatMat1(MICARY_FloatArray *pcaOutMat ,int g,int r) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It computes a dot product of a conjugated vector with another vector(0:false 1:true).
 \brief \~Japanese dot�|���Z(0:�G���[ 1:����I��)
 \param pcaOut \~English It's the complex  vector.
 \~Japanese �o�͕��f���x�N�g��
 \param caIn1 \~English It's the input vector1.
 \~Japanese ���͕��f���x�N�g��1
 \param caIn2 \~English It's the input vector2.
 \~Japanese ���͕��f���x�N�g��2
*/
int DotcCompMat1(MICARY_ComplexArray *pcaOut ,MICARY_ComplexArray caIn1 ,MICARY_ComplexArray caIn2) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It calculates abs complex matrix.
 \brief \~Japanese �s��̐�Βl�̌v�Z
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese �o�͕��f���s��
 \param caInMat \~English It's the complex  matrix.
 \~Japanese ���͕��f���s��
*/
int AbsCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It calculates max value.
 \brief \~Japanese �s��(real part)�̍ő�l�̌v�Z
 \param caInMat \~English It's the complex  matrix.
 \~Japanese ���͕��f���s��
*/
HA_Float MaxCompMat1(MICARY_ComplexArray caInMat) ;

// ---------------------------------------------------------------------

#endif


