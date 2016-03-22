// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_SeparateMat.cpp
 \brief	\~English Source file for execute GHDSS. (GHDSS program)
 \brief	\~Japanese GHDSS実行のためのソースファイル(GHDSS program)
 \author	Sachiko Suzuki , Aki Yamamoto(2010) 
 \date	22 Feb 2011
 \version	$Id: GHDSS_SeparateMat.cpp 3538 2012-02-29 07:25:35Z tanaka $
*/
// ---------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GHDSS_SeparateMat.h"

#define NDEBUG

// added 2012/03/06
	// モジュール内変数の定義サイズ
static int g_SepMat_Nsrc;
static int g_SepMat_Nmic;

	// _dCalcSSStepSize_GHDSS用変数
static MICARY_dComplexArray g_SS_Ryy, g_SS_Zns1, g_SS_E, g_SS_Jss, g_SS_Jpri, g_SS_Znm1, g_SS_Znsns, g_SS_Znsnm, g_SS_Z11;
static MICARY_ComplexArray g_SS_PY, g_SS_QY ;

	// _dCalcLCStepSize_GHDSS用変数
static MICARY_dComplexArray g_LC_E, g_LC_Znsns, g_LC_Znsnm, g_LC_Jlc ;
static MICARY_dComplexArray g_LC_Atmp ; //TF(caAはTF_CONJ)

	// dUpdateSeparationMatGHDSS用変数
static MICARY_ComplexArray g_Up_A, g_Up_X, g_Up_W, g_Up_Y, g_Up_J_SS, g_Up_J_LC; //Ryy,Rxx,
static MICARY_dComplexArray g_Up_Rxx, g_Up_Ryy ;

	// モジュール内変数の初期化
void __SepMat_InitializeValues(){
	g_SepMat_Nsrc = 0;
	g_SepMat_Nmic = 0;

	// SS
    dComplexArrayAlloc(&g_SS_Ryy,0) ;
    dComplexArrayAlloc(&g_SS_Zns1,0) ;
    dComplexArrayAlloc(&g_SS_Znm1,0) ;
    dComplexArrayAlloc(&g_SS_E,0) ;
    dComplexArrayAlloc(&g_SS_Jss,0) ;
    dComplexArrayAlloc(&g_SS_Jpri,0) ;
    dComplexArrayAlloc(&g_SS_Znsns,0) ;
    dComplexArrayAlloc(&g_SS_Znsnm,0) ;
    dComplexArrayAlloc(&g_SS_Z11,0) ;
    ComplexArrayAlloc(&g_SS_PY,0) ;
    ComplexArrayAlloc(&g_SS_QY,0) ;
	
    dComplexArrayAlloc(&g_SS_Z11,1*1) ;	// ここで決める

	// LC
	dComplexArrayAlloc(&g_LC_E,0) ;
	dComplexArrayAlloc(&g_LC_Znsns,0) ;
	dComplexArrayAlloc(&g_LC_Znsnm,0) ;
	dComplexArrayAlloc(&g_LC_Jlc,0) ;
	dComplexArrayAlloc(&g_LC_Atmp,0) ;
	
	// Update
	ComplexArrayAlloc(&g_Up_A,0) ;
	ComplexArrayAlloc(&g_Up_X,0) ;
	ComplexArrayAlloc(&g_Up_W,0) ;
	ComplexArrayAlloc(&g_Up_Y,0) ;
	dComplexArrayAlloc(&g_Up_Ryy,0) ;
	dComplexArrayAlloc(&g_Up_Rxx,0) ;
	ComplexArrayAlloc(&g_Up_J_SS,0) ;
	ComplexArrayAlloc(&g_Up_J_LC,0) ;
}
	// モジュール内変数のAlloc (Nsrc, Nmicにより変化するもののみ)
void __SepMat_AllocValues(int Nsrc, int Nmic){
	
	if (Nsrc != g_SepMat_Nsrc){
		__SepMat_FreeValues(true, false);
		
		// SS
	    dComplexArrayAlloc(&g_SS_Ryy,Nsrc*Nsrc) ;
    	dComplexArrayAlloc(&g_SS_Zns1,Nsrc*1) ;
    	dComplexArrayAlloc(&g_SS_E,Nsrc*Nsrc) ;
    	dComplexArrayAlloc(&g_SS_Jss,Nsrc*Nmic) ;
    	dComplexArrayAlloc(&g_SS_Jpri,Nsrc*Nmic) ;
    	dComplexArrayAlloc(&g_SS_Znsns,Nsrc*Nsrc) ;
    	dComplexArrayAlloc(&g_SS_Znsnm,Nsrc*Nmic) ;
	    ComplexArrayAlloc(&g_SS_PY,Nsrc*1) ;
    	ComplexArrayAlloc(&g_SS_QY,Nsrc*1) ;

	// 2014/11/5 Freeしたまま再設定していない		
    dComplexArrayAlloc(&g_SS_Z11,1*1) ;	// ここで決める

		// LC
		dComplexArrayAlloc(&g_LC_E,Nsrc*Nsrc) ;
		dComplexArrayAlloc(&g_LC_Znsns,Nsrc*Nsrc) ;
		dComplexArrayAlloc(&g_LC_Znsnm,Nsrc*Nmic) ;
		dComplexArrayAlloc(&g_LC_Jlc,Nsrc*Nmic) ;
		dComplexArrayAlloc(&g_LC_Atmp,Nsrc*Nmic) ;
		
		// Update
		ComplexArrayAlloc(&g_Up_A,Nmic*Nsrc) ;
		ComplexArrayAlloc(&g_Up_W,Nsrc*Nmic) ;
		ComplexArrayAlloc(&g_Up_Y,Nsrc) ;
		dComplexArrayAlloc(&g_Up_Ryy,Nsrc*Nsrc) ;
		ComplexArrayAlloc(&g_Up_J_SS,Nsrc*Nmic) ;
		ComplexArrayAlloc(&g_Up_J_LC,Nsrc*Nmic) ;
	}
	if (Nmic != g_SepMat_Nmic){
		__SepMat_FreeValues(false, true);

		// SS
	    dComplexArrayAlloc(&g_SS_Znm1,Nmic*1) ;
		
		// Update
		ComplexArrayAlloc(&g_Up_X,Nmic) ;
		dComplexArrayAlloc(&g_Up_Rxx,Nmic*Nmic) ;
	}
	
	g_SepMat_Nsrc = Nsrc;
	g_SepMat_Nmic = Nmic;
}
	// モジュール内変数のFree
void __SepMat_FreeValues(bool bFreeSrc, bool bFreeMic){
	if (bFreeSrc){
		// SS
		dComplexArrayFree(&g_SS_Ryy) ;
		dComplexArrayFree(&g_SS_Zns1) ;
		dComplexArrayFree(&g_SS_E) ;
		dComplexArrayFree(&g_SS_Jss) ;
		dComplexArrayFree(&g_SS_Jpri) ;
		dComplexArrayFree(&g_SS_Znsns) ;
		dComplexArrayFree(&g_SS_Znsnm) ;
		dComplexArrayFree(&g_SS_Z11) ;
		ComplexArrayFree(&g_SS_PY) ;
		ComplexArrayFree(&g_SS_QY) ;
	
		// LC
		dComplexArrayFree(&g_LC_E) ;
		dComplexArrayFree(&g_LC_Znsns) ;
		dComplexArrayFree(&g_LC_Znsnm) ;
		dComplexArrayFree(&g_LC_Jlc) ;
		dComplexArrayFree(&g_LC_Atmp) ;
	
		// Update
		ComplexArrayFree(&g_Up_A) ;
		ComplexArrayFree(&g_Up_W) ;
		ComplexArrayFree(&g_Up_Y) ;
		dComplexArrayFree(&g_Up_Ryy) ;
		ComplexArrayFree(&g_Up_J_SS) ;
		ComplexArrayFree(&g_Up_J_LC) ;

		g_SepMat_Nsrc = 0;
	}
	if (bFreeMic){
		// SS
		dComplexArrayFree(&g_SS_Znm1) ;
		
		// Update
		ComplexArrayFree(&g_Up_X) ;
		dComplexArrayFree(&g_Up_Rxx) ;

		g_SepMat_Nmic = 0;
	}
}

// ---------------------------------------------------------------------
//2011.02.03
int CalcTFFreeH(MICARY_TFDatabase * psTFDB , 
					 HA_Integer iUpperFreq ,
					 MICARY_PrmFrame sPrmFrame ,
					 MICARY_LocationArray locaSrcPos ,
					 MICARY_LocationArray locaMicPos ,
					 HA_Float fSpeedSound)
{	
	int idsrc, idmic, nf, index ;
	double k, freq, sx, sy, sz, mx, my, mz, dr, tmp, c;
	
	//pi = 3.141592653589793238462643383279;
	// 2010.9.29
	//c = 331.5 + 0.6 * fTemperature ;
	c = fSpeedSound ;

	index = 0 ;
	nf = 0 ;

	// alloc
	if(psTFDB->micary_locaMic.ha_numLocation != locaMicPos.ha_numLocation || 
		psTFDB -> micary_locaSource.ha_numLocation != locaSrcPos .ha_numLocation ||
		psTFDB ->micary_iaFreqLine.ha_numValue != iUpperFreq+1)
	{
		// 2011/11/16 changed by N.Tanaka
		//TFDatabaseFree(psTFDB);
		//TFDatabaseAlloc(psTFDB, iUpperFreq+1,locaSrcPos.ha_numLocation,locaMicPos.ha_numLocation);
		TFDatabaseClear(psTFDB);
		TFDatabaseAlloc_r(psTFDB, iUpperFreq+1,locaSrcPos.ha_numLocation,locaMicPos.ha_numLocation,
								__MICARY_def_MAX_NFL, __MICARY_def_MAX_NSRC, __MICARY_def_MAX_NCH);
	}
	
	
	while(__TRUE__){
		
		freq = nf * sPrmFrame.ha_fFs / sPrmFrame.ha_numNFFT ; 
		k = 2.0 * M_pi *  freq / c;
		
		for(idsrc = 0 ; idsrc < locaSrcPos.ha_numLocation ; idsrc ++){
			for(idmic = 0 ; idmic < locaMicPos .ha_numLocation ; idmic ++){
		
		
		// Changed by morio 2012/11/09
				//sx = locaSrcPos.micary_Location[idsrc].ha_fX ;
				//sy = locaSrcPos.micary_Location[idsrc].ha_fY ;
				//sz = locaSrcPos.micary_Location[idsrc].ha_fZ ;
				
				mx = locaSrcPos.micary_Location[idsrc].ha_fX - locaMicPos.micary_Location[idmic].ha_fX ;
				my = locaSrcPos.micary_Location[idsrc].ha_fY - locaMicPos.micary_Location[idmic].ha_fY ;
				mz = locaSrcPos.micary_Location[idsrc].ha_fZ - locaMicPos.micary_Location[idmic].ha_fZ ;
				
				//dr = sqrt(pow((float)sx, (float)2.0)+pow((float)sy, (float)2.0)+pow((float)sz, (float)2.0)) - sqrt(pow((float)mx, (float)2.0)+pow((float)my, (float)2.0)+pow((float)mz, (float)2.0)) ;
				//dr = sqrt(pow((double)sx, (double)2.0)+pow((double)sy, (double)2.0)+pow((double)sz, (double)2.0)) - sqrt(pow((double)mx, (double)2.0)+pow((double)my, (double)2.0)+pow((double)mz, (double)2.0)) ;
				dr = sqrt(pow((double)mx, (double)2.0)+pow((double)my, (double)2.0)+pow((double)mz, (double)2.0)) ;
		//////		
		
				tmp = cos(-1.0*k*dr) ;
				
				psTFDB ->micary_cmpaTF .ha_cmpValue[index].re = (1+(dr*sPrmFrame.ha_fFs/c)/200) * tmp ;
			
				tmp = sin(-1.0*k*dr) ;
				psTFDB ->micary_cmpaTF .ha_cmpValue[index].im = (1+(dr*sPrmFrame.ha_fFs/c)/200) * tmp ;
							
				index ++ ;
			}
		}
		psTFDB -> micary_iaFreqLine.ha_iValue[nf] = nf ;

		if (nf >= iUpperFreq){
			break ;
		}
		nf ++ ;
		idsrc = 0 ;
		idmic = 0 ;

		
	}
	psTFDB ->micary_iaFreqLine.ha_numValue = nf+1 ;
	memcpy(&psTFDB ->micary_locaMic , &locaMicPos , sizeof(MICARY_LocationArray)) ;
	memcpy(&psTFDB ->micary_locaSource , &locaSrcPos , sizeof(MICARY_LocationArray)) ;
	
	return __TRUE__ ;
}
// ---------------------------------------------------------------------
//2011.02.03
int CalcTFFreeH2(MICARY_TFDatabase * psTFDB , 
					 MICARY_PrmFrame sPrmFrame ,
					 int idsrc,
					 HA_Float fSpeedSound)
{	
	int idmic, nf, index ;
	double k, freq, sx, sy, sz, mx, my, mz, dr, tmp, c;
	
	MICARY_LocationArray *plocaSrcPos = &(psTFDB->micary_locaSource);
	MICARY_LocationArray *plocaMicPos = &(psTFDB->micary_locaMic);

	//pi = 3.141592653589793238462643383279;
	// 2010.9.29
	//c = 331.5 + 0.6 * fTemperature ;
	c = fSpeedSound ;

	nf = 0 ;
	while(__TRUE__){
		
		freq = nf * sPrmFrame.ha_fFs / sPrmFrame.ha_numNFFT ; 
		k = 2.0 * M_pi *  freq / c;
		
		for(idmic = 0 ; idmic < plocaMicPos->ha_numLocation ; idmic ++){
		
			index = nf*plocaSrcPos->ha_numLocation*plocaMicPos->ha_numLocation + idsrc*plocaMicPos->ha_numLocation;

	// Changed by morio 2012/11/09
	
			//sx = plocaSrcPos->micary_Location[idsrc].ha_fX ;
			//sy = plocaSrcPos->micary_Location[idsrc].ha_fY ;
			//sz = plocaSrcPos->micary_Location[idsrc].ha_fZ ;
				
			mx = plocaSrcPos->micary_Location[idsrc].ha_fX - plocaMicPos->micary_Location[idmic].ha_fX ;
			my = plocaSrcPos->micary_Location[idsrc].ha_fY - plocaMicPos->micary_Location[idmic].ha_fY ;
			mz = plocaSrcPos->micary_Location[idsrc].ha_fZ - plocaMicPos->micary_Location[idmic].ha_fZ ;
				
			//dr = sqrt(pow((float)sx, (float)2.0)+pow((float)sy, (float)2.0)+pow((float)sz, (float)2.0)) - sqrt(pow((float)mx, (float)2.0)+pow((float)my, (float)2.0)+pow((float)mz, (float)2.0)) ;
			//dr = sqrt(pow((double)sx, (double)2.0)+pow((double)sy, (double)2.0)+pow((double)sz, (double)2.0)) - sqrt(pow((double)mx, (double)2.0)+pow((double)my, (double)2.0)+pow((double)mz, (double)2.0)) ;
			dr = sqrt(pow((double)mx, (double)2.0)+pow((double)my, (double)2.0)+pow((double)mz, (double)2.0)) ;
	//////		
			
			tmp = cos(-1.0*k*dr) ;
			
			psTFDB ->micary_cmpaTF .ha_cmpValue[index].re = (1+(dr*sPrmFrame.ha_fFs/c)/200) * tmp ;
		
			tmp = sin(-1.0*k*dr) ;
			psTFDB ->micary_cmpaTF .ha_cmpValue[index].im = (1+(dr*sPrmFrame.ha_fFs/c)/200) * tmp ;
			
		}

		nf ++ ;
		
		if (nf >= psTFDB ->micary_iaFreqLine.ha_numValue){
			break ;
		}
	}
	
	return __TRUE__ ;
}
// ---------------------------------------------------------------------
int MakeWMatGHDSS(MICARY_TFDatabase sTFDB,
					  MICARY_SeparationMat * psSeparationMat)
{
	int index , count_g , count_r,g,r ,count_f,i;
	index = 0 ;
	count_g = 0 ;
	count_r = 0 ;
	count_f = 0 ;
	g = sTFDB.micary_locaMic.ha_numLocation ;
	r = sTFDB.micary_locaSource.ha_numLocation ;

	//alloc
	if(psSeparationMat ->micary_cmpaSeparation.ha_numValue != sTFDB.micary_cmpaTF.ha_numValue){
		// 2011/11/16 changed by N.Tanaka
		//SeparationMatFree(psSeparationMat) ;
		//SeparationMatAlloc(psSeparationMat,sTFDB .micary_iaFreqLine.ha_numValue,
		//	sTFDB.micary_locaSource.ha_numLocation,sTFDB.micary_locaMic.ha_numLocation) ;
		SeparationMatClear(psSeparationMat) ;
		SeparationMatAlloc_r(psSeparationMat,sTFDB .micary_iaFreqLine.ha_numValue,
			sTFDB.micary_locaSource.ha_numLocation,sTFDB.micary_locaMic.ha_numLocation,
			__MICARY_def_MAX_NFL, __MICARY_def_MAX_NSRC, __MICARY_def_MAX_NCH) ;
	}
	// 2010.8.26
	// 周波数ラインを更新
	for(i = 0 ; i < sTFDB.micary_iaFreqLine.ha_numValue ;i++){
		psSeparationMat->micary_iaFreqLinex.ha_iValue[i] = sTFDB.micary_iaFreqLine.ha_iValue[i] ;
	}
	psSeparationMat->micary_iaFreqLinex.ha_numValue = sTFDB.micary_iaFreqLine.ha_numValue ;


	//伝達関数から分離行列の初期値を作成
	while(__TRUE__){
		if(index >= sTFDB.micary_iaFreqLine.ha_numValue * sTFDB.micary_locaMic.ha_numLocation*sTFDB.micary_locaSource.ha_numLocation){
			break ;
		}

		psSeparationMat ->micary_cmpaSeparation.ha_cmpValue[index].re = sTFDB.micary_cmpaTF.ha_cmpValue[count_f*g*r+count_r*g+count_g].re / sTFDB.micary_locaMic.ha_numLocation ;
		// modyfied 2008/7/24
		psSeparationMat ->micary_cmpaSeparation.ha_cmpValue[index].im = sTFDB.micary_cmpaTF.ha_cmpValue[count_f*g*r+count_r*g+count_g].im / sTFDB.micary_locaMic.ha_numLocation ;

		index ++ ;
		count_r ++ ;
		if(count_r >= r){
			count_g ++ ;
			count_r = 0 ;
			if(count_g >=g){
				count_r = 0 ;
				count_g = 0 ;
				count_f ++ ;
			}
		}
	}
						  
	return __TRUE__ ;
}

// ---------------------------------------------------------------------------------------------
// 2012/01/13 added
int MakeWMatGHDSS2(MICARY_TFDatabase sTFDB,
					  MICARY_SeparationMat * psSeparationMat,
					  int idsrc)
{
	int index_tf, index_sep , count_g , count_r,g,r ,count_f,i;
	count_g = 0 ;
	count_f = 0 ;
	g = sTFDB.micary_locaMic.ha_numLocation ;
	r = sTFDB.micary_locaSource.ha_numLocation ;

	//伝達関数から分離行列の初期値を作成
	for (count_f = 0 ; count_f < sTFDB.micary_iaFreqLine.ha_numValue ; count_f++){
		for (count_g = 0 ; count_g < g ; count_g++){
			index_tf = count_f*g*r + idsrc*g + count_g;
			index_sep = count_f*g*r + count_g*r + idsrc;
			
			psSeparationMat ->micary_cmpaSeparation.ha_cmpValue[index_sep].re = sTFDB.micary_cmpaTF.ha_cmpValue[index_tf].re / sTFDB.micary_locaMic.ha_numLocation ;
			psSeparationMat ->micary_cmpaSeparation.ha_cmpValue[index_sep].im = sTFDB.micary_cmpaTF.ha_cmpValue[index_tf].im / sTFDB.micary_locaMic.ha_numLocation ;
		}
	}
	
	return __TRUE__ ;
}

// ---------------------------------------------------------------------------------------------
// GHDSSの実行
// 2011/10/27 changed by N.Tanaka
/*
void _dGetPQY(MICARY_ComplexArray *caPY,MICARY_ComplexArray *caQY,
							 MICARY_ComplexArray caY,HA_Float ha_fSSScal)
{
	int i ;
	HA_Float fabsY,fabsYs,ftmp ;
	HA_Complex cmpExpY ;

	for(i = 0 ; i < caY.ha_numValue ; i++){
	    fabsY = sqrt(pow((double)caY.ha_cmpValue[i].re, (double)2.0) + pow((double)caY.ha_cmpValue[i].im, (double)2.0)) + _MINIMUM_ZERO_VALUE;
	    fabsYs = fabsY * ha_fSSScal ;
	    cmpExpY.re = caY.ha_cmpValue[i].re/fabsY ;
	    cmpExpY.im = caY.ha_cmpValue[i].im/fabsY ;
	    
	    caPY->ha_cmpValue[i].re = tanh(fabsYs) * cmpExpY.re ;
	    caPY->ha_cmpValue[i].im = tanh(fabsYs) * cmpExpY.im ;
	    
	    ftmp = pow((double)cosh(fabsYs), (double)2.0) ;
	    ftmp = fabsYs/ftmp*ha_fSSScal ;
	    caQY->ha_cmpValue[i].re = (tanh(fabsYs) + ftmp) * cmpExpY.re ;
	    caQY->ha_cmpValue[i].im = (tanh(fabsYs) + ftmp) * cmpExpY.im ;
	}
	return ;
}
*/
/*
void _dGetPQY(MICARY_ComplexArray *caPY,MICARY_ComplexArray *caQY,
							 MICARY_ComplexArray caY,HA_Float ha_fSSScal)
{
	int i ;
	HA_Float fabsY,fabsYs,ftmp, ftanhY;
	HA_Complex cmpExpY ;

	for(i = 0 ; i < caY.ha_numValue ; i++){
	    fabsY = sqrt(pow((double)caY.ha_cmpValue[i].re, (double)2.0) + pow((double)caY.ha_cmpValue[i].im, (double)2.0)) + _MINIMUM_ZERO_VALUE;
	    fabsYs = fabsY * ha_fSSScal ;
	    
	    ftanhY = tanh(fabsYs);
	    
	    cmpExpY.re = caY.ha_cmpValue[i].re/fabsY ;
	    cmpExpY.im = caY.ha_cmpValue[i].im/fabsY ;
	    
	    caPY->ha_cmpValue[i].re = ftanhY * cmpExpY.re ;
	    caPY->ha_cmpValue[i].im = ftanhY * cmpExpY.im ;
	    
	    ftmp = pow((double)cosh(fabsYs), (double)2.0) ;
	    ftmp = fabsYs/ftmp*ha_fSSScal ;
	    caQY->ha_cmpValue[i].re = (ftanhY + ftmp) * cmpExpY.re ;
	    caQY->ha_cmpValue[i].im = (ftanhY + ftmp) * cmpExpY.im ;
	}
	return ;
}
*/
void _dGetPQY(MICARY_ComplexArray *caPY,MICARY_ComplexArray *caQY,
							 MICARY_ComplexArray caY,HA_Float ha_fSSScal)
{
	int i ;
	HA_Float fabsY,fabsYs,ftmp, ftanhY;
	HA_Complex cmpExpY ;
	HA_Complex *pYValue, *pPYValue, *pQYValue;

	pYValue = caY.ha_cmpValue;
	pPYValue = caPY->ha_cmpValue;
	pQYValue = caQY->ha_cmpValue;

	for(i = 0 ; i < caY.ha_numValue ; i++, pYValue++, pPYValue++, pQYValue++){
	    fabsY = sqrt(pow((double)pYValue->re, (double)2.0) + pow((double)pYValue->im, (double)2.0)) + _MINIMUM_ZERO_VALUE;
	    fabsYs = fabsY * ha_fSSScal ;
	    
	    ftanhY = tanh(fabsYs);
	    
	    cmpExpY.re = pYValue->re/fabsY ;
	    cmpExpY.im = pYValue->im/fabsY ;
	    
	    pPYValue->re = ftanhY * cmpExpY.re ;
	    pPYValue->im = ftanhY * cmpExpY.im ;
	    
	    ftmp = pow((double)cosh(fabsYs), (double)2.0) ;
	    ftmp = fabsYs/ftmp*ha_fSSScal ;
	    pQYValue->re = (ftanhY + ftmp) * cmpExpY.re ;
	    pQYValue->im = (ftanhY + ftmp) * cmpExpY.im ;
	}
	return ;
}

// ---------------------------------------------------------------------------------------------
// SSの計算
int _dCalcSSStepSize_GHDSS(MICARY_ComplexArray caW ,
							MICARY_ComplexArray caA ,
							MICARY_ComplexArray caY ,
							MICARY_ComplexArray caX , 
							HA_Integer iSSMethod ,
							HA_Float fSSMyu ,
							HA_Float fSSScal ,
							HA_Float fNoiseFloor ,
							MICARY_ComplexArray *pcamyu_J_SS)
{
    int Nsrc,Nmic, k;
	// deleted 2012/03/07
	/*
    MICARY_dComplexArray Ryy, Zns1, E, Jss, Jpri, Znm1, Znsns, Znsnm, Z11;
    MICARY_ComplexArray PY, QY ;
	*/
    double fTmp, fYrms;
    double fTmp2 ;
    
    Nmic = caX.ha_numValue ;
    Nsrc = caY.ha_numValue ;
    
	// changed 2012/03/07
	/*
    // Alloc 
    dComplexArrayAlloc(&Ryy,0) ;
    dComplexArrayAlloc(&Zns1,0) ;
    dComplexArrayAlloc(&Znm1,0) ;
    dComplexArrayAlloc(&E,0) ;
    dComplexArrayAlloc(&Jss,0) ;
    dComplexArrayAlloc(&Jpri,0) ;
    dComplexArrayAlloc(&Znsns,0) ;
    dComplexArrayAlloc(&Znsnm,0) ;
    dComplexArrayAlloc(&Z11,0) ;
    
    
    // Initialize
    dComplexArrayAlloc(&Ryy,Nsrc*Nsrc) ;
    dComplexArrayAlloc(&Zns1,Nsrc*1) ;
    dComplexArrayAlloc(&Znm1,Nmic*1) ;
    dComplexArrayAlloc(&E,Nsrc*Nsrc) ;
    dComplexArrayAlloc(&Jss,Nsrc*Nmic) ;
    dComplexArrayAlloc(&Jpri,Nsrc*Nmic) ;
    dComplexArrayAlloc(&Znsns,Nsrc*Nsrc) ;
    dComplexArrayAlloc(&Znsnm,Nsrc*Nmic) ;
    dComplexArrayAlloc(&Z11,1*1) ;
    
    //if (iOCRAMethod == _OCRA_METHOD_NONE){	//No Average
    ComplexArrayAlloc(&PY,0) ;
    ComplexArrayAlloc(&QY,0) ;
    ComplexArrayAlloc(&PY,Nsrc*1) ;
    ComplexArrayAlloc(&QY,Nsrc*1) ;
    */
    __SepMat_AllocValues(Nsrc, Nmic);
    	
    	
	//printf("Y = ") ;
	//for(k = 0 ; k < caY.ha_numValue ; k++){
	//  printf("%e , %e /",caY.ha_cmpValue[k].re , caY.ha_cmpValue[k].im) ;
	//}
	//printf("\n") ;

    //PY,QY
    _dGetPQY(&g_SS_PY,&g_SS_QY,caY,fSSScal) ;
    
    //Ryy = y*y'
    if(MICARY_ConjTrans_dfCompMat(&g_SS_Zns1, caY, Nsrc, 1) != __TRUE__){
    	/*
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		ComplexArrayFree(&PY) ;
		ComplexArrayFree(&QY) ;
    	*/
		return __FALSE__ ;
    }
    
    // GHDSS
    //if(ProductfdCompMat1(&Ryy,caY,Nsrc,1,Zns1,1,Nsrc) != 1){
    if(MICARY_Product_fdCompMat(&g_SS_Ryy, g_SS_PY, Nsrc, 1, g_SS_Zns1, 1, Nsrc) != __TRUE__){
    	/*
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		ComplexArrayFree(&PY) ;
		ComplexArrayFree(&QY) ;
    	*/

		return __FALSE__ ;
    }


 //   {
	//int i;
	//
	//printf("Ryy: ");
	//for (i = 0; i < Ryy.ha_numValue; i++) {
	//    printf("%e ", Ryy.da_cmpValue[i].re);
	//}
	//printf("\n");
	//
 //   }

    
    //E = Ryy - diag(Ryy)
    //ComplexArrayAlloc(&Znsns,Nsrc*Nsrc) ;
    dComplexArrayInit(&g_SS_Znsns) ;
    //if(DiagdCompMat1(&Znsns ,Ryy,Nsrc, Nsrc) != __TRUE__){
	if(MICARY_Diag_CompMat(&g_SS_Znsns, g_SS_Ryy, Nsrc, Nsrc) != __TRUE__){
		/*
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		ComplexArrayFree(&PY) ;
		ComplexArrayFree(&QY) ;
		*/
		return __FALSE__ ;
    }

    if(MICARY_Sub_CompMat(&g_SS_E, g_SS_Ryy, Nsrc, Nsrc, g_SS_Znsns, Nsrc, Nsrc) != __TRUE__){
    	/*
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		ComplexArrayFree(&PY) ;
		ComplexArrayFree(&QY) ;
    	*/
	
		return __FALSE__ ;
    }
    //{
	//int i;
	//
	//printf("E: ");
	//for (i = 0; i < E.ha_numValue; i++) {
	//    printf("%e ", E.da_cmpValue[i].re);
	//}
	//printf("\n");
	
    //}

    // Jss = (E*qy)*x'
    // Zns1 = E*qy
    //if(ProductdfCompMat1(&Zns1,E,Nsrc,Nsrc,QY,Nsrc,1) != 1){
	if(MICARY_Product_dfCompMat(&g_SS_Zns1, g_SS_E, Nsrc, Nsrc, g_SS_QY, Nsrc, 1) != __TRUE__){
		/*
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		ComplexArrayFree(&PY) ;
		ComplexArrayFree(&QY) ;
		*/
	
		return __FALSE__;
    }

    //X = X'
    //if(ConjTransCompMat1(&caX ,caX,Nmic,1) != 1){
	if(MICARY_ConjTrans_fCompMat(&caX, caX, Nmic, 1) != __TRUE__){
		/*
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		ComplexArrayFree(&PY) ;
		ComplexArrayFree(&QY) ;
		*/
	
		return __FALSE__;
    }
    // Jss = Zns1 * X
    //if(ProductdfCompMat1(&Jss, Zns1, Nsrc, 1, caX, 1, Nmic) != 1){
	if(MICARY_Product_dfCompMat(&g_SS_Jss, g_SS_Zns1, Nsrc, 1, caX, 1, Nmic) != __TRUE__){
		/*
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		ComplexArrayFree(&PY) ;
		ComplexArrayFree(&QY) ;
		*/
	
		return __FALSE__ ;
    }
    
    // Jpri = conj(Jss)
    //if(ConjdCompMat1(&Jpri, Jss) ==0){
	if(MICARY_Conj_CompMat(&g_SS_Jpri, g_SS_Jss) == __FALSE__){
		/*
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		ComplexArrayFree(&PY) ;
		ComplexArrayFree(&QY) ;
		*/
		return __FALSE__ ;
    }
    
    /*
    ComplexArrayFree(&PY) ;
    ComplexArrayFree(&QY) ;
    */
    
    //} // end No Average
   
	// 2010.4.7
	//printf("Eval = ") ;
	//for(k = 0 ; k < E.ha_numValue ; k++){
	//  printf("%e , %e /",E.da_cmpValue[k].re , E.da_cmpValue[k].im) ;
	//}
	//printf("\n") ;


    if(iSSMethod == 0){			// ●myu_J_SS : Fix
	// myu_J_SS = 2.0 * Jss	
		for(k = 0 ; k < g_SS_Jss.ha_numValue ;k++){
			if(g_SS_Jss.da_cmpValue[k].re *(double)fSSMyu > __MAX_FLOAT_VALUE && 
				g_SS_Jss.da_cmpValue[k].im *(double)fSSMyu > __MAX_FLOAT_VALUE){
					printf("Data is Inf.\n") ;
					/*
					dComplexArrayFree(&Ryy) ;
					dComplexArrayFree(&Zns1) ;
					dComplexArrayFree(&Znm1) ;
					dComplexArrayFree(&E) ;
					dComplexArrayFree(&Jss) ;
					dComplexArrayFree(&Jpri) ;
					dComplexArrayFree(&Znsns) ;
					dComplexArrayFree(&Znsnm) ;
					dComplexArrayFree(&Z11) ;
					*/
		
					return __FALSE__ ;
			}
			pcamyu_J_SS->ha_cmpValue[k].re = (HA_Float)(g_SS_Jss.da_cmpValue[k].re * (double)fSSMyu) ;
			pcamyu_J_SS->ha_cmpValue[k].im = (HA_Float)(g_SS_Jss.da_cmpValue[k].im * (double)fSSMyu) ;
		}
    }
    else if(iSSMethod == 1){	// ●myu_J_SS : same as myu_LC(=fSSMyu)
		//Rxx = x'*x 
		//X = X'
		dComplexArrayInit(&g_SS_Znm1) ;

		//if(ConjTransdfCompMat1(&Znm1 ,caX,Nmic,1) != 1){
		if(MICARY_ConjTrans_dfCompMat(&g_SS_Znm1, caX, Nmic, 1) != __TRUE__){
			/*
			dComplexArrayFree(&Ryy) ;
			dComplexArrayFree(&Zns1) ;
			dComplexArrayFree(&Znm1) ;
			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Jss) ;
			dComplexArrayFree(&Jpri) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Z11) ;
			*/
	    
			return __FALSE__ ;
		}
	
		//if(ProductdfCompMat1(&Z11,Znm1,1,Nmic,caX,Nmic,1) != 1){
		if(MICARY_Product_dfCompMat(&g_SS_Z11, g_SS_Znm1, 1, Nmic, caX, Nmic, 1) != __TRUE__){
			/*
			dComplexArrayFree(&Ryy) ;
			dComplexArrayFree(&Zns1) ;
			dComplexArrayFree(&Znm1) ;
			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Jss) ;
			dComplexArrayFree(&Jpri) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Z11) ;
			*/
	    
			return __FALSE__ ;
		}

		//add 2009.2.24
		if(fabs(pow(g_SS_Z11.da_cmpValue[0].re,2.0)) < _MINIMUM_ZERO_VALUE){
			fTmp = 1.0/_MINIMUM_ZERO_VALUE ;
		}
		else{						
			// myu_J_SS = LC_myu(fSSMyu)*Rxx^-2 * Jss	
			fTmp = (double)(1.0/(pow(g_SS_Z11.da_cmpValue[0].re, 2.0))) ;
	    }	
		fTmp = fTmp * (double)fSSMyu ;

		for(k = 0 ; k < g_SS_Jss.ha_numValue ;k++){
			if(g_SS_Jss.da_cmpValue[k].re *fTmp > __MAX_FLOAT_VALUE && 
				g_SS_Jss.da_cmpValue[k].im *fTmp > __MAX_FLOAT_VALUE){
				printf("Data is Inf.\n") ;
				/*
				dComplexArrayFree(&Ryy) ;
				dComplexArrayFree(&Zns1) ;
				dComplexArrayFree(&Znm1) ;
				dComplexArrayFree(&E) ;
				dComplexArrayFree(&Jss) ;
				dComplexArrayFree(&Jpri) ;
				dComplexArrayFree(&Znsns) ;
				dComplexArrayFree(&Znsnm) ;
				dComplexArrayFree(&Z11) ;
				*/
		
				return __FALSE__;
			}
			pcamyu_J_SS->ha_cmpValue[k].re = (HA_Float)(g_SS_Jss.da_cmpValue[k].re *fTmp) ;
			pcamyu_J_SS->ha_cmpValue[k].im = (HA_Float)(g_SS_Jss.da_cmpValue[k].im *fTmp) ;
		}
    }	// end iSSMethod = 1    
    else {			// ●iSSMethod = 2	myu_J_SS : adaptive
		// Znsns = abs(E)
		dComplexArrayInit(&g_SS_Znsns) ;

		//if(AbsdCompMat1(&Znsns ,E) != 1){
		if(MICARY_Abs_CompMat(&g_SS_Znsns , g_SS_E) != __TRUE__){
			/*
			dComplexArrayFree(&Ryy) ;
	    
			dComplexArrayFree(&Zns1) ;
			dComplexArrayFree(&Znm1) ;
			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Jss) ;
			dComplexArrayFree(&Jpri) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Z11) ;
			*/
	    
			return __FALSE__;
		}
		
		// Znsns = Znsns.*Znsns
		//if(DotcdCompMat1(&Znsns ,Znsns ,Znsns) !=1){
		if(MICARY_Dot_CompMat(&g_SS_Znsns ,g_SS_Znsns ,g_SS_Znsns) != __TRUE__){
			/*
			dComplexArrayFree(&Ryy) ;
	    
			dComplexArrayFree(&Zns1) ;
			dComplexArrayFree(&Znm1) ;
			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Jss) ;
			dComplexArrayFree(&Jpri) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Z11) ;
			*/
	    
			return __FALSE__;
		}
		// fTmp = sum(Znsns)
		fTmp = 0.0 ;
		for(k = 0 ; k < g_SS_Znsns.ha_numValue ; k++){
			fTmp = fTmp + g_SS_Znsns.da_cmpValue[k].re ;
		}
		// Znsnm = Jss.*Jpri
		//ComplexArrayAlloc(&Znsnm,Nsrc*Nmic) ;
		dComplexArrayInit(&g_SS_Znsnm) ;
		//if(DotcdCompMat1(&Znsnm ,Jss ,Jpri) !=1){
		if(MICARY_Dot_CompMat(&g_SS_Znsnm , g_SS_Jss , g_SS_Jpri) != __TRUE__){
			/*
			dComplexArrayFree(&Ryy) ;
	    
			dComplexArrayFree(&Zns1) ;
			dComplexArrayFree(&Znm1) ;
			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Jss) ;
			dComplexArrayFree(&Jpri) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Z11) ;
			*/
	    
			return __FALSE__;
		}

		// Znsnm = abs(Znsnm)
		if(MICARY_Abs_CompMat(&g_SS_Znsnm ,g_SS_Znsnm) != __TRUE__){
			/*
			dComplexArrayFree(&Ryy) ;
	    
			dComplexArrayFree(&Zns1) ;
			dComplexArrayFree(&Znm1) ;
			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Jss) ;
			dComplexArrayFree(&Jpri) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Z11) ;
			*/
	    
			return __FALSE__;
		}

		// fTmp = sum(Znsns)
		fTmp2 = 0.0 ;
		for(k = 0 ; k < g_SS_Znsnm.ha_numValue ; k++){
			fTmp2 = fTmp2 + g_SS_Znsnm.da_cmpValue[k].re ;
		}
		if(fabs(fTmp2) < _MINIMUM_ZERO_VALUE){
			//printf("Caution! devide by zero.\n") ;
			fTmp2 = fTmp2 + _MINIMUM_ZERO_VALUE;
		}
		//2010.4.7

		//printf("opt1 = %f",fTmp) ;
		//printf("opt2 = %f",fTmp2) ;

		fTmp = (fTmp / fTmp2 / (double)2.0) ;
		//printf("opt = %10.10f\n",fTmp) ;
	
		// myu_J_SS = fTmp * Js
		for(k = 0 ; k < g_SS_Jss.ha_numValue ;k++){
	    
			if(g_SS_Jss.da_cmpValue[k].re *fTmp > 3.4028E+038 && 
				g_SS_Jss.da_cmpValue[k].im *fTmp > 3.4028E+038){
				printf("Data is Inf val = %e (%e)* %e .\n",g_SS_Jss.da_cmpValue[k].re,g_SS_Jss.da_cmpValue[k].im,fTmp) ;
				/*
				dComplexArrayFree(&Ryy) ;
				dComplexArrayFree(&Zns1) ;
				dComplexArrayFree(&Znm1) ;
				dComplexArrayFree(&E) ;
				dComplexArrayFree(&Jss) ;
				dComplexArrayFree(&Jpri) ;
				dComplexArrayFree(&Znsns) ;
				dComplexArrayFree(&Znsnm) ;
				dComplexArrayFree(&Z11) ;
				*/
		
				return __FALSE__;
			}
	    
			pcamyu_J_SS->ha_cmpValue[k].re = (HA_Float)(g_SS_Jss.da_cmpValue[k].re *fTmp) ;
			pcamyu_J_SS->ha_cmpValue[k].im = (HA_Float)(g_SS_Jss.da_cmpValue[k].im *fTmp) ;
		}
    }	// end iSSMethod = 2
    	// 2010.4.7

	//printf("myuJss = ") ;
	//for(k = 0 ; k < pcamyu_J_SS->ha_numValue ; k++){
	//  printf("%f , %f /",pcamyu_J_SS->ha_cmpValue[k].re , pcamyu_J_SS->ha_cmpValue[k].im) ;
	//}
	//printf("\n") ;

	// noise floor
	// Zns1 = abs(caY) ;
	//ComplexArrayAlloc(&Zns1,Nsrc*1) ;
    dComplexArrayInit(&g_SS_Zns1) ;
    //if(AbsdfCompMat1(&Zns1 ,caY) != 1){
	if(MICARY_Abs_dfCompMat(&g_SS_Zns1 ,caY) != __TRUE__){
		/*
		dComplexArrayFree(&Ryy) ;
	
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		*/
	
		return __FALSE__ ;
    }

    // fYrms
    fYrms = 0.0 ;
    for ( k = 0 ; k < g_SS_Zns1.ha_numValue ; k++){
		fYrms = (double)pow(g_SS_Zns1.da_cmpValue[k].re,2.0) + fYrms ;
    }
    fYrms = fYrms / g_SS_Zns1.ha_numValue ;
    fYrms = (double)sqrt(fYrms) ;
 
    if(fYrms < fNoiseFloor ){
       printf("Noise floor  reset\n") ;
		ComplexArrayInit(pcamyu_J_SS) ;
    }
    /*
    dComplexArrayFree(&Ryy) ;		
    dComplexArrayFree(&Zns1) ;
    dComplexArrayFree(&Znm1) ;
    dComplexArrayFree(&E) ;
    dComplexArrayFree(&Jss) ;
    dComplexArrayFree(&Jpri) ;
    dComplexArrayFree(&Znsns) ;
    dComplexArrayFree(&Znsnm) ;
    dComplexArrayFree(&Z11) ;
    */
 
    return __TRUE__ ;
}
// LCの計算
// ---------------------------------------------------------------------------
int _dCalcLCStepSize_GHDSS(MICARY_ComplexArray caW ,
							MICARY_ComplexArray caA ,
							HA_Integer iNsrc ,
							HA_Integer iNmic ,
							HA_Integer	iLCConst ,
							HA_Integer	iLCMethod ,
							HA_Float fLCMyu ,
							MICARY_ComplexArray *pcamyu_J_LC ) 
{
	// deleted 2012/03/07
	//MICARY_dComplexArray E ,Znsns ,Znsnm,Jlc ;
	int k ;
	double fJlcp,foptLCMyu ;
	// deleted 2012/03/07
	//MICARY_dComplexArray Atmp ; //TF(caAはTF_CONJ)

	// changed 2012/03/07
	/*
	 //Alloc
	dComplexArrayAlloc(&E,0) ;
	dComplexArrayAlloc(&Znsns,0) ;
	dComplexArrayAlloc(&Znsnm,0) ;
	dComplexArrayAlloc(&Jlc,0) ;
	dComplexArrayAlloc(&Atmp,0) ;

	 //Initialize
	dComplexArrayAlloc(&E,iNsrc*iNsrc) ;
	dComplexArrayAlloc(&Znsns,iNsrc*iNsrc) ;
	dComplexArrayAlloc(&Znsnm,iNsrc*iNmic) ;
	dComplexArrayAlloc(&Jlc,iNsrc*iNmic) ;
	dComplexArrayAlloc(&Atmp,iNsrc*iNmic) ;
	*/
	__SepMat_AllocValues(iNsrc, iNmic);
	
	for(k = 0 ; k < caA.ha_numValue ; k++ ){
		g_LC_Atmp.da_cmpValue[k].re = caA.ha_cmpValue[k].re ;
		g_LC_Atmp.da_cmpValue[k].im = caA.ha_cmpValue[k].im * (-1.0) ;
	}
	
	//Znsns = eye(Nsrc) ;
	//if(EyedCompMat1v2(&Znsns,iNsrc,iNsrc) != 1){
	if(MICARY_Eye_CompMat(&g_LC_Znsns,iNsrc,iNsrc) != __TRUE__){
		/*
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;
		*/

		return __FALSE__;
	}

	 //E = W*A
	//if(ProductfdCompMat1(&E,caW,iNsrc,iNmic,Atmp,iNmic,iNsrc) != __TRUE__){
	if(MICARY_Product_fdCompMat(&g_LC_E, caW, iNsrc, iNmic, g_LC_Atmp, iNmic, iNsrc) != __TRUE__){
		/*
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;	
		*/

		return __FALSE__;
	}
	

	 //E = E - Znsns
	//if(SubdCompMat1(&E,E,iNsrc,iNsrc,Znsns,iNsrc,iNsrc) != 1){
	if(MICARY_Sub_CompMat(&g_LC_E, g_LC_E, iNsrc, iNsrc, g_LC_Znsns, iNsrc, iNsrc) != __TRUE__){
		/*
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;
		*/

		return __FALSE__;
	}
	 //LC_Const = FULLは以上まで
	
	if(iLCConst == _LC_CONST_DIAG){			// ●LC_CONST = DIAG
		//if(DiagdCompMat1(&E,E,iNsrc,iNsrc) != 1) {
		if(MICARY_Diag_CompMat(&g_LC_E,g_LC_E,iNsrc,iNsrc) != __TRUE__) {
			/*
			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Jlc) ;
			dComplexArrayFree(&Atmp) ;
			*/

			return __FALSE__;
		}
	}	// end LC_CONST = DIAG
	
	 //Znsnm = A'
	//if(ConjTransdCompMat1(&Znsnm,Atmp,iNmic,iNsrc) !=1){
	if(MICARY_ConjTrans_CompMat(&g_LC_Znsnm, g_LC_Atmp, iNmic, iNsrc) != __TRUE__){
		/*
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;
		*/

		return __FALSE__;
	}
	
	 //Jlc = E * Znsnm
	//if(ProductdCompMat1(&Jlc,E,iNsrc,iNsrc,Znsnm,iNsrc,iNmic) != __TRUE__){
	if(MICARY_Product_CompMat(&g_LC_Jlc, g_LC_E, iNsrc, iNsrc, g_LC_Znsnm, iNsrc, iNmic) != __TRUE__){
		/*
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;
		*/

		return __FALSE__;
	}

	 //ステップサイズと更新量の決定
	if(iLCMethod == _LC_METHOD_FIX){	//	●LC_Method = Fix
		for(k = 0 ; k < g_LC_Jlc.ha_numValue ;k++){
			if(g_LC_Jlc.da_cmpValue[k].re *(double)fLCMyu > __MAX_FLOAT_VALUE && 
				g_LC_Jlc.da_cmpValue[k].im *(double)fLCMyu > __MAX_FLOAT_VALUE){
				printf("Data is Inf.\n") ;
				/*
				dComplexArrayFree(&E) ;
				dComplexArrayFree(&Znsns) ;
				dComplexArrayFree(&Znsnm) ;
				dComplexArrayFree(&Jlc) ;
				dComplexArrayFree(&Atmp) ;
				*/
			
			  return __FALSE__ ;
			}
			pcamyu_J_LC->ha_cmpValue[k].re = (HA_Float)(g_LC_Jlc.da_cmpValue[k].re *(double)fLCMyu) ;
			pcamyu_J_LC->ha_cmpValue[k].im = (HA_Float)(g_LC_Jlc.da_cmpValue[k].im *(double)fLCMyu) ;
		}
	} // end LC_Method = Fix
	else{								//  ●LC_METHOD_ADAPTIVE
		dComplexArrayInit(&g_LC_Znsnm) ;
		//if(AbsdCompMat1(&Znsnm,Jlc) != __TRUE__) {
		if(MICARY_Abs_CompMat(&g_LC_Znsnm,g_LC_Jlc) != __TRUE__) {
			/*
			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Jlc) ;
			dComplexArrayFree(&Atmp) ;
			*/
			
			return __FALSE__;
		}

		fJlcp = 0.0 ;
		for(k = 0 ; k< g_LC_Znsnm.ha_numValue ; k++) {
			fJlcp = (double)pow(g_LC_Znsnm.da_cmpValue[k].re,(double)2.0) + fJlcp ;
		}
		fJlcp = (double)(fJlcp + 0.0000000001) ;
		
		dComplexArrayInit(&g_LC_Znsns) ;
		//if(AbsdCompMat1(&Znsns,E) != __TRUE__) {
		if(MICARY_Abs_CompMat(&g_LC_Znsns,g_LC_E) != __TRUE__) {
			/*
			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Jlc) ;
			dComplexArrayFree(&Atmp) ;
			*/

			return __FALSE__;
		}

		//fJlcp = sum(abs(Jlc)^2)
		foptLCMyu = 0.0 ;
		for(k = 0 ; k< g_LC_Znsns.ha_numValue ; k++) {
			foptLCMyu = (double)(pow(g_LC_Znsns.da_cmpValue[k].re,2.0) + foptLCMyu) ;
		}
		
		//zero divide 2008/10.2
		if(fabs(fJlcp) < _MINIMUM_ZERO_VALUE){
			foptLCMyu = foptLCMyu / (fJlcp+_MINIMUM_ZERO_VALUE) / (int)2 ;
		}
		else{
			foptLCMyu = foptLCMyu / fJlcp / (int)2 ;
		}

		for(k = 0 ; k < g_LC_Jlc.ha_numValue ;k++){
			if(g_LC_Jlc.da_cmpValue[k].re *foptLCMyu > __MAX_FLOAT_VALUE && 
			  g_LC_Jlc.da_cmpValue[k].im *foptLCMyu > __MAX_FLOAT_VALUE){
				
				printf("Data is Inf.\n") ;
			  	/*
				dComplexArrayFree(&E) ;
				dComplexArrayFree(&Znsns) ;
				dComplexArrayFree(&Znsnm) ;
				dComplexArrayFree(&Jlc) ;
				dComplexArrayFree(&Atmp) ;
			  	*/
	
				return __FALSE__;
			}
			pcamyu_J_LC->ha_cmpValue[k].re = g_LC_Jlc.da_cmpValue[k].re *foptLCMyu ;
			pcamyu_J_LC->ha_cmpValue[k].im = g_LC_Jlc.da_cmpValue[k].im *foptLCMyu ;
		}
	}	// end LC_Method = Adaptive

			//printf("LCmyu = ") ;
	//for(k = 0 ; k < pcamyu_J_LC->ha_numValue ; k++){
	//	printf("%e,%e /",pcamyu_J_LC->ha_cmpValue[k].re,pcamyu_J_LC->ha_cmpValue[k].im) ;
	//}
		//printf("\n") ;

	/*
	dComplexArrayFree(&E) ;
	dComplexArrayFree(&Znsns) ;
	dComplexArrayFree(&Znsnm) ;
	dComplexArrayFree(&Jlc) ;
	dComplexArrayFree(&Atmp) ;
	*/

	return __TRUE__ ;
}
// -------------------------------------------------------------------------------------------------
// GHDSSの実行
int dUpdateSeparationMatGHDSS(MICARY_SavedSeparation * psSavedSeparation ,
							MICARY_ComplexSpecArray csSeparationSpec ,
							MICARY_ComplexSpecArray csInputSpec , 
							MICARY_dComplexSpecArray *pcsRxx ,
							MICARY_dComplexSpecArray *pcsRyy ,
							MICARY_PrmSeparation_GHDSS sPrmSeparation,
							HA_Float ha_fWmyu,
							HA_Float ha_fThreshold,
							HA_Integer ha_iUpdate)
{
	//cout << "start" << endl;

    int Nsrc,Nfreq,Nmic,ifreq,ind;
	// deleted 2012/03/07
	/*
	MICARY_ComplexArray A,X ,W ,Y,J_SS, J_LC; //Ryy,Rxx,
	MICARY_dComplexArray Rxx,Ryy ;
	*/
	HA_Float Threshold_val ,Max_val,fTmp;
	int iUp ;
	int iOCRAMethod = _OCRA_METHOD_NONE ;	// OCRAは無効
	//int k ;
	iUp = 0 ;

	//Trehold_val
	Max_val = 32768.0 ;
	Max_val = 10*log10(Max_val) ;
	fTmp = (Max_val - ha_fThreshold)/10.0 ; 
	Threshold_val = pow((double)10.0, (double)fTmp) ;

	ifreq = csInputSpec.ha_iLowerFreq ; //開始周波数インデックス
	
	Nmic = csInputSpec.ha_numCh ;
	// 2010.8.26
	//Nfreq = csInputSpec.ha_iUpperFreq - csInputSpec.ha_iLowerFreq + 1 ;
	Nfreq = csInputSpec.micary_cmpaSpec.ha_numValue/Nmic ;
	Nsrc = csSeparationSpec.ha_numCh ;
	
	
	// 2010.8.26
	//周波数帯域は一致していなければならない
	if(psSavedSeparation -> micary_smSeparation.micary_iaFreqLinex.ha_numValue != Nfreq || 
	  psSavedSeparation ->micary_tdTFDB.micary_iaFreqLine.ha_numValue != Nfreq){
	  printf("InputSpec size %d and Separation matrix %d (TF %d ) size is mismatch.\n",Nfreq,psSavedSeparation -> micary_smSeparation.micary_iaFreqLinex.ha_numValue,psSavedSeparation ->micary_tdTFDB.micary_iaFreqLine.ha_numValue) ;
	  return __FALSE__ ;
	}
	
	
	// changed 2012/03/07
	/*
	// Alloc
	ComplexArrayAlloc(&A,0) ;
	ComplexArrayAlloc(&X,0) ;
	ComplexArrayAlloc(&W,0) ;
	ComplexArrayAlloc(&Y,0) ;
	dComplexArrayAlloc(&Ryy,0) ;
	dComplexArrayAlloc(&Rxx,0) ;
	ComplexArrayAlloc(&J_SS,0) ;
	ComplexArrayAlloc(&J_LC,0) ;

	// Initialize
	ComplexArrayAlloc(&A,Nmic*Nsrc) ;
	ComplexArrayAlloc(&X,Nmic) ;
	ComplexArrayAlloc(&W,Nsrc*Nmic) ;
	ComplexArrayAlloc(&Y,Nsrc) ;
	dComplexArrayAlloc(&Ryy,Nsrc*Nsrc) ;
	dComplexArrayAlloc(&Rxx,Nmic*Nmic) ;
	ComplexArrayAlloc(&J_SS,Nsrc*Nmic) ;
	ComplexArrayAlloc(&J_LC,Nsrc*Nmic) ;
	*/
	__SepMat_AllocValues(Nsrc, Nmic);

	
	while(__TRUE__){	// frequency loop
		
	    if (ifreq == 0){	//D.C
			ifreq++ ;
			continue ;
		}
		//cout << "freq=" << ifreq << endl;
		if (ifreq > csInputSpec.ha_iUpperFreq){
			break ;
		}
		// 2010.8.26
		if (ifreq < csInputSpec.ha_iLowerFreq){
		  ifreq++ ;
		  continue ;
		}
		
		// 2010.8.26
		if(ifreq != psSavedSeparation->micary_tdTFDB.micary_iaFreqLine.ha_iValue[ifreq] || ifreq != psSavedSeparation->micary_smSeparation.micary_iaFreqLinex.ha_iValue[ifreq]){
		   //printf("ifreq = %d,TF = %d, W = %d -> continue\n",ifreq,psSavedSeparation->micary_tdTFDB.micary_iaFreqLine.ha_iValue[ifreq], psSavedSeparation->micary_smSeparation.micary_iaFreqLinex.ha_iValue[ifreq]) ;
		   ifreq++ ;
		  continue ;
		}
		 
		//--- Copy :: Spec array -> Complex array ---//
		// Transfer function matrix A[Nmic*Nsrc]
		// 2010.8.30
		// memcpy(A.ha_cmpValue,&(psSavedSeparation->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nsrc*Nmic]),sizeof(HA_Complex)*Nsrc*Nmic) ;
		   memcpy(g_Up_A.ha_cmpValue,&(psSavedSeparation->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[(ifreq) * Nsrc*Nmic]),sizeof(HA_Complex)*Nsrc*Nmic) ;
		// Input spectrum X[Nmic,1]
		// (注意!!) spectrum data:frame->ch->freqの順に並んでいる
		for (ind = 0 ; ind < Nmic ; ind ++){
		  //memcpy(&(X.ha_cmpValue[ind]),&(csInputSpec.micary_cmpaSpec.ha_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq)+ind*Nfreq]),sizeof(HA_Complex)) ;
		  // 2010.8.30
		   memcpy(&(g_Up_X.ha_cmpValue[ind]),&(csInputSpec.micary_cmpaSpec.ha_cmpValue[(ifreq)+ind*Nfreq]),sizeof(HA_Complex)) ;
		}
		
		// Separation matrix W[Nsrc*Nmic]
		// 2010.8.30
		//memcpy(W.ha_cmpValue,&(psSavedSeparation->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nsrc*Nmic]),sizeof(HA_Complex)*Nsrc*Nmic) ;
		memcpy(g_Up_W.ha_cmpValue,&(psSavedSeparation->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[(ifreq) * Nsrc*Nmic]),sizeof(HA_Complex)*Nsrc*Nmic) ;
		
		// Correlation matrix Rxx [Nmic*Nmic]
		// 2010.8.30
		//memcpy(Rxx.da_cmpValue,&(pcsRxx->micary_cmpaSpec.da_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nmic*Nmic]),sizeof(DA_Complex)*Nmic*Nmic) ;
		memcpy(g_Up_Rxx.da_cmpValue,&(pcsRxx->micary_cmpaSpec.da_cmpValue[(ifreq) * Nmic*Nmic]),sizeof(DA_Complex)*Nmic*Nmic) ;
		// Correlation matrix Ryy [Nsrc*Nsrc]
		// 2010.8.30
		//memcpy(Ryy.da_cmpValue,&(pcsRyy->micary_cmpaSpec.da_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nsrc*Nsrc]),sizeof(DA_Complex)*Nsrc*Nsrc) ;
		memcpy(g_Up_Ryy.da_cmpValue,&(pcsRyy->micary_cmpaSpec.da_cmpValue[(ifreq) * Nsrc*Nsrc]),sizeof(DA_Complex)*Nsrc*Nsrc) ;

		// ------------------------------------------//
		// ---------- Execute GHDSS --------------- //
		if(MICARY_Product_fCompMat(&g_Up_Y,g_Up_W,Nsrc,Nmic,g_Up_X,Nmic,1) != __TRUE__){
			/*
			ComplexArrayFree(&A) ;
			ComplexArrayFree(&X) ;
			ComplexArrayFree(&W) ;
			ComplexArrayFree(&Y) ;
			dComplexArrayFree(&Ryy) ;
			dComplexArrayFree(&Rxx) ;
			ComplexArrayFree(&J_SS) ;
			ComplexArrayFree(&J_LC) ;
			*/

			return __FALSE__;
		}
		/*
	        int i;
		printf("Y_%d = ,ifreq");
	        for (i = 0; i < Y.ha_numValue; i++) {
		  printf("%e ", Y.ha_cmpValue[i].re);
		}
	        printf("\n");
		*/
		
		// --- Copy : Y -> Separated spectrum(src->freqの順) --- //
		for(ind = 0 ; ind < Nsrc ; ind ++){
		    csSeparationSpec.micary_cmpaSpec.ha_cmpValue[ind*Nfreq+ifreq].re = g_Up_Y.ha_cmpValue[ind].re ;
		    csSeparationSpec.micary_cmpaSpec.ha_cmpValue[ind*Nfreq+ifreq].im = g_Up_Y.ha_cmpValue[ind].im ;
		}
		// --------------------------------------- //
		
		
		// ------ Update separated matrix --------- //
		// Threshold
       	for(ind = 0 ; ind < g_Up_X.ha_numValue ; ind++){
			fTmp = pow((double)g_Up_X.ha_cmpValue[ind].re, (double)2.0) + pow((double)g_Up_X.ha_cmpValue[ind].im, (double)2.0) ;
			fTmp = sqrt(fTmp) ;
			if(fTmp < Threshold_val){
				iUp = 1 ;
				break ;
			}		    
		}
		
		if(iUp == 0){
			//cout << "SS" << endl;
		    // SS
		    if(sPrmSeparation.ha_iSSMethod == _SS_METHOD_LCMYU){ //LCMyuと共通化
				sPrmSeparation.ha_fSSMyu = sPrmSeparation.ha_fLCMyu ;
		    }
		    
		    if(_dCalcSSStepSize_GHDSS(g_Up_W,g_Up_A,g_Up_Y,g_Up_X,
									sPrmSeparation.ha_iSSMethod,
									sPrmSeparation.ha_fSSMyu,
									sPrmSeparation.ha_fSSScal,
									sPrmSeparation.ha_fNoiseFloor,
									&g_Up_J_SS) != __TRUE__ ){
			
				/*
				ComplexArrayFree(&A) ;
				ComplexArrayFree(&X) ;
				ComplexArrayFree(&W) ;
				ComplexArrayFree(&Y) ;
				dComplexArrayFree(&Ryy) ;
				dComplexArrayFree(&Rxx) ;
				ComplexArrayFree(&J_SS) ;
				ComplexArrayFree(&J_LC) ;
				*/
			
				return __FALSE__;
			}

		    if(ha_iUpdate == 0){	//段階的 
				if(MICARY_Sub_fCompMat(&g_Up_W,g_Up_W,Nsrc,Nmic,g_Up_J_SS,Nsrc,Nmic) != __TRUE__){
					/*
					ComplexArrayFree(&A) ;
					ComplexArrayFree(&X) ;
					ComplexArrayFree(&W) ;
					ComplexArrayFree(&Y) ;
					dComplexArrayFree(&Ryy) ;
					dComplexArrayFree(&Rxx) ;
					ComplexArrayFree(&J_SS) ;
					ComplexArrayFree(&J_LC) ;
					*/
			    
					return __FALSE__;
				}
			}
		    
		    
		    // LC
			//cout << "LC" << endl;
		    if(_dCalcLCStepSize_GHDSS(g_Up_W,g_Up_A,Nsrc,Nmic,
					      sPrmSeparation.ha_iLCConst,sPrmSeparation.ha_iLCMethod,
					      sPrmSeparation.ha_fLCMyu,&g_Up_J_LC) != __TRUE__){			
				/*
				ComplexArrayFree(&A) ;
				ComplexArrayFree(&X) ;
				ComplexArrayFree(&W) ;
				ComplexArrayFree(&Y) ;
				dComplexArrayFree(&Ryy) ;
				dComplexArrayFree(&Rxx) ;
				ComplexArrayFree(&J_SS) ;
				ComplexArrayFree(&J_LC) ;
				*/
			
				return __FALSE__;
		    }
		   
		    if(ha_iUpdate == 0){	// 段階的		    
				// Update W
				if(MICARY_Sub_fCompMat(&g_Up_W,g_Up_W,Nsrc,Nmic,g_Up_J_LC,Nsrc,Nmic) != __TRUE__){
			    
					/*
					ComplexArrayFree(&A) ;
					ComplexArrayFree(&X) ;
					ComplexArrayFree(&W) ;
					ComplexArrayFree(&Y) ;
					dComplexArrayFree(&Ryy) ;
					dComplexArrayFree(&Rxx) ;
					ComplexArrayFree(&J_SS) ;
					ComplexArrayFree(&J_LC) ;
					*/
			    
					return __FALSE__;
				}
		   
				// 発散防止
				/*
				// 2010/4/8
				for(k = 0 ; k < W.ha_numValue ; k++){
				 W.ha_cmpValue[k].re = W.ha_cmpValue[k].re * (1.0-ha_fWmyu) ;
				W.ha_cmpValue[k].im = W.ha_cmpValue[k].im * (1.0-ha_fWmyu) ;

				}
				*/			
		    }
		    else{
				// Wmyuで発散防止
				/*
		      	for(k = 0 ; k < W.ha_numValue ; k++){
				W.ha_cmpValue[k].re = W.ha_cmpValue[k].re * (1.0-ha_fWmyu) ;
				W.ha_cmpValue[k].im = W.ha_cmpValue[k].im * (1.0-ha_fWmyu) ;

				}
				*/
				if(MICARY_Sub_fCompMat(&g_Up_W,g_Up_W,Nsrc,Nmic,g_Up_J_SS,Nsrc,Nmic) != __TRUE__){
			    
					/*
					ComplexArrayFree(&A) ;
					ComplexArrayFree(&X) ;
					ComplexArrayFree(&W) ;
					ComplexArrayFree(&Y) ;
					dComplexArrayFree(&Ryy) ;
					dComplexArrayFree(&Rxx) ;
					ComplexArrayFree(&J_SS) ;
					ComplexArrayFree(&J_LC) ;
					*/
			    
					return __FALSE__;
				}

				if(MICARY_Sub_fCompMat(&g_Up_W,g_Up_W,Nsrc,Nmic,g_Up_J_LC,Nsrc,Nmic) != __TRUE__){
			    
					/*
					ComplexArrayFree(&A) ;
					ComplexArrayFree(&X) ;
					ComplexArrayFree(&W) ;
					ComplexArrayFree(&Y) ;
					dComplexArrayFree(&Ryy) ;
					dComplexArrayFree(&Rxx) ;
					ComplexArrayFree(&J_SS) ;
					ComplexArrayFree(&J_LC) ;
					*/
			    
					return __FALSE__;
				}
		    }
		}
		
		// --------------------------------------- //
		/*
		if(ifreq == 1){
		  printf("W1 = ") ;
		for(k = 0 ; k < W.ha_numValue ; k++){
		  printf("%e,%e /",W.ha_cmpValue[k].re,W.ha_cmpValue[k].im) ;
		}
		printf("\n") ;
		}
		*/
		//--- Update Spec array ---//
		// Separation matrix W[Nsrc*Nmic]
		// 2010.8.30
		//memcpy(&(psSavedSeparation->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nsrc*Nmic]),W.ha_cmpValue,sizeof(HA_Complex)*Nsrc*Nmic) ;
		memcpy(&(psSavedSeparation->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[(ifreq) * Nsrc*Nmic]),g_Up_W.ha_cmpValue,sizeof(HA_Complex)*Nsrc*Nmic) ;
		
		// Correlation matrix Rxx [Nmic*Nmic]
		//2010.8.30
		//memcpy(&(pcsRxx->micary_cmpaSpec.da_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nmic*Nmic]),Rxx.da_cmpValue,sizeof(DA_Complex)*Nmic*Nmic) ;
		memcpy(&(pcsRxx->micary_cmpaSpec.da_cmpValue[(ifreq) * Nmic*Nmic]),g_Up_Rxx.da_cmpValue,sizeof(DA_Complex)*Nmic*Nmic) ;
		// Correlation matrix Ryy [Nsrc*Nsrc]
		// 2010.8.30
		//memcpy(&(pcsRyy->micary_cmpaSpec.da_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nsrc*Nsrc]),Ryy.da_cmpValue,sizeof(DA_Complex)*Nsrc*Nsrc) ;
		memcpy(&(pcsRyy->micary_cmpaSpec.da_cmpValue[(ifreq) * Nsrc*Nsrc]),g_Up_Ryy.da_cmpValue,sizeof(DA_Complex)*Nsrc*Nsrc) ;
		// ------------------------------------------//
		
		ifreq ++ ;		
	}

	//exit(0);
	/*
	ComplexArrayFree(&A) ;
	ComplexArrayFree(&X) ;
	ComplexArrayFree(&W) ;
	ComplexArrayFree(&Y) ;
	dComplexArrayFree(&Ryy) ;
	dComplexArrayFree(&Rxx) ;
	ComplexArrayFree(&J_SS) ;
	ComplexArrayFree(&J_LC) ;
	*/

	return __TRUE__ ;
}

