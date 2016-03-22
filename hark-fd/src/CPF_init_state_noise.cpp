// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2007 Honda Research Institute Japan, Co., Ltd.

 \file CPF_init_state_noise.cpp
 \brief \~English Source file for estimating the state noise of the each channnel
 \brief \~Japanese 各チャンネルの定常雑音推定クラスのソース(Post Filter)
 \author Natsu Tanaka
 \date 6 Jun 2007
 \version $Id: CPF_init_state_noise.cpp 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
// ---------------------------------------------------------------------
#include "CPF_init_state_noise.h"
// ---------------------------------------------------------------------
#include "PF_Init.h"
#include "PF_init_state_noise.h"
// ---------------------------------------------------------------------
// コンストラクタ
CPF_init_state_noise::CPF_init_state_noise()
{
	m_psSavedInitMCRA = (MICARY_SavedInitMCRA *)calloc(1, sizeof(MICARY_SavedInitMCRA));
	InitPrmInMCRA(&m_sPrmInitMCRA);
}
// デストラクタ
CPF_init_state_noise::~CPF_init_state_noise()
{
	this->Clear();
	free(m_psSavedInitMCRA);
}
// ---------------------------------------------------------------------
// Savedのクリア
int CPF_init_state_noise::Clear()
{
	FreeSavedInitMCRA(m_psSavedInitMCRA);

	return 1;
}
// ---------------------------------------------------------------------
// Savedの確保
int CPF_init_state_noise::Initialize(int iLowerFreq, int iUpperFreq, int iNCh)
{
	if (iUpperFreq - iLowerFreq < 0 || iNCh < 0){
		return 0;
	}

	this->Clear();

	AllocSavedInitMCRA(m_psSavedInitMCRA, iUpperFreq, iLowerFreq, iNCh);

	return 1;
}
// ---------------------------------------------------------------------
// MCRAの実行
int CPF_init_state_noise::InitStateNoise(MICARY_FloatSpecArray faInputPSpec,
                                         MICARY_FloatSpecArray *pfsInitNoisePSpec)
{
	if (!pfsInitNoisePSpec){
		return 0;
	}

	// サイズの確認 
	if (faInputPSpec.ha_numCh != pfsInitNoisePSpec->ha_numCh ||
		faInputPSpec.ha_iLowerFreq != pfsInitNoisePSpec->ha_iLowerFreq ||
		faInputPSpec.ha_iUpperFreq != pfsInitNoisePSpec->ha_iUpperFreq){

		return 0;
	}


	// 初期化
	if (faInputPSpec.ha_numCh != m_psSavedInitMCRA->micary_fsStateNoisePSpec.ha_numCh ||
		faInputPSpec.ha_iLowerFreq != m_psSavedInitMCRA->micary_fsStateNoisePSpec.ha_iLowerFreq ||
		faInputPSpec.ha_iUpperFreq != m_psSavedInitMCRA->micary_fsStateNoisePSpec.ha_iUpperFreq){

		this->Initialize(faInputPSpec.ha_iLowerFreq, faInputPSpec.ha_iUpperFreq, faInputPSpec.ha_numCh);
	}


	// 定常雑音推定
	if (PF_InitStateNoise(faInputPSpec, m_sPrmInitMCRA, m_psSavedInitMCRA, pfsInitNoisePSpec) != 1){
		return (0);
	}

	return (1);
}
// ---------------------------------------------------------------------
// 入力信号のMCRAパラメータの設定
int CPF_init_state_noise::SetPrmInitMCRA(MICARY_PrmInitMCRA sPrmInitMCRA)
{

	memcpy(&m_sPrmInitMCRA, &sPrmInitMCRA, sizeof(MICARY_PrmInitMCRA));

	return 1;
}
// ---------------------------------------------------------------------
