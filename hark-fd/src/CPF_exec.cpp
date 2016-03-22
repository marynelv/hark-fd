// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2007 Honda Research Institute Japan, Co., Ltd.

 \file CPF_exec.cpp
 \brief \~English Source file for execute Post-Filter. 
 \brief \~Japanese Post-Filterの実行クラスのソース(CPF)
 \author Sachiko Suzuki
 \date 1 Aug 2007
 \version $Id: CPF_exec.cpp 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>

// ---------------------------------------------------------------------
#include "CPF_exec.h"
#include "PF_Init.h"
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
// ---------------------------------------------------------------------
// コンストラクタ
CPF_exec::CPF_exec()
{
	m_pfsNoiseSpec = (MICARY_FloatSpecArray *)calloc(1,sizeof(MICARY_FloatSpecArray)) ;	//背景雑音Spectrum
	m_psSavedMCRA  = (MICARY_SavedMCRA *)calloc(1,sizeof(MICARY_SavedMCRA)) ;	//MCRA保存値(current frame)
	m_psSavedRev = (MICARY_SavedEstRev *)calloc(1,sizeof(MICARY_SavedEstRev)) ;	//残響成分保存値(current frame)
	m_psSavedEstSN = (MICARY_SavedEstSN *)calloc(1,sizeof(MICARY_SavedEstSN)) ;	//SN比推定保存値
	m_psSavedVoiceP = (MICARY_SavedVoiceP *)calloc(1,sizeof(MICARY_SavedVoiceP)) ;	//音声存在確率保存値
	m_psSavedMCRATmp = (MICARY_SavedMCRA *)calloc(1,sizeof(MICARY_SavedMCRA)) ;	//MCRA保存値Tmp(1つ前フレーム)
	m_pfsSavedLeak = (MICARY_FloatSpecArray *)calloc(1,sizeof(MICARY_FloatSpecArray)) ;	//漏れ保存値
	m_psSavedRevTmp = (MICARY_SavedEstRev *)calloc(1,sizeof(MICARY_SavedEstRev)) ;	//残響成分保存値Tmp(1つ前フレーム)
	m_pfsLamda_total = (MICARY_FloatSpecArray *)calloc(1,sizeof(MICARY_FloatSpecArray)) ;	

	// PF用Parameter構造体
	MICARY_PrmVoiceP sPrmVoiceP ;	//音声powerパラメータ
	MICARY_PrmEstSN sPrmEstSN ;	//S/N比推定パラメータ
	MICARY_PrmRevNoise sPrmRev ;	//残響成分パラメータ
	MICARY_PrmLeakNoise sPrmLeak ;	//漏れ成分パラメータ
	MICARY_PrmMCRA sPrmMCRA ;	//MCRAパラメータ	
	MICARY_PrmGain sPrmGain ;	//最適Gainパラメータ

	//ポインタに変更2008/2/15
	tmp_sSavedMCRA  = (MICARY_SavedMCRA *)calloc(1,sizeof(MICARY_SavedMCRA)) ;	//MCRA保存値(current frame)
	tmp_sSavedMCRATmp  = (MICARY_SavedMCRA *)calloc(1,sizeof(MICARY_SavedMCRA)) ;	//MCRA保存値(current frame)
	tmp_sSavedRev = (MICARY_SavedEstRev *)calloc(1,sizeof(MICARY_SavedEstRev)) ;	//残響成分保存値(current frame)
	tmp_sSavedRevTmp = (MICARY_SavedEstRev *)calloc(1,sizeof(MICARY_SavedEstRev)) ;	//残響成分保存値(current frame)
	tmp_sSavedEstSN = (MICARY_SavedEstSN *)calloc(1,sizeof(MICARY_SavedEstSN)) ;	//SN比推定保存値
	tmp_sSavedVoiceP = (MICARY_SavedVoiceP *)calloc(1,sizeof(MICARY_SavedVoiceP)) ;	//音声存在確率保存値
	tmp_fsSavedLeak = (MICARY_FloatSpecArray *)calloc(1,sizeof(MICARY_FloatSpecArray)) ;	//漏れ保存値
	tmp_fsLamda_total = (MICARY_FloatSpecArray *)calloc(1,sizeof(MICARY_FloatSpecArray)) ;	

	/*
	// add 2008/2/5
	
	AllocSavedMCRA(&tmp_sSavedMCRA,1,0,0) ;
	AllocSavedMCRA(&tmp_sSavedMCRATmp,1,0,0) ;
	AllocSavedRev(&tmp_sSavedRev,1,0,0) ; 
	AllocSavedRev(&tmp_sSavedRevTmp,1,0,0) ;
	AllocSavedSN(&tmp_sSavedEstSN,1,0,0) ; 
	AllocSavedVoiceP(&tmp_sSavedVoiceP,1,0,0);
	AllocFloatSpecArray(&tmp_fsSavedLeak,0,0,1);
	AllocFloatSpecArray(&tmp_fsLamda_total,0,0,1);

	//add 2008/2/12
	memset(&tmp_sSavedMCRA,0,sizeof(MICARY_SavedMCRA)) ;
	memset(&tmp_sSavedMCRATmp,0,sizeof(MICARY_SavedMCRA)) ;
	memset(&tmp_sSavedRev,0,sizeof(MICARY_SavedEstRev));
	memset(&tmp_sSavedRevTmp,0,sizeof(MICARY_SavedEstRev)) ;
	memset(&tmp_sSavedEstSN,0,sizeof(MICARY_SavedEstSN)) ;
	memset(&tmp_sSavedVoiceP,0,sizeof(MICARY_SavedVoiceP)) ;
	memset(&tmp_fsSavedLeak,0,sizeof(MICARY_FloatSpecArray)) ;
	memset(&tmp_fsLamda_total,0,sizeof(MICARY_FloatSpecArray)) ;
	*/
}

// ---------------------------------------------------------------------
// デストラクタ
CPF_exec::~CPF_exec()
{
	this ->Clear() ;
	if(m_pfsNoiseSpec){
	  free(m_pfsNoiseSpec) ;
	  m_pfsNoiseSpec = NULL ;
	}
	if(m_psSavedMCRA){	
	  free(m_psSavedMCRA) ;
	  m_psSavedMCRA = NULL ;
	}
	if(m_psSavedRev){	
	  free(m_psSavedRev) ;
	  m_psSavedRev = NULL ;
	}
	if(m_psSavedEstSN){
	  free(m_psSavedEstSN) ;
	  m_psSavedEstSN = NULL ;
	}
	if(m_psSavedVoiceP){
	  free(m_psSavedVoiceP) ;
	  m_psSavedVoiceP = NULL ;
	}
	if(m_psSavedMCRATmp){
	  free(m_psSavedMCRATmp) ;
	  m_psSavedMCRATmp = NULL ;
	}
	if(m_pfsSavedLeak){
	  free(m_pfsSavedLeak) ;
	  m_pfsSavedLeak = NULL ;
	}
	if(m_psSavedRevTmp){
	  free(m_psSavedRevTmp) ;
	  m_psSavedRevTmp = NULL ;
	}
	if(m_pfsLamda_total){
	  free(m_pfsLamda_total) ;
	  m_pfsLamda_total = NULL ;
	}
	
	// add 2008/2/15
	if(tmp_sSavedMCRA){
	  FreeSavedMCRA(tmp_sSavedMCRA) ;
	  free(tmp_sSavedMCRA) ;
	  tmp_sSavedMCRA = NULL ;
	}
	if(tmp_sSavedMCRATmp){
		FreeSavedMCRA(tmp_sSavedMCRATmp) ;
		free(tmp_sSavedMCRATmp) ;
		tmp_sSavedMCRATmp = NULL ;
	}
	if(tmp_sSavedRev){
		FreeSavedRev(tmp_sSavedRev) ;
		free(tmp_sSavedRev) ;
		tmp_sSavedRev = NULL ;
	}
	if(tmp_sSavedRevTmp){
	  FreeSavedRev(tmp_sSavedRevTmp) ;
	  free(tmp_sSavedRevTmp) ;
	  tmp_sSavedRevTmp = NULL ;
	}
	if(tmp_sSavedEstSN){
	  FreeSavedSN(tmp_sSavedEstSN) ;
	  free(tmp_sSavedEstSN) ;
	  tmp_sSavedEstSN = NULL ;
	}
	if(tmp_sSavedVoiceP){
	  FreeSavedVoiceP(tmp_sSavedVoiceP);
	  free(tmp_sSavedVoiceP) ;
	  tmp_sSavedVoiceP = NULL ;
	}
	if(tmp_fsSavedLeak){
	  FreeFloatSpecArray(tmp_fsSavedLeak);
	  free(tmp_fsSavedLeak) ;
	  tmp_fsSavedLeak = NULL ;

	}
	if(tmp_fsLamda_total){
	  FreeFloatSpecArray(tmp_fsLamda_total);
	  free(tmp_fsLamda_total) ;
	  tmp_fsLamda_total = NULL ;
	}
	
}

// ---------------------------------------------------------------------
// Post-Filter用保存値のクリア
int CPF_exec::Clear()
{
  if(m_pfsNoiseSpec){
    FreeFloatSpecArray(m_pfsNoiseSpec) ;
  }
  if(m_psSavedMCRA){
	FreeSavedMCRA(m_psSavedMCRA) ;
  }
  if(m_psSavedRev){
	FreeSavedRev(m_psSavedRev) ;
  }
  if(m_psSavedEstSN){
	FreeSavedSN(m_psSavedEstSN) ;
  }
  if(m_psSavedVoiceP){
	FreeSavedVoiceP(m_psSavedVoiceP) ;
  }
  if(m_psSavedMCRATmp){
	FreeSavedMCRA(m_psSavedMCRATmp) ;
  }
  if(m_pfsSavedLeak){
	FreeFloatSpecArray(m_pfsSavedLeak) ;
  }
  if(m_psSavedRevTmp){
	FreeSavedRev(m_psSavedRevTmp) ;
  }
  if(m_pfsLamda_total){
	FreeFloatSpecArray(m_pfsLamda_total) ;
  }

	return 1 ;
}

// ---------------------------------------------------------------------
//Post-Filter用保存値配列の確保と作成
int CPF_exec::Initialize(int iLower_freq,int iUpper_freq,int iNpos,int iNproc) 
{
	this->Clear() ;
	// Initialize 2008.10.31
	AllocFloatSpecArray(m_pfsNoiseSpec,0,0,0) ;
	AllocSavedMCRA(m_psSavedMCRA,0,0,0) ;
	AllocSavedRev(m_psSavedRev,0,0,0) ;
	AllocSavedSN(m_psSavedEstSN,0,0,0) ;
	AllocSavedVoiceP(m_psSavedVoiceP,0,0,0) ;
	AllocSavedMCRA(m_psSavedMCRATmp,0,0,0) ;
	AllocFloatSpecArray(m_pfsSavedLeak,0,0,0) ;
	AllocSavedRev(m_psSavedRevTmp,0,0,0) ;
	AllocFloatSpecArray(m_pfsLamda_total,0,0,0) ;

	// Set size
	AllocFloatSpecArray(m_pfsNoiseSpec,(HA_Num)iNpos,(HA_Integer)iLower_freq,(HA_Integer)iUpper_freq) ;
	AllocSavedMCRA(m_psSavedMCRA,(HA_Integer)iUpper_freq,(HA_Integer)iLower_freq,(HA_Integer)iNpos) ;
	AllocSavedRev(m_psSavedRev,(HA_Integer)iUpper_freq,(HA_Integer)iLower_freq,(HA_Integer)iNpos) ;
	AllocSavedSN(m_psSavedEstSN,(HA_Integer)iUpper_freq,(HA_Integer)iLower_freq,(HA_Integer)iNpos) ;
	AllocSavedVoiceP(m_psSavedVoiceP,(HA_Integer)iUpper_freq,(HA_Integer)iLower_freq,(HA_Integer)iNpos) ;
	AllocSavedMCRA(m_psSavedMCRATmp,(HA_Integer)iUpper_freq,(HA_Integer)iLower_freq,(HA_Integer)iNpos) ;
	AllocFloatSpecArray(m_pfsSavedLeak,(HA_Num)iNpos,(HA_Integer)iLower_freq,(HA_Integer)iUpper_freq) ;
	AllocSavedRev(m_psSavedRevTmp,(HA_Integer)iUpper_freq,(HA_Integer)iLower_freq,(HA_Integer)iNpos) ;
	AllocFloatSpecArray(m_pfsLamda_total,(HA_Num)iNpos,(HA_Integer)iLower_freq,(HA_Integer)iUpper_freq) ;


	
	return 1 ;
}

// ---------------------------------------------------------------------
// Post-Filter用パラメータの代入
// Gain
int CPF_exec :: Set_PF_PrmGain(float fMinGain)
{
	this->sPrmGain.ha_fMinGain = (HA_Float)fMinGain ;

	return 1 ;
}
// ---------------------------------------------------------------------
// Leak
int CPF_exec :: Set_PF_PrmLeak(float fLeak_factor,float fOver_cancel)
{
	this ->sPrmLeak.ha_fAlp_leak = (HA_Float)(fLeak_factor * fOver_cancel) ;
	
	return 1 ;
}
// --------------------------------------------------------------------
// MCRA
int CPF_exec ::Set_PF_PrmMCRA(float fNoise_compens,float fSmooth_spec_factor,float fAmp_leak_factor,
							  float fBacknoise_est_factor,float fLeak_factor,float fSs_floor,int iL,float fDelta)
{	
	this ->sPrmMCRA.ha_fAlp_d = (HA_Float)fBacknoise_est_factor ;
	this ->sPrmMCRA.ha_fAlp_s = (HA_Float)fSmooth_spec_factor ;
	this ->sPrmMCRA.ha_fDelta = (HA_Float)fDelta ;
	this ->sPrmMCRA.ha_fLeak_ampleak_factor = (HA_Float)(fLeak_factor * fAmp_leak_factor) ;
	this ->sPrmMCRA.ha_fNoise_compense = (HA_Float)fNoise_compens ;
	this ->sPrmMCRA.ha_fSs_floor = (HA_Float)fSs_floor ;
	this ->sPrmMCRA.ha_iL = (HA_Integer)iL ;
	
	return 1 ;
}
// --------------------------------------------------------------------
// Rev
int CPF_exec ::Set_PF_PrmRev(float fReverb_decay,float fReverb_level)
{
	this ->sPrmRev.ha_fDelta = (HA_Float)fReverb_level ;
	this ->sPrmRev.ha_fGamma = (HA_Float)fReverb_decay ;
	
	return 1 ;
}
// --------------------------------------------------------------------
// SNratio
int CPF_exec :: Set_PF_PrmSN (float fPrior_factor,float fAlp_pmag,float fAlp_pmin,
							  float fXi_max,float fTheta_max,float fTheta_min)
{
	this ->sPrmEstSN.ha_fA = (HA_Float)fPrior_factor ;
	this ->sPrmEstSN.ha_fAlp_pmag = (HA_Float)fAlp_pmag ;
	this ->sPrmEstSN.ha_fAlp_pmin = (HA_Float)fAlp_pmin ;
	this ->sPrmEstSN.ha_fTheta_max = (HA_Float)fTheta_max ;
	this ->sPrmEstSN.ha_fTheta_min = (HA_Float)fTheta_min ;
	this ->sPrmEstSN.ha_fXi_max = (HA_Float)fXi_max ;

	return 1;
}
// --------------------------------------------------------------------
// 
int CPF_exec :: Set_PF_PrmVoiceP(float fSmooth_prior_factor ,float fZminF ,float fZmaxF ,float fZminG,
								 float fZmaxG,float fZminL,float fZmaxL,int iBwFen,
								 int iBwFst,int iBwG,int iBwL,float fZthF,float fZpmin,
								 float fZpmax,float fAmpF,float fAmpG,float fAmpL,float fQmin,float fQmax)
{
	this ->sPrmVoiceP.ha_fAmpF = (HA_Float)fAmpF ;
	this ->sPrmVoiceP.ha_fAmpG = (HA_Float)fAmpG ;
	this ->sPrmVoiceP.ha_fAmpL = (HA_Float)fAmpL ;
	this ->sPrmVoiceP.ha_fB = (HA_Float)fSmooth_prior_factor ;
	this ->sPrmVoiceP.ha_fBwFen = (HA_Integer)iBwFen ;
	this ->sPrmVoiceP.ha_fBwFst = (HA_Integer)iBwFst ;
	this ->sPrmVoiceP.ha_fBwG = (HA_Integer)iBwG ;
	this ->sPrmVoiceP.ha_fBwL = (HA_Integer)iBwL ;
	this ->sPrmVoiceP.ha_fQmax = (HA_Float)fQmax ;
	this ->sPrmVoiceP.ha_fQmin = (HA_Float)fQmin ;
	this ->sPrmVoiceP.ha_fZmaxF = (HA_Float)fZmaxF ;
	this ->sPrmVoiceP.ha_fZmaxG = (HA_Float)fZmaxG ;
	this ->sPrmVoiceP.ha_fZmaxL = (HA_Float)fZmaxL ;
	this ->sPrmVoiceP.ha_fZminF = (HA_Float)fZminF ;
	this ->sPrmVoiceP.ha_fZminG = (HA_Float)fZminG ;
	this ->sPrmVoiceP.ha_fZminL = (HA_Float)fZminL ;
	this ->sPrmVoiceP.ha_fZpmax = (HA_Float)fZpmax ;
	this ->sPrmVoiceP.ha_fZpmin = (HA_Float)fZpmin ;
	this ->sPrmVoiceP.ha_fZthF = (HA_Float)fZthF ;
	
	return 1;
}
// --------------------------------------------------------------------
// Post-Filterの実行
int CPF_exec :: ExecutePF(MICARY_ComplexSpecArray csInputCSpec,
						  MICARY_ComplexSpecArray *pcsOutputCSpec,
						  MICARY_FloatSpecArray *pfsEstNoise)
{
  //MICARY_FloatSpecArray *pfsLamda_total ; // total noise
	int Nch,iLowerFreq,iUpperFreq ,k;

	if(!&csInputCSpec){
	  	this ->Clear() ;
		if(m_pfsNoiseSpec){
		  free(m_pfsNoiseSpec) ;
		  m_pfsNoiseSpec = NULL ;
		}
		if(m_psSavedMCRA){	
		  free(m_psSavedMCRA) ;
		  m_psSavedMCRA = NULL ;
		}
		if(m_psSavedRev){	
		  free(m_psSavedRev) ;
		  m_psSavedRev = NULL ;
		}
		if(m_psSavedEstSN){
		  free(m_psSavedEstSN) ;
		  m_psSavedEstSN = NULL ;
		}
		if(m_psSavedVoiceP){
		  free(m_psSavedVoiceP) ;
		  m_psSavedVoiceP = NULL ;
		}
		if(m_psSavedMCRATmp){
		  free(m_psSavedMCRATmp) ;
		  m_psSavedMCRATmp = NULL ;
		}
		if(m_pfsSavedLeak){
		  free(m_pfsSavedLeak) ;
		  m_pfsSavedLeak = NULL ;
		}
		if(m_psSavedRevTmp){
		  free(m_psSavedRevTmp) ;
		  m_psSavedRevTmp = NULL ;
		}
		if(m_pfsLamda_total){
		  free(m_pfsLamda_total) ;
		  m_pfsLamda_total = NULL ;
		}
	
		// add 2008/2/5
		if(tmp_sSavedMCRA){
		  FreeSavedMCRA(tmp_sSavedMCRA) ;
		  free(tmp_sSavedMCRA) ;
		  tmp_sSavedMCRA = NULL ;
		}
		if(tmp_sSavedMCRATmp){
		  FreeSavedMCRA(tmp_sSavedMCRATmp) ;
		  free(tmp_sSavedMCRATmp) ;
		  tmp_sSavedMCRATmp = NULL ;
		}
		if(tmp_sSavedRev){
		  FreeSavedRev(tmp_sSavedRev) ;
		  free(tmp_sSavedRev) ;
		  tmp_sSavedRev= NULL ;
		}
		if(tmp_sSavedRevTmp){
		  FreeSavedRev(tmp_sSavedRevTmp) ;
		  free(tmp_sSavedRevTmp) ;
		  tmp_sSavedRevTmp = NULL ;
		}
		if(tmp_sSavedEstSN){
		  FreeSavedSN(tmp_sSavedEstSN) ;
		  free(tmp_sSavedEstSN) ;
		  tmp_sSavedEstSN = NULL ;
		}
		if(tmp_sSavedVoiceP){
		  FreeSavedVoiceP(tmp_sSavedVoiceP);
		  free(tmp_sSavedVoiceP) ;
		  tmp_sSavedVoiceP = NULL ;
		}
		if(tmp_fsSavedLeak){
		  FreeFloatSpecArray(tmp_fsSavedLeak);
		  free(tmp_fsSavedLeak) ;
		  tmp_fsSavedLeak = NULL ;
		}
		if(tmp_fsLamda_total){
		  FreeFloatSpecArray(tmp_fsLamda_total);
		  free(tmp_fsLamda_total) ;
		  tmp_fsLamda_total = NULL ;
		}
		return 0 ;
	}
	if(!pcsOutputCSpec){
	  	this ->Clear() ;
		if(m_pfsNoiseSpec){
		  free(m_pfsNoiseSpec) ;
		  m_pfsNoiseSpec = NULL ;
		}
		if(m_psSavedMCRA){	
		  free(m_psSavedMCRA) ;
		  m_psSavedMCRA = NULL ;
		}
		if(m_psSavedRev){	
		  free(m_psSavedRev) ;
		  m_psSavedRev = NULL ;
		}
		if(m_psSavedEstSN){
		  free(m_psSavedEstSN) ;
		  m_psSavedEstSN = NULL ;

		}
		if(m_psSavedVoiceP){
		  free(m_psSavedVoiceP) ;
		  m_psSavedVoiceP = NULL ;
		}
		if(m_psSavedMCRATmp){
		  free(m_psSavedMCRATmp) ;
		  m_psSavedMCRATmp = NULL ;
		}
		if(m_pfsSavedLeak){
		  free(m_pfsSavedLeak) ;
		  m_pfsSavedLeak = NULL ;
		}
		if(m_psSavedRevTmp){
		  free(m_psSavedRevTmp) ;
		  m_psSavedRevTmp = NULL ;
		}
		if(m_pfsLamda_total){
		  free(m_pfsLamda_total) ;
		  m_pfsLamda_total = NULL ;
		}
	
		// add 2008/2/5
		if(tmp_sSavedMCRA){
		  FreeSavedMCRA(tmp_sSavedMCRA) ;
		  free(tmp_sSavedMCRA) ;
		  tmp_sSavedMCRA = NULL ;
		}
		if(tmp_sSavedMCRATmp){
		  FreeSavedMCRA(tmp_sSavedMCRATmp) ;
		  free(tmp_sSavedMCRATmp) ;
		  tmp_sSavedMCRATmp = NULL ;
		}
		if(tmp_sSavedRev){
		  FreeSavedRev(tmp_sSavedRev) ;
		  free(tmp_sSavedRev) ;
		  tmp_sSavedRev = NULL ;
		}
		if(tmp_sSavedRevTmp){
		  FreeSavedRev(tmp_sSavedRevTmp) ;
		  free(tmp_sSavedRevTmp) ;
		  tmp_sSavedRevTmp = NULL ;
		}
		if(tmp_sSavedEstSN){
		  FreeSavedSN(tmp_sSavedEstSN) ;
		  free(tmp_sSavedEstSN) ;
		  tmp_sSavedEstSN = NULL ;
		}
		if(tmp_sSavedVoiceP){
		  FreeSavedVoiceP(tmp_sSavedVoiceP);
		  free(tmp_sSavedVoiceP) ;
		  tmp_sSavedVoiceP = NULL ;
		}
		if(tmp_fsSavedLeak){
		  FreeFloatSpecArray(tmp_fsSavedLeak);
		  free(tmp_fsSavedLeak) ;
		  tmp_fsSavedLeak = NULL ;
		}
		if(tmp_fsLamda_total){
		  FreeFloatSpecArray(tmp_fsLamda_total);
		  free(tmp_fsLamda_total) ;
		  tmp_fsLamda_total = NULL ;
		}
	
		return 0 ;
	}
	if(!pfsEstNoise){
	  this ->Clear() ;
		if(m_pfsNoiseSpec){
		  free(m_pfsNoiseSpec) ;
		  m_pfsNoiseSpec = NULL ;
		}
		if(m_psSavedMCRA){	
		  free(m_psSavedMCRA) ;
		  m_psSavedMCRA = NULL ;
		}
		if(m_psSavedRev){	
		  free(m_psSavedRev) ;
		  m_psSavedRev = NULL ;
		}
		if(m_psSavedEstSN){
		  free(m_psSavedEstSN) ;
		  m_psSavedEstSN = NULL ;
		}
		if(m_psSavedVoiceP){
		  free(m_psSavedVoiceP) ;
		  m_psSavedVoiceP = NULL ;
		}
		if(m_psSavedMCRATmp){
		  free(m_psSavedMCRATmp) ;
		  m_psSavedMCRATmp = NULL ;
		}
		if(m_pfsSavedLeak){
		  free(m_pfsSavedLeak) ;
		  m_pfsSavedLeak = NULL ;
		}
		if(m_psSavedRevTmp){
		  free(m_psSavedRevTmp) ;
		  m_psSavedRevTmp = NULL ;
		}
		if(m_pfsLamda_total){
		  free(m_pfsLamda_total) ;
		  m_pfsLamda_total = NULL ;
		}
	
		// add 2008/2/5
		if(tmp_sSavedMCRA){
		  FreeSavedMCRA(tmp_sSavedMCRA) ;
		  free(tmp_sSavedMCRA) ;
		  tmp_sSavedMCRA = NULL ;
		}
		if(tmp_sSavedMCRATmp){
		  FreeSavedMCRA(tmp_sSavedMCRATmp) ;
		  free(tmp_sSavedMCRATmp) ;
		  tmp_sSavedMCRATmp = NULL ;
		}
		if(tmp_sSavedRev){
		  FreeSavedRev(tmp_sSavedRev) ;
		  free(tmp_sSavedRev) ;
		  tmp_sSavedRev = NULL ;
		}
		if(tmp_sSavedRevTmp){
		  FreeSavedRev(tmp_sSavedRevTmp) ;
		  free(tmp_sSavedRevTmp) ;
		  tmp_sSavedRevTmp = NULL ;
		}
		if(tmp_sSavedEstSN){
		  FreeSavedSN(tmp_sSavedEstSN) ;
		  free(tmp_sSavedEstSN) ;
		  tmp_sSavedEstSN = NULL ;
		}
		if(tmp_sSavedVoiceP){
		  FreeSavedVoiceP(tmp_sSavedVoiceP);
		  free(tmp_sSavedVoiceP) ;
		  tmp_sSavedVoiceP = NULL ;
		}
		if(tmp_fsSavedLeak){
		  FreeFloatSpecArray(tmp_fsSavedLeak);
		  free(tmp_fsSavedLeak) ;
		  tmp_fsSavedLeak = NULL ;
		}
		if(tmp_fsLamda_total){
		  FreeFloatSpecArray(tmp_fsLamda_total);
		  free(tmp_fsLamda_total) ;
		  tmp_fsLamda_total = NULL ;
		}
		return 0 ;
	}
	Nch = (int)csInputCSpec.ha_numCh ;
	iLowerFreq = (int)csInputCSpec.ha_iLowerFreq ;
	iUpperFreq = (int)csInputCSpec.ha_iUpperFreq ;

	//pfsLamda_total = (MICARY_FloatSpecArray *)calloc(1,sizeof(MICARY_FloatSpecArray)) ;	
	//AllocFloatSpecArray(pfsLamda_total,Nch,iLowerFreq,iUpperFreq) ;
	/*
	if(pfsEstNoise->micary_faSpec.ha_numValue != csInputCSpec.micary_cmpaSpec.ha_numValue){
		FreeFloatSpecArray(pfsEstNoise);
        AllocFloatSpecArray(pfsEstNoise,Nch,iLowerFreq,iUpperFreq) ;
	}
	*/
	
	//	std::cout << "Number of ch = " << Nch << std::endl ;
	//std::cout << "Lower freq line = " << iLowerFreq <<std::endl;
	//std::cout << "Upper freq line = " << iUpperFreq <<std::endl ;
	//int ii;
	//  //printf("%d ",iUpperFreq);
	//  for (ii=0;ii<Nch;ii++)
	//  {
	//	printf("%f %f ",m_psSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[ii*(iUpperFreq+1)],m_psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[ii*(iUpperFreq+1)]);
	//  }
	//printf("\n");
	if(PF_ExecutePostFilter(csInputCSpec,pcsOutputCSpec,this->m_psSavedMCRA,
		this->m_psSavedMCRATmp,this->sPrmMCRA,this->m_pfsSavedLeak,this->sPrmLeak,
		this->m_psSavedRev,this->m_psSavedRevTmp,this->sPrmRev,this->m_psSavedEstSN,this->sPrmEstSN,
		this->m_psSavedVoiceP,this->sPrmVoiceP,/*this->sPrmGain,*/this->m_pfsLamda_total) != 1)
	{
		std :: cout << "Fail to executing Post-Filter." << std ::endl;
		//FreeFloatSpecArray(pfsLamda_total) ;
		//free(pfsLamda_total) ;
		this ->Clear() ;
		if(m_pfsNoiseSpec){
		  free(m_pfsNoiseSpec) ;
		  m_pfsNoiseSpec = NULL ;
		}
		if(m_psSavedMCRA){	
		  free(m_psSavedMCRA) ;
		  m_psSavedMCRA = NULL ;
		}
		if(m_psSavedRev){	
		  free(m_psSavedRev) ;
		  m_psSavedRev = NULL ;
		}
		if(m_psSavedEstSN){
		  free(m_psSavedEstSN) ;
		  m_psSavedEstSN = NULL ;
		}
		if(m_psSavedVoiceP){
		  free(m_psSavedVoiceP) ;
		  m_psSavedVoiceP = NULL ;
		}
		if(m_psSavedMCRATmp){
		  free(m_psSavedMCRATmp) ;
		  m_psSavedMCRATmp = NULL ;
		}
		if(m_pfsSavedLeak){
		  free(m_pfsSavedLeak) ;
		  m_pfsSavedLeak = NULL ;
		}
		if(m_psSavedRevTmp){
		  free(m_psSavedRevTmp) ;
		  m_psSavedRevTmp = NULL ;
		}
		if(m_pfsLamda_total){
		  free(m_pfsLamda_total) ;
		  m_pfsLamda_total = NULL ;
		}
	
		// add 2008/2/5
		if(tmp_sSavedMCRA){
		  FreeSavedMCRA(tmp_sSavedMCRA) ;
		  free(tmp_sSavedMCRA) ;
		  tmp_sSavedMCRA = NULL ;
		}
		if(tmp_sSavedMCRATmp){
		  FreeSavedMCRA(tmp_sSavedMCRATmp) ;
		  free(tmp_sSavedMCRATmp) ;
		  tmp_sSavedMCRATmp = NULL ;
		}
		if(tmp_sSavedRev){
		  FreeSavedRev(tmp_sSavedRev) ;
		  free(tmp_sSavedRev) ;
		  tmp_sSavedRev = NULL ;
		}
		if(tmp_sSavedRevTmp){
		  FreeSavedRev(tmp_sSavedRevTmp) ;
		  free(tmp_sSavedRevTmp) ;
		  tmp_sSavedRevTmp = NULL ;
		}
		if(tmp_sSavedEstSN){
		  FreeSavedSN(tmp_sSavedEstSN) ;
		  free(tmp_sSavedEstSN) ;
		  tmp_sSavedEstSN = NULL ;
		}
		if(tmp_sSavedVoiceP){
		  FreeSavedVoiceP(tmp_sSavedVoiceP);
		  free(tmp_sSavedVoiceP) ;
		  tmp_sSavedVoiceP = NULL ;
		}
		if(tmp_fsSavedLeak){
		  FreeFloatSpecArray(tmp_fsSavedLeak);
		  free(tmp_fsSavedLeak) ;
		  tmp_fsSavedLeak = NULL ;

		}
		if(tmp_fsLamda_total){
		  FreeFloatSpecArray(tmp_fsLamda_total);
		  free(tmp_fsLamda_total) ;
		  tmp_fsLamda_total = NULL ;
		}	

		return 0 ;
	}
	
	  //printf("%d ",iUpperFreq);
	/*  for (ii=0;ii<Nch;ii++)
	  {
		printf("%f %f ",m_psSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[ii*(iUpperFreq+1)],m_psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[ii*(iUpperFreq+1)]);
	  }
	printf("\n");*/
	// Output State Noise
	for (k = 0 ; k < this->m_psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_numValue ; k++){
	pfsEstNoise->micary_faSpec.ha_fValue[k] = this->m_psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[k] ;
	}

	// FreeFloatSpecArray(pfsLamda_total) ;
	//free(pfsLamda_total) ;

	return 1 ;
}
// --------------------------------------------------------------------
// SavedArrayの初期化、入力信号から推定定常雑音
int CPF_exec :: InitSavedArray(MICARY_FloatSpecArray faInStatNoise,int iLower_freq,
							   int iUpper_freq,int iNpos,int iNch)
{
  if(!&faInStatNoise){
    this ->Clear() ;
		if(m_pfsNoiseSpec){
		  free(m_pfsNoiseSpec) ;
		  m_pfsNoiseSpec = NULL ;
		}
		if(m_psSavedMCRA){	
		  free(m_psSavedMCRA) ;
		  m_psSavedMCRA = NULL ;
		}
		if(m_psSavedRev){	
		  free(m_psSavedRev) ;
		  m_psSavedRev = NULL ;
		}
		if(m_psSavedEstSN){
		  free(m_psSavedEstSN) ;
		  m_psSavedEstSN = NULL ;
		}
		if(m_psSavedVoiceP){
		  free(m_psSavedVoiceP) ;
		  m_psSavedVoiceP = NULL ;
		}
		if(m_psSavedMCRATmp){
		  free(m_psSavedMCRATmp) ;
		  m_psSavedMCRATmp = NULL ;
		}
		if(m_pfsSavedLeak){
		  free(m_pfsSavedLeak) ;
		  m_pfsSavedLeak = NULL ;
		}
		if(m_psSavedRevTmp){
		  free(m_psSavedRevTmp) ;
		  m_psSavedRevTmp = NULL ;
		}
		if(m_pfsLamda_total){
		  free(m_pfsLamda_total) ;
		  m_pfsLamda_total = NULL ;
		}
	
		// add 2008/2/15
		if(tmp_sSavedMCRA){
		  FreeSavedMCRA(tmp_sSavedMCRA) ;
		  free(tmp_sSavedMCRA) ;
		  tmp_sSavedMCRA = NULL ;
		}
		if(tmp_sSavedMCRATmp){
		  FreeSavedMCRA(tmp_sSavedMCRATmp) ;
		  free(tmp_sSavedMCRATmp) ;
		  tmp_sSavedMCRATmp = NULL ;
		}
		if(tmp_sSavedRev){
		  FreeSavedRev(tmp_sSavedRev) ;
		  free(tmp_sSavedRev) ;
		  tmp_sSavedRev = NULL ;
		}
		if(tmp_sSavedRevTmp){
		  FreeSavedRev(tmp_sSavedRevTmp) ;
		  free(tmp_sSavedRevTmp) ;
		  tmp_sSavedRevTmp = NULL ;
		}
		if(tmp_sSavedEstSN){
		  FreeSavedSN(tmp_sSavedEstSN) ;
		  free(tmp_sSavedEstSN) ;
		  tmp_sSavedEstSN = NULL ;
		}
		if(tmp_sSavedVoiceP){
		  FreeSavedVoiceP(tmp_sSavedVoiceP);
		  free(tmp_sSavedVoiceP) ;
		  tmp_sSavedVoiceP = NULL ;
		}
		if(tmp_fsSavedLeak){
		  FreeFloatSpecArray(tmp_fsSavedLeak);
		  free(tmp_fsSavedLeak) ;
		  tmp_fsSavedLeak = NULL ;
		}
		if(tmp_fsLamda_total){
		  FreeFloatSpecArray(tmp_fsLamda_total);
		  free(tmp_fsLamda_total) ;
		  tmp_fsLamda_total = NULL ;
		}
		return 0 ;
  }

  
  // std::cout << "Start InitTmp" << std::endl ;
	if(PFInitTmp(this->m_psSavedMCRATmp,this->m_psSavedRev,this->m_psSavedEstSN,
		this->m_psSavedVoiceP,&faInStatNoise.micary_faSpec,(HA_Float)this->sPrmMCRA.ha_fNoise_compense,
		(HA_Integer)iLower_freq,(HA_Integer)iUpper_freq,(HA_Integer)iNpos,(HA_Integer)iNch) != 1)
	
	{
		std ::cout << "Fail to Initialize Saved array and Estimated state noise." << std ::endl;
		this ->Clear() ;
		if(m_pfsNoiseSpec){
		  free(m_pfsNoiseSpec) ;
		  m_pfsNoiseSpec = NULL ;
		}
		if(m_psSavedMCRA){	
		  free(m_psSavedMCRA) ;
		  m_psSavedMCRA = NULL ;
		}
		if(m_psSavedRev){	
		  free(m_psSavedRev) ;
		  m_psSavedRev = NULL ;
		}
		if(m_psSavedEstSN){
		  free(m_psSavedEstSN) ;
		  m_psSavedEstSN = NULL ;
		}
		if(m_psSavedVoiceP){
		  free(m_psSavedVoiceP) ;
		  m_psSavedVoiceP = NULL ;
		}
		if(m_psSavedMCRATmp){
		  free(m_psSavedMCRATmp) ;
		  m_psSavedMCRATmp = NULL ;
		}
		if(m_pfsSavedLeak){
		  free(m_pfsSavedLeak) ;
		  m_pfsSavedLeak = NULL ;
		}
		if(m_psSavedRevTmp){
		  free(m_psSavedRevTmp) ;
		  m_psSavedRevTmp = NULL ;
		}
		if(m_pfsLamda_total){
		  free(m_pfsLamda_total) ;
		  m_pfsLamda_total = NULL ;
		}
	
		// add 2008/2/5
		if(tmp_sSavedMCRA){
		  FreeSavedMCRA(tmp_sSavedMCRA) ;
		  free(tmp_sSavedMCRA) ;
		  tmp_sSavedMCRA = NULL ;
		}
		if(tmp_sSavedMCRATmp){
		  FreeSavedMCRA(tmp_sSavedMCRATmp) ;
		  free(tmp_sSavedMCRATmp) ;
		  tmp_sSavedMCRATmp = NULL ;
		}
		if(tmp_sSavedRev){
		  FreeSavedRev(tmp_sSavedRev) ;
		  free(tmp_sSavedRev) ;
		  tmp_sSavedRev = NULL ;
		}
		if(tmp_sSavedRevTmp){
		  FreeSavedRev(tmp_sSavedRevTmp) ;
		  free(tmp_sSavedRevTmp) ;
		  tmp_sSavedRevTmp = NULL ;
		}
		if(tmp_sSavedEstSN){
		  FreeSavedSN(tmp_sSavedEstSN) ;
		  free(tmp_sSavedEstSN) ;
		  tmp_sSavedEstSN = NULL ;

		}
		if(tmp_sSavedVoiceP){
		  FreeSavedVoiceP(tmp_sSavedVoiceP);
		  free(tmp_sSavedVoiceP) ;
		  tmp_sSavedVoiceP = NULL ;
		}
		if(tmp_fsSavedLeak){
		  FreeFloatSpecArray(tmp_fsSavedLeak);
		  free(tmp_fsSavedLeak) ;
		  tmp_fsSavedLeak = NULL ;
		}
		if(tmp_fsLamda_total){
		  FreeFloatSpecArray(tmp_fsLamda_total);
		  free(tmp_fsLamda_total) ;
		  tmp_fsLamda_total = NULL ;
		}
		return 0 ;
	}

	return 1 ;
}
// ---------------------------------------------------------------------
//音源移動時の処理
int CPF_exec ::ChangeSrcPos(MICARY_FloatSpecArray faInStatNoise,MICARY_IntArray iaSrcCur,MICARY_IntArray iaSrcIni)
{
	int s,n,k,m;
	int iLowerFreq,iUpperFreq,Npos_ini,Npos_cur,Nfreq ;

	/*
	MICARY_SavedMCRA tmp_sSavedMCRA,tmp_sSavedMCRATmp ;//MCRA保存値(temporary)
	MICARY_SavedEstRev tmp_sSavedRev,tmp_sSavedRevTmp ;//残響成分保存値(temporary)
	MICARY_SavedEstSN tmp_sSavedEstSN ;	//SN比推定保存値(temporary)
	MICARY_SavedVoiceP tmp_sSavedVoiceP ;	//音声存在確率保存値(temporary)
	MICARY_FloatSpecArray tmp_fsSavedLeak ;	//漏れ保存値(temporary)
	MICARY_FloatSpecArray tmp_fsLamda_total ; // total lamda(temporary)
	*/

	//tmp_sSavedMCRA = (MICARY_SavedMCRA *)calloc(1,sizeof(MICARY_SavedMCRA)) ;		//tmp_sSavedRev = (MICARY_SavedEstRev *)calloc(1,sizeof(MICARY_SavedEstRev)) ; 
	//tmp_sSavedEstSN = (MICARY_SavedEstSN *)calloc(1,sizeof(MICARY_SavedEstSN)) ; 
	//tmp_sSavedVoiceP = (MICARY_SavedVoiceP *)calloc(1,sizeof(MICARY_SavedVoiceP));
	//tmp_fsSavedLeak = (MICARY_FloatSpecArray *)calloc(1,sizeof(MICARY_FloatSpecArray)) ;

	Npos_ini = iaSrcIni.ha_numValue ;
	Npos_cur = iaSrcCur.ha_numValue ;
	
	iLowerFreq = this ->m_pfsSavedLeak->ha_iLowerFreq ;
	iUpperFreq = this ->m_pfsSavedLeak->ha_iUpperFreq ;

	Nfreq = iUpperFreq - iLowerFreq + 1 ; 
	
	// alloc
	if(tmp_sSavedMCRA->micary_fsS.micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_sSavedMCRA){
	    FreeSavedMCRA(tmp_sSavedMCRA) ;
	  }
	  AllocSavedMCRA(tmp_sSavedMCRA,iUpperFreq,iLowerFreq,Npos_ini) ;
	}
	if(tmp_sSavedMCRATmp->micary_fsS.micary_faSpec.ha_numValue != Nfreq *Npos_ini){
	  if(tmp_sSavedMCRATmp){
	    FreeSavedMCRA(tmp_sSavedMCRATmp) ;
	  }
	  AllocSavedMCRA(tmp_sSavedMCRATmp,iUpperFreq,iLowerFreq,Npos_ini) ;
	}
	if(tmp_sSavedRev->micary_fsRevPSpec.micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_sSavedRev){
	    FreeSavedRev(tmp_sSavedRev) ;
	  }
	  AllocSavedRev(tmp_sSavedRev,iUpperFreq,iLowerFreq,Npos_ini) ;
	}
	if(tmp_sSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_sSavedRevTmp){
	    FreeSavedRev(tmp_sSavedRevTmp) ;
	  }
	  AllocSavedRev(tmp_sSavedRevTmp,iUpperFreq,iLowerFreq,Npos_ini) ;
	}
	if(tmp_sSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_numValue != Nfreq*Npos_ini){
	  if(tmp_sSavedEstSN){
	    FreeSavedSN(tmp_sSavedEstSN) ;
	  }
	  AllocSavedSN(tmp_sSavedEstSN,iUpperFreq,iLowerFreq,Npos_ini) ;
	}
	if(tmp_sSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_sSavedVoiceP){
	    FreeSavedVoiceP(tmp_sSavedVoiceP) ;
	  }
	  AllocSavedVoiceP(tmp_sSavedVoiceP,iUpperFreq,iLowerFreq,Npos_ini) ;
	}
	if(tmp_fsSavedLeak->micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_fsSavedLeak){
	    FreeFloatSpecArray(tmp_fsSavedLeak) ;
	  }
	  AllocFloatSpecArray(tmp_fsSavedLeak,Npos_ini,iLowerFreq,iUpperFreq) ;
	}
	if(tmp_fsLamda_total->micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_fsLamda_total){
	    FreeFloatSpecArray(tmp_fsLamda_total) ;
	  }
	  AllocFloatSpecArray(tmp_fsLamda_total,Npos_ini,iLowerFreq,iUpperFreq) ;
	}

	//バックアップ
	tmp_sSavedMCRA->ha_numProc = this->m_psSavedMCRA->ha_numProc ;
	tmp_sSavedMCRATmp->ha_numProc = this->m_psSavedMCRATmp->ha_numProc ;
	for(s = 0 ; s < Nfreq*Npos_ini; s++){
	  tmp_sSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[s] ;

	  tmp_sSavedMCRATmp->micary_fsS.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRATmp->micary_fsS.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRATmp->micary_fsSmin.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRATmp->micary_fsSmin.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRATmp->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRATmp->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRATmp->micary_fsStmp.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRATmp->micary_fsStmp.micary_faSpec.ha_fValue[s] ;
	
	  tmp_fsSavedLeak->micary_faSpec.ha_fValue[s] = this->m_pfsSavedLeak->micary_faSpec.ha_fValue[s] ;
	 
	  tmp_sSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[s] ;
	
	  tmp_sSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedEstSN->micary_fsSN.micary_faSpec.ha_fValue[s] = this->m_psSavedEstSN->micary_fsSN.micary_faSpec.ha_fValue[s] ;
	
	  tmp_sSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_fValue[s] = this->m_psSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_fValue[s] ;

	  tmp_fsLamda_total->micary_faSpec.ha_fValue[s] = this->m_pfsLamda_total->micary_faSpec.ha_fValue[s] ;
	}
	
	for(s = 0 ; s < Npos_ini;s++){
	  tmp_sSavedVoiceP->micary_faSmSNRF.ha_fValue[s] = this->m_psSavedVoiceP->micary_faSmSNRF.ha_fValue[s] ;
	  tmp_sSavedVoiceP->micary_faSmSNRP.ha_fValue[s] = this->m_psSavedVoiceP->micary_faSmSNRP.ha_fValue[s] ;	  
	}

	//音源数の確認
	if(Npos_ini != Npos_cur){
        
	  CPF_exec ::Initialize(iLowerFreq,iUpperFreq,Npos_cur,1) ;
	}

	//savedデータの初期化t
	int Nch = faInStatNoise.ha_numCh ; 
	
	CPF_exec::InitSavedArray(faInStatNoise,iLowerFreq,iUpperFreq,Npos_cur,Nch) ;
	//CPF_exec::InitSavedArray(faInStatNoise,iLowerFreq,iUpperFreq,Npos_cur,Nch,ha_fNoise_Compense) ;
	if(this->m_psSavedMCRA->micary_fsS.micary_faSpec.ha_numValue != Nfreq*Npos_cur){
	  if(m_psSavedMCRA){
	    FreeSavedMCRA(this->m_psSavedMCRA);
	  }
	  AllocSavedMCRA(this->m_psSavedMCRA,iUpperFreq,iLowerFreq,Npos_cur) ;
	}
	if(this->m_psSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_numValue != Nfreq*Npos_cur){
	  if(m_psSavedRevTmp){
	    FreeSavedRev(this->m_psSavedRevTmp);
	  }
	  AllocSavedRev(this->m_psSavedRevTmp,iUpperFreq,iLowerFreq,Npos_cur) ;
	}
		

	// データの引継ぎ/初期化
	for(s = 0 ; s < Npos_cur ; s++){
	  for(n = 0 ; n < Npos_ini ; n++){
	    if(iaSrcCur.ha_iValue[s] == iaSrcIni.ha_iValue[n]){
	    // データの引継ぎ
	      for(k = 0 ; k < Nfreq ; k++){
		//for MCRA
		this->m_psSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		// for MCRATmp
		this->m_psSavedMCRATmp->micary_fsS.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRATmp->micary_fsS.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRATmp->micary_fsSmin.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRATmp->micary_fsSmin.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRATmp->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRATmp->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRATmp->micary_fsStmp.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRATmp->micary_fsStmp.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		
		//for Leak noise
		this->m_pfsSavedLeak->micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_fsSavedLeak->micary_faSpec.ha_fValue[n*Nfreq+k] ;
		
		//for Rev
		this->m_psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		
		//for SN
		this->m_psSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedEstSN->micary_fsSN.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedEstSN->micary_fsSN.micary_faSpec.ha_fValue[n*Nfreq+k] ;
				
	        // for VoiceP
		this->m_psSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_fValue[n*Nfreq+k] ;

		//for Lamda total
		this->m_pfsLamda_total->micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_fsLamda_total->micary_faSpec.ha_fValue[n*Nfreq+k] ;
	      } //end freq loop

	      this->m_psSavedVoiceP->micary_faSmSNRF.ha_fValue[s] = tmp_sSavedVoiceP->micary_faSmSNRF.ha_fValue[n] ;
	      this->m_psSavedVoiceP->micary_faSmSNRP.ha_fValue[s] = tmp_sSavedVoiceP->micary_faSmSNRP.ha_fValue[n] ;	
			
	      break ;
	    } // end if (SrcCur = SrcIni)
	  } //end Ini loop
	 
	}//end Cur loop
	// 2008.11.27
	this->m_psSavedMCRA->ha_numProc = tmp_sSavedMCRA->ha_numProc ;
	this->m_psSavedMCRATmp->ha_numProc = tmp_sSavedMCRATmp->ha_numProc;

	//End change procces

	/*
	// free
	FreeSavedMCRA(&tmp_sSavedMCRA) ;
	FreeSavedMCRA(&tmp_sSavedMCRATmp) ;
	FreeSavedRev(&tmp_sSavedRev) ;
	FreeSavedRev(&tmp_sSavedRevTmp) ;
	FreeSavedSN(&tmp_sSavedEstSN) ;
	FreeSavedVoiceP(&tmp_sSavedVoiceP) ;
	FreeFloatSpecArray(&tmp_fsSavedLeak) ;
	FreeFloatSpecArray(&tmp_fsLamda_total) ;
	*/
	return 1 ;
}

// ---------------------------------------------------------------------------------------------------------------

int CPF_exec::InitSavedArray(MICARY_FloatSpecArray faInStatNoise, int iLower_freq, int iUpper_freq, int iNpos, int iNch, HA_Float ha_fNoise_compense)
{
  if(!&faInStatNoise){
 
	this ->Clear() ;
		if(m_pfsNoiseSpec){
		  free(m_pfsNoiseSpec) ;
		  m_pfsNoiseSpec = NULL ;
		}
		if(m_psSavedMCRA){	
		  free(m_psSavedMCRA) ;
		  m_psSavedMCRA = NULL ;
		}
		if(m_psSavedRev){	
		  free(m_psSavedRev) ;
		  m_psSavedRev = NULL ;
		}
		if(m_psSavedEstSN){
		  free(m_psSavedEstSN) ;
		  m_psSavedEstSN = NULL ;
		}
		if(m_psSavedVoiceP){
		  free(m_psSavedVoiceP) ;
		  m_psSavedVoiceP = NULL ;
		}
		if(m_psSavedMCRATmp){
		  free(m_psSavedMCRATmp) ;
		  m_psSavedMCRATmp = NULL ;
		}
		if(m_pfsSavedLeak){
		  free(m_pfsSavedLeak) ;
		  m_pfsSavedLeak = NULL ;
		}
		if(m_psSavedRevTmp){
		  free(m_psSavedRevTmp) ;
		  m_psSavedRevTmp = NULL ;
		}
		if(m_pfsLamda_total){
		  free(m_pfsLamda_total) ;
		  m_pfsLamda_total = NULL ;
		}
	
		// add 2008/2/5
		if(tmp_sSavedMCRA){
		  FreeSavedMCRA(tmp_sSavedMCRA) ;
		  free(tmp_sSavedMCRA) ;
		  tmp_sSavedMCRA = NULL ;
		}
		if(tmp_sSavedMCRATmp){
		  FreeSavedMCRA(tmp_sSavedMCRATmp) ;
		  free(tmp_sSavedMCRATmp) ;
		  tmp_sSavedMCRATmp = NULL ;
		}
		if(tmp_sSavedRev){
		  FreeSavedRev(tmp_sSavedRev) ;
		  free(tmp_sSavedRev) ;
		  tmp_sSavedRev = NULL ;
		}
		if(tmp_sSavedRevTmp){
		  FreeSavedRev(tmp_sSavedRevTmp) ;
		  free(tmp_sSavedRevTmp) ;
		  tmp_sSavedRevTmp = NULL ;
		}
		if(tmp_sSavedEstSN){
		  FreeSavedSN(tmp_sSavedEstSN) ;
		  free(tmp_sSavedEstSN) ;
		  tmp_sSavedEstSN = NULL ;
		}
		if(tmp_sSavedVoiceP){
		  FreeSavedVoiceP(tmp_sSavedVoiceP);
		  free(tmp_sSavedVoiceP) ;
		  tmp_sSavedVoiceP = NULL ;
		}
		if(tmp_fsSavedLeak){
		  FreeFloatSpecArray(tmp_fsSavedLeak);
		  free(tmp_fsSavedLeak) ;
		  tmp_fsSavedLeak = NULL ;
		}
		if(tmp_fsLamda_total){
		  FreeFloatSpecArray(tmp_fsLamda_total);
		  free(tmp_fsLamda_total) ;
		  tmp_fsLamda_total = NULL ;
		}
		return 0 ;
  }
	//修正 2009.3.16 morio MCRAだけTmpを入力している
	//if(PFInitTmp(this->m_psSavedMCRATmp,this->m_psSavedRev,this->m_psSavedEstSN,
	if(PFInitTmp(this->m_psSavedMCRA,this->m_psSavedRev,this->m_psSavedEstSN,
		this->m_psSavedVoiceP,&faInStatNoise.micary_faSpec,ha_fNoise_compense,
		(HA_Integer)iLower_freq,(HA_Integer)iUpper_freq,(HA_Integer)iNpos,(HA_Integer)iNch) != 1)
	
	{
		std ::cout << "Fail to Initialize Saved array and Estimated state noise." << std ::endl;
		this ->Clear() ;
		if(m_pfsNoiseSpec){
		  free(m_pfsNoiseSpec) ;
		  m_pfsNoiseSpec = NULL ;
		}
		if(m_psSavedMCRA){	
		  free(m_psSavedMCRA) ;
		  m_psSavedMCRA = NULL ;
		}
		if(m_psSavedRev){	
		  free(m_psSavedRev) ;
		  m_psSavedRev = NULL ;
		}
		if(m_psSavedEstSN){
		  free(m_psSavedEstSN) ;
		  m_psSavedEstSN = NULL ;
		}
		if(m_psSavedVoiceP){
		  free(m_psSavedVoiceP) ;
		  m_psSavedVoiceP = NULL ;
		}
		if(m_psSavedMCRATmp){
		  free(m_psSavedMCRATmp) ;
		  m_psSavedMCRATmp = NULL ;
		}
		if(m_pfsSavedLeak){
		  free(m_pfsSavedLeak) ;
		  m_pfsSavedLeak = NULL ;
		}
		if(m_psSavedRevTmp){
		  free(m_psSavedRevTmp) ;
		  m_psSavedRevTmp = NULL ;
		}
		if(m_pfsLamda_total){
		  free(m_pfsLamda_total) ;
		  m_pfsLamda_total = NULL ;
		}
	
		// add 2008/2/5
		if(tmp_sSavedMCRA){
		  FreeSavedMCRA(tmp_sSavedMCRA) ;
		  free(tmp_sSavedMCRA) ;
		  tmp_sSavedMCRA = NULL ;
		}
		if(tmp_sSavedMCRATmp){
		  FreeSavedMCRA(tmp_sSavedMCRATmp) ;
		  free(tmp_sSavedMCRATmp) ;
		  tmp_sSavedMCRATmp = NULL ;
		}
		if(tmp_sSavedRev){
		  FreeSavedRev(tmp_sSavedRev) ;
		  free(tmp_sSavedRev) ;
		  tmp_sSavedRev = NULL ;
		}
		if(tmp_sSavedRevTmp){
		  FreeSavedRev(tmp_sSavedRevTmp) ;
		  free(tmp_sSavedRevTmp) ;
		  tmp_sSavedRevTmp = NULL ;

		}
		if(tmp_sSavedEstSN){
		  FreeSavedSN(tmp_sSavedEstSN) ;
		  free(tmp_sSavedEstSN) ;
		  tmp_sSavedEstSN = NULL ;
		}
		if(tmp_sSavedVoiceP){
		  FreeSavedVoiceP(tmp_sSavedVoiceP);
		  free(tmp_sSavedVoiceP) ;
		  tmp_sSavedVoiceP = NULL ;
		}
		if(tmp_fsSavedLeak){
		  FreeFloatSpecArray(tmp_fsSavedLeak);
		  free(tmp_fsSavedLeak) ;
		  tmp_fsSavedLeak = NULL ;
		}
		if(tmp_fsLamda_total){
		  FreeFloatSpecArray(tmp_fsLamda_total);
		  free(tmp_fsLamda_total) ;
		  tmp_fsLamda_total = NULL ;
		}
		return 0 ;
	}
	  
	return 1 ;
}

int CPF_exec::ChangeSrcPos(MICARY_FloatSpecArray faInStatNoise, MICARY_IntArray iaSrcCur, MICARY_IntArray iaSrcIni, HA_Float ha_fNoise_Compense)
{
int s,n,k,m;
	int iLowerFreq,iUpperFreq,Npos_ini,Npos_cur,Nfreq ;
	
	/*
	MICARY_SavedMCRA tmp_sSavedMCRA,tmp_sSavedMCRATmp ;//MCRA保存値(temporary)
	MICARY_SavedEstRev tmp_sSavedRev,tmp_sSavedRevTmp ;//残響成分保存値(temporary)
	MICARY_SavedEstSN tmp_sSavedEstSN ;	//SN比推定保存値(temporary)
	MICARY_SavedVoiceP tmp_sSavedVoiceP ;	//音声存在確率保存値(temporary)
	MICARY_FloatSpecArray tmp_fsSavedLeak ;	//漏れ保存値(temporary)
	MICARY_FloatSpecArray tmp_fsLamda_total ; // total lamda(temporary)
	*/

	//tmp_sSavedMCRA = (MICARY_SavedMCRA *)calloc(1,sizeof(MICARY_SavedMCRA)) ;		//tmp_sSavedRev = (MICARY_SavedEstRev *)calloc(1,sizeof(MICARY_SavedEstRev)) ; 
	//tmp_sSavedEstSN = (MICARY_SavedEstSN *)calloc(1,sizeof(MICARY_SavedEstSN)) ; 
	//tmp_sSavedVoiceP = (MICARY_SavedVoiceP *)calloc(1,sizeof(MICARY_SavedVoiceP));
	//tmp_fsSavedLeak = (MICARY_FloatSpecArray *)calloc(1,sizeof(MICARY_FloatSpecArray)) ;

	Npos_ini = iaSrcIni.ha_numValue ;
	Npos_cur = iaSrcCur.ha_numValue ;
	
	iLowerFreq = this ->m_pfsSavedLeak->ha_iLowerFreq ;
	iUpperFreq = this ->m_pfsSavedLeak->ha_iUpperFreq ;

	Nfreq = iUpperFreq - iLowerFreq + 1 ; 

	// alloc
	if(tmp_sSavedMCRA->micary_fsS.micary_faSpec.ha_numValue != Nfreq * Npos_ini){ 
	  if(tmp_sSavedMCRA){
	    FreeSavedMCRA(tmp_sSavedMCRA) ;
	  }
	  AllocSavedMCRA(tmp_sSavedMCRA,iUpperFreq,iLowerFreq,Npos_ini) ;
	}

	if(tmp_sSavedMCRATmp->micary_fsS.micary_faSpec.ha_numValue != Nfreq *Npos_ini){
	  if(tmp_sSavedMCRATmp){
	    FreeSavedMCRA(tmp_sSavedMCRATmp) ;
	  }
	  AllocSavedMCRA(tmp_sSavedMCRATmp,iUpperFreq,iLowerFreq,Npos_ini) ;
	}

	if(tmp_sSavedRev->micary_fsRevPSpec.micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_sSavedRev){
	    FreeSavedRev(tmp_sSavedRev) ;
	  }
	  AllocSavedRev(tmp_sSavedRev,iUpperFreq,iLowerFreq,Npos_ini) ;
	}


	if(tmp_sSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_sSavedRevTmp){
	    FreeSavedRev(tmp_sSavedRevTmp) ;
	  }
	  AllocSavedRev(tmp_sSavedRevTmp,iUpperFreq,iLowerFreq,Npos_ini) ;
	}


	if(tmp_sSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_numValue != Nfreq*Npos_ini){
	  if(tmp_sSavedEstSN){
	    FreeSavedSN(tmp_sSavedEstSN) ;
	  }
	  AllocSavedSN(tmp_sSavedEstSN,iUpperFreq,iLowerFreq,Npos_ini) ;
	}

	if(tmp_sSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_sSavedVoiceP){
	    FreeSavedVoiceP(tmp_sSavedVoiceP) ;
	  }
	  AllocSavedVoiceP(tmp_sSavedVoiceP,iUpperFreq,iLowerFreq,Npos_ini) ;
	}

	if(tmp_fsSavedLeak->micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_fsSavedLeak){
	    FreeFloatSpecArray(tmp_fsSavedLeak) ;
	  }
	  AllocFloatSpecArray(tmp_fsSavedLeak,Npos_ini,iLowerFreq,iUpperFreq) ;
	}

	if(tmp_fsLamda_total->micary_faSpec.ha_numValue != Nfreq * Npos_ini){
	  if(tmp_fsLamda_total){
	    FreeFloatSpecArray(tmp_fsLamda_total) ;
	  }
	  AllocFloatSpecArray(tmp_fsLamda_total,Npos_ini,iLowerFreq,iUpperFreq) ;
	}


	//バックアップ
	tmp_sSavedMCRA->ha_numProc = this->m_psSavedMCRA->ha_numProc ;
	tmp_sSavedMCRATmp->ha_numProc = this->m_psSavedMCRATmp->ha_numProc ;
	for(s = 0 ; s < Nfreq*Npos_ini; s++){
	  tmp_sSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[s] ;

	  tmp_sSavedMCRATmp->micary_fsS.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRATmp->micary_fsS.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRATmp->micary_fsSmin.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRATmp->micary_fsSmin.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRATmp->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRATmp->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedMCRATmp->micary_fsStmp.micary_faSpec.ha_fValue[s] = this->m_psSavedMCRATmp->micary_fsStmp.micary_faSpec.ha_fValue[s] ;
	
	  tmp_fsSavedLeak->micary_faSpec.ha_fValue[s] = this->m_pfsSavedLeak->micary_faSpec.ha_fValue[s] ;
	 
	  tmp_sSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[s] ;
	
	  tmp_sSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[s] = this->m_psSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[s] ;
	  tmp_sSavedEstSN->micary_fsSN.micary_faSpec.ha_fValue[s] = this->m_psSavedEstSN->micary_fsSN.micary_faSpec.ha_fValue[s] ;
	
	  tmp_sSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_fValue[s] = this->m_psSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_fValue[s] ;

	  tmp_fsLamda_total->micary_faSpec.ha_fValue[s] = this->m_pfsLamda_total->micary_faSpec.ha_fValue[s] ;
	}
	 
	for(s = 0 ; s < Npos_ini;s++){
	  tmp_sSavedVoiceP->micary_faSmSNRF.ha_fValue[s] = this->m_psSavedVoiceP->micary_faSmSNRF.ha_fValue[s] ;
	  tmp_sSavedVoiceP->micary_faSmSNRP.ha_fValue[s] = this->m_psSavedVoiceP->micary_faSmSNRP.ha_fValue[s] ;	
	  //printf("     %d %f ",iaSrcCur.ha_iValue[s],tmp_sSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[s*Nfreq]);
	}

	//音源数の確認
	if(Npos_ini != Npos_cur){
        
	  CPF_exec ::Initialize(iLowerFreq,iUpperFreq,Npos_cur,1) ;
	}

	//savedデータの初期化
	int Nch = faInStatNoise.ha_numCh ; 
	
	//CPF_exec::InitSavedArray(faInStatNoise,iLowerFreq,iUpperFreq,Npos_cur,Nch) ;
	//	std::cout << "Noise = " << &faInStatNoise << "/Low = " << iLowerFreq << " / Up = " << iUpperFreq << " /Npos = " << Npos_cur << " /Nch = " << Nch << " /Noise_conpens = " << ha_fNoise_Compense << std::endl ;
	CPF_exec::InitSavedArray(faInStatNoise,iLowerFreq,iUpperFreq,Npos_cur,Nch,ha_fNoise_Compense) ;


	if(this->m_psSavedMCRA->micary_fsS.micary_faSpec.ha_numValue != Nfreq*Npos_cur){
	  if(m_psSavedMCRA){
	    FreeSavedMCRA(this->m_psSavedMCRA);
	  }
	  AllocSavedMCRA(this->m_psSavedMCRA,iUpperFreq,iLowerFreq,Npos_cur) ;
	}
	
	if(this->m_psSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_numValue != Nfreq*Npos_cur){
	  if(m_psSavedRevTmp){
	    FreeSavedRev(this->m_psSavedRevTmp);
	  }
	  AllocSavedRev(this->m_psSavedRevTmp,iUpperFreq,iLowerFreq,Npos_cur) ;
	}
		

	// データの引継ぎ/初期化
	for(s = 0 ; s < Npos_cur ; s++){
	  for(n = 0 ; n < Npos_ini ; n++){
	    if(iaSrcCur.ha_iValue[s] == iaSrcIni.ha_iValue[n]){
	    // データの引継ぎ
	      for(k = 0 ; k < Nfreq ; k++){
		//for MCRA
		this->m_psSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		// for MCRATmp
		this->m_psSavedMCRATmp->micary_fsS.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRATmp->micary_fsS.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRATmp->micary_fsSmin.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRATmp->micary_fsSmin.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRATmp->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRATmp->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedMCRATmp->micary_fsStmp.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedMCRATmp->micary_fsStmp.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		
		//for Leak noise
		this->m_pfsSavedLeak->micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_fsSavedLeak->micary_faSpec.ha_fValue[n*Nfreq+k] ;
		
		//for Rev
		this->m_psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		
		//for SN
		this->m_psSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedEstSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[n*Nfreq+k] ;
		this->m_psSavedEstSN->micary_fsSN.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedEstSN->micary_fsSN.micary_faSpec.ha_fValue[n*Nfreq+k] ;
				
	        // for VoiceP
		this->m_psSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_sSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_fValue[n*Nfreq+k] ;

		//for Lamda total
		this->m_pfsLamda_total->micary_faSpec.ha_fValue[s*Nfreq+k] = tmp_fsLamda_total->micary_faSpec.ha_fValue[n*Nfreq+k] ;
	      } //end freq loop

	      this->m_psSavedVoiceP->micary_faSmSNRF.ha_fValue[s] = tmp_sSavedVoiceP->micary_faSmSNRF.ha_fValue[n] ;
	      this->m_psSavedVoiceP->micary_faSmSNRP.ha_fValue[s] = tmp_sSavedVoiceP->micary_faSmSNRP.ha_fValue[n] ;	
			
	      break ;
		
	    } // end if (SrcCur = SrcIni)
	  } //end Ini loop
	
	}//end Cur loop

	//End change procces
	//printf("\n   ");
	//int ii;
	//  //printf("%d ",Nfreq);
	//  for (ii=0;ii<Npos_cur;ii++)
	//  {
	//	printf("Rev@PosChange %f %f",m_psSavedRevTmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[ii*Nfreq],m_psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[ii*Nfreq]);
	//  }
	//printf("\n");
	/*
	// free
	FreeSavedMCRA(&tmp_sSavedMCRA) ;
	FreeSavedMCRA(&tmp_sSavedMCRATmp) ;
	FreeSavedRev(&tmp_sSavedRev) ;
	FreeSavedRev(&tmp_sSavedRevTmp) ;
	FreeSavedSN(&tmp_sSavedEstSN) ;
	FreeSavedVoiceP(&tmp_sSavedVoiceP) ;
	FreeFloatSpecArray(&tmp_fsSavedLeak) ;
	FreeFloatSpecArray(&tmp_fsLamda_total) ;
	*/
	return 1 ;
}
