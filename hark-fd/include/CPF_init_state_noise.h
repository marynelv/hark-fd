// ---------------------------------------------------------------------
/**
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.

 \file CPF_init_state_noise.h
 \brief \~English Header file for estimating the state noise of the each channnel
 \brief \~Japanese 各チャンネルの定常雑音推定クラスのヘッダ(Post Filter)
 \author Natsu Tanaka
 \date 6 Jun 2007
 \version $Id: CPF_init_state_noise.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	CPF_init_state_noise_h_defined
#define	CPF_init_state_noise_h_defined
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
#include "PF_values.h"
#include "ArrayInit.h"
// ---------------------------------------------------------------------
/**
 \class CPF_init_state_noise
 \brief \~English Class for estimating the state noise of the each channnel
 \brief \~Japanese MCRA用初期ノイズ推定クラス
*/
class CPF_init_state_noise{
private:
	MICARY_SavedInitMCRA	*m_psSavedInitMCRA;	// 入力チャンネルMCRA前フレーム保存値
	MICARY_PrmInitMCRA	m_sPrmInitMCRA;	// MCRAパラメータ
	
public:
	/**
	 \brief \~Japanese コンストラクタ
	*/
	CPF_init_state_noise();
	/**
	 \brief \~Japanese デストラクタ
	*/
	~CPF_init_state_noise();
	
	/**
	 \brief \~Japanese クリア
	*/
	int Clear();

	/**
	 \brief \~Japanese MCRA用初期ノイズ推定クラスの初期化
	 \param iLowerFreq \~English Lower frequency[line]
	 \~Japanese 下限周波数ライン
	 \param iUpperFreq \~English Upper frequency[line]
	 \~Japanese 上限周波数ライン
	 \param iNCh \~English Number of channel
	 \~Japanese チャンネル数
	*/
	int Initialize(int iLowerFreq, int iUpperFreq, int iNCh);


	/**
	 \brief \~Japanese MCRA用初期ノイズの取得
	 \param faInputPSpec \~English Input power spectrum (N, M)
	 \~Japanese 入力パワースペクトル
	 \param pfsInitNoisePSpec \~English MCRA values
	 \~Japanese MCRA初期ノイズ
	*/
	int InitStateNoise(MICARY_FloatSpecArray faInputPSpec,
                       MICARY_FloatSpecArray *pfsInitNoisePSpec);

	/**
	 \brief \~Japanese MCRA用初期ノイズ用パラメータの設定
	 \param sPrmInitMCRA \~English Parameters for Initial noise of MCRA calc.
	 \~Japanese MCRA初期ノイズ計算パラメータ
	*/
	int SetPrmInitMCRA(MICARY_PrmInitMCRA sPrmInitMCRA);
};
// ---------------------------------------------------------------------
#endif // CPF_init_state_noise_h_defined
