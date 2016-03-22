// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_hypergeom_gain.h
 \brief \~English Header file for Hypergeom Gain function
 \brief \~Japanese SN比・ゲイン推定の内部から呼ばれるhypergeom_gain関数
 \author Masatoshi Tsutsumi
 \date 23 Nov 2006
 \version $Id: PF_hypergeom_gain.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_HYPERGEOM_GAIN_h_defined
#define	PF_HYPERGEOM_GAIN_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------

/**
 \brief \~Japanese hypergeom_gain値の算出(1:正常終了, 0:エラー)
 \param x \~English It's Variable for optimum gain estimator
 \~Japanese 最適ゲイン値
 \param y \English return value
 \~Japanese 
*/
int PF_HyperGeomGain(HA_Float x, HA_Float *y);

#ifdef __cplusplus
}
#endif
// ----------------------------------------------------------------------
#endif	// PF_HYPERGEOM_GAIN_h_defined