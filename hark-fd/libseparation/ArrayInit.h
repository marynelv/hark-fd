// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file ArrayInit.h
 \brief \~English Header file for initializing arrays (Microphone Array System)
 \brief \~Japanese �z����܂ލ\���̂�����������w�b�_�t�@�C��(Microphone Array System)
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

// 2007/06/11 added by N.Tanaka C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif

/**
 \brief \~English It allocates float array
 \brief \~Japanese HA_Float�z��̗̈���m�ۂ���
 \param array \English It's floatArray
 \~Japanese HA_Float�z����܂ލ\����
 \param iArraySize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void FloatArrayAlloc(MICARY_FloatArray * array, int iArraySize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates float array
 \brief \~Japanese HA_Float�z��̗̈���m�ۂ���
 \param array \English It's floatArray
 \~Japanese HA_Float�z����܂ލ\����
 \param iArraySize \~English It's the size of array in use
 \~Japanese �g�p�v�f��
 \param iReservedSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void FloatArrayAlloc_r(MICARY_FloatArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief \~English It frees float array
 \brief \~Japanese HA_Float�z���������� 
 \param array \~English FloatArray
 \~Japanese HA_Float�z����܂ލ\����
*/
void FloatArrayFree(MICARY_FloatArray * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees float array
 \brief \~Japanese HA_Float�z��̎g�p�T�C�Y���N���A���� 
 \param array \~English FloatArray
 \~Japanese HA_Float�z����܂ލ\����
*/
void FloatArrayClear(MICARY_FloatArray * array);
#endif

/**
 \brief \~English It allocates int array
 \brief \~Japanese HA_Integer�z��̗̈���m�ۂ���
 \param array \~English It's intArray
 \~Japanese HA_Integer�z����܂ލ\����
 \param iArraySize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void IntArrayAlloc(MICARY_IntArray * array, int iArraySize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates int array
 \brief \~Japanese HA_Int�z��̗̈���m�ۂ���
 \param array \English It's intArray
 \~Japanese HA_Float�z����܂ލ\����
 \param iArraySize \~English It's the size of array in use
 \~Japanese �g�p�v�f��
 \param iReservedSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void IntArrayAlloc_r(MICARY_IntArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief It frees int array
 \brief \~Japanese HA_Integer�z���������� 
 \param array \~English IntArray
 \~Japanese HA_Integer�z����܂ލ\����
*/
void IntArrayFree(MICARY_IntArray * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees int array
 \brief \~Japanese HA_Int�z��̎g�p�T�C�Y���N���A���� 
 \param array \~English IntArray
 \~Japanese HA_Int�z����܂ލ\����
*/
void IntArrayClear(MICARY_IntArray * array);
#endif

/**
 \brief \~English It allocates complex array
 \brief \~Japanese HA_Complex�z��̗̈���m�ۂ���
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex�z����܂ލ\����
 \param iArraySize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void ComplexArrayAlloc(MICARY_ComplexArray * array, int iArraySize);
// 2011/11/16 added by N.Tanaka
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
void ComplexArrayAlloc_r(MICARY_ComplexArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex�z���������� 
 \param array \~English It's ComplexArray
 \~Japanese HA_Complex�z����܂ލ\����
*/
void ComplexArrayFree(MICARY_ComplexArray * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex array
 \brief \~Japanese HA_Complex�z��̎g�p�T�C�Y���N���A���� 
 \param array \~English ComplexArray
 \~Japanese HA_Complex�z����܂ލ\����
*/
void ComplexArrayClear(MICARY_ComplexArray * array);
#endif

/**
 \brief \~English It allocates location array
 \brief \~Japanese MICARY_Location�z����m�ۂ���
 \param array \~English It's LocationArray
 \~Japanese MICARY_Location�z����܂ލ\����
 \param iArraySize It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void LocationArrayAlloc(MICARY_LocationArray * array, int iArraySize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates location array
 \brief \~Japanese MICARY_Location�z����m�ۂ���
 \param array \~English It's LocationArray
 \~Japanese MICARY_Location�z����܂ލ\����
 \param iArraySize \~English It's the size of array in use
 \~Japanese �g�p�v�f��
 \param iReservedSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void LocationArrayAlloc_r(MICARY_LocationArray * array, int iArraySize, int iReservedSize);
#endif

/**
 \brief It frees location array
 \brief \~Japanese MICARY_Location�z����������
 \param array It's LocationArray
 \~Japanese MICARY_Location�z����܂ލ\����
*/
void LocationArrayFree(MICARY_LocationArray * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees location array
 \brief \~Japanese MICARY_Location�z��̎g�p�T�C�Y���N���A���� 
 \param array \~English LocationArray
 \~Japanese MICARY_Location�z����܂ލ\����
*/
void LocationArrayClear(MICARY_LocationArray * array);
#endif

/**
 \brief \~English It allocates database of beamforming
 \brief \~Japanese BF�W�����i�[����z����m�ۂ���
 \param BFD \~English It's Database of beamforming
 \~Japanese BF�W�����i�[����z����܂ލ\����\n
 [�����ʒu[�}�C�N���z���ʒu��[���g�����C����]]�̏��Ɋi�[����\n
 ��(�����ʒu��s, �}�C�N�ʒu��m, ���g�����C����f�̏ꍇ):\n
 BF(S1M1F1), BF(S1M1F2), ..., BF(S1M1Ff), BF(S1M2F1), ..., BF(S1MmFf), BF(S2M1F1), ..., BF(SsMmFf)
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese BF�W���̎��g�����C����
 \param iLocSize \~English It's the number of location
 \~Japanese BF�W���̉����ʒu��
 \param iMicSize \~English It's the number of microphone
 \~Japanese BF�W���̃}�C�N���z���ʒu��
*/
void BFDatabaseAlloc(MICARY_BFDatabase * BFD, int iFreqSize, int iLocSize, int iMicSize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates database of beamforming
 \brief \~Japanese BF�W�����i�[����z����m�ۂ���
 \param BFD \~English It's Database of beamforming
 \~Japanese BF�W�����i�[����z����܂ލ\����\n
 [�����ʒu[�}�C�N���z���ʒu��[���g�����C����]]�̏��Ɋi�[����\n
 ��(�����ʒu��s, �}�C�N�ʒu��m, ���g�����C����f�̏ꍇ):\n
 BF(S1M1F1), BF(S1M1F2), ..., BF(S1M1Ff), BF(S1M2F1), ..., BF(S1MmFf), BF(S2M1F1), ..., BF(SsMmFf)
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese BF�W���̎��g�����C����
 \param iLocSize \~English It's the number of location
 \~Japanese BF�W���̉����ʒu��
 \param iMicSize \~English It's the number of microphone
 \~Japanese BF�W���̃}�C�N���z���ʒu��
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Mic)
*/
void BFDatabaseAlloc_r(MICARY_BFDatabase * BFD, int iFreqSize, int iLocSize, int iMicSize, 
											  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize);
#endif

/**
 \brief \~English It frees database of beamforming
 \brief \~Japanese BF�W�����i�[����z����J������
 \param BFD \English It's Database of beamforming
 \~Japanese BF�W�����i�[����z����܂ލ\����\n
*/
void BFDatabaseFree(MICARY_BFDatabase * BFD);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees database of beamforming
 \brief \~Japanese BF�W�����i�[����z��̎g�p�T�C�Y���N���A���� 
 \param BFD \English It's Database of beamforming
 \~Japanese BF�W�����i�[����z����܂ލ\����\n
*/
void BFDatabaseClear(MICARY_BFDatabase * BFD);
#endif

/**
 \brief \~English It allocates database of transfer function
 \brief \~Japanese �`�B�֐����i�[����z����m�ۂ���
 \param TFD \~English It's Database of transfer function
 \~Japanese �`�B�֐����i�[����z����܂ލ\����
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese �`�B�֐��̎��g�����C����
 \param iLocSize \~English It's the number of location
 \~Japanese �`�B�֐��̉����ʒu��
 \param iMicSize \~English It's the number of microphone
 \~Japanese �`�B�֐��̃}�C�N���z���ʒu��
*/
void TFDatabaseAlloc(MICARY_TFDatabase * TFD, int iFreqSize, int iLocSize, int iMicSize);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates database of transfer function
 \brief \~Japanese �`�B�֐����i�[����z����m�ۂ���
 \param TFD \~English It's Database of transfer function
 \~Japanese �`�B�֐����i�[����z����܂ލ\����
 \param iFreqSize \~English It's the number of frequency Line
 \~Japanese �`�B�֐��̎��g�����C����
 \param iLocSize \~English It's the number of location
 \~Japanese �`�B�֐��̉����ʒu��
 \param iMicSize \~English It's the number of microphone
 \~Japanese �`�B�֐��̃}�C�N���z���ʒu��
 \param iReservedFreqSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Freq)
 \param iReservedLocSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Location)
 \param iReservedMicSize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��(Mic)
*/
void TFDatabaseAlloc_r(MICARY_TFDatabase * TFD, int iFreqSize, int iLocSize, int iMicSize,
											  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize);
#endif

/**
 \brief \~English It frees database of transfer function
 \brief \~Japanese �`�B�֐����i�[����z����J������
 \param TFD \~English It's Database of transfer function
 \~Japanese �`�B�֐����i�[����z����܂ލ\����
*/
void TFDatabaseFree(MICARY_TFDatabase * TFD);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees database of transfer function
 \brief \~Japanese �`�B�֐����i�[����z��̎g�p�T�C�Y���N���A����
 \param TFD \~English It's Database of transfer function
 \~Japanese �`�B�֐����i�[����z����܂ލ\����
*/
void TFDatabaseClear(MICARY_TFDatabase * TFD);
#endif

/**
 \brief \~English It allocates int array(dim:2)
 \brief \~Japanese int�^�̓񎟌��z����m�ۂ���
 \param array \~English It's the array of integer. (iArraySize1 * iArraySize2)
 \~Japanese int�^�̓񎟌��z��̃|�C���^
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese ����1�̔z��̌�
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese ����2�̔z��̌�
*/
void IntArray2Alloc(MICARY_IntArray2 * array, int iArraySize1, int iArraySize2);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates int array(dim:2)
 \brief \~Japanese int�^�̓񎟌��z����m�ۂ���
 \param array \~English It's the array of integer. (iArraySize1 * iArraySize2)
 \~Japanese int�^�̓񎟌��z��̃|�C���^
 \param iArraySize1 \~English It's the size of array (dim:1)
 \~Japanese ����1�̔z��̌�
 \param iArraySize2 \~English It's the size of array (dim:2)
 \~Japanese ����2�̔z��̌�
 \param iReservedArraySize1 \~English It's the size of array (dim:1)
 \~Japanese ����1�̔z��̊m�ی�
 \param iReservedArraySize2 \~English It's the size of array (dim:2)
 \~Japanese ����2�̔z��̊m�ی�
*/
void IntArray2Alloc_r(MICARY_IntArray2 * array, int iArraySize1, int iArraySize2,
											  int iReservedSize1, int iReservedSize2);
#endif

/**
 \brief \~English It frees int array(dim:2)
 \brief \~Japanese int�^�̓񎟌��z����J������
 \param array \~English It's the array of integer. (iArraySize1 * iArraySize2)
 \~Japanese int�^�̓񎟌��z��̃|�C���^
*/
void IntArray2Free(MICARY_IntArray2 * array);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees int array(dim:2)
 \brief \~Japanese int�^�̓񎟌��z��̎g�p�T�C�Y���N���A����
 \param array \~English It's the array of integer. (iArraySize1 * iArraySize2)
 \~Japanese int�^�̓񎟌��z��̃|�C���^
*/
void IntArray2Clear(MICARY_IntArray2 * array);
#endif

// 2006/10/12 added by N.Tanaka
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
void AllocComplexSpecArray(MICARY_ComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex);
// 2011/11/16 added by N.Tanaka
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
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Complex�z��̗̈���J������
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complex�̃X�y�N�g���p�\����
*/
void FreeComplexSpecArray(MICARY_ComplexSpecArray *pcsSpec);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees complex spectrum array
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Complex�z��̎g�p�T�C�Y�N���A
 \param pcsSpec \~English It's complex spectrum array
 \~Japanese HA_Complex�̃X�y�N�g���p�\����
*/
void ClearComplexSpecArray(MICARY_ComplexSpecArray *pcsSpec);
#endif

// 2006/10/12 added by N.Tanaka
/**
 \brief \~English It allocates float spectrum array
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Float�z��̗̈���m�ۂ���
 \param pfsSpec \~English It's float spectrum array
 \~Japanese HA_Float�̃X�y�N�g���p�\����
 \param numCh \~English It's the number of channel.
 \~Japanese �`�����l����
 \param iLowerIndex \~English It's the lower frequency index.
 \~Japanese �������g���C���f�b�N�X
 \param iUpperIndex \~English It's the upper frequency index.
 \~Japanese ������g���C���f�b�N�X
*/
void AllocFloatSpecArray(MICARY_FloatSpecArray *pfsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It allocates freq spectrum array
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Float�z��̗̈���m�ۂ���
 \param pfsSpec \~English It's float spectrum array
 \~Japanese HA_Float�̃X�y�N�g���p�\����
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
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Float�z��̗̈���J������
 \param pfsSpec \~English It's float spectrum array
 \~Japanese HA_Float�̃X�y�N�g���p�\����
*/
void FreeFloatSpecArray(MICARY_FloatSpecArray *pfsSpec);
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
/**
 \brief \~English It frees float spectrum array
 \brief \~Japanese �X�y�N�g���f�[�^�p��HA_Float�z��̎g�p�T�C�Y���N���A����
 \param pfsSpec \~English It's float spectrum array
 \~Japanese HA_Float�̃X�y�N�g���p�\����
*/
void ClearFloatSpecArray(MICARY_FloatSpecArray *pfsSpec);
#endif

// 2007/08/22 added by N.Tanaka
/**
 \brief \~English It allocates char array(string)
 \brief \~Japanese char�z��(string)�̗̈���m�ۂ���
 \param array \English It's CharArray(string)
 \~Japanese char�z��(string)���܂ލ\����
 \param iArraySize \~English It's the size of array
 \~Japanese �m�ۂ���v�f��
*/
void StringAlloc(HA_String * array, int iArraySize);

// 2007/08/22 added by N.Tanaka
/**
 \brief \~English It frees char array(string)
 \brief \~Japanese char�z��(string)��������� 
 \param array \~English CharArray(string)
 \~Japanese char�z��(string)���܂ލ\����
*/
void StringFree(HA_String * array);


#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------
#endif	// ArrayInit_h_defined
