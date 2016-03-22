// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_exec.h
 \brief \~English Header file for Execute PostFilter function
 \brief \~Japanese PostFilter実行関数のヘッダファイル
 \author Sachiko Suzuki
 \date 30 Jan 2007
 \version $Id: PF_exec.h 504 2009-09-09 09:05:29Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	PF_EXEC_h_defined
#define	PF_EXEC_h_defined
// ---------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "MICARYHA_struct.h"
#include "ArrayInit.h"
#include "MICARY_PF_Struct.h"
//#include "MICARY_SpecFile.h"
#include "PF_Init.h"
#include "PF_est_leak.h"
#include "PF_est_noise.h"
#include "PF_est_rev.h"
#include "PF_est_sn.h"
#include "PF_est_state_noise.h"
#include "PF_est_voice_p.h"

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif


// ---------------------------------------------------------------------

/**
 \brief \~Japanese PFの実行(1:正常終了, 0:エラー)
 \param csInputSpec \~English Input spectrum
 \~Japanese 入力スペクトル
 \param pcsOnputSpec \~English Output spectrum 
 \~Japanese 出力スペクトル
 \param pSavedMCRA \~English saved data struct for MCRA
 \~Japanese MCRA保存値構造体
 \param pSavedMCRA \~English saved data struct for MCRA(previous frame)
 \~Japanese 前フレームMCRA保存値
 \param prmMCRA \~English parameter struct for MCRA
 \~Japanese MCRAパラメータ
 \param pSavedLeak \~English saved data struct for Leak noise
 \~Japanese 漏れノイズ推定保存値
 \param prmLeak \~English parameter struct for Leak noise
 \~Japanese 漏れノイズ推定パラメータ
 \param pSavedRev \~English saved data struct for Reverbaration
 \~Japanese 残響成分推定保存値
 \param pSavedRevtmp \~English saved data struct for Reverbration(previous frame)
 \~Japanese 前フレーム残響成分推定保存値
 \param prmRev \~English parameter struct for Reverbraion
 \~Japanese 残響成分推定パラメータ
 \param pSavedSN \~English saved data struct for SN ratio
 \~Japanese SN比推定保存値
 \param prmSN \~English parameter struct for SN ratio
 \~Japanese SN比推定パラメータ
 \param pSavedVoiceP \~English saved data struct for Speech probability
 \~Japanese 音声存在確率保存値
 \param prmVoiceP \~English parameter struct for Speech probability
 \~Japanese 音声存在確率パラメータ
 \param PrmGain \~English parameter optimum gain
 \~Japanese 最適ゲインパラメータ
 \param pSavedLamda \~English saved data estimated back ground noise
 \~Japanese 推定背景ノイズデータ
 */

int PF_ExecutePostFilter(MICARY_ComplexSpecArray csInputSpec , MICARY_ComplexSpecArray *pcsOutputSpec ,
							MICARY_SavedMCRA *pSavedMCRA ,MICARY_SavedMCRA *pSavedMCRAtmp ,MICARY_PrmMCRA prmMCRA ,
							MICARY_FloatSpecArray *pSavedLeak ,MICARY_PrmLeakNoise prmLeak ,
							MICARY_SavedEstRev *pSavedRev ,MICARY_SavedEstRev *pSavedRevtmp ,
							MICARY_PrmRevNoise prmRev , 
							MICARY_SavedEstSN *pSavedSN , MICARY_PrmEstSN prmSN,
							MICARY_SavedVoiceP *pSavedVoiceP , MICARY_PrmVoiceP prmVoiceP ,
			 /*MICARY_PrmGain PrmGain ,*/ MICARY_FloatSpecArray *pSavedLamda) ;

/**
 \brief \~Japanese MCRA背景ノイズの初期化(1:正常終了, 0:エラー)
 \param csInputSpec \~English Input spectrum
 \~Japanese 入力スペクトル
 \param pfsOutputSpec \~English Output Noise spectrum
 \~Japanese 出力ノイズスペクトル
 \param ha_fNoiseCoef \~English Noise compensation value 
 \~Japanese ノイズ補正パラメータ
*/

int PF_InitMCRA(MICARY_ComplexSpecArray csInputSpec ,
				MICARY_FloatSpecArray *pfsOutputSpec ,HA_Float ha_fNoiseCoef) ;

// ---------------------------------------------------------------------
/**
 \brief \~English It calculate power spectrum .
 \brief \~Japanese パワースペクトルの作成(1:正常終了, 0:エラー)
 \param pfsPowerSpec \English Power spectrum array struct pointer
 \~Japanese パワースペクトルアレイポインタ
 \param csSpec \English Spectrum array struct pointer
 \~Japanese スペクトルアレイポインタ
*/
int MakePowerSpec(MICARY_FloatSpecArray * pfsPowerSpec ,
					MICARY_ComplexSpecArray csSpec ) ;

#ifdef __cplusplus
}
#endif
// ----------------------------------------------------------------------------
#endif	// PF_EXEC_h_defined
