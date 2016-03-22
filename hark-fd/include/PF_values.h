// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_values.h
 \brief \~English Header file for assign option parameter (Post Filter System)
 \brief \~Japanese Post Filterのオプションパラメータのヘッダファイル(Post Filter System)
 \author Sachiko Suzuki
 \date 19 Feb 2007
 \version $Id: PF_values.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_VALUES_H_DEFINED
#define	PF_VALUES_H_DEFINED
// ---------------------------------------------------------------------
//#include "MICARYHA_struct.h"	// 2008/11/11 deleted by N.Tanaka MICARY_struct.h経由で呼ぶこととする
#include "MICARY_struct.h"	// 2008/11/11 added by N.Tanaka
// ---------------------------------------------------------------------

/**
 Defines for Estimate speech present probability
 \author	Masatoshi Tsutsumi
 \date	1 Dec 2006
*/

#ifndef	EST_VOIVE_P_B
#define		EST_VOIVE_P_COEF_B	((HA_Float)(0.7))
#endif

#ifndef	EST_VOICE_P_BW_F_ST
#define		EST_VOICE_P_BW_F_ST	((HA_Integer)(8))
#endif

#ifndef	EST_VOICE_P_BW_F_EN
#define		EST_VOICE_P_BW_F_EN	((HA_Integer)(99))
#endif

#ifndef	EST_VOICE_P_BW_G
#define		EST_VOICE_P_BW_G	((HA_Integer)(29))
#endif

#ifndef	EST_VOICE_P_BW_L
#define		EST_VOICE_P_BW_L	((HA_Integer)(5))
#endif

#ifndef	EST_VOICE_P_ZMIN_F
#define		EST_VOICE_P_ZMIN_F	((HA_Float)(0.1))
#endif

#ifndef	EST_VOICE_P_ZMAX_F
#define		EST_VOICE_P_ZMAX_F	((HA_Float)(0.316))
#endif

#ifndef	EST_VOICE_P_ZMIN_G
#define		EST_VOICE_P_ZMIN_G	((HA_Float)(0.1))
#endif

#ifndef	EST_VOICE_P_ZMAX_G
#define		EST_VOICE_P_ZMAX_G	((HA_Float)(0.316))
#endif

#ifndef	EST_VOICE_P_ZMIN_L
#define		EST_VOICE_P_ZMIN_L	((HA_Float)(0.1))
#endif

#ifndef	EST_VOICE_P_ZMAX_L
#define		EST_VOICE_P_ZMAX_L	((HA_Float)(0.316))
#endif

#ifndef	EST_VOICE_P_ZETA_F0
#define		EST_VOICE_P_ZETA_F0	((HA_Float)(1.5))
#endif

#ifndef	EST_VOICE_P_ZPMIN
#define		EST_VOICE_P_ZPMIN	((HA_Float)(1.0))
#endif

#ifndef	EST_VOICE_P_ZPMAX
#define		EST_VOICE_P_ZPMAX	((HA_Float)(10.0))
#endif

#ifndef	EST_VOICE_P_AMP_L
#define		EST_VOICE_P_AMP_L	((HA_Float)(0.9))
#endif

#ifndef	EST_VOICE_P_AMP_G
#define		EST_VOICE_P_AMP_G	((HA_Float)(0.9))
#endif

#ifndef	EST_VOICE_P_AMP_F
#define		EST_VOICE_P_AMP_F	((HA_Float)(0.7))
#endif

#ifndef	EST_VOICE_P_QMAX
#define		EST_VOICE_P_QMAX	((HA_Float)(0.98))
#endif

#ifndef	EST_VOICE_P_QMIN
#define		EST_VOICE_P_QMIN	((HA_Float)(0.02))
#endif

/**
 Defines for Estimate gain
 \author	Sachiko Suzuki
 \date	24 Jan 2007
*/

#ifndef	EST_SN_P_XIMAX
#define		EST_SN_P_XIMAX	((HA_Float)(100))
#endif

#ifndef	EST_SN_P_THETAMAX
#define		EST_SN_P_THETAMAX	((HA_Float)(20))
#endif

#ifndef	EST_SN_P_THETAMIN
#define		EST_SN_P_THETAMIN	((HA_Float)(6))
#endif

/**
 Defines for Initialize MCRA module
 \author	Sachiko Suzuki
 \date	19 Feb 2007
*/

#ifndef	INIT_MCRA_DELTA
#define		INIT_MCRA_DELTA	((HA_Float)(3))
#endif

#ifndef	INIT_MCRA_L
#define		INIT_MCRA_L	((HA_Float)(150))
#endif

#ifndef	INIT_MCRA_ALP_S
#define		INIT_MCRA_ALP_S	((HA_Float)(0.7))
#endif

#ifndef	INIT_MCRA_NOISE_COMPENSE
#define		INIT_MCRA_NOISE_COMPENSE	((HA_Float)(1))
#endif

#ifndef	INIT_MCRA_ALP_DMIN
#define		INIT_MCRA_ALP_DMIN	((HA_Float)(0.05))
#endif

#ifndef	INIT_MCRA_INIT_FRAME
#define		INIT_MCRA_INIT_FRAME	((HA_Integer)(100))
#endif

// ---------------------------------------------------------------------
#endif	// PF_VALUES_H_DEFINED
