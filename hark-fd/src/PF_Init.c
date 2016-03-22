// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_Init.c
 \brief \~English Initialize parameter and Temporary data for Post filter 
 \brief \~Japanese 変数、テンポラリデータの初期化
 \author Sachiko Suzuki
 \date 24 Jan 2007
 \version $Id: PF_Init.c 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------
#include "PF_Init.h"
#include <math.h>
#include <stdlib.h>
// ---------------------------------------------------------------------
// initialize PF parameter
int InitPFsPrm(MICARY_PrmMCRA *psPrmMCRA , MICARY_PrmLeakNoise *psPrmLeak ,
			   MICARY_PrmRevNoise *psPrmRev , MICARY_PrmEstSN *psPrmSN ,
			   MICARY_PrmVoiceP *psPrmVoiceP , MICARY_PrmGain *psPrmGain)
{
    // for estimate SN gain
	// 2008/08/06 changed by N.Tanaka
	/*
	psPrmSN -> ha_fA = 0.8 ;
	psPrmSN -> ha_fAlp_pmag = 0.9 ;

	psPrmSN -> ha_fAlp_pmin = 0.05 ;
	psPrmSN -> ha_fTheta_max = EST_SN_P_THETAMAX ;
	psPrmSN -> ha_fTheta_min = EST_SN_P_THETAMIN ;
	psPrmSN -> ha_fXi_max = EST_SN_P_XIMAX ;
	*/
	if (psPrmSN){
		psPrmSN -> ha_fA = 0.8 ;
		psPrmSN -> ha_fAlp_pmag = 0.9 ;

		psPrmSN -> ha_fAlp_pmin = 0.05 ;
		psPrmSN -> ha_fTheta_max = EST_SN_P_THETAMAX ;
		psPrmSN -> ha_fTheta_min = EST_SN_P_THETAMIN ;
		psPrmSN -> ha_fXi_max = EST_SN_P_XIMAX ;
	}

	// for estimate minimum gain 
	// 2008/08/06 changed by N.Tanaka
	/*
	psPrmGain -> ha_fMinGain = 0.1 ;
	*/
	if (psPrmGain){
		psPrmGain -> ha_fMinGain = 0.1 ;
	}
	
	// for Leak noise
	// 2008/08/06 changed by N.Tanaka
	/*
	psPrmLeak -> ha_fAlp_leak = 0.25 ;
	*/
	if (psPrmLeak){
		psPrmLeak -> ha_fAlp_leak = 0.25 ;
	}
	
	// for MCRA module
	// 2008/08/06 changed by N.Tanaka
	/*
	psPrmMCRA -> ha_fAlp_d = 0.98 ;
	psPrmMCRA -> ha_fAlp_s = 0.5 ;
	psPrmMCRA -> ha_fDelta = 3.0 ;
	psPrmMCRA -> ha_fLeak_ampleak_factor = 0.25 * 1.5 ;
	psPrmMCRA -> ha_fNoise_compense = 1.2 ;
	psPrmMCRA -> ha_fSs_floor = 0.1 ;
	psPrmMCRA -> ha_iL = 80 ;
	*/
	if (psPrmMCRA){
		psPrmMCRA -> ha_fAlp_d = 0.98 ;
		psPrmMCRA -> ha_fAlp_s = 0.5 ;
		psPrmMCRA -> ha_fDelta = 3.0 ;
		psPrmMCRA -> ha_fLeak_ampleak_factor = 0.25 * 1.5 ;
		psPrmMCRA -> ha_fNoise_compense = 1.2 ;
		psPrmMCRA -> ha_fSs_floor = 0.1 ;
		psPrmMCRA -> ha_iL = 80 ;
	}

	// for Rev
	// 2008/08/06 changed by N.Tanaka
	/*
	psPrmRev -> ha_fDelta = 0.2 ;
	psPrmRev -> ha_fGamma = 0.5 ;
	*/
	if (psPrmRev){
		psPrmRev -> ha_fDelta = 0.2 ;
		psPrmRev -> ha_fGamma = 0.5 ;
	}

	//for VoiceP
	/*
	psPrmVoiceP -> ha_fAmpF = 0.7 ;
	psPrmVoiceP -> ha_fAmpG = 0.9 ;
	psPrmVoiceP -> ha_fAmpL = 0.9 ;
	psPrmVoiceP -> ha_fB = 0.7 ;
	psPrmVoiceP -> ha_fBwFen = 99 ;
	psPrmVoiceP -> ha_fBwFst = 8 ;
	psPrmVoiceP -> ha_fBwG = 29 ;
	psPrmVoiceP -> ha_fBwL = 5 ;
	psPrmVoiceP -> ha_fQmax = 0.98 ;
	psPrmVoiceP -> ha_fQmin = 0.02 ;
	psPrmVoiceP -> ha_fZmaxF = 0.316 ;
	psPrmVoiceP -> ha_fZmaxG = 0.316 ;
	psPrmVoiceP -> ha_fZmaxL = 0.316 ;
	psPrmVoiceP -> ha_fZminF = 0.1 ;
	psPrmVoiceP -> ha_fZminG = 0.1 ;
	psPrmVoiceP -> ha_fZminL = 0.1 ;
	psPrmVoiceP -> ha_fZpmax = 10 ;
	psPrmVoiceP -> ha_fZpmin = 1.0 ;
	psPrmVoiceP -> ha_fZthF = 1.5 ;
	*/
	// 2008/08/06 changed by N.Tanaka
	/*
	psPrmVoiceP -> ha_fAmpF = EST_VOICE_P_AMP_F ;
	psPrmVoiceP -> ha_fAmpG = EST_VOICE_P_AMP_G ;
	psPrmVoiceP -> ha_fAmpL = EST_VOICE_P_AMP_L ;
	psPrmVoiceP -> ha_fB = EST_VOIVE_P_COEF_B ;
	psPrmVoiceP -> ha_fBwFen = EST_VOICE_P_BW_F_EN ;
	psPrmVoiceP -> ha_fBwFst = EST_VOICE_P_BW_F_ST ;
	psPrmVoiceP -> ha_fBwG = EST_VOICE_P_BW_G ;
	psPrmVoiceP -> ha_fBwL = EST_VOICE_P_BW_L ;
	psPrmVoiceP -> ha_fQmax = EST_VOICE_P_QMAX ;
	psPrmVoiceP -> ha_fQmin = EST_VOICE_P_QMIN ;
	psPrmVoiceP -> ha_fZmaxF = EST_VOICE_P_ZMAX_F ;
	psPrmVoiceP -> ha_fZmaxG = EST_VOICE_P_ZMAX_G ;
	psPrmVoiceP -> ha_fZmaxL = EST_VOICE_P_ZMAX_L ;
	psPrmVoiceP -> ha_fZminF = EST_VOICE_P_ZMIN_F ;
	psPrmVoiceP -> ha_fZminG = EST_VOICE_P_ZMIN_G ;
	psPrmVoiceP -> ha_fZminL = EST_VOICE_P_ZMIN_L ;
	psPrmVoiceP -> ha_fZpmax = EST_VOICE_P_ZPMAX ;
	psPrmVoiceP -> ha_fZpmin = EST_VOICE_P_ZPMIN ;
	psPrmVoiceP -> ha_fZthF = EST_VOICE_P_ZETA_F0 ;
	*/
	if (psPrmVoiceP){
		psPrmVoiceP -> ha_fAmpF = EST_VOICE_P_AMP_F ;
		psPrmVoiceP -> ha_fAmpG = EST_VOICE_P_AMP_G ;
		psPrmVoiceP -> ha_fAmpL = EST_VOICE_P_AMP_L ;
		psPrmVoiceP -> ha_fB = EST_VOIVE_P_COEF_B ;
		psPrmVoiceP -> ha_fBwFen = EST_VOICE_P_BW_F_EN ;
		psPrmVoiceP -> ha_fBwFst = EST_VOICE_P_BW_F_ST ;
		psPrmVoiceP -> ha_fBwG = EST_VOICE_P_BW_G ;
		psPrmVoiceP -> ha_fBwL = EST_VOICE_P_BW_L ;
		psPrmVoiceP -> ha_fQmax = EST_VOICE_P_QMAX ;
		psPrmVoiceP -> ha_fQmin = EST_VOICE_P_QMIN ;
		psPrmVoiceP -> ha_fZmaxF = EST_VOICE_P_ZMAX_F ;
		psPrmVoiceP -> ha_fZmaxG = EST_VOICE_P_ZMAX_G ;
		psPrmVoiceP -> ha_fZmaxL = EST_VOICE_P_ZMAX_L ;
		psPrmVoiceP -> ha_fZminF = EST_VOICE_P_ZMIN_F ;
		psPrmVoiceP -> ha_fZminG = EST_VOICE_P_ZMIN_G ;
		psPrmVoiceP -> ha_fZminL = EST_VOICE_P_ZMIN_L ;
		psPrmVoiceP -> ha_fZpmax = EST_VOICE_P_ZPMAX ;
		psPrmVoiceP -> ha_fZpmin = EST_VOICE_P_ZPMIN ;
		psPrmVoiceP -> ha_fZthF = EST_VOICE_P_ZETA_F0 ;
	}

	return 1 ;
}
//    
// Temporary dataの確保と開放
// for MCRA
int AllocSavedMCRA(MICARY_SavedMCRA *psSavedMCRA ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) 
{
  int ArraySize ;
  if(!psSavedMCRA){
    return 0 ;
  }
  
  // modified 2008.10.31
  ArraySize = Nch * (iUpperFreq - iLowerFreq + 1) ;
  if(ArraySize == 0){
    AllocFloatSpecArray(&(psSavedMCRA->micary_fsS),0,0,0) ;
    AllocFloatSpecArray(&(psSavedMCRA->micary_fsSmin),0 ,0,0) ;
    AllocFloatSpecArray(&(psSavedMCRA->micary_fsStmp),0,0,0) ;
    AllocFloatSpecArray(&(psSavedMCRA->micary_fsStateNoisePSpec),0 ,0,0) ;
    psSavedMCRA->ha_numProc = 0 ;
    return 1;
  }
	if(psSavedMCRA ->micary_fsS.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		AllocFloatSpecArray(&(psSavedMCRA->micary_fsS),Nch ,iLowerFreq,iUpperFreq) ;
	}
	if(psSavedMCRA ->micary_fsSmin.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		AllocFloatSpecArray(&(psSavedMCRA->micary_fsSmin),Nch ,iLowerFreq,iUpperFreq) ;
	}
	if(psSavedMCRA ->micary_fsStmp.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		AllocFloatSpecArray(&(psSavedMCRA->micary_fsStmp),Nch ,iLowerFreq,iUpperFreq) ;
	}
	if(psSavedMCRA ->micary_fsStateNoisePSpec.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		AllocFloatSpecArray(&(psSavedMCRA->micary_fsStateNoisePSpec),Nch ,iLowerFreq,iUpperFreq) ;
	}
	psSavedMCRA->ha_numProc = 0 ;

	return 1 ;
}

int FreeSavedMCRA(MICARY_SavedMCRA *psSavedMCRA) 
{
	if(!psSavedMCRA){
		return 0 ;
	}
	FreeFloatSpecArray(&(psSavedMCRA->micary_fsS));
	FreeFloatSpecArray(&(psSavedMCRA->micary_fsSmin));
	FreeFloatSpecArray(&(psSavedMCRA->micary_fsStateNoisePSpec));
	FreeFloatSpecArray(&(psSavedMCRA->micary_fsStmp));

	return 1;
}
//// for EstSN
//int AllocSavedEstSN(MICARY_SavedEstSN *psSavedEstSN ,HA_Integer iUpperFreq ,
//				  HA_Integer iLowerFreq ,HA_Integer Nch) 
//{
//  int ArraySize ;
//  if(!psSavedEstSN){
//    return 0 ;
//  }
//  
//  // modified 2008.10.31
//  ArraySize = Nch * (iUpperFreq - iLowerFreq + 1) ;
//  if(ArraySize == 0){
//    AllocFloatSpecArray(&(psSavedEstSN->micary_fsPreSNR),0,0,0) ;
//    AllocFloatSpecArray(&(psSavedEstSN->micary_fsPreDirectPowSpec),0 ,0,0) ;
//   
//    return 1;
//  }
//	if(psSavedEstSN->micary_fsPreDirectPowSpec.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
//		AllocFloatSpecArray(&(psSavedEstSN->micary_fsPreSNR),Nch ,iLowerFreq,iUpperFreq) ;
//	}
//	if(psSavedEstSN->micary_fsPreDirectPowSpec.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
//		AllocFloatSpecArray(&(psSavedEstSN->micary_fsPreDirectPowSpec),Nch ,iLowerFreq,iUpperFreq) ;
//	}
//	
//	return 1 ;
//}
//int FreeSavedEstSN(MICARY_SavedEstSN *psSavedEstSN) 
//{
//	if(!psSavedEstSN){
//		return 0 ;
//	}
//	FreeFloatSpecArray(&(psSavedEstSN->micary_fsPreSNR));
//	FreeFloatSpecArray(&(psSavedEstSN->micary_fsPreDirectPowSpec));
//	
//	return 1;
//}

// for Reverb
int AllocSavedRev(MICARY_SavedEstRev *psSavedRev ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) 
{
  int ArraySize ;
  if(!psSavedRev){
    return 0 ;
  }

  // modified 2008.10.31
  ArraySize = Nch * (iUpperFreq - iLowerFreq + 1) ;
  if(ArraySize == 0){
	AllocFloatSpecArray(&(psSavedRev->micary_fsRevPSpec),0,0,0) ;
		psSavedRev ->micary_fsRevPSpec.ha_iLowerFreq = 0 ;
		psSavedRev ->micary_fsRevPSpec.ha_iUpperFreq = 0 ;
		psSavedRev ->micary_fsRevPSpec.ha_numCh = 0 ;

				return 1 ;
  }

	if(psSavedRev ->micary_fsRevPSpec.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		AllocFloatSpecArray(&(psSavedRev->micary_fsRevPSpec),Nch ,iLowerFreq,iUpperFreq) ;
		psSavedRev ->micary_fsRevPSpec.ha_iLowerFreq = iLowerFreq ;
		psSavedRev ->micary_fsRevPSpec.ha_iUpperFreq = iUpperFreq ;
		psSavedRev ->micary_fsRevPSpec.ha_numCh = Nch ;
	}
	return 1 ;

}

int FreeSavedRev(MICARY_SavedEstRev *psSavedRev) 
{
	if(!psSavedRev){
		return 0 ;
	}
	FreeFloatSpecArray(&(psSavedRev->micary_fsRevPSpec));
	return 1 ;
}

// for SN ratio
int AllocSavedSN(MICARY_SavedEstSN *psSavedSN ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) 
{
  int ArraySize ;

  if(!psSavedSN){
    return 0 ;
  }
  // modified 2008.10.31
  ArraySize = Nch * (iUpperFreq - iLowerFreq + 1) *Nch ;
  if(ArraySize == 0){
    	AllocFloatSpecArray(&(psSavedSN->micary_fsInputPSpec),0 ,0,0) ;
	AllocFloatSpecArray(&(psSavedSN->micary_fsSN),0 ,0,0) ;
        psSavedSN ->micary_fsInputPSpec.ha_iLowerFreq = 0 ;
	psSavedSN ->micary_fsInputPSpec.ha_iUpperFreq = 0 ;
	psSavedSN ->micary_fsInputPSpec.ha_numCh =0 ;
	psSavedSN ->micary_fsSN.ha_iLowerFreq = 0 ;
		psSavedSN ->micary_fsSN.ha_iUpperFreq =0 ;
		psSavedSN ->micary_fsSN.ha_numCh = 0 ;

	return 1 ;
  }

	if(psSavedSN ->micary_fsInputPSpec.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		AllocFloatSpecArray(&(psSavedSN->micary_fsInputPSpec),Nch ,iLowerFreq,iUpperFreq) ;
		psSavedSN ->micary_fsInputPSpec.ha_iLowerFreq = iLowerFreq ;
		psSavedSN ->micary_fsInputPSpec.ha_iUpperFreq = iUpperFreq ;
		psSavedSN ->micary_fsInputPSpec.ha_numCh = Nch ;
	}
	if(psSavedSN ->micary_fsSN.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		AllocFloatSpecArray(&(psSavedSN->micary_fsSN),Nch ,iLowerFreq,iUpperFreq) ;
		psSavedSN ->micary_fsSN.ha_iLowerFreq = iLowerFreq ;
		psSavedSN ->micary_fsSN.ha_iUpperFreq = iUpperFreq ;
		psSavedSN ->micary_fsSN.ha_numCh = Nch ;
	}
	return 1 ;

}

int FreeSavedSN(MICARY_SavedEstSN *psSavedSN ) 
{
	if(!psSavedSN){
		return 0 ;
	}
	FreeFloatSpecArray(&(psSavedSN->micary_fsInputPSpec));
	FreeFloatSpecArray(&(psSavedSN->micary_fsSN));

	return 1 ;

}

int AllocSavedVoiceP(MICARY_SavedVoiceP *psSavedVoiceP ,HA_Integer iUpperFreq ,
				  HA_Integer iLowerFreq ,HA_Integer Nch) 
{
  int ArraySize ;
  if(!psSavedVoiceP){
    return 0 ;
  }
  // modified 2008.10.31
  ArraySize = Nch * (iUpperFreq - iLowerFreq + 1) ;
  if(ArraySize == 0){
    		FloatArrayAlloc(&(psSavedVoiceP->micary_faSmSNRF),0) ;
		FloatArrayAlloc(&(psSavedVoiceP->micary_faSmSNRP),0) ;
	AllocFloatSpecArray(&(psSavedVoiceP->micary_fsSmSNR),0 ,0,0) ;
		psSavedVoiceP ->micary_fsSmSNR.ha_iLowerFreq = 0 ;
		psSavedVoiceP ->micary_fsSmSNR.ha_iUpperFreq = 0 ;
		psSavedVoiceP ->micary_fsSmSNR.ha_numCh = 0 ;
		return 1 ;
  }
	if(psSavedVoiceP ->micary_faSmSNRF.ha_numValue != Nch){
		FloatArrayAlloc(&(psSavedVoiceP->micary_faSmSNRF),Nch) ;
	}
	if(psSavedVoiceP ->micary_faSmSNRP.ha_numValue != Nch){
		FloatArrayAlloc(&(psSavedVoiceP->micary_faSmSNRP),Nch) ;
	}
	if(psSavedVoiceP ->micary_fsSmSNR.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq +1)*Nch){
		AllocFloatSpecArray(&(psSavedVoiceP->micary_fsSmSNR),Nch ,iLowerFreq,iUpperFreq) ;
		psSavedVoiceP ->micary_fsSmSNR.ha_iLowerFreq = iLowerFreq ;
		psSavedVoiceP ->micary_fsSmSNR.ha_iUpperFreq = iUpperFreq ;
		psSavedVoiceP ->micary_fsSmSNR.ha_numCh = Nch ;
	}

	return 1;
}

int FreeSavedVoiceP(MICARY_SavedVoiceP *psSavedVoiceP ) 
{
	if(!psSavedVoiceP){
		return 0 ;
	}
	FloatArrayFree(&(psSavedVoiceP->micary_faSmSNRF));
	FloatArrayFree(&(psSavedVoiceP->micary_faSmSNRP));
	FreeFloatSpecArray(&(psSavedVoiceP->micary_fsSmSNR));

	return 1;
}

// for Init_MCRA 
int AllocSavedInitMCRA(MICARY_SavedInitMCRA *psSavedInitMCRA ,HA_Integer iUpperFreq ,
					   HA_Integer iLowerFreq ,HA_Integer Nch)  
{
  int ArraySize ;
	if(!psSavedInitMCRA){
		return 0 ;
	}
	// modofied 2008.10.31
	 ArraySize = Nch * (iUpperFreq - iLowerFreq + 1) ;
	 if(ArraySize == 0){
	   AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsS),0,0,0) ;
	AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsSmin),0,0,0) ;
	AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsStmp),0,0,0) ;
	AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsStateNoisePSpec),0,0,0) ;
	AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsSf),0,0,0) ;
	psSavedInitMCRA->ha_numProc = 0 ;
	return 1 ;
	 }
	if(psSavedInitMCRA ->micary_fsS.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		if(&psSavedInitMCRA->micary_fsS){
			AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsS),Nch ,iLowerFreq,iUpperFreq) ;
		}
	}
	if(psSavedInitMCRA ->micary_fsSmin.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		if(&psSavedInitMCRA->micary_fsSmin){
			AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsSmin),Nch ,iLowerFreq,iUpperFreq) ;
		}
	}
	if(psSavedInitMCRA ->micary_fsStmp.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		if(&psSavedInitMCRA->micary_fsStmp){
			AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsStmp),Nch ,iLowerFreq,iUpperFreq) ;
		}
	}
	if(psSavedInitMCRA ->micary_fsStateNoisePSpec.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		if(&psSavedInitMCRA->micary_fsStateNoisePSpec){
			AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsStateNoisePSpec),Nch ,iLowerFreq,iUpperFreq) ;
		}
	}
	if(psSavedInitMCRA ->micary_fsSf.micary_faSpec.ha_numValue != (iUpperFreq - iLowerFreq+1)*Nch){
		if(&psSavedInitMCRA->micary_fsSf){	
			AllocFloatSpecArray(&(psSavedInitMCRA->micary_fsSf),Nch ,iLowerFreq,iUpperFreq) ;
		}
	}
	psSavedInitMCRA->ha_numProc = 0 ;
	return 1 ;
}

int FreeSavedInitMCRA(MICARY_SavedInitMCRA *psSavedInitMCRA)  
{
	if(!psSavedInitMCRA){
		return 0 ;
	}

	// 2008/11/11 changed by N.Tanaka
	/*
	if(&psSavedInitMCRA->micary_fsSf){
		FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsSf));
	}

	if(&psSavedInitMCRA->micary_fsStateNoisePSpec){
		FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsStateNoisePSpec));
	}

	if(&psSavedInitMCRA->micary_fsStmp){
		FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsStmp));
	}

	if(&psSavedInitMCRA->micary_fsSmin){
		FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsSmin));
	}
	
	if(&psSavedInitMCRA->micary_fsS){
		FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsS));
	}
	return 111;
	*/
	FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsSf));
	FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsStateNoisePSpec));
	FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsStmp));
	FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsSmin));
	FreeFloatSpecArray(&(psSavedInitMCRA->micary_fsS));
	
	return 1;
}

int InitPrmInMCRA(MICARY_PrmInitMCRA *psPrmMCRA)
{
	psPrmMCRA->ha_fDelta = INIT_MCRA_DELTA ;
	psPrmMCRA->ha_fAlp_dmin = INIT_MCRA_ALP_DMIN ;
	psPrmMCRA->ha_fAlp_s = INIT_MCRA_ALP_S ; 
	psPrmMCRA ->ha_fNoise_compense = INIT_MCRA_NOISE_COMPENSE ;
	psPrmMCRA->ha_iInit_frame = INIT_MCRA_INIT_FRAME ;
	psPrmMCRA->ha_iL = INIT_MCRA_L ;

	return 1 ;
}

int PFInitTmp(MICARY_SavedMCRA *psSavedMCRA ,MICARY_SavedEstRev *psSavedRev,MICARY_SavedEstSN *psSavedSN ,
			  MICARY_SavedVoiceP *psSavedVoiceP,MICARY_FloatArray *faNoise ,HA_Float noise_compense,
			  HA_Integer iLowerFreq ,HA_Integer iUpperFreq,HA_Integer Nsrc,HA_Integer Nmic)
{
	MICARY_FloatArray *pLamdaInit;
	HA_Integer ind,k,Nmic2,Nfreq;
	HA_Num Prm_L ;
	int ii ;

	Nmic2 = (int)pow(Nmic,2.0) ;
	Nfreq = iUpperFreq -iLowerFreq +1 ;

	pLamdaInit = (MICARY_FloatArray *)calloc(1,sizeof(MICARY_FloatArray )) ;
		
	// Initialize 2008.10.31
	FloatArrayAlloc(pLamdaInit,0) ;
	
	FloatArrayAlloc(pLamdaInit,Nfreq) ;
	
	for(ind = 0 ; ind < Nfreq ;ind ++){
		for(k = 0 ; k < Nmic ; k++){
			pLamdaInit->ha_fValue[ind] = pLamdaInit->ha_fValue[ind] +faNoise->ha_fValue[k*Nfreq+ind] ;
		}
		pLamdaInit->ha_fValue[ind] = pLamdaInit->ha_fValue[ind]/Nmic2 * noise_compense ;
	}
	// for MCRA
	Prm_L = psSavedMCRA->ha_numProc ;

	
	if(psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_numValue != Nfreq*Nsrc){
		if(psSavedMCRA){
			FreeSavedMCRA(psSavedMCRA) ;
		}

		AllocSavedMCRA(psSavedMCRA ,iUpperFreq ,iLowerFreq ,Nsrc) ;
	}
		
	for(ii = 0 ; ii < Nsrc ; ii ++){
	  	memcpy(&psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[ii*Nfreq],&pLamdaInit->ha_fValue[0],
	 		sizeof(HA_Float)*Nfreq) ;
	}
	
	/*
	for(ind = 0 ; ind < Nsrc ; ind ++){
		for(k = 0 ; k < Nfreq ; k++){
		   psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[ind*Nfreq+k] = pLamdaInit->ha_fValue[k] ;
		}
	}
	*/
       	psSavedMCRA->ha_numProc = Prm_L ;
		
	// for rev
	if(psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_numValue != Nfreq*Nsrc){
		if(psSavedRev){
			FreeSavedRev(psSavedRev) ;
		}
		AllocSavedRev(psSavedRev ,iUpperFreq , iLowerFreq ,Nsrc) ;
	}
	//for GAIN
	if(psSavedSN->micary_fsInputPSpec.micary_faSpec.ha_numValue != Nfreq*Nsrc){
		if(psSavedSN){
			FreeSavedSN(psSavedSN) ;
		}
		AllocSavedSN(psSavedSN ,iUpperFreq ,iLowerFreq , Nsrc) ;
	}
	// for VoiceP
	if(psSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_numValue != Nfreq*Nsrc){
		if(psSavedVoiceP){
			FreeSavedVoiceP(psSavedVoiceP) ;
		}
		AllocSavedVoiceP(psSavedVoiceP ,iUpperFreq ,iLowerFreq ,Nsrc) ;
	}

	FloatArrayFree(pLamdaInit) ;
	if(pLamdaInit){
		free(pLamdaInit) ;
		pLamdaInit = NULL ;
	}
	
	return 1 ;
}



//// 2008/12/5 added by K.Morio
int PFInitTmp2(MICARY_SavedMCRA *psSavedMCRA ,MICARY_SavedEstRev *psSavedRev,MICARY_SavedEstSN *psSavedSN ,
			  MICARY_SavedVoiceP *psSavedVoiceP,MICARY_FloatArray *pfCalcStateNoise,
			  HA_Integer iLowerFreq ,HA_Integer iUpperFreq)
{
	
	HA_Integer Nfreq;

	Nfreq = iUpperFreq -iLowerFreq +1 ;

	

	// for MCRA
	if(psSavedMCRA!=0)
	{
		//Prm_L = psSavedMCRA->ha_numProc ;

		
		if(psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_numValue != Nfreq){
			if(psSavedMCRA){
				FreeSavedMCRA(psSavedMCRA) ;
			}

			AllocSavedMCRA(psSavedMCRA ,iUpperFreq ,iLowerFreq ,1) ;
		}
			
		if(pfCalcStateNoise!=0)
		{
			memcpy(psSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue,pfCalcStateNoise->ha_fValue,
	 						sizeof(HA_Float)*Nfreq) ;

		}
	  		
		

	}
	// for rev
	if(psSavedRev!=0){
		if(psSavedRev->micary_fsRevPSpec.micary_faSpec.ha_numValue != Nfreq){
			if(psSavedRev){
				FreeSavedRev(psSavedRev) ;
			}
			AllocSavedRev(psSavedRev ,iUpperFreq , iLowerFreq ,1) ;
		}
	}
	//for GAIN
	if(psSavedSN!=0){
		if(psSavedSN->micary_fsInputPSpec.micary_faSpec.ha_numValue != Nfreq){
			if(psSavedSN){
				FreeSavedSN(psSavedSN) ;
			}
			AllocSavedSN(psSavedSN ,iUpperFreq ,iLowerFreq , 1) ;
		}
	}
	// for VoiceP
	if(psSavedVoiceP){
		if(psSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_numValue != Nfreq){
			if(psSavedVoiceP){
				FreeSavedVoiceP(psSavedVoiceP) ;
			}
			AllocSavedVoiceP(psSavedVoiceP ,iUpperFreq ,iLowerFreq,1) ;
		}
	}

	
	return 1 ;
}

// 2008/12/5 added by K.Morio
int CalcInitStateNoise(MICARY_FloatArray faNoise ,HA_Float noise_compense,MICARY_FloatArray *pfCalcStateNoise,
			  HA_Integer iLowerFreq ,HA_Integer iUpperFreq,HA_Integer Nsrc,HA_Integer Nmic)
{
	MICARY_FloatArray *pLamdaInit;
	HA_Integer ind,k,Nmic2,Nfreq;

	int ii ;

	Nmic2 = (int)pow(Nmic,2.0) ;
	Nfreq = iUpperFreq -iLowerFreq +1 ;

	pLamdaInit = (MICARY_FloatArray *)calloc(1,sizeof(MICARY_FloatArray )) ;
		
	FloatArrayAlloc(pLamdaInit,Nfreq) ;
	
	for(ind = 0 ; ind < Nfreq ;ind ++){
		for(k = 0 ; k < Nmic ; k++){
			pLamdaInit->ha_fValue[ind] = pLamdaInit->ha_fValue[ind] +faNoise.ha_fValue[k*Nfreq+ind] ;
		}
		pLamdaInit->ha_fValue[ind] = pLamdaInit->ha_fValue[ind]/Nmic2 * noise_compense ;
	}

	memcpy(pfCalcStateNoise->ha_fValue,pLamdaInit->ha_fValue,sizeof(HA_Float)*Nfreq) ;
	FloatArrayFree(pLamdaInit) ;
	if(pLamdaInit){
		free(pLamdaInit) ;
		pLamdaInit = NULL ;
	}
	
	return 1 ;
}


// PF_est_state_noise.c,PF_init_state_noise.cで使用関数
HA_Float minmat(HA_Float A ,HA_Float B){
	HA_Float rtn ,D;
	D = A - B ;
	if(D < 0.0){
		D = 0 ;
	}
	rtn = A - D ;
	return rtn ;
}

// Leak
int Input_PF_PrmLeak(MICARY_PrmLeakNoise *psPrmLeakNoise, float fLeak_factor,float fOver_cancel)
{
	if (psPrmLeakNoise==NULL){
		return 0;
	}

	psPrmLeakNoise->ha_fAlp_leak = (HA_Float)(fLeak_factor * fOver_cancel) ;
	
	return 1 ;
}
	










	
