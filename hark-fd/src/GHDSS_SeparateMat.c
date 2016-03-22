// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_SeparateMat.c
 \brief	\~English Source file for execute GHDSS. (GHDSS program)
 \brief	\~Japanese GHDSS実行のためのソースファイル(GHDSS program)
 \author	Sachiko Suzuki
 \date	24 Dec 2009
 \version	$Id$
*/
// ---------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "GHDSS_SeparateMat.h"
#include "ArrayInit.h"
#include "MICARY_GHDSS_struct.h"
//#include "GHDSS_MathMat2.h"
#include "GHDSS_MathMatDouble.h"

// ---------------------------------------------------------------------
int CalcTFFreeH(MICARY_TFDatabase * psTFDB , 
					 HA_Integer iUpperFreq ,
					 MICARY_PrmFrame sPrmFrame ,
					 MICARY_LocationArray locaSrcPos ,
					 MICARY_LocationArray locaMicPos ,
					 HA_Float fSpeedSound)
{	int idsrc , idmic ,nf,index ;
	float k ,freq ,sx,sy,sz,mx,my,mz,dr,tmp,c;
	
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
		TFDatabaseFree(psTFDB);
		TFDatabaseAlloc(psTFDB, iUpperFreq+1,locaSrcPos.ha_numLocation,locaMicPos.ha_numLocation);
	}
	
	
	while(1){
		
		freq = nf * sPrmFrame.ha_fFs / sPrmFrame.ha_numNFFT ; 
		k = 2.0 * M_pi *  freq / c;
		
		for(idsrc = 0 ; idsrc < locaSrcPos.ha_numLocation ; idsrc ++){
			for(idmic = 0 ; idmic < locaMicPos .ha_numLocation ; idmic ++){
		
				sx = locaSrcPos.micary_Location[idsrc].ha_fX ;
				sy = locaSrcPos.micary_Location[idsrc].ha_fY ;
				sz = locaSrcPos.micary_Location[idsrc].ha_fZ ;
				
				mx = locaSrcPos.micary_Location[idsrc].ha_fX - locaMicPos.micary_Location[idmic].ha_fX ;
				my = locaSrcPos.micary_Location[idsrc].ha_fY - locaMicPos.micary_Location[idmic].ha_fY ;
				mz = locaSrcPos.micary_Location[idsrc].ha_fZ - locaMicPos.micary_Location[idmic].ha_fZ ;
				
				dr = sqrt(pow(sx,2.0)+pow(sy,2.0)+pow(sz,2.0)) - sqrt(pow(mx,2.0)+pow(my,2.0)+pow(mz,2.0)) ;

				tmp = cos(-1.0*k*dr) ;
				
				psTFDB ->micary_cmpaTF .ha_cmpValue[index].re = (1+(dr*sPrmFrame.ha_fFs/c)/200) * tmp ;
			
				tmp = sin(-1.0*k*dr) ;
				psTFDB ->micary_cmpaTF .ha_cmpValue[index].im = (1+(dr*sPrmFrame.ha_fFs/c)/200) * tmp ;
							
				index ++ ;
			}
		}
		psTFDB -> micary_iaFreqLine.ha_iValue[nf] = nf ;
		//printf("TF freq = %d",psTFDB->micary_iaFreqLine.ha_iValue[nf]);
		if (nf >= iUpperFreq){
			break ;
		}
		nf ++ ;
		idsrc = 0 ;
		idmic = 0 ;

		
	}
	//	printf("\n") ;
	psTFDB ->micary_iaFreqLine.ha_numValue = nf+1 ;
	memcpy(&psTFDB ->micary_locaMic , &locaMicPos , sizeof(MICARY_LocationArray)) ;
	memcpy(&psTFDB ->micary_locaSource , &locaSrcPos , sizeof(MICARY_LocationArray)) ;
	
	//memcpy(&psTFDB ->micary_locaMic , &locaMicPos , (sizeof(MICARY_Location)+sizeof(int)*2)*locaMicPos.ha_numLocation) ;
	//memcpy(&psTFDB ->micary_locaSource , &locaSrcPos , (sizeof(MICARY_Location)+sizeof(int)*2)*locaSrcPos.ha_numLocation) ;
	
	return 1 ;
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
		SeparationMatFree(psSeparationMat) ;
		SeparationMatAlloc(psSeparationMat,sTFDB .micary_iaFreqLine.ha_numValue,
			sTFDB.micary_locaSource.ha_numLocation,sTFDB.micary_locaMic.ha_numLocation) ;
	}
	// 2010.8.26
	// 周波数ラインを更新
	for(i = 0 ; i < sTFDB.micary_iaFreqLine.ha_numValue ;i++){
	  psSeparationMat->micary_iaFreqLinex.ha_iValue[i] = sTFDB.micary_iaFreqLine.ha_iValue[i] ;
	}
	psSeparationMat->micary_iaFreqLinex.ha_numValue = sTFDB.micary_iaFreqLine.ha_numValue ;


	//伝達関数から分離行列の初期値を作成
	while(1){
		if(index >= sTFDB.micary_iaFreqLine.ha_numValue * sTFDB.micary_locaMic.ha_numLocation*sTFDB.micary_locaSource.ha_numLocation){
			break ;
		}
	

		psSeparationMat ->micary_cmpaSeparation.ha_cmpValue[index].re = sTFDB.micary_cmpaTF.ha_cmpValue[count_f*g*r+count_r*g+count_g].re / sTFDB.micary_locaMic.ha_numLocation ;
		// modyfied 2008/7/24
		psSeparationMat ->micary_cmpaSeparation.ha_cmpValue[index].im = sTFDB.micary_cmpaTF.ha_cmpValue[count_f*g*r+count_r*g+count_g].im / sTFDB.micary_locaMic.ha_numLocation ;
		
		//psSeparationMat ->micary_cmpaSeparation.ha_cmpValue[index].im = (-1.0)*sTFDB.micary_cmpaTF.ha_cmpValue[count_f*g*r+count_r*g+count_g].im / sTFDB.micary_locaMic.ha_numLocation ;
		
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
						  
return 1 ;
}

// ---------------------------------------------------------------------------------------------
// GHDSSの実行
void _dGetPQY(MICARY_ComplexArray *caPY,MICARY_ComplexArray *caQY,
							 MICARY_ComplexArray caY,HA_Float ha_fSSScal)
{
	int i ;
	HA_Float fabsY,fabsYs,ftmp ;
	HA_Complex cmpExpY ;

	//printf("absY/PY/QY = ") ;
	//printf("Y2 = ") ;

	for(i = 0 ; i < caY.ha_numValue ; i++){
	    fabsY = sqrt(pow(caY.ha_cmpValue[i].re,2.0) + pow(caY.ha_cmpValue[i].im,2.0)) + _MINIMUM_ZERO_VALUE;
	    fabsYs = fabsY * ha_fSSScal ;
	    cmpExpY.re = caY.ha_cmpValue[i].re/fabsY ;
	    cmpExpY.im = caY.ha_cmpValue[i].im/fabsY ;
	    
	    caPY->ha_cmpValue[i].re = tanh(fabsYs) * cmpExpY.re ;
	    caPY->ha_cmpValue[i].im = tanh(fabsYs) * cmpExpY.im ;
	    
	    ftmp = pow(cosh(fabsYs),2.0) ;
	    ftmp = fabsYs/ftmp*ha_fSSScal ;
	    caQY->ha_cmpValue[i].re = (tanh(fabsYs) + ftmp) * cmpExpY.re ;
	    caQY->ha_cmpValue[i].im = (tanh(fabsYs) + ftmp) * cmpExpY.im ;

	    //printf("%e , %e /",caY.ha_cmpValue[i].re , caY.ha_cmpValue[i].im) ;

	    /*
	    {
	      printf("[%e]/[%e,%e]/[%e,%e]\n",fabsY,caPY->ha_cmpValue[i].re, caPY->ha_cmpValue[i].im, caQY->ha_cmpValue[i].re, caQY->ha_cmpValue[i].im) ;

	    }
	    */
	}
	//	printf("\n") ;

	return ;
	
}
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
    MICARY_dComplexArray Ryy,Zns1,E,Jss,Jpri,Znm1, Znsns,  Znsnm , Z11;
    MICARY_ComplexArray PY,QY ;
    double fTmp,fYrms;
    double fTmp2 ;
    
    Nmic = caX.ha_numValue ;
    Nsrc = caY.ha_numValue ;
    
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
    /*    
	printf("Y = ") ;
	for(k = 0 ; k < caY.ha_numValue ; k++){
	  printf("%e , %e /",caY.ha_cmpValue[k].re , caY.ha_cmpValue[k].im) ;
	}
	printf("\n") ;
    */
    //PY,QY
    _dGetPQY(&PY,&QY,caY,fSSScal) ;
    
    //Ryy = y*y'
    if(ConjTransdfCompMat1(&Zns1,caY,Nsrc,1) != 1){
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
	return 0 ;
    }
    
    // GHDSS
    //if(ProductfdCompMat1(&Ryy,caY,Nsrc,1,Zns1,1,Nsrc) != 1){
    if(ProductfdCompMat1(&Ryy,PY,Nsrc,1,Zns1,1,Nsrc) != 1){
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
	return 0 ;
    }

    /*
    {
	int i;
	
	printf("Ryy: ");
	for (i = 0; i < Ryy.ha_numValue; i++) {
	    printf("%e ", Ryy.da_cmpValue[i].re);
	}
	printf("\n");
	
    }
    */
    
    //E = Ryy - diag(Ryy)
    //ComplexArrayAlloc(&Znsns,Nsrc*Nsrc) ;
    dComplexArrayInit(&Znsns) ;
    if(DiagdCompMat1(&Znsns ,Ryy,Nsrc, Nsrc) != 1){
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
			
	return 0 ;
    }
    if(SubdCompMat1(&E,Ryy,Nsrc,Nsrc,Znsns,Nsrc,Nsrc) != 1){
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
	
	return 0 ;
    }

    /* 
    {
	int i;
	
	printf("E: ");
	for (i = 0; i < E.ha_numValue; i++) {
	    printf("%e ", E.da_cmpValue[i].re);
	}
	printf("\n");
	
    }
    */
    // Jss = (E*qy)*x'
    // Zns1 = E*qy
    if(ProductdfCompMat1(&Zns1,E,Nsrc,Nsrc,QY,Nsrc,1) != 1){
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
	
	return 0 ;
    }
    //X = X'
    if(ConjTransCompMat1(&caX ,caX,Nmic,1) != 1){
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
	
	return 0 ;
    }
    // Jss = Zns1 * X
    if(ProductdfCompMat1(&Jss,Zns1,Nsrc,1,caX,1,Nmic) != 1){
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
	
	return 0 ;
    }
    
    // Jpri = conj(Jss)
    if(ConjdCompMat1(&Jpri,Jss) ==0){
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
	
	return 0 ;
    }
    
    ComplexArrayFree(&PY) ;
    ComplexArrayFree(&QY) ;
    
    //} // end No Average
   
	// 2010.4.7
    /*
	printf("Eval = ") ;
	for(k = 0 ; k < E.ha_numValue ; k++){
	  printf("%e , %e /",E.da_cmpValue[k].re , E.da_cmpValue[k].im) ;
	}
	printf("\n") ;
    */

    if(iSSMethod == 0){	// myu_J_SS : Fix
	// myu_J_SS = 2.0 * Jss	
	
	for(k = 0 ; k < Jss.ha_numValue ;k++){
	    if(Jss.da_cmpValue[k].re *(double)fSSMyu > __MAX_FLOAT_VALUE && 
	       Jss.da_cmpValue[k].im *(double)fSSMyu > __MAX_FLOAT_VALUE){
		printf("Data is Inf.\n") ;
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		
		return 0 ;
	    }
	    pcamyu_J_SS->ha_cmpValue[k].re = (HA_Float)(Jss.da_cmpValue[k].re *(double)fSSMyu) ;
	    pcamyu_J_SS->ha_cmpValue[k].im = (HA_Float)(Jss.da_cmpValue[k].im *(double)fSSMyu) ;
	}
	
    }
    
    else if(iSSMethod == 1){	// myu_J_SS : same as myu_LC(=fSSMyu)
	//Rxx = x'*x 
	//X = X'
	//ComplexArrayAlloc(&Znm1,Nmic*1) ;
	dComplexArrayInit(&Znm1) ;
	if(ConjTransdfCompMat1(&Znm1 ,caX,Nmic,1) != 1){
	    dComplexArrayFree(&Ryy) ;
	    dComplexArrayFree(&Zns1) ;
	    dComplexArrayFree(&Znm1) ;
	    dComplexArrayFree(&E) ;
	    dComplexArrayFree(&Jss) ;
	    dComplexArrayFree(&Jpri) ;
	    dComplexArrayFree(&Znsns) ;
	    dComplexArrayFree(&Znsnm) ;
	    dComplexArrayFree(&Z11) ;
	    
			return 0 ;
	}
		//ComplexArrayAlloc(&Z11,1*1) ;
	
	if(ProductdfCompMat1(&Z11,Znm1,1,Nmic,caX,Nmic,1) != 1){
	    dComplexArrayFree(&Ryy) ;
	    dComplexArrayFree(&Zns1) ;
	    dComplexArrayFree(&Znm1) ;
	    dComplexArrayFree(&E) ;
	    dComplexArrayFree(&Jss) ;
	    dComplexArrayFree(&Jpri) ;
	    dComplexArrayFree(&Znsns) ;
	    dComplexArrayFree(&Znsnm) ;
	    dComplexArrayFree(&Z11) ;
	    
	    return 0 ;
	}
	//add 2009.2.24
	if(fabs(pow(Z11.da_cmpValue[0].re,2.0)) < _MINIMUM_ZERO_VALUE){
	    fTmp = 1.0/_MINIMUM_ZERO_VALUE ;
	}
	else{
	    // myu_J_SS = LC_myu(fSSMyu)*Rxx^-2 * Jss	
	    fTmp = (double)(1.0/(pow(Z11.da_cmpValue[0].re,2.0))) ;
	    
	}	
	fTmp = fTmp * (double)fSSMyu ;
	for(k = 0 ; k < Jss.ha_numValue ;k++){
	    if(Jss.da_cmpValue[k].re *fTmp > __MAX_FLOAT_VALUE && 
	       Jss.da_cmpValue[k].im *fTmp > __MAX_FLOAT_VALUE){
		printf("Data is Inf.\n") ;
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		
		return 0 ;
	    }
	    pcamyu_J_SS->ha_cmpValue[k].re = (HA_Float)(Jss.da_cmpValue[k].re *fTmp) ;
	    pcamyu_J_SS->ha_cmpValue[k].im = (HA_Float)(Jss.da_cmpValue[k].im *fTmp) ;
	}
	
    }	// end iSSMethod = 1
    
    else {	// iSSMethod = 2	myu_J_SS : adaptive
		// Znsns = abs(E)
		//ComplexArrayAlloc(&Znsns,Nsrc*Nsrc) ;
	dComplexArrayInit(&Znsns) ;


	if(AbsdCompMat1(&Znsns ,E) != 1){
	    dComplexArrayFree(&Ryy) ;
	    
	    dComplexArrayFree(&Zns1) ;
	    dComplexArrayFree(&Znm1) ;
	    dComplexArrayFree(&E) ;
	    dComplexArrayFree(&Jss) ;
	    dComplexArrayFree(&Jpri) ;
	    dComplexArrayFree(&Znsns) ;
	    dComplexArrayFree(&Znsnm) ;
	    dComplexArrayFree(&Z11) ;
	    
	    return 0 ;
	}
	// Znsns = Znsns.*Znsns
	if(DotcdCompMat1(&Znsns ,Znsns ,Znsns) !=1){
	    dComplexArrayFree(&Ryy) ;
	    
	    dComplexArrayFree(&Zns1) ;
	    dComplexArrayFree(&Znm1) ;
	    dComplexArrayFree(&E) ;
	    dComplexArrayFree(&Jss) ;
	    dComplexArrayFree(&Jpri) ;
	    dComplexArrayFree(&Znsns) ;
	    dComplexArrayFree(&Znsnm) ;
	    dComplexArrayFree(&Z11) ;
	    
	    return 0 ;
	}
	// fTmp = sum(Znsns)
	fTmp = 0.0 ;
	for(k = 0 ; k < Znsns.ha_numValue ; k++){
	    fTmp = fTmp + Znsns.da_cmpValue[k].re ;
	}
	// Znsnm = Jss.*Jpri
	//ComplexArrayAlloc(&Znsnm,Nsrc*Nmic) ;
	dComplexArrayInit(&Znsnm) ;
	if(DotcdCompMat1(&Znsnm ,Jss ,Jpri) !=1){
	    dComplexArrayFree(&Ryy) ;
	    
	    dComplexArrayFree(&Zns1) ;
	    dComplexArrayFree(&Znm1) ;
	    dComplexArrayFree(&E) ;
	    dComplexArrayFree(&Jss) ;
	    dComplexArrayFree(&Jpri) ;
	    dComplexArrayFree(&Znsns) ;
	    dComplexArrayFree(&Znsnm) ;
	    dComplexArrayFree(&Z11) ;
	    
	    return 0 ;
	}
	// Znsnm = abs(Znsnm)
	if(AbsdCompMat1(&Znsnm ,Znsnm) != 1){
	    dComplexArrayFree(&Ryy) ;
	    
	    dComplexArrayFree(&Zns1) ;
	    dComplexArrayFree(&Znm1) ;
	    dComplexArrayFree(&E) ;
	    dComplexArrayFree(&Jss) ;
	    dComplexArrayFree(&Jpri) ;
	    dComplexArrayFree(&Znsns) ;
	    dComplexArrayFree(&Znsnm) ;
	    dComplexArrayFree(&Z11) ;
	    
	    return 0 ;
	}
	// fTmp = sum(Znsns)
	fTmp2 = 0.0 ;
	for(k = 0 ; k < Znsnm.ha_numValue ; k++){
	    fTmp2 = fTmp2 + Znsnm.da_cmpValue[k].re ;
	}
	if(fabs(fTmp2) < _MINIMUM_ZERO_VALUE){
	    //printf("Caution! devide by zero.\n") ;
	    fTmp2 = fTmp2 + _MINIMUM_ZERO_VALUE;
	}
	//2010.4.7
	/*
	printf("opt1 = %f",fTmp) ;
	printf("opt2 = %f",fTmp2) ;
	*/
	fTmp = (fTmp / fTmp2 / (double)2.0) ;
	//printf("opt = %10.10f\n",fTmp) ;
	
	// myu_J_SS = fTmp * Js
	for(k = 0 ; k < Jss.ha_numValue ;k++){
	    
	    if(Jss.da_cmpValue[k].re *fTmp > 3.4028E+038 && 
	       Jss.da_cmpValue[k].im *fTmp > 3.4028E+038){
	      printf("Data is Inf val = %e (%e)* %e .\n",Jss.da_cmpValue[k].re,Jss.da_cmpValue[k].im,fTmp) ;
		dComplexArrayFree(&Ryy) ;
		dComplexArrayFree(&Zns1) ;
		dComplexArrayFree(&Znm1) ;
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Jss) ;
		dComplexArrayFree(&Jpri) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Z11) ;
		
		return 0 ;
	    }
	    
	    pcamyu_J_SS->ha_cmpValue[k].re = (HA_Float)(Jss.da_cmpValue[k].re *fTmp) ;
	    pcamyu_J_SS->ha_cmpValue[k].im = (HA_Float)(Jss.da_cmpValue[k].im *fTmp) ;
	}
	
    }	// end iSSMethod = 2
    	// 2010.4.7
    /*
	printf("myuJss = ") ;
	for(k = 0 ; k < pcamyu_J_SS->ha_numValue ; k++){
	  printf("%f , %f /",pcamyu_J_SS->ha_cmpValue[k].re , pcamyu_J_SS->ha_cmpValue[k].im) ;
	}
	printf("\n") ;
    */
	// noise floor
	// Zns1 = abs(caY) ;
	//ComplexArrayAlloc(&Zns1,Nsrc*1) ;
    dComplexArrayInit(&Zns1) ;
    if(AbsdfCompMat1(&Zns1 ,caY) != 1){
	dComplexArrayFree(&Ryy) ;
	
	dComplexArrayFree(&Zns1) ;
	dComplexArrayFree(&Znm1) ;
	dComplexArrayFree(&E) ;
	dComplexArrayFree(&Jss) ;
	dComplexArrayFree(&Jpri) ;
	dComplexArrayFree(&Znsns) ;
	dComplexArrayFree(&Znsnm) ;
	dComplexArrayFree(&Z11) ;
	
	return 0 ;
    }
    // fYrms
    fYrms = 0.0 ;
    for ( k = 0 ; k < Zns1.ha_numValue ; k++){
	fYrms = (double)pow(Zns1.da_cmpValue[k].re,2.0) + fYrms ;
    }
    fYrms = fYrms / Zns1.ha_numValue ;
    fYrms = (double)sqrt(fYrms) ;
 
    if(fYrms < fNoiseFloor ){
       printf("Noise floor  reset\n") ;
	//ComplexArrayAlloc(pcamyu_J_SS,Nsrc*Nmic) ;
	ComplexArrayInit(pcamyu_J_SS) ;
    }
    dComplexArrayFree(&Ryy) ;		
    dComplexArrayFree(&Zns1) ;
    dComplexArrayFree(&Znm1) ;
    dComplexArrayFree(&E) ;
    dComplexArrayFree(&Jss) ;
    dComplexArrayFree(&Jpri) ;
    dComplexArrayFree(&Znsns) ;
    dComplexArrayFree(&Znsnm) ;
    dComplexArrayFree(&Z11) ;
    
    return 1 ;

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
							MICARY_ComplexArray *pcamyu_J_LC ) {
	
	MICARY_dComplexArray E ,Znsns ,Znsnm,Jlc ;
	int k ;
	double fJlcp,foptLCMyu ;
	MICARY_dComplexArray Atmp ; //TF(caAはTF_CONJ)

	// Alloc
	dComplexArrayAlloc(&E,0) ;
	dComplexArrayAlloc(&Znsns,0) ;
	dComplexArrayAlloc(&Znsnm,0) ;
	dComplexArrayAlloc(&Jlc,0) ;
	dComplexArrayAlloc(&Atmp,0) ;

	// Initialize
	dComplexArrayAlloc(&E,iNsrc*iNsrc) ;
	dComplexArrayAlloc(&Znsns,iNsrc*iNsrc) ;
	dComplexArrayAlloc(&Znsnm,iNsrc*iNmic) ;
	dComplexArrayAlloc(&Jlc,iNsrc*iNmic) ;
	dComplexArrayAlloc(&Atmp,iNsrc*iNmic) ;
	
	for(k = 0 ; k < caA.ha_numValue ; k++ ){
	  Atmp.da_cmpValue[k].re = caA.ha_cmpValue[k].re ;
	  Atmp.da_cmpValue[k].im = caA.ha_cmpValue[k].im * (-1.0) ;
	}
	
	//Znsns = eye(Nsrc) ;
	if(EyedCompMat1v2(&Znsns,iNsrc,iNsrc) != 1){

		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;

		return 0 ;
	}

	// E = W*A
	if(ProductfdCompMat1(&E,caW,iNsrc,iNmic,Atmp,iNmic,iNsrc) != 1){
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;	

		return 0 ;
	}
	

	// E = E - Znsns
	if(SubdCompMat1(&E,E,iNsrc,iNsrc,Znsns,iNsrc,iNsrc) != 1){
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;

		return 0 ;
	}
	// LC_Const = FULLは以上まで
	
	if(iLCConst == _LC_CONST_DIAG){	//LC_CONST = DIAG
		if(DiagdCompMat1(&E,E,iNsrc,iNsrc) != 1) {

			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Jlc) ;
			dComplexArrayFree(&Atmp) ;

			return 0 ;
		}
	}	// end LC_CONST = DIAG
	
	// Znsnm = A'
	if(ConjTransdCompMat1(&Znsnm,Atmp,iNmic,iNsrc) !=1){
		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;

		return 0 ;
	}
	
	// Jlc = E * Znsnm
	if(ProductdCompMat1(&Jlc,E,iNsrc,iNsrc,Znsnm,iNsrc,iNmic)!= 1){

		dComplexArrayFree(&E) ;
		dComplexArrayFree(&Znsns) ;
		dComplexArrayFree(&Znsnm) ;
		dComplexArrayFree(&Jlc) ;
		dComplexArrayFree(&Atmp) ;

		return 0 ;
	}
	// ステップサイズと更新量の決定
	if(iLCMethod == _LC_METHOD_FIX){	//LC_Method = Fix
	   for(k = 0 ; k < Jlc.ha_numValue ;k++){
		   if(Jlc.da_cmpValue[k].re *(double)fLCMyu > __MAX_FLOAT_VALUE && 
			  Jlc.da_cmpValue[k].im *(double)fLCMyu > __MAX_FLOAT_VALUE){
				printf("Data is Inf.\n") ;
				dComplexArrayFree(&E) ;
				dComplexArrayFree(&Znsns) ;
				dComplexArrayFree(&Znsnm) ;
				dComplexArrayFree(&Jlc) ;
				dComplexArrayFree(&Atmp) ;
			
			  return 0 ;
			}
	    pcamyu_J_LC->ha_cmpValue[k].re = (HA_Float)(Jlc.da_cmpValue[k].re *(double)fLCMyu) ;
	    pcamyu_J_LC->ha_cmpValue[k].im = (HA_Float)(Jlc.da_cmpValue[k].im *(double)fLCMyu) ;
	  }
	   
	} // end LC_Method = Fix
	else{	// _LC_METHOD_ADAPTIVE
		dComplexArrayInit(&Znsnm) ;
		if(AbsdCompMat1(&Znsnm,Jlc) != 1) {

			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Jlc) ;
			dComplexArrayFree(&Atmp) ;
			
			return 0 ;
		}
		fJlcp = 0.0 ;
		for(k = 0 ; k< Znsnm.ha_numValue ; k++) {
			fJlcp = (double)pow(Znsnm.da_cmpValue[k].re,(double)2.0) + fJlcp ;
		}
		fJlcp = (double)(fJlcp + 0.0000000001) ;
		
		dComplexArrayInit(&Znsns) ;
		if(AbsdCompMat1(&Znsns,E) != 1) {

			dComplexArrayFree(&E) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Jlc) ;
			dComplexArrayFree(&Atmp) ;

			return 0 ;
		}
		//fJlcp = sum(abs(Jlc)^2)
		foptLCMyu = 0.0 ;
		for(k = 0 ; k< Znsns.ha_numValue ; k++) {
			foptLCMyu = (double)(pow(Znsns.da_cmpValue[k].re,2.0) + foptLCMyu) ;
		}
		// zero divide 2008/10.2
		if(fabs(fJlcp) < _MINIMUM_ZERO_VALUE){
		  foptLCMyu = foptLCMyu / (fJlcp+_MINIMUM_ZERO_VALUE) / 2 ;
		  
		}
		else{
				foptLCMyu = foptLCMyu / fJlcp / 2 ;
		}
		 for(k = 0 ; k < Jlc.ha_numValue ;k++){
			if(Jlc.da_cmpValue[k].re *foptLCMyu > __MAX_FLOAT_VALUE && 
			  Jlc.da_cmpValue[k].im *foptLCMyu > __MAX_FLOAT_VALUE){
			  printf("Data is Inf.\n") ;
			  dComplexArrayFree(&E) ;
			dComplexArrayFree(&Znsns) ;
			dComplexArrayFree(&Znsnm) ;
			dComplexArrayFree(&Jlc) ;
			dComplexArrayFree(&Atmp) ;

			
			  return 0 ;
			}
			pcamyu_J_LC->ha_cmpValue[k].re = Jlc.da_cmpValue[k].re *foptLCMyu ;
			pcamyu_J_LC->ha_cmpValue[k].im = Jlc.da_cmpValue[k].im *foptLCMyu ;
		 }
       	}	// end LC_Method = Adaptive
	/*
	printf("LCmyu = ") ;
	for(k = 0 ; k < pcamyu_J_LC->ha_numValue ; k++){
	  printf("%e,%e /",pcamyu_J_LC->ha_cmpValue[k].re,pcamyu_J_LC->ha_cmpValue[k].im) ;
}
	printf("\n") ;
	*/
	dComplexArrayFree(&E) ;
	dComplexArrayFree(&Znsns) ;
	dComplexArrayFree(&Znsnm) ;
	dComplexArrayFree(&Jlc) ;
	dComplexArrayFree(&Atmp) ;

	return 1 ;
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

        int Nsrc,Nfreq,Nmic,ifreq,ind;
	MICARY_ComplexArray A,X ,W ,Y,J_SS, J_LC; //Ryy,Rxx,
	MICARY_dComplexArray Rxx,Ryy ;
	HA_Float Threshold_val ,Max_val,fTmp;
	int iUp ;
	int iOCRAMethod = _OCRA_METHOD_NONE ;	// OCRAは無効
	int k ;
	iUp = 0 ;

	//Trehold_val
	Max_val = 32768.0 ;
	Max_val = 10*log10(Max_val) ;
	fTmp = (Max_val - ha_fThreshold)/10.0 ; 
	Threshold_val = pow(10.0,fTmp) ;

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
	  return 0 ;
	}
	
	
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

	while(1){	// frequency loop
		
	        if (ifreq == 0){	//D.C
			ifreq++ ;
			continue ;
		}

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
		   memcpy(A.ha_cmpValue,&(psSavedSeparation->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[(ifreq) * Nsrc*Nmic]),sizeof(HA_Complex)*Nsrc*Nmic) ;
		// Input spectrum X[Nmic,1]
		// (注意!!) spectrum data:frame->ch->freqの順に並んでいる
		for (ind = 0 ; ind < Nmic ; ind ++){
		  //memcpy(&(X.ha_cmpValue[ind]),&(csInputSpec.micary_cmpaSpec.ha_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq)+ind*Nfreq]),sizeof(HA_Complex)) ;
		  // 2010.8.30
		   memcpy(&(X.ha_cmpValue[ind]),&(csInputSpec.micary_cmpaSpec.ha_cmpValue[(ifreq)+ind*Nfreq]),sizeof(HA_Complex)) ;
		}
		
		// Separation matrix W[Nsrc*Nmic]
		// 2010.8.30
		//memcpy(W.ha_cmpValue,&(psSavedSeparation->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nsrc*Nmic]),sizeof(HA_Complex)*Nsrc*Nmic) ;
		memcpy(W.ha_cmpValue,&(psSavedSeparation->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[(ifreq) * Nsrc*Nmic]),sizeof(HA_Complex)*Nsrc*Nmic) ;
		
		// Correlation matrix Rxx [Nmic*Nmic]
		// 2010.8.30
		//memcpy(Rxx.da_cmpValue,&(pcsRxx->micary_cmpaSpec.da_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nmic*Nmic]),sizeof(DA_Complex)*Nmic*Nmic) ;
		memcpy(Rxx.da_cmpValue,&(pcsRxx->micary_cmpaSpec.da_cmpValue[(ifreq) * Nmic*Nmic]),sizeof(DA_Complex)*Nmic*Nmic) ;
		// Correlation matrix Ryy [Nsrc*Nsrc]
		// 2010.8.30
		//memcpy(Ryy.da_cmpValue,&(pcsRyy->micary_cmpaSpec.da_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nsrc*Nsrc]),sizeof(DA_Complex)*Nsrc*Nsrc) ;
		memcpy(Ryy.da_cmpValue,&(pcsRyy->micary_cmpaSpec.da_cmpValue[(ifreq) * Nsrc*Nsrc]),sizeof(DA_Complex)*Nsrc*Nsrc) ;

		// ------------------------------------------//
		// ---------- Execute GHDSS --------------- //
		if(ProductCompMat1(&Y,W,Nsrc,Nmic,X,Nmic,1) != 1){
			
			ComplexArrayFree(&A) ;
			ComplexArrayFree(&X) ;
			ComplexArrayFree(&W) ;
			ComplexArrayFree(&Y) ;
			dComplexArrayFree(&Ryy) ;
			dComplexArrayFree(&Rxx) ;
			ComplexArrayFree(&J_SS) ;
			ComplexArrayFree(&J_LC) ;

			return 0 ;
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
		    csSeparationSpec.micary_cmpaSpec.ha_cmpValue[ind*Nfreq+ifreq].re = Y.ha_cmpValue[ind].re ;
		    csSeparationSpec.micary_cmpaSpec.ha_cmpValue[ind*Nfreq+ifreq].im = Y.ha_cmpValue[ind].im ;
		}

		
		// --------------------------------------- //
		
		
		// ------ Update separated matrix --------- //
		// Threshold
       		for(ind = 0 ; ind < X.ha_numValue ; ind++){
		    fTmp = pow(X.ha_cmpValue[ind].re,2.0) + pow(X.ha_cmpValue[ind].im,2.0) ;
		   fTmp = sqrt(fTmp) ;
		    if(fTmp < Threshold_val){
			iUp = 1 ;
			break ;
		    }
		    
		}
		
		if(iUp == 0){
		    // SS
		    if(sPrmSeparation.ha_iSSMethod == _SS_METHOD_LCMYU){ //LCMyuと共通化
			sPrmSeparation.ha_fSSMyu = sPrmSeparation.ha_fLCMyu ;
		    }
		    
		    if(_dCalcSSStepSize_GHDSS(W,A,Y,X,
					      sPrmSeparation.ha_iSSMethod,
					      sPrmSeparation.ha_fSSMyu,
					      sPrmSeparation.ha_fSSScal,
					      sPrmSeparation.ha_fNoiseFloor,
					      &J_SS) != 1 ){
			
			ComplexArrayFree(&A) ;
			ComplexArrayFree(&X) ;
			ComplexArrayFree(&W) ;
			ComplexArrayFree(&Y) ;
			dComplexArrayFree(&Ryy) ;
			dComplexArrayFree(&Rxx) ;
			ComplexArrayFree(&J_SS) ;
			ComplexArrayFree(&J_LC) ;
			
			return 0 ;
		    }

		    


		    if(ha_iUpdate == 0){	//段階的

		     
			if(SubCompMat1(&W,W,Nsrc,Nmic,J_SS,Nsrc,Nmic) != 1){

			    ComplexArrayFree(&A) ;
			    ComplexArrayFree(&X) ;
			    ComplexArrayFree(&W) ;
			    ComplexArrayFree(&Y) ;
			    dComplexArrayFree(&Ryy) ;
			    dComplexArrayFree(&Rxx) ;
			    ComplexArrayFree(&J_SS) ;
			    ComplexArrayFree(&J_LC) ;
			    
			    return 0 ;
			}
		   
			
		    }
		    
		    
		    // LC
		    if(_dCalcLCStepSize_GHDSS(W,A,Nsrc,Nmic,
					      sPrmSeparation.ha_iLCConst,sPrmSeparation.ha_iLCMethod,
					      sPrmSeparation.ha_fLCMyu,&J_LC) != 1){
			
			ComplexArrayFree(&A) ;
			ComplexArrayFree(&X) ;
			ComplexArrayFree(&W) ;
			ComplexArrayFree(&Y) ;
			dComplexArrayFree(&Ryy) ;
			dComplexArrayFree(&Rxx) ;
			ComplexArrayFree(&J_SS) ;
			ComplexArrayFree(&J_LC) ;
			
			return 0 ;
		    }
		   
		    if(ha_iUpdate == 0){	// 段階的
		    
		      // Update W
			if(SubCompMat1(&W,W,Nsrc,Nmic,J_LC,Nsrc,Nmic) != 1){
			    
			    ComplexArrayFree(&A) ;
			    ComplexArrayFree(&X) ;
			    ComplexArrayFree(&W) ;
			    ComplexArrayFree(&Y) ;
			    dComplexArrayFree(&Ryy) ;
			    dComplexArrayFree(&Rxx) ;
			    ComplexArrayFree(&J_SS) ;
			    ComplexArrayFree(&J_LC) ;
			    
			    return 0 ;
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
			if(SubCompMat1(&W,W,Nsrc,Nmic,J_SS,Nsrc,Nmic) != 1){
			    
			    ComplexArrayFree(&A) ;
			    ComplexArrayFree(&X) ;
			    ComplexArrayFree(&W) ;
			    ComplexArrayFree(&Y) ;
			    dComplexArrayFree(&Ryy) ;
			    dComplexArrayFree(&Rxx) ;
			    ComplexArrayFree(&J_SS) ;
			    ComplexArrayFree(&J_LC) ;
			    
			    return 0 ;
			}
			if(SubCompMat1(&W,W,Nsrc,Nmic,J_LC,Nsrc,Nmic) != 1){
			    
			    ComplexArrayFree(&A) ;
			    ComplexArrayFree(&X) ;
			    ComplexArrayFree(&W) ;
			    ComplexArrayFree(&Y) ;
			    dComplexArrayFree(&Ryy) ;
			    dComplexArrayFree(&Rxx) ;
			    ComplexArrayFree(&J_SS) ;
			    ComplexArrayFree(&J_LC) ;
			    
			    return 0 ;
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
		memcpy(&(psSavedSeparation->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[(ifreq) * Nsrc*Nmic]),W.ha_cmpValue,sizeof(HA_Complex)*Nsrc*Nmic) ;
		
		// Correlation matrix Rxx [Nmic*Nmic]
		//2010.8.30
		//memcpy(&(pcsRxx->micary_cmpaSpec.da_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nmic*Nmic]),Rxx.da_cmpValue,sizeof(DA_Complex)*Nmic*Nmic) ;
		memcpy(&(pcsRxx->micary_cmpaSpec.da_cmpValue[(ifreq) * Nmic*Nmic]),Rxx.da_cmpValue,sizeof(DA_Complex)*Nmic*Nmic) ;
		// Correlation matrix Ryy [Nsrc*Nsrc]
		// 2010.8.30
		//memcpy(&(pcsRyy->micary_cmpaSpec.da_cmpValue[(ifreq-csInputSpec.ha_iLowerFreq) * Nsrc*Nsrc]),Ryy.da_cmpValue,sizeof(DA_Complex)*Nsrc*Nsrc) ;
		memcpy(&(pcsRyy->micary_cmpaSpec.da_cmpValue[(ifreq) * Nsrc*Nsrc]),Ryy.da_cmpValue,sizeof(DA_Complex)*Nsrc*Nsrc) ;
		// ------------------------------------------//
		ifreq ++ ;		
	}
	//exit(0);
	ComplexArrayFree(&A) ;
	ComplexArrayFree(&X) ;
	ComplexArrayFree(&W) ;
	ComplexArrayFree(&Y) ;
	dComplexArrayFree(&Ryy) ;
	dComplexArrayFree(&Rxx) ;
	ComplexArrayFree(&J_SS) ;
	ComplexArrayFree(&J_LC) ;

	return 1 ;
}

