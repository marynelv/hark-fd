// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file MICARY_TimeWin.h
 \brief \~English Header file for calculating time window function. 
 \brief \~Japanese ���ԑ��쐬�֐��̃w�b�_�t�@�C��
 \author Sachiko Suzuki
 \date 17 Nov 2006
 \version $Id: MICARY_TimeWin.h 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#ifndef MICARY_TimeWin_h_defined
#define MICARY_TimeWin_h_defined
// ---------------------------------------------------------------------
#include "MICARY_struct.h"
#include "ArrayInit.h"
#include "MICARY_GHDSS_struct.h"

#ifndef M_PI
#define M_PI (3.141592653589793238)
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------
/**
 \brief \~English It makes time window function . (1: true, 0: false)
 \brief \~Japanese ���ԑ��̍쐬�֐��i1:����I���C0:�G���[�j
 \param pfaTWin \~English It's the window function .
 \~Japanese ���֐�
 \param iNwin \~English It's the length of the window function.
 \~Japanese ����
 \param facoef \~English It's the coefficient value 
 \~Japanese �W��
 \param enumWin \~English It's the veriety of time window. (1: hanning window , 0: Special window)
 \~Japanese ���ԑ��̎��(1:Hanning���C0:Special��)
*/
int MakeTimeWin(MICARY_FloatArray * pfaTWin,
					 HA_Integer iNwin,float facoef,MICARY_WindowEnum enumWin);
/**
 \brief \~English It products data window function . 
 \brief \~Japanese ���n��f�[�^�Ɏ��ԑ���������
 \param pfaTWin \~English It's the window function .
 \~Japanese ���֐�
 \param iaData \~English It's data.
 \~Japanese �f�[�^[�`�����l����][�T���v��]
 */
MICARY_IntArray2 *ProductTimeWin(MICARY_FloatArray * pfaTWin,	MICARY_IntArray2 iaData) ;

/**
 \brief \~English It products float data window function . 
 \brief \~Japanese ���n��float�f�[�^�Ɏ��ԑ���������
 \param pfaTWin \~English It's the window function .
 \~Japanese ���֐�
 \param faData \~English It's float data.
 \~Japanese �f�[�^[�`�����l����*�T���v��]
 \param FrameLen \~English It's data length by one frame.
 \~Japanese �t���[����

 */
MICARY_FloatArray *ProductTimeWinF(MICARY_FloatArray * pfaTWin,
								MICARY_FloatArray faData,HA_Num FrameLen) ;

/**
 \brief \~English It products data(float) window function . 
 \brief \~Japanese ���n��(float)�f�[�^�Ɏ��ԑ���������
 \param pfaTWin \~English It's the window function .
 \~Japanese ���֐�
 \param iaData \~English It's (float) data.
 \~Japanese  �f�[�^[�`�����l����][�T���v��]
 */
MICARY_FloatArray2 *ProductTimeWinFloat2(MICARY_FloatArray * pfaTWin,MICARY_IntArray2 iaData);

/**
 \brief \~English It products data(float) window function . 
 \brief \~Japanese ���n��(float)�f�[�^�Ɏ��ԑ���������
 \param faReturn \~English It's the windowed  signal .
 \~Japanese ���֐������������n��f�[�^�i2�����z��j
 \param pfaTWin \~English It's the window function .
 \~Japanese ���֐�
 \param iaData \~English It's (float) data.
 \~Japanese  �f�[�^[�`�����l����][�T���v��]
 */
int ProductTimeWinFloat3(MICARY_FloatArray2 *faReturn,MICARY_FloatArray * pfaTWin,
				MICARY_IntArray2 iaData) ;
/**
 \brief \~English It products float data window function . 
 \brief \~Japanese ���n��float�f�[�^�Ɏ��ԑ���������
 \param faReturn \~English It's the windowed signal .
 \~Japanese ���֐������������n��f�[�^�i1�����z��j
 \param pfaTWin \~English It's the window function .
 \~Japanese ���֐�
 \param faData \~English It's float data.
 \~Japanese �f�[�^[�`�����l����*�T���v��]
 \param FrameLen \~English It's data length by one frame.
 \~Japanese �t���[����

 */
int ProductTimeWinF3(MICARY_FloatArray *faReturn,MICARY_FloatArray * pfaTWin,
				MICARY_FloatArray faData,HA_Num FrameLen) ;


#ifdef __cplusplus
}
#endif

// ---------------------------------------------------------------------
#endif
