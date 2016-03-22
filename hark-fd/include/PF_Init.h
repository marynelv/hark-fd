/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

\file PF_Init.h
\brief \~Japanese Post filter 変数、データの初期化
\author Sachiko Suzuki
\date 24 Jan 2007
\Version $Id: PF_Init.h 453 2009-03-17 04:44:42Z tanaka $
*/
// ---------------------------------------------------------------------
#ifndef	PFInit_h_defined
#define	PFInit_h_defined
// ---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include "MICARYHA_struct.h"	// 2008/11/11 deleted by N.Tanaka MICARY_struct.h経由で呼ぶ
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
#include "PF_values.h"
#include "ArrayInit.h"

// 2007/07/13 added by S.Suzuki C++からCを呼び出す
#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------

/**
 \brief \~English It initialize the parameter for Post filter.
 \brief \~Japanese 変数の初期化(1:正常終了, 0:エラー)
 \param psPrmMCRA \English Setting parameter for MCRA
 \~Japanese MCRA設定パラメータ
 \param psPrmLeak \English Setting parameter for Leak noise
 \~Japanese 漏れ設定パラメータ
 \param psPrmRev \English Setting parameter for Reverb factor
 \~Japanese 残響成分設定パラメータ
 \param psPrmSN \English Setting parameter for estimate SN ratio
 \~Japanese SN比推定設定パラメータ
 \param psPrmVoiceP \English Setting parameter for Voice Probability
 \~Japanese 音声存在確率設定パラメータ
 \param psPrmGain \English Setting parameter for estimate gain
 \~Japanese 最適ゲイン推定設定パラメータ 
*/
int InitPFsPrm(MICARY_PrmMCRA *psPrmMCRA , MICARY_PrmLeakNoise *psPrmLeak ,
			   MICARY_PrmRevNoise *psPrmRev , MICARY_PrmEstSN *psPrmSN ,
			   MICARY_PrmVoiceP *psPrmVoiceP , MICARY_PrmGain *psPrmGain) ;

// ---------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(MCRA) for Post filter.
 \brief \~Japanese データ保存値(MCRA)の確保(1:正常終了, 0:エラー)
 \param psSavedMCRA \English Saved data for MCRA
 \~Japanese MCRA保存値
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese 上限周波数ライン
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese 下限周波数ライン
 \param Nch \English Number of source position
 \~Japanese 音源数
*/
int AllocSavedMCRA(MICARY_SavedMCRA *psSavedMCRA ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) ;

// ---------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(MCRA) for Post filter.
 \brief \~Japanese データ保存値(MCRA)の開放(1:正常終了, 0:エラー)
 \param psSavedMCRA \English Saved data for MCRA
 \~Japanese MCRA保存値
*/
int FreeSavedMCRA(MICARY_SavedMCRA *psSavedMCRA) ;
//
//// ---------------------------------------------------------------------
///**
// \brief \~English It frees the saved data(EstSN) for Post filter.
// \brief \~Japanese データ保存値(EstSN)の開放(1:正常終了, 0:エラー)
// \param psSavedEstSN \English Saved data for EstSN
// \~Japanese EstSN保存値
//*/
//int FreeSavedEstSN(MICARY_SavedEstSN *psSavedEstSN) ;


// ---------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(estimate Rev) for Post filter.
 \brief \~Japanese データ保存値(Rev)の確保(1:正常終了, 0:エラー)
 \param psSavedRev \English saved data for Reverb factor
 \~Japanese 残響成分保存値
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese 上限周波数ライン
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese 下限周波数ライン
 \param Nch \English Number of source position
 \~Japanese 音源数
*/
int AllocSavedRev(MICARY_SavedEstRev *psSavedRev ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) ;

// ---------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(estimate rev) for Post filter.
 \brief \~Japanese データ保存値(Rev)の開放(1:正常終了, 0:エラー)
 \param psSavedRev \English Saved data for Reverb factor
 \~Japanese 残響成分保存値
*/
int FreeSavedRev(MICARY_SavedEstRev *psSavedRev) ;

// ----------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(estimate SN ratio) for Post filter.
 \brief \~Japanese データ保存値(SN ratio)の確保(1:正常終了, 0:エラー)
 \param psSavedSN \English saved data for SN ratio
 \~Japanese SN比保存値
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese 上限周波数ライン
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese 下限周波数ライン
 \param Nch \English Number of source position
 \~Japanese 音源数
*/
int AllocSavedSN(MICARY_SavedEstSN *psSavedSN ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) ;
// ----------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(estimate SN ratio) for Post filter.
 \brief \~Japanese データ保存値(SN ratio)の開放(1:正常終了, 0:エラー)
 \param psSavedSN \English saved data for SN ratio
 \~Japanese SN比保存値
*/
int FreeSavedSN(MICARY_SavedEstSN *psSavedSN ) ;
// ----------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(Voice Probability) for Post filter.
 \brief \~Japanese データ保存値(Voice Probability)の確保(1:正常終了, 0:エラー)
 \param psSavedVoiceP \English saved data for Voice Probability
 \~Japanese 音声存在確率保存値
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese 上限周波数ライン
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese 下限周波数ライン
 \param Nch \English Number of source position
 \~Japanese 音源数
*/
int AllocSavedVoiceP(MICARY_SavedVoiceP *psSavedVoiceP ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) ;
// ----------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(Voice Probability) for Post filter.
 \brief \~Japanese データ保存値(Voice Probability)の開放(1:正常終了, 0:エラー)
 \param psSavedVoiceP \English saved data for Voice Probability
 \~Japanese 音声存在確率保存値
*/
int FreeSavedVoiceP(MICARY_SavedVoiceP *psSavedVoiceP ) ;
// ----------------------------------------------------------------------
// ---------------------------------------------------------------------
/**
 \brief \~English It allocates the saved data(Initialized MCRA) for Post filter.
 \brief \~Japanese データ保存値(MCRAの初期値)の確保(1:正常終了, 0:エラー)
 \param psSavedInitMCRA \English Saved data for MCRA
 \~Japanese MCRA初期保存値
 \param iUpperFreq \English Upper frequency [Line]
 \~Japanese 上限周波数ライン
 \param iLowerFreq \English Lower frequency [Line]
 \~Japanese 下限周波数ライン
 \param Nch \English Number of source position
 \~Japanese 音源数
*/
int AllocSavedInitMCRA(MICARY_SavedInitMCRA *psSavedInitMCRA ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch)  ;

// ---------------------------------------------------------------------
/**
 \brief \~English It frees the saved data(Initialized MCRA) for Post filter.
 \brief \~Japanese データ保存値(MCRAの初期値)の開放(1:正常終了, 0:エラー)
 \param psSavedInitMCRA \English Saved data for MCRA
 \~Japanese MCRA初期保存値
*/
int FreeSavedInitMCRA(MICARY_SavedInitMCRA *psSavedInitMCRA)  ;

// ---------------------------------------------------------------------
/**
 \brief \~English It assigns the initialized MCRA parameter  for Post filter.
 \brief \~Japanese MCRAの初期値用パラメータの設定(1:正常終了, 0:エラー)
 \param psPrmInitMCRA \English Parameter structure for Initialized MCRA
 \~Japanese MCRA初期パラメータ
*/
int InitPrmInMCRA(MICARY_PrmInitMCRA *psPrmInitMCRA) ;

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
/**
 \brief \~English It initializes temporary data for Post filter.
 \brief \~Japanese 保存値データの初期化(1:正常終了, 0:エラー)
 \param psSavedMCRA \English Saved data structure for Initialized MCRA
 \~Japanese MCRA初期化データ構造体
 \param psSavedRev \English Saved data structure for Initialized REV
 \~Japanese REV推定用初期化データ構造体
 \param psSavedSN \English Saved data structure for Initialized SN
 \~Japanese SN推定用初期化データ構造体
 \param psSavedVoiceP \English Saved data structure for Initialized VoiceP
 \~Japanese 音声確率初期化データ構造体
 \param faNoise \English Estimated noise power data array
 \~Japanese 推定ノイズパワー
 \param noise_compense \English parameter of noise_compense
 \~Japanese 定常雑音パワーパラメータ
 \param iLowerFreq \English Lower Frequency line index
 \~Japanese 下限周波数ライン番号
 \param iUpperFreq \English Upper frequency line index
 \~Japanese 上限周波数ライン番号
 \param Nsrc \English Number of Source
 \~Japanese 音源数
 \param Nmic \English Number of microphone
 \~Japanese マイクチャンネル数
*/
int PFInitTmp(MICARY_SavedMCRA *psSavedMCRA ,MICARY_SavedEstRev *psSavedRev,MICARY_SavedEstSN *psSavedSN ,
			  MICARY_SavedVoiceP *psSavedVoiceP,MICARY_FloatArray *faNoise ,HA_Float noise_compense,
			  HA_Integer iLowerFreq ,HA_Integer iUpperFreq,HA_Integer Nsrc,HA_Integer Nmic) ;



// 2008/12/5 added by K.Morio
/**
 \brief \~English It initializes temporary data for Post filter.
 \brief \~Japanese 保存値データの初期化(1:正常終了, 0:エラー)
 \param psSavedMCRA \English Saved data structure for Initialized MCRA
 \~Japanese MCRA初期化データ構造体
 \param psSavedRev \English Saved data structure for Initialized REV
 \~Japanese REV推定用初期化データ構造体
 \param psSavedSN \English Saved data structure for Initialized SN
 \~Japanese SN推定用初期化データ構造体
 \param psSavedVoiceP \English Saved data structure for Initialized VoiceP
 \~Japanese 音声確率初期化データ構造体
 \param faNoise \English Initial State Noise data array
 \~Japanese 定常雑音初期値
 \param iLowerFreq \English Lower Frequency line index
 \~Japanese 下限周波数ライン番号
 \param iUpperFreq \English Upper frequency line index
 \~Japanese 上限周波数ライン番号


*/
int PFInitTmp2(MICARY_SavedMCRA *psSavedMCRA ,MICARY_SavedEstRev *psSavedRev,MICARY_SavedEstSN *psSavedSN ,
			  MICARY_SavedVoiceP *psSavedVoiceP,MICARY_FloatArray *pfCalcStateNoise,
			  HA_Integer iLowerFreq ,HA_Integer iUpperFreq);

// 2008/12/5 added by K.Morio
/**
 \brief \~English It calcurates Initial state noise power.
 \brief \~Japanese 定常雑音初期値の算出(1:正常終了, 0:エラー)
 \param faNoise \English Estimated noise power data array
 \~Japanese 推定ノイズパワー
 \param noise_compense \English parameter of noise_compense
 \~Japanese 定常雑音パワーパラメータ
  \param faNoise \English Initial State Noise data array
 \~Japanese 定常雑音初期値
 \param iLowerFreq \English Lower Frequency line index
 \~Japanese 下限周波数ライン番号
 \param iUpperFreq \English Upper frequency line index
 \~Japanese 上限周波数ライン番号
 \param Nsrc \English Number of Source
 \~Japanese 音源数
 \param Nmic \English Number of microphone
 \~Japanese マイクチャンネル数


*/
int CalcInitStateNoise(MICARY_FloatArray faNoise ,HA_Float noise_compense,MICARY_FloatArray *pfCalcStateNoise,
			  HA_Integer iLowerFreq ,HA_Integer iUpperFreq,HA_Integer Nsrc,HA_Integer Nmic);



/**
 \brief \~English It calculates minimum sum (PF_init_state_noise.c/PF_est_state_noise.c).
 \brief \~Japanese 変数A、Bの最小値の差を算出(PF_init_state_coise.c/PF_est_state_noise.c).
 \param A \English Variable A
 \~Japanese 変数A
 \param B \English Variable B
 \~Japanese 変数B
*/
HA_Float minmat(HA_Float A ,HA_Float B) ;






// 2008/08/06 added by N.Tanaka
/**
 \brief \~Japanese 漏れ推定用パラメータの設定
 \param psPrmLeakNoise \~English Parameter struct for estimating leak noise
 \~Japanese 漏れ推定用パラメータの設定
 \param fLeak_factor \~English Leak factor
 \~Japanese 漏れ係数
 \param fOver_cancel \~English Over cancel value
 \~Japanese オーバーキャンセル値
*/
int Input_PF_PrmLeak(MICARY_PrmLeakNoise *psPrmLeakNoise, float fLeak_factor,float fOver_cancel) ;


#ifdef __cplusplus
}
#endif
// ----------------------------------------------------------------------------------------

#endif // PF_CmdLine_h_defined

