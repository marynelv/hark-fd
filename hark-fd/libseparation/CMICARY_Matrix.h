/**
 Copyright (c) 2004-2010 Honda Research Institute Japan, Co., Ltd.

 \file CMICARY_Matrix.h
 \brief \~English Header file for matrix
 \brief \~Japanese �s�񉉎Z�w�b�_�t�@�C��(GDHSS program)
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
#include <boost/numeric/ublas/io.hpp>      // Matrix �X�g���[�����o�͗p�w�b�_
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
	DA_Complex *	da_cmpValue; ///<\~Japanese HA_Complex�z��
	HA_Num	ha_numValue; ///<\~Japanese �z��̃T�C�Y
// 2012/01/13 added
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num ha_numReserved; ///<\~Japanese �m�ۗv�f��
#endif
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
// 2012/01/13 added
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	HA_Num ha_numReservedChSize; ///<\~Japanese �m�ۗv�f�� (ch)
	HA_Num ha_numReservedFreqSize; ///<\~Japanese �m�ۗv�f�� (freq)
#endif
} MICARY_dComplexSpecArray;


// ----------- Add 2011.01.19 -----------------
/**
 \brief \~English It allocates complex array
 \brief \~Japanese HA_Complex�z��̗̈���m�ۂ���
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex�z����܂ލ\����
 \param iArraySize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void dComplexArrayAlloc(MICARY_dComplexArray * array, int iArraySize);

// 2012/01/13 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates complex array
 \brief \~Japanese HA_Complex�z��̗̈���m�ۂ���
 \param array \English It's complexArray
 \~Japanese HA_Complex�z����܂ލ\����
 \param iArraySize \~English It's the size of array in use
 \~Japanese �g�p�v�f��
 \param iReservedSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void dComplexArrayAlloc_r(MICARY_dComplexArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex�z���������� 
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex�z����܂ލ\����
*/
void dComplexArrayFree(MICARY_dComplexArray * array);

// 2012/01/13 added
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex�z��̎g�p�T�C�Y���N���A���� 
 \param array \~English ComplexArray
 \~Japanese HA_Complex�z����܂ލ\����
*/
void dComplexArrayClear(MICARY_dComplexArray * array);
#endif

	
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
void AllocdComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec,HA_Num numCh, HA_Integer iLowerIndex, HA_Integer iUpperIndex);

// 2012/01/13 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
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
 \param iReservedChSize \~English It's the size of array
 \~Japanese �z��̊m�ی�(ch)
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese �z��̊m�ی�(Freq)
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
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Complex�z��̗̈���J������
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complex�̃X�y�N�g���p�\����
*/
void FreedComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec);

// 2012/01/13 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex spectrum array
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Complex�z��̎g�p�T�C�Y�N���A
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complex�̃X�y�N�g���p�\����
*/
void CleardComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec);
#endif


// ****************** boost/numeric/ublas/ ���g�p���Ȃ� *******************
// -----  double  ------

/**
 \brief \~English It products complex matrix.(double)
 \brief \~Japanese ���f�s��̊|���Z(double)
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��
 \param caInMat1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͕��f�s��1�̗�T�C�Y
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese ���͕��f�s��2
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
*/
int MICARY_Product_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It devids complex matrix.
 \brief \~Japanese ���f�s��̊���Z
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��
 \param caInMat1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͕��f�s��1�̗�T�C�Y
 \param caInMat2 \~English It's the complex array struct.
 \~Japanese ���͕��f�s��2
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
*/
int MICARY_Divide_CompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It adds complex matrix.
 \brief \~Japanese ���f�s��̂����Z
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
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
*/
int MICARY_Sum_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2);

/**
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
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
*/
int MICARY_Sub_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2);


/**
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
int MICARY_ConjTrans_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,int g,int r);

/**
 \brief \~English It makes diagonal complex matrix.
 \brief \~Japanese �Ίp�s��(�����s��̏ꍇ)
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese �o�͕��f�s��
 \param caInMat \~English It's the number of row for input matrix.
 \~Japanese ���͕��f�s��
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int MICARY_Diag_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat, int g , int r);

/**
 \brief \~English It conjugates complex matrix.
 \brief \~Japanese ���𕡑f��
 \param pcaOutMat \~English It's the complex matrix struct.
 \~Japanese �o�͕��f�s��
 \param caInMat \~English It's the complex matrix struct.
 \~Japanese ���͕��f�s��
*/
int MICARY_Conj_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat);

/**
 \brief \~English It computes a dot product of a conjugated vector with another vector(0:false 1:true).
 \brief \~Japanese dot�|���Z(0:�G���[ 1:����I��)
 \param pcaOut \~English It's the complex  vector.
 \~Japanese �o�͕��f���x�N�g��
 \param caIn1 \~English It's the input vector1.
 \~Japanese ���͕��f���x�N�g��1
 \param caIn2 \~English It's the input vector2.
 \~Japanese ���͕��f���x�N�g��2
*/
int MICARY_Dot_CompMat(MICARY_dComplexArray *pcaOut, MICARY_dComplexArray caIn1, MICARY_dComplexArray caIn2);

/**
 \brief \~English It makes unit matrix.
 \brief \~Japanese �P�ʍs��̍쐬
 \param pcaOutMat \~English It's the complex matrix struct
 \~Japanese �o�͕��f���s��
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int MICARY_Eye_CompMat(MICARY_dComplexArray *pcaOutMat,int g,int r);


/**
 \brief \~English It multiplicates complex matrix.
 \brief \~Japanese ���f�s��̒萔�{
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��
 \param caInMat \~English It's the complex array struct.
 \~Japanese ���͕��f�s��
 \param a \~English It's the coeficient value.
 \~Japanese �W���l
*/
int MICARY_Multipl_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat, double a);

/*
 \brief \~English It devids complex matrix.
 \brief \~Japanese ���f�s��̕�����
 \param pcaOutMat \~English It's the complex array struct.
 \~Japanese �o�͕��f�s��
 \param caInMat1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��
*/
int MICARY_Sqrt_CompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1) ;


/**
 \brief \~English It calculates abs complex matrix.(double)
 \brief \~Japanese �s��̐�Βl�̌v�Z(double)
 \param pcaOutMat \~English It's the complex  matrix.
 \~Japanese �o�͕��f���s��
 \param caInMat \~English It's the complex  matrix.
 \~Japanese ���͕��f���s��
*/
int MICARY_Abs_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat) ;


// -----  float  ------

/**
 \brief \~English It substracts complex matrix.
 \brief \~Japanese ���f�s��̂Ђ��Z
 \param pcaOutMat \~English It's the complex array struct.(float)
 \~Japanese �o�͕��f�s��(float)
 \param caInMat1 \~English It's the complex array struct.(float)
 \~Japanese ���͕��f�s��1(float)
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͕��f�s��1�̗�T�C�Y
 \param caInMat2 \~English It's the complex array struct.(float)
 \~Japanese ���͕��f�s��2(float)
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
*/
int MICARY_Sub_fCompMat(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
										MICARY_ComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It products complex matrix.
 \brief \~Japanese ���f�s��̊|���Z
 \param pcaOutMat \~English It's the complex array struct.(double)
 \~Japanese �o�͕��f�s��(double)
 \param caInMat1 \~English It's the number of row for input matrix1.(float)
 \~Japanese ���͕��f�s��1(float)
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͕��f�s��1�̗�T�C�Y
 \param caInMat2 \~English It's the complex array struct.(double)
 \~Japanese ���͕��f�s��2(double)
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
*/
int MICARY_Product_fdCompMat(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It products complex matrix.
 \brief \~Japanese ���f�s��̊|���Z
 \param pcaOutMat \~English It's the complex array struct.(double)
 \~Japanese �o�͕��f�s��(double)
 \param caInMat1 \~English It's the number of row for input matrix1.(double)
 \~Japanese ���͕��f�s��1(double)
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͕��f�s��1�̗�T�C�Y
 \param caInMat2 \~English It's the complex array struct.(float)
 \~Japanese ���͕��f�s��2(float)
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
*/
int MICARY_Product_dfCompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2);


//int MICARY_Product_ddfCompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
//									MICARY_ComplexArray caInMat2,int g2,int r2);

/**
 \brief \~English It products complex matrix.
 \brief \~Japanese ���f�s��̊|���Z
 \param pcaOutMat \~English It's the complex array struct.(float)
 \~Japanese �o�͕��f�s��(float)
 \param caInMat1 \~English It's the number of row for input matrix1.(float)
 \~Japanese ���͕��f�s��1(float)
 \param g1 \~English It's the number of row for input matrix1.
 \~Japanese ���͕��f�s��1�̍s�T�C�Y
 \param r1 \~English It's the number of column for input matrix1.
 \~Japanese ���͕��f�s��1�̗�T�C�Y
 \param caInMat2 \~English It's the complex array struct.(float)
 \~Japanese ���͕��f�s��2(float)
 \param g2 \~English It's the number of row for input matrix2.
 \~Japanese ���͕��f�s��2�̍s�T�C�Y
 \param r2 \~English It's the number of column for input matrix2.
 \~Japanese ���͕��f�s��2�̗�T�C�Y
*/
int MICARY_Product_fCompMat(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2);


/**
 \brief \~English It translates conjugate complex matrix.
 \brief \~Japanese ����]�u���f��
 \param pcaOutMat \~English It's the complex matrix struct.(double)
 \~Japanese �o�͕��f�s��(double)
 \param caInMat \~English It's the complex matrix struct.(float)
 \~Japanese ���͕��f�s��(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int MICARY_ConjTrans_dfCompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r);


/**
 \brief \~English It translates conjugate complex matrix.
 \brief \~Japanese ����]�u���f��
 \param pcaOutMat \~English It's the complex matrix struct.(float)
 \~Japanese �o�͕��f�s��(float)
 \param caInMat \~English It's the complex matrix struct.(float)
 \~Japanese ���͕��f�s��(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int MICARY_ConjTrans_fCompMat(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r);

/**
 \brief \~English It makes diagonal complex matrix.
 \brief \~Japanese �Ίp�s��(�����s��̏ꍇ)
 \param pcaOutMat \~English It's the complex matrix struct.(float)
 \~Japanese �o�͕��f�s��(float)
 \param caInMat \~English It's the number of row for input matrix.(float)
 \~Japanese ���͕��f�s��(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int MICARY_Diag_fCompMat(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat, int g , int r);


/**
 \brief \~English It multiplicates complex matrix.
 \brief \~Japanese ���f�s��̒萔�{
 \param pcaOutMat \~English It's the complex array struct.(float)
 \~Japanese �o�͕��f�s��(float)
 \param caInMat \~English It's the complex array struct.(float)
 \~Japanese ���͕��f�s��(float)
 \param a \~English It's the coeficient value.
 \~Japanese �W���l
*/
int MICARY_Multipl_fCompMat(MICARY_ComplexArray *pcaOutMat, MICARY_ComplexArray caInMat, float a);

/**
 \brief \~English It conjugates complex matrix.
 \brief \~Japanese ���𕡑f��
 \param pcaOutMat \~English It's the complex matrix struct.(float)
 \~Japanese �o�͕��f�s��(float)
 \param caInMat \~English It's the complex matrix struct.(float)
 \~Japanese ���͕��f�s��(float)
*/
int MICARY_Conj_fCompMat(MICARY_ComplexArray *pcaOutMat, MICARY_ComplexArray caInMat);

/**
 \brief \~English It makes unit matrix.
 \brief \~Japanese �P�ʍs��̍쐬
 \param pcaOutMat \~English It's the complex matrix struct
 \~Japanese �o�͕��f���s��(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int MICARY_Eye_fCompMat(MICARY_ComplexArray *pcaOutMat,int g,int r);

/**
 \brief \~English It transrates complex matrix.
 \brief \~Japanese ���f���̓]�u
 \param pcaOutMat \~English It's the complex matrix struct.(float)
 \~Japanese �o�͕��f�s��(float)
 \param caInMat \~English It's the complex matrix struct.(float)
 \~Japanese ���͕��f�s��(float)
 \param g \~English It's the number of row for input matrix.
 \~Japanese ���͍s��̍s�T�C�Y
 \param r \~English It's the number of column for input matrix.
 \~Japanese ���͍s��̗�T�C�Y
*/
int MICARY_Trans_fCompMat(MICARY_ComplexArray *pcaOutMat, MICARY_ComplexArray caInMat,int g,int r);

/**
 \brief \~English It calculates abs complex matrix.
 \brief \~Japanese �s��̐�Βl�̌v�Z
 \param pcaOutMat \~English It's the complex  matrix.(float)
 \~Japanese �o�͕��f���s��(float)
 \param caInMat \~English It's the complex  matrix.(float)
 \~Japanese ���͕��f���s��(float)
*/
int MICARY_Abs_dfCompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat); 

/**
 \brief \~English It calculates max value.
 \brief \~Japanese �s��(real part)�̍ő�l�̌v�Z
 \param caInMat \~English It's the complex  matrix.
 \~Japanese ���͕��f���s��
*/
double MICARY_Max_CompMat(MICARY_dComplexArray caInMat);



//   ******************* boost/numeric/ublas/ ���g�p *******************
// 2011.02.16���_�ł͖��g�p

bool _InitCompMat(cdMat *pcaOutMat);

bool _DiagCompMat(cdMat *pcaOutMat, cdMat caInMat);

bool _EyeCompMat(cdMat *pcaOutMat);

bool _AbsCompMat(cdMat *pcaOutMat ,cdMat caInMat);


double _MaxCompMat(cdMat caInMat);

bool _SqrtCompMat(cdMat *pcaOutMat ,cdMat caInMat);

bool _MultiplCompMat(cdMat *pcaOutMat ,cdMat caInMat, double a);

//--- uBlas::matrix�^�ւ̕ϊ� ---
bool _Convert_ComplexSpec(cdMat *pcaOutMat, MICARY_ComplexArray caIn, int Nrow, int Ncol, int iFreqN);

bool _Convert_Complex_InputSpec(cdMat *pcaOutMat ,MICARY_ComplexArray caIn, int Nfreq, int iFreq);

bool _Convert_dComplexSpec(cdMat *pcaOutMat ,MICARY_dComplexArray caIn,   int Nrow, int Nmic, int iFreqN);
// ---

//--- uBlas::matrix�^����̕ϊ�
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