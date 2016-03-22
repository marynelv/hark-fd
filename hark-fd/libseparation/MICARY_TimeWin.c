// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file MICARY_TimeWin.c
 \brief \~English Source file for making time window function.
 \brief \~Japanese 時間窓関数作成のためのソースファイル
 \author Sachiko Suzuki
 \date 17 Nov 2006
 \version $Id: MICARY_TimeWin.c 450 2009-03-16 10:12:38Z suzuki $
 */
// ---------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "MICARY_GHDSS_struct.h"
#include "MICARY_struct.h"
#include "MICARY_TimeWin.h"

//#include "ArrayInit.h"
// ---------------------------------------------------------------------
//VALIN window function
float VALINwindfunc( float fX){
	float fY ;
	fY = cos(fX) ;
	fY = pow((0.5 - 0.5 * fY),2.0) ;
	return fY ;
}

int MakeTimeWin(MICARY_FloatArray * pfaTWin,
					 HA_Integer iNwin,
					 float facoef,
					 MICARY_WindowEnum enumWin)
{
	int i;
	HA_Float ft,fY ;
	float pi ;
	
	fY = 0 ;
	pi = 3.141592653589793238462643383279;

	if (pfaTWin ->ha_numValue != iNwin)
		{
			FloatArrayFree(pfaTWin);
			FloatArrayAlloc(pfaTWin,iNwin);
		}
		
	switch (enumWin){
	case nwnSpecial : // Special window
		
		for(i = 0 ; i < iNwin  ; i++){
			ft = 4 * (float)i / (float)iNwin ;
			
			if(ft >= 0.0 && ft < 1.0){
				fY = VALINwindfunc(facoef *ft) ;
				pfaTWin ->ha_fValue[i] = sqrt(fY) ;
			}
			else if(ft >= 1.0 && ft < 2.0 ){
				fY = VALINwindfunc(facoef * (2.0 - ft)) ;
				pfaTWin -> ha_fValue[i] = sqrt(1-fY) ;
			}
			else if(ft >= 2.0 && ft < 3.0){
				fY = VALINwindfunc(facoef * (ft -2)) ;
				pfaTWin -> ha_fValue[i] = sqrt(1-fY) ;
			}
			else if(ft >= 3.0 && ft < 4.0){
				fY = VALINwindfunc(facoef * (4-ft)) ;
				pfaTWin ->ha_fValue[i] = sqrt(fY) ;
			}
		}
	break ;	
	case nwnHanning : // Hanning window
		
		for(i = 0 ; i < iNwin ; i++){
			//2009.3.16 変更 Morio 最終フレームに0を代入する必要がない

			//if (i >= 0 && i < iNwin-1 ){
			//	//fY =  cos(2.0 * M_PI * (float)(i)/((float)(iNwin-1))) ; 
			//	fY =  cos(2.0 * M_PI * (float)(i+1)/((float)(iNwin+1))) ; //MatLab
			//	fY = fY * 0.5 ;
			//	pfaTWin ->ha_fValue[i] = 0.5 - fY ;
			//}
			//else{
			//	pfaTWin ->ha_fValue[i] = 0 ;
			//}

			fY =  cos(2.0 * M_PI * (float)(i+1)/((float)(iNwin+1))) ; //MatLab
			fY = fY * 0.5 ;
			pfaTWin ->ha_fValue[i] = 0.5 - fY ;

			
		}
		break ;

	}

	return 1;
}
// ------------------------------------------------------------------------------
//時間窓をかける(MICARY_IntArray2版)
MICARY_IntArray2 *ProductTimeWin(MICARY_FloatArray * pfaTWin,
				MICARY_IntArray2 iaData)
{
	MICARY_IntArray2 *iaReturn ;
	int ind1,ind2 ;
	iaReturn = (MICARY_IntArray2 *)calloc(1,sizeof(MICARY_IntArray2)) ;
	if(iaData.ha_numValue2 != pfaTWin->ha_numValue){
		exit(1) ;
	}
	IntArray2Alloc(iaReturn,iaData.ha_numValue1,iaData.ha_numValue2);

	for(ind1 = 0 ; ind1 < iaData.ha_numValue1 ; ind1 ++){
		for(ind2 = 0 ; ind2 < iaData.ha_numValue2 ; ind2 ++){		
			iaReturn->ha_iValue[ind1][ind2] = (HA_Integer)(iaData.ha_iValue[ind1][ind2] * pfaTWin->ha_fValue[ind2]) ;
		}
	}
	return iaReturn ;
}

// ------------------------------------------------------------------------------
//時間窓をかける(MICARY_FloatArray版)
MICARY_FloatArray *ProductTimeWinF(MICARY_FloatArray * pfaTWin,
				MICARY_FloatArray faData,HA_Num FrameLen)
{
	MICARY_FloatArray *faReturn ;
	int i,iIndex;
	faReturn = (MICARY_FloatArray *)calloc(1,sizeof(MICARY_FloatArray)) ;
	if(FrameLen != pfaTWin->ha_numValue){
		exit(1) ;
	}
	FloatArrayAlloc(faReturn,faData.ha_numValue);
	
	i = 0 ;
	iIndex = 0 ;
	while(1){
		faReturn->ha_fValue[iIndex] = faData.ha_fValue[iIndex] * pfaTWin->ha_fValue[i] ;
		i ++ ;
		iIndex ++ ;
		if(i >= FrameLen){
			i = 0 ;
		}
		if(iIndex >= faData.ha_numValue ){
			break ;
		}
	}
	return faReturn ;
}
// -----------------------------------------------------------------------------------------------------------
//時間窓をかける(input:MICARY_IntArray2 ->output:MICARY_FloatArray2版)
MICARY_FloatArray2 *ProductTimeWinFloat2(MICARY_FloatArray * pfaTWin,
				MICARY_IntArray2 iaData)
{
	MICARY_FloatArray2 *faReturn ;
	int ind1,ind2 ;
	faReturn = (MICARY_FloatArray2 *)calloc(1,sizeof(MICARY_FloatArray2)) ;
	if(iaData.ha_numValue2 != pfaTWin->ha_numValue){
		exit(1) ;
	}
	FloatArray2Alloc(faReturn,iaData.ha_numValue1,iaData.ha_numValue2);

	for(ind1 = 0 ; ind1 < iaData.ha_numValue1 ; ind1 ++){
		for(ind2 = 0 ; ind2 < iaData.ha_numValue2 ; ind2 ++){		
			faReturn->ha_fValue[ind1][ind2] = (HA_Float)(iaData.ha_iValue[ind1][ind2] * pfaTWin->ha_fValue[ind2]) ;
		}
	}
	return faReturn ;
}
// -----------------------------------------------------------------------------------------------------------
//時間窓をかける(input:MICARY_IntArray2 ->output:MICARY_FloatArray2版)
int ProductTimeWinFloat3(MICARY_FloatArray2 *faReturn,MICARY_FloatArray * pfaTWin,
				MICARY_IntArray2 iaData)
{
	int ind1,ind2 ;
	//faReturn = (MICARY_FloatArray2 *)calloc(1,sizeof(MICARY_FloatArray2)) ;
	//FloatArray2Alloc(faReturn,iaData.ha_numValue1,iaData.ha_numValue2);

	if(!faReturn)
		return 0 ;

	if(iaData.ha_numValue2 != pfaTWin->ha_numValue){
		return 0;
	}
	
	for(ind1 = 0 ; ind1 < iaData.ha_numValue1 ; ind1 ++){
		for(ind2 = 0 ; ind2 < iaData.ha_numValue2 ; ind2 ++){		
			faReturn->ha_fValue[ind1][ind2] = (HA_Float)(iaData.ha_iValue[ind1][ind2] * pfaTWin->ha_fValue[ind2]) ;
		}
	}
	return 1 ;
}
//------------------------------------------------------------------------------------------------
//時間窓をかける(MICARY_FloatArray版)
int ProductTimeWinF3(MICARY_FloatArray *faReturn,MICARY_FloatArray * pfaTWin,
				MICARY_FloatArray faData,HA_Num FrameLen)
{
	int i,iIndex;
	
	if(FrameLen != pfaTWin->ha_numValue){
		return 0 ;
	}
	i = 0 ;
	iIndex = 0 ;
	while(1){
		faReturn->ha_fValue[iIndex] = faData.ha_fValue[iIndex] * pfaTWin->ha_fValue[i] ;
		i ++ ;
		iIndex ++ ;
		if(i >= FrameLen){
			i = 0 ;
		}
		if(iIndex >= faData.ha_numValue ){
			break ;
		}
	}
	return 1;
}