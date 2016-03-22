// ---------------------------------------------------------------------
/*
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_MathMatDouble.h
 \brief	\~English Header file for matrix operation[type double]. (GHDSS System)
 \brief	\~Japanese �s�񉉎Z�̂��߂̃w�b�_�t�@�C��(GHDSS System)
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
	DA_Complex *	da_cmpValue; ///<\~Japanese HA_Complex�z��
	HA_Num	ha_numValue; ///<\~Japanese �z��̃T�C�Y
} MICARY_dComplexArray;

/**
 Spectrum struct (Double Complex)
 \author	Sachiko Suzuki
 \date 01 Jul 2008
*/
typedef struct{
	MICARY_dComplexArray	micary_cmpaSpec; ///<\~Japanese �X�y�N�g��(Complex) 
	HA_Num	ha_numCh; ///<\~Japanese �`�����l����
	HA_Integer	ha_iLowerFreq; ///<\~Japanese �������g��[Line]
	HA_Integer	ha_iUpperFreq; ///<\~Japanese ������g��[Line]
} MICARY_dComplexSpecArray;


// ---------------------------------------------------------------------
/**
 \brief \~English It allocates complex array
 \brief \~Japanese HA_Complex�z��̗̈���m�ۂ���
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex�z����܂ލ\����
 \param iArraySize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void dComplexArrayAlloc(MICARY_dComplexArray * array, int iArraySize);

/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex�z���������� 
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex�z����܂ލ\����
*/
void dComplexArrayFree(MICARY_dComplexArray * array);

/**
 \brief \~English It initializes complex array
 \brief \~Japanese Complex�z��̏�����(0����)
 \param array \~English It's complex array
 \~Japanese Complex�z����܂ލ\����
*/

void dComplexArrayInit(MICARY_dComplexArray * array) ;

// 2008/07/01 added by S.Suzuki
/**
 \brief \~English It allocates complex spectrum array
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Complex�z��̗̈���m�ۂ���
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complex�̃X�y�N�g���p�\����
 \param numCh \~English It's the number of channel.
 \~Japanese �`�����l����
 \param iLowerIndex \~English It's the lower frequency index.
 \~Japanese �������g���C���f�b�N�X
 \param iUpperIndex \~English It's the upper frequency index.
 \~Japanese ������g���C���f�b�N�X
*/
void AllocdComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex);

/**
 \brief \~English It frees complex spectrum array
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Complex�z��̗̈���J������
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complex�̃X�y�N�g���p�\����
*/
void FreedComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec);

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
int ProductdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2) ;
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
int ProductdfCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2) ;

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
int ProductfdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2) ;

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
int ProductdffCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2) ;
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
int ProductfddCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2) ;

// ---------------------------------------------------------------------

/*
 \brief \~English It conjugates complex matrix.
 \brief \~Japanese ���𕡑f��
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese �o�͕��f�s��
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese ���͕��f�s��
*/
int ConjdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat);

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
int DiagdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat, int g , int r) ;
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
int DiagdfCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat, int g , int r) ;

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
int ConjTransdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,int g,int r) ;
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
int ConjTransdfCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r) ;
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
int EyedCompMat1(MICARY_dComplexArray *pcaOutMat,int g,int r);

int EyedCompMat1v2(MICARY_dComplexArray *caOutMat,int g,int r) ;
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

int TransdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,int g,int r) ;
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

int SumdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2) ;
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

int SumffdCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2) ;

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

int SubdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2) ;
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

int SubfdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2) ;

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

int MultipldCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,double a) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It multiplicates complex matrix[float * double].
 \brief \~Japanese ���f�s��̒萔�{
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��
 \param caInMat1 \~English It's the complex array struct.
 \~Japanese ���͕��f�s��
 \param a \~English It's the coeficient value.
 \~Japanese �W���l
 
*/
int MultiplfdCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,double a) ;

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
int DotcdCompMat1(MICARY_dComplexArray *pcaOut ,MICARY_dComplexArray caIn1 ,MICARY_dComplexArray caIn2) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It calculates abs complex matrix.
 \brief \~Japanese �s��̐�Βl�̌v�Z
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese �o�͕��f���s��
 \param caInMat \~English It's the complex  matrix.
 \~Japanese ���͕��f���s��
*/
int AbsdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat) ;
// ---------------------------------------------------------------------
/*
 \brief \~English It calculates abs complex matrix.
 \brief \~Japanese �s��̐�Βl�̌v�Z
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese �o�͕��f���s��
 \param caInMat \~English It's the complex  matrix.
 \~Japanese ���͕��f���s��
*/
int AbsdfCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat) ;

// ---------------------------------------------------------------------
/*
 \brief \~English It calculates max value.
 \brief \~Japanese �s��(real part)�̍ő�l�̌v�Z
 \param caInMat \~English It's the complex  matrix.
 \~Japanese ���͕��f���s��
*/
double MaxdCompMat1(MICARY_dComplexArray caInMat) ;

/*
 \brief \~English It devids complex matrix.
 \brief \~Japanese ���f�s��̊���Z
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
int DividedCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2) ;

/*
 \brief \~English It devids complex matrix.
 \brief \~Japanese ���f�s��̕�����
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��
*/
int SqrtdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1) ;

#ifdef __cplusplus
}
#endif
#endif


