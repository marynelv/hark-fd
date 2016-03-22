// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_hypergeom_gain.h
 \brief \~English Header file for Hypergeom Gain function
 \brief \~Japanese SN��E�Q�C������̓�������Ă΂��hypergeom_gain�֐�
 \author Masatoshi Tsutsumi
 \date 23 Nov 2006
 \version $Id: PF_hypergeom_gain.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_HYPERGEOM_GAIN_h_defined
#define	PF_HYPERGEOM_GAIN_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"

// 2007/07/13 added by S.Suzuki C++����C���Ăяo��
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------

/**
 \brief \~Japanese hypergeom_gain�l�̎Z�o(1:����I��, 0:�G���[)
 \param x \~English It's Variable for optimum gain estimator
 \~Japanese �œK�Q�C���l
 \param y \English return value
 \~Japanese 
*/
int PF_HyperGeomGain(HA_Float x, HA_Float *y);

#ifdef __cplusplus
}
#endif
// ----------------------------------------------------------------------
#endif	// PF_HYPERGEOM_GAIN_h_defined