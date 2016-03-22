// ---------------------------------------------------------------------
/**
  Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file PF_execV2.c
 \brief \~English Execute Post Filter function
 \brief \~Japanese Post Filterの実行関数
 \author Sachiko Suzuki
 \date 19 Jun 2007
 \version $Id: PF_execV2.c 504 2009-09-09 09:05:29Z suzuki $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include "PF_exec.h"
// ---------------------------------------------------------------------
static int iPFCounter = 0;


int PF_ExecutePostFilter(MICARY_ComplexSpecArray csInputSpec , MICARY_ComplexSpecArray *pcsOutputSpec ,
							MICARY_SavedMCRA *pSavedMCRA ,MICARY_SavedMCRA *pSavedMCRAtmp ,MICARY_PrmMCRA prmMCRA ,
							MICARY_FloatSpecArray *pSavedLeak ,MICARY_PrmLeakNoise prmLeak ,
							MICARY_SavedEstRev *pSavedRev ,MICARY_SavedEstRev *pSavedRevtmp ,
							MICARY_PrmRevNoise prmRev , 
							MICARY_SavedEstSN *pSavedSN , MICARY_PrmEstSN prmSN,
							MICARY_SavedVoiceP *pSavedVoiceP , MICARY_PrmVoiceP prmVoiceP ,
			 /*MICARY_PrmGain PrmGain ,*/ MICARY_FloatSpecArray *pSavedLamda)
{
	MICARY_FloatSpecArray fsInputPSpec ,fsGH1 ,fsEstVal ,fsVoiceP; 
	MICARY_SavedVoiceP *pSavedVoicePtmp ;
	int n ;

	//Add 2008/2/15
	memset(&fsInputPSpec,0,sizeof(MICARY_FloatSpecArray)) ;
	memset(&fsGH1,0,sizeof(MICARY_FloatSpecArray)) ;
	memset(&fsEstVal,0,sizeof(MICARY_FloatSpecArray)) ;
	memset(&fsVoiceP,0,sizeof(MICARY_FloatSpecArray)) ;


	// 保存値構造体の確保
	if(pcsOutputSpec->micary_cmpaSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	            FreeComplexSpecArray(pcsOutputSpec) ;
	            AllocComplexSpecArray(pcsOutputSpec,csInputSpec.ha_numCh ,
			csInputSpec.ha_iLowerFreq , csInputSpec.ha_iUpperFreq ) ;
	}

	if(pSavedMCRA->micary_fsS.micary_faSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	  FreeSavedMCRA(pSavedMCRA) ; 
	            AllocSavedMCRA(pSavedMCRA ,csInputSpec.ha_iUpperFreq ,csInputSpec.ha_iLowerFreq ,csInputSpec.ha_numCh) ;
	}

	if(pSavedMCRAtmp->micary_fsS.micary_faSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	  FreeSavedMCRA(pSavedMCRAtmp) ;
	         AllocSavedMCRA(pSavedMCRAtmp ,csInputSpec.ha_iUpperFreq ,csInputSpec.ha_iLowerFreq ,csInputSpec.ha_numCh) ;
	}

	if(pSavedLeak ->micary_faSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	  FreeFloatSpecArray(pSavedLeak) ;
	  AllocFloatSpecArray(pSavedLeak,csInputSpec.ha_numCh ,csInputSpec.ha_iLowerFreq ,csInputSpec.ha_iUpperFreq) ;
	}

	if(pSavedRev ->micary_fsRevPSpec.micary_faSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	  FreeSavedRev(pSavedRev) ; 
	  AllocSavedRev(pSavedRev ,csInputSpec.ha_iUpperFreq ,csInputSpec.ha_iLowerFreq ,csInputSpec.ha_numCh ) ;
	}

	if(pSavedRevtmp ->micary_fsRevPSpec.micary_faSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	  FreeSavedRev(pSavedRevtmp) ;		
	  AllocSavedRev(pSavedRevtmp ,csInputSpec.ha_iUpperFreq ,csInputSpec.ha_iLowerFreq ,csInputSpec.ha_numCh ) ;
	}

	if(pSavedLamda ->micary_faSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	  FreeFloatSpecArray(pSavedLamda) ;	
	  AllocFloatSpecArray(pSavedLamda,csInputSpec.ha_numCh ,csInputSpec.ha_iLowerFreq ,csInputSpec.ha_iUpperFreq) ;
	}

	if(pSavedSN ->micary_fsInputPSpec.micary_faSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	  FreeSavedSN(pSavedSN) ;
	  AllocSavedSN(pSavedSN ,csInputSpec.ha_iUpperFreq ,csInputSpec.ha_iLowerFreq ,csInputSpec.ha_numCh ) ;
	}
	
	//if(pSavedVoiceP ->micary_faSmSNRF.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	if(pSavedVoiceP ->micary_fsSmSNR.micary_faSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	  FreeSavedVoiceP(pSavedVoiceP) ;	
	  AllocSavedVoiceP(pSavedVoiceP ,csInputSpec.ha_iUpperFreq ,csInputSpec.ha_iLowerFreq ,csInputSpec.ha_numCh ) ;
	}

	pSavedVoicePtmp = (MICARY_SavedVoiceP *)calloc(1,sizeof(MICARY_SavedVoiceP)) ;
	//Initialize 2008.10.31
	AllocSavedVoiceP(pSavedVoicePtmp ,0,0,0) ;

	// Input Power Spectrumの確保
	AllocFloatSpecArray(&(fsInputPSpec),0,0,0) ;

	// 最適Gain値
	AllocFloatSpecArray(&(fsGH1),0,0,0) ;
	
	AllocFloatSpecArray(&(fsEstVal),0,0,0) ;
	//音声確率
	AllocFloatSpecArray(&(fsVoiceP),0,0,0) ;

	// Set size
	AllocSavedVoiceP(pSavedVoicePtmp ,csInputSpec.ha_iUpperFreq ,csInputSpec.ha_iLowerFreq , csInputSpec.ha_numCh) ;

	// Input Power Spectrumの確保
	AllocFloatSpecArray(&(fsInputPSpec),csInputSpec.ha_numCh , 
		csInputSpec.ha_iLowerFreq , csInputSpec.ha_iUpperFreq ) ;
	// PowerSpectrumの作成
	MakePowerSpec(&(fsInputPSpec) ,csInputSpec ) ;

	// 最適Gain値
	AllocFloatSpecArray(&(fsGH1),csInputSpec.ha_numCh , 
		csInputSpec.ha_iLowerFreq , csInputSpec.ha_iUpperFreq ) ;
	
	AllocFloatSpecArray(&(fsEstVal),csInputSpec.ha_numCh , 
		csInputSpec.ha_iLowerFreq , csInputSpec.ha_iUpperFreq ) ;
	//音声確率
	AllocFloatSpecArray(&(fsVoiceP),csInputSpec.ha_numCh , 
		csInputSpec.ha_iLowerFreq , csInputSpec.ha_iUpperFreq ) ;
	
	//テンポラリデータの更新
	//MCRA
	
	//変更 2009.3.16 Morio 音源変更時にha_numProcは0になるため，値が更新されなくなってしまうため修正。
	for(n = 0; n < pSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_numValue ; n ++){
		pSavedMCRAtmp->micary_fsS.micary_faSpec.ha_fValue[n] = pSavedMCRA->micary_fsS.micary_faSpec.ha_fValue[n] ;
		pSavedMCRAtmp->micary_fsSmin.micary_faSpec.ha_fValue[n] = pSavedMCRA->micary_fsSmin.micary_faSpec.ha_fValue[n] ;
		pSavedMCRAtmp->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[n] = pSavedMCRA->micary_fsStateNoisePSpec.micary_faSpec.ha_fValue[n] ;
		pSavedMCRAtmp->micary_fsStmp.micary_faSpec.ha_fValue[n] = pSavedMCRA->micary_fsStmp.micary_faSpec.ha_fValue[n] ;
	}
	pSavedMCRAtmp->ha_numProc = pSavedMCRA->ha_numProc;


	for(n = 0 ; n < pSavedRev->micary_fsRevPSpec.micary_faSpec.ha_numValue ; n ++ ){
		pSavedRevtmp->micary_fsRevPSpec.micary_faSpec.ha_fValue[n] = pSavedRev->micary_fsRevPSpec.micary_faSpec.ha_fValue[n] ;
	}
	//VoiceP
	for(n = 0 ; n < pSavedVoiceP->micary_faSmSNRF.ha_numValue ; n ++){
		pSavedVoicePtmp->micary_faSmSNRF.ha_fValue[n] = pSavedVoiceP ->micary_faSmSNRF.ha_fValue[n] ;
		pSavedVoicePtmp->micary_faSmSNRP.ha_fValue[n] = pSavedVoiceP ->micary_faSmSNRP.ha_fValue[n] ;
	}
	for(n = 0 ; n < pSavedVoiceP->micary_fsSmSNR.micary_faSpec.ha_numValue ; n ++){
		pSavedVoicePtmp->micary_fsSmSNR.micary_faSpec.ha_fValue[n] = pSavedVoiceP ->micary_fsSmSNR.micary_faSpec.ha_fValue[n] ;
	}
	
	
	//Estimate Noise(妨害音の推定)
	// 1.MCRA(estStateNoise)	
	if(PF_EstStateNoise( fsInputPSpec ,prmMCRA ,pSavedMCRAtmp ,&(pSavedMCRA->micary_fsStateNoisePSpec) ,
		&(pSavedMCRA->micary_fsSmin) ,	&(pSavedMCRA->micary_fsStmp) ,&(pSavedMCRA->micary_fsS) ) != 1){
			printf("Error! Fail to estimate MCRA.\n");
			
		        FreeSavedVoiceP(pSavedVoicePtmp) ;
			free(pSavedVoicePtmp) ;
			pSavedVoicePtmp = NULL ;
		

			// Input Power SpectrumのFree
			FreeFloatSpecArray(&(fsInputPSpec)) ;
	
			// 最適Gain値 Free
			FreeFloatSpecArray(&(fsGH1 )) ;
	
			FreeFloatSpecArray(&(fsEstVal)) ;
			//音声確率
			FreeFloatSpecArray(&(fsVoiceP)) ;
	
			return 0 ;
		}
	
	
	pSavedMCRA->ha_numProc ++ ;
	

	
	// 2.Leak noise
	if(PF_EstLeak(&(pSavedMCRA ->micary_fsS) ,prmLeak ,pSavedLeak ) != 1){
		printf("Error! Fail to estimate Leak noise.\n");
	        FreeSavedVoiceP(pSavedVoicePtmp) ;
	       	free(pSavedVoicePtmp) ;
		pSavedVoicePtmp = NULL ;
		
	    	// Input Power SpectrumのFree
	      	FreeFloatSpecArray(&(fsInputPSpec)) ;
	
	       	// 最適Gain値 Free
	       	FreeFloatSpecArray(&(fsGH1 )) ;
	
	       	FreeFloatSpecArray(&(fsEstVal)) ;
	       	//音声確率
	       	FreeFloatSpecArray(&(fsVoiceP)) ;
		return 0 ;
	}
	
	
	// 3.Estimate total noise(Noise=MCRA0 + Leak + Rev0)
	if(PF_EstNoise(&(pSavedMCRAtmp->micary_fsStateNoisePSpec) ,pSavedLeak ,
		&(pSavedRevtmp->micary_fsRevPSpec) ,pSavedLamda) != 1){
			printf("Error! Fail to estimate total noise.\n");
			FreeSavedVoiceP(pSavedVoicePtmp) ;
			free(pSavedVoicePtmp) ;
			pSavedVoicePtmp = NULL ;
		
			// Input Power SpectrumのFree
			FreeFloatSpecArray(&(fsInputPSpec)) ;
	
			// 最適Gain値 Free
			FreeFloatSpecArray(&(fsGH1 )) ;
	
			FreeFloatSpecArray(&(fsEstVal)) ;
			//音声確率
			FreeFloatSpecArray(&(fsVoiceP)) ;
			return 0 ;
		}
	
	// 4.Estimate SN比
	if(PF_EstSN(fsInputPSpec ,*pSavedLamda ,pSavedSN ,
		prmSN ,&(fsGH1),&(pSavedSN->micary_fsSN),&(fsEstVal)) != 1){
			printf("Error! Fail to estimate SN ratio.\n");
			
			FreeSavedVoiceP(pSavedVoicePtmp) ;
			free(pSavedVoicePtmp) ;
			pSavedVoicePtmp = NULL ;
		
			// Input Power SpectrumのFree
			FreeFloatSpecArray(&(fsInputPSpec)) ;
	
			// 最適Gain値 Free
			FreeFloatSpecArray(&(fsGH1 )) ;
	
			FreeFloatSpecArray(&(fsEstVal)) ;
			//音声確率
			FreeFloatSpecArray(&(fsVoiceP)) ;
			return 0 ;
	}
	
	// 5.Estimate Speech probability
	if(PF_EstVoiceP(pSavedSN->micary_fsSN ,fsEstVal ,prmVoiceP ,pSavedVoicePtmp ,&fsVoiceP,
		&(pSavedVoiceP->micary_fsSmSNR) ,&(pSavedVoiceP->micary_faSmSNRF) ,&(pSavedVoiceP->micary_faSmSNRP)) != 1){
			printf("Error! Fail to estimate speech probability.\n");
			FreeSavedVoiceP(pSavedVoicePtmp) ;
			free(pSavedVoicePtmp) ;
			pSavedVoicePtmp = NULL ;
		
			// Input Power SpectrumのFree
			FreeFloatSpecArray(&(fsInputPSpec)) ;
	
			// 最適Gain値 Free
			FreeFloatSpecArray(&(fsGH1 )) ;
	
			FreeFloatSpecArray(&(fsEstVal)) ;
			//音声確率
			FreeFloatSpecArray(&(fsVoiceP)) ;
			return 0 ;
		}
	
	// 6.Direct Component : Shatp ==> SavedSN->micary_fsInputPSpec
	for(n = 0 ; n < fsInputPSpec.micary_faSpec.ha_numValue ; n++){
		pSavedSN->micary_fsInputPSpec.micary_faSpec.ha_fValue[n] = pow(fsGH1.micary_faSpec.ha_fValue[n],2.0)*fsInputPSpec.micary_faSpec.ha_fValue[n] ; 
	}

	// 7.output(Result PF)
	for(n = 0 ; n < csInputSpec.micary_cmpaSpec.ha_numValue ; n ++){
		pcsOutputSpec->micary_cmpaSpec.ha_cmpValue[n].re = csInputSpec.micary_cmpaSpec.ha_cmpValue[n].re
			*fsGH1.micary_faSpec.ha_fValue[n] * fsVoiceP.micary_faSpec.ha_fValue[n] ;
		pcsOutputSpec->micary_cmpaSpec.ha_cmpValue[n].im = csInputSpec.micary_cmpaSpec.ha_cmpValue[n].im
			*fsGH1.micary_faSpec.ha_fValue[n] * fsVoiceP.micary_faSpec.ha_fValue[n] ;
	}
	
	
	// 8.Estimate Reverbaration 
	if(PF_EstRev(pSavedSN->micary_fsInputPSpec ,prmRev ,pSavedRevtmp ,&pSavedRev->micary_fsRevPSpec ) != 1){
		printf("Error! Fail to estimate reverbration.\n");
		FreeSavedVoiceP(pSavedVoicePtmp) ;
	       	free(pSavedVoicePtmp) ;
	    	pSavedVoicePtmp = NULL ;
		
		// Input Power SpectrumのFree
	      	FreeFloatSpecArray(&(fsInputPSpec)) ;
	
	       	// 最適Gain値 Free
	       	FreeFloatSpecArray(&(fsGH1 )) ;
	
	       	FreeFloatSpecArray(&(fsEstVal)) ;
	       	//音声確率
	       	FreeFloatSpecArray(&(fsVoiceP)) ;
		return 0 ;
	}
		
	//2007/06/19
	FreeFloatSpecArray(&fsVoiceP);
	FreeFloatSpecArray(&fsEstVal);
	FreeFloatSpecArray(&fsGH1) ;
	FreeFloatSpecArray(&fsInputPSpec);
	FreeSavedVoiceP(pSavedVoicePtmp) ;
	
	//if(!pSavedVoicePtmp)
	free(pSavedVoicePtmp) ;
	pSavedVoicePtmp = NULL ;
		
	
	return 1;

}

// -----------------------------------------------------------------------------------------------------
// 背景ノイズの初期化
int PF_InitMCRA(MICARY_ComplexSpecArray csInputSpec ,
				MICARY_FloatSpecArray *pfsOutputSpec ,HA_Float ha_fNoiseCoef)
{
	HA_Float Xpsum = 0 ; 
	int index ;

	if(pfsOutputSpec->micary_faSpec.ha_numValue != csInputSpec.micary_cmpaSpec.ha_numValue){
	  FreeFloatSpecArray(pfsOutputSpec) ;	
	  AllocFloatSpecArray(pfsOutputSpec,csInputSpec.ha_numCh ,
			csInputSpec.ha_iLowerFreq , csInputSpec.ha_iUpperFreq ) ;
	}

	for(index = 0 ; index < csInputSpec.micary_cmpaSpec.ha_numValue ; index ++){
		Xpsum += pow(csInputSpec.micary_cmpaSpec.ha_cmpValue[index].re,2) +pow(csInputSpec.micary_cmpaSpec.ha_cmpValue[index].im,2) ;
	}
	for(index = 0 ; index < csInputSpec.micary_cmpaSpec.ha_numValue ; index ++){
		pfsOutputSpec -> micary_faSpec.ha_fValue[index] = Xpsum / pow(csInputSpec.ha_numCh,2) * ha_fNoiseCoef ;
	}
	return 1 ;
}


// ---------------------------------------------------------------------------
// Clculate Power Spectrum
int MakePowerSpec(MICARY_FloatSpecArray * pfsPowerSpec ,
					MICARY_ComplexSpecArray csSpec ) 
{
	int iRtn = 0 ;
	int count = 0 ;

	pfsPowerSpec -> ha_iLowerFreq = csSpec .ha_iLowerFreq ;
	pfsPowerSpec -> ha_iUpperFreq = csSpec .ha_iUpperFreq;
	pfsPowerSpec -> ha_numCh = csSpec .ha_numCh ;
	pfsPowerSpec -> micary_faSpec.ha_numValue = csSpec .micary_cmpaSpec.ha_numValue ;
	
	while(1)
	{
		pfsPowerSpec -> micary_faSpec.ha_fValue[count] 
			= pow(csSpec .micary_cmpaSpec.ha_cmpValue[count].re,2) +pow(csSpec .micary_cmpaSpec.ha_cmpValue[count]. im,2) ;
		
			if(count == csSpec .micary_cmpaSpec.ha_numValue-1)
			{
				iRtn = 1 ;
				break ;
			}
			count ++ ;

	}
	return iRtn ;
}

