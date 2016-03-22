// ---------------------------------------------------------------------
/**
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.

 \file CPF_exec.h
 \brief \~English Header file for execute Post-Filter.
 \brief \~Japanese Post-Filterの実行クラスのヘッダ(CPF)
 \author Sachiko Suzuki
 \date 1 Aug 2007
 \version $Id: CPF_exec.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	CPF_exec_h_defined
#define	CPF_exec_h_defined
// ---------------------------------------------------------------------
#include "ArrayInit.h"
//#include "ArrayUtil.h"
#include "PF_exec.h"
#include "PF_Init.h"
#include "MICARY_PF_Struct.h"

#include <string>
#include <iostream>
#include <vector>
// ---------------------------------------------------------------------
/**
 \class CPF_exec
 \brief \~English Class for executing Post-Filter process
 \brief \~Japanese Post-Filterの実行クラス
*/
class CPF_exec{
private:
  //MICARY_FloatSpecArray *m_pfsNoiseSpec ;	//背景雑音Spectra
	MICARY_SavedMCRA *m_psSavedMCRA ;	//MCRA保存値(current frame)
	MICARY_SavedEstRev *m_psSavedRev ;	//残響成分保存値(current frame)
	MICARY_SavedEstSN *m_psSavedEstSN ;	//SN比推定保存値
	MICARY_SavedVoiceP *m_psSavedVoiceP ;	//音声存在確率保存値
	MICARY_SavedMCRA *m_psSavedMCRATmp ;	//MCRA保存値Tmp(1つ前フレーム)
	MICARY_FloatSpecArray *m_pfsSavedLeak ;	//漏れ保存値
	MICARY_SavedEstRev *m_psSavedRevTmp ;	//残響成分保存値Tmp(1つ前フレーム)
	MICARY_FloatSpecArray *m_pfsLamda_total ; //Total noise

	// Temporary array for change source(ポインタに変更　2008/2/15)
	MICARY_SavedMCRA *tmp_sSavedMCRA,*tmp_sSavedMCRATmp ;//MCRA保存値(temporary)
	MICARY_SavedEstRev *tmp_sSavedRev,*tmp_sSavedRevTmp ;//残響成分保存値(temporary)
	MICARY_SavedEstSN *tmp_sSavedEstSN ;	//SN比推定保存値(temporary)
	MICARY_SavedVoiceP *tmp_sSavedVoiceP ;	//音声存在確率保存値(temporary)
	MICARY_FloatSpecArray *tmp_fsSavedLeak ;	//漏れ保存値(temporary)
	MICARY_FloatSpecArray *tmp_fsLamda_total ; // total lamda(temporary)
	
	
public:
	virtual int ChangeSrcPos(MICARY_FloatSpecArray faInStatNoise,MICARY_IntArray iaSrcCur,MICARY_IntArray iaSrcIni,HA_Float ha_fNoise_Compense);
	virtual int InitSavedArray(MICARY_FloatSpecArray faInStatNoise,int iLower_freq,int iUpper_freq,int iNpos,int iNch,HA_Float ha_fNoise_compense);
	/**
	 \brief \~Japanese コンストラクタ
	*/
	CPF_exec() ;
	/**
	 \brief \~Japanese デストラクタ
	*/
	~CPF_exec() ;
	/**
	 \brief \~Japanese クリア
	*/
	int Clear() ;
	MICARY_FloatSpecArray *m_pfsNoiseSpec ;	//背景雑音Spectra

	// PF用Parameter構造体
	MICARY_PrmVoiceP sPrmVoiceP ;	//音声powerパラメータ
	MICARY_PrmEstSN sPrmEstSN ;	//S/N比推定パラメータ
	MICARY_PrmRevNoise sPrmRev ;	//残響成分パラメータ
	MICARY_PrmLeakNoise sPrmLeak ;	//漏れ成分パラメータ
	MICARY_PrmMCRA sPrmMCRA ;	//MCRAパラメータ	
	MICARY_PrmGain sPrmGain ;	//最適Gainパラメータ

	/**
	 \brief \~Japanese Post-Filter用保存値配列の確保
	 \param iLower_freq \~English Index of lower bound frequency
	 \~Japanese 下限周波数インデックス
	 \param iUpper_freq \~English Index of upper bound frequency
	 \~Japanese 上限周波数インデックス
	 \param iNpos \~English Size of sources
	 \~Japanese 音源数
	 \param iNch \~English Size of mic channels
	 \~Japanese マイクチャンネル数
	 \param iNproc \~English Number of process
	 \~Japanese 処理回数
	*/
	int Initialize(int iLower_freq,int iUpper_freq,int iNch,int iNproc) ;
	/**
	 \brief \~Japanese Post-Filter用保存値配列の初期化
	 \param faInStatNoise \~English Power spectra estimated the state noise of each channel
	 \~Japanese 推定されたMCRA用初期ノイズのパワースペクトル
	 \param iLower_freq \~English Index of lower bound frequency
	 \~Japanese 下限周波数インデックス
	 \param iUpper_freq \~English Index of upper bound frequency
	 \~Japanese 上限周波数インデックス
	 \param iNpos \~English Size of sources
	 \~Japanese 音源数
	 \param iNch \~English Size of mic channels
	 \~Japanese マイクチャンネル数
	*/
	int InitSavedArray(MICARY_FloatSpecArray faInStatNoise,int iLower_freq,int iUpper_freq,int iNpos,int iNch);
	/**
	 \brief \~Japanese MCAR用パラメータの設定
	 \param fNoise_compens \~English Noise compens
	 \~Japanese 雑音補正値
	 \param fSmooth_spec_factor \~English Smoothing factor for input spectra
	 \~Japanese 入力スペクトルの平滑化係数
	 \param fAmp_leak_factor \~English Leak factor of amplitude
	 \~Japanese 振幅漏れ係数
	 \param fBacknoise_est_factor \~English Smoothing factor for estimated BGN
	 \~Japanese 推定背景雑音の平滑化係数
	 \param fLeak_factor \~English Leak factor
	 \~Japanese 漏れ係数
	 \param fSs_floor \~English Floor factor
	 \~Japanese floor係数
	 \param iL \~English Time width for detection
	 \~Japanese 検出時間幅
	 \param fDelta \~English Threshold level of power ratio
	 \~Japanese パワー比閾値
	*/
	int Set_PF_PrmMCRA(float fNoise_compens,float fSmooth_spec_factor,float fAmp_leak_factor,
		float fBacknoise_est_factor, float fLeak_factor,float fSs_floor,int iL,float fDelta) ;
	/**
	 \brief \~Japanese 漏れ推定用パラメータの設定
	 \param fLeak_factor \~English Leak factor
	 \~Japanese 漏れ係数
	 \param fOver_cancel \~English Over cancel value
	 \~Japanese オーバーキャンセル値
	*/
	int Set_PF_PrmLeak(float fLeak_factor,float fOver_cancel) ;
	/**
	 \brief \~Japanese 残響成分推定用パラメータの設定
	 \param fReverb_decay \~English Reverb decay value
	 \~Japanese 残響減衰定数
	 \param fReverb_level \~English Direct component ratio
	 \~Japanese 直接成分比
	*/
	int Set_PF_PrmRev(float fReverb_decay,float fReverb_level) ;
	/**
	 \brief \~Japanese SN比推定用パラメータの設定
	 \param fPrior_factor \~English Time SN ratio
	 \~Japanese 時間SN比
	 \param fAlp_pmag \~English Amplitude factor of voice inclusion rate
	 \~Japanese 音声含有率の振幅係数
	 \param fAlp_pmin \~English Minimum voice inclusion rate
	 \~Japanese 最小音声含有率
	 \param fXi_max \~English Max value of posterior SN ratio
	 \~Japanese 事後SN比の最大値
	 \param fTheta_max \~English Max value of optimum gain mid variable(v)
	 \~Japanese 最適ゲイン中間変数(v)の最大値
	 \param fTheta_min \~English Minimum value of optimum gain mid variable(v)
	 \~Japanese 最適ゲイン中間変数(v)の最小値
	*/
	int Set_PF_PrmSN(float fPrior_factor,float fAlp_pmag,float fAlp_pmin,float fXi_max,
		float fTheta_max,float fTheta_min) ;
	/**
	 \brief \~Japanese 音声確率推定用パラメータの設定
	 \param fSmooth_prior_factor \~English Smoothing factor of prior SN ratio
	 \~Japanese 事前SN比の平滑化係数
	 \param fZminF \~English Minimum value of smoothing SN ratio(frame)
	 \~Japanese 平滑化SN比の最小値(frame)
	 \param fZmaxF \~English Max value of smoothing SN ratio(frame)
	 \~Japanese 平滑化SN比の最大値(frame)
	 \param fZminG \~English Minimum value of smoothing SN ratio(global)
	 \~Japanese 平滑化SN比の最小値(global)
	 \param fZmaxG \~English Max value of smoothing SN ratio(global)
	 \~Japanese 平滑化SN比の最大値(global)
	 \param fZminL \~English Minimum value of smoothing SN ratio(local)
	 \~Japanese 平滑化SN比の最小値(local)
	 \param fZmaxL \~English Max value of smoothing SN ratio(local)
	 \~Japanese 平滑化SN比の最大値(local)
     \param fBwFen \~English Max value of smoothing interval(frame)
	 \~Japanese 平滑化区間の最大値(frame)
     \param fBwFst \~English Minimum value of smoothing interval(frame)
	 \~Japanese 平滑化区間の最小値(frame)
     \param fBwG \~English Width of smoothing interval(global)
	 \~Japanese 平滑化区間幅(global)
     \param fBwL \~English Width of smoothing interval(local)
	 \~Japanese 平滑化区間幅(local)
	 \param fZthF \~English Threshold of smoothing SN ratio(frame)
	 \~Japanese 平滑化SN比の閾値(frame)
	 \param fZpmin \~English Minimum peak value of smoothing SN ratio
	 \~Japanese 平滑化SN比ピークの最小値
	 \param fZpmax \~English Max peak value of smoothing SN ratio
	 \~Japanese 平滑化SN比ピークの最大値
	 \param fAmpF \~English Smoothing factor of voice probability(frame)
	 \~Japanese 音声確率平滑化係数(frame)
	 \param fAmpG \~English Smoothing factor of voice probability(global)
	 \~Japanese 音声確率平滑化係数(global)
	 \param fAmpL \~English Smoothing factor of voice probability(local)
	 \~Japanese 音声確率平滑化係数(local)
	 \param fQmin \~English Minimum value of break probability
	 \~Japanese 休止確率の最小値
	 \param fQmax \~English Max value of break probability
	 \~Japanese 休止確率の最大値
	*/
	int Set_PF_PrmVoiceP(float fSmooth_prior_factor,float fZminF,float fZmaxF,float fZminG,float fZmaxG,
		float fZminL,float fZmaxL,int iBwFen,int iBwFst,int iBwG,int iBwL,float fZthF,
		float fZpmin,float fZpmax,float fAmpF,float fAmpG,float fAmpL,float fQmin,float fQmax) ;
	/**
	 \brief \~Japanese 最小ゲインパラメータの設定
	 \param fMinGain \~English Minimum gain while without speeching
	 \~Japanese 無音声時の最小ゲイン
	*/
	int Set_PF_PrmGain(float fMinGain) ;
	/**
	 \brief \~Japanese Post-Filterの実行
	 \param csInputCSpec \~English Input signal spectra(complex)
	 \~Japanese 入力信号スペクトル(complex)
	 \param pcsOutputCSpec \~English Output signal spectra(complex)
	 \~Japanese 出力信号スペクトル(complex)
	 \param pfsEstNoise \~English Estimated noise power spectra
	 \~Japanese 推定雑音パワースペクトル
	*/
	int ExecutePF(MICARY_ComplexSpecArray csInputCSpec,MICARY_ComplexSpecArray *pcsOutputCSpec,
		MICARY_FloatSpecArray *pfsEstNoise) ;
	/**
	 \brief \~Japanese 音源変更時の処理
	 \param faInStatNoise \~English Power spectra estimated the state noise of each channel
	 \~Japanese 推定されたMCRA用初期ノイズのパワースペクトル
	 \param iaSrcCur \~English Source ID array for current frame
	 \~Japanese 現フレームの音源ID配列
	 \param iaSrcIni \~English Source ID array in front of frame
	 \~Japanese 1フレーム前の音源ID配列
	*/
	int ChangeSrcPos(MICARY_FloatSpecArray faInStatNoise,MICARY_IntArray iaSrcCur,MICARY_IntArray iaSrcIni) ;
		
};
// ---------------------------------------------------------------------
#endif // CPF_exec_h_defined
