// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2010 Honda Research Institute Japan, Co., Ltd.

 \file CMICARY_Matrix.cpp
 \brief \~English Source file for matrix
 \brief \~Japanese 行列演算用ソースファイル(GDHSS program)
 \author Aki Yamamoto
 \date 22 Feb 2011
 \version $Id: CMICARAY_Matrix.cpp $
*/
// ---------------------------------------------------------------------
#define NDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// ---------------------------------------------------------------------
#include "CMICARY_Matrix.h"
// --------------Add 2011.01.19-------------------------------------------

void dComplexArrayAlloc(MICARY_dComplexArray * array, int iArraySize)
{
	if (!array){
		return;
	}
	
	if (iArraySize == 0){
		array->da_cmpValue = NULL;
		array->ha_numValue = 0;		
// 2012/01/13 added
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved = 0;
#endif
		return;
	}
/*
	if (array->ha_cmpValue){
		ComplexArrayFree(array);
	}
*/	
	array->da_cmpValue = (DA_Complex *)calloc(iArraySize, sizeof(DA_Complex));
	array->ha_numValue = iArraySize;
// 2012/01/13 added
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved = iArraySize;
#endif
}
// ---------------------------------------------------------------------
// 2012/01/13 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void dComplexArrayAlloc_r(MICARY_dComplexArray * array, int iArraySize, int iReservedSize)
{
	if (!array){
		return;
	}
	
	if (iArraySize == 0 && iReservedSize == 0){
		array->da_cmpValue = NULL;
		array->ha_numValue = 0;		
		array->ha_numReserved = 0;
		return;
	}

	if (iArraySize > iReservedSize){
		iReservedSize = iArraySize;
	}
	
	if (array->ha_numReserved < iReservedSize){
		if (array->ha_numValue > 0){
			dComplexArrayFree(array);
		}
		array->da_cmpValue = (DA_Complex *)calloc(iReservedSize, sizeof(DA_Complex));
		array->ha_numReserved = iReservedSize;
	}
	
	array->ha_numValue = iArraySize;
}
#endif
// ---------------------------------------------------------------------

void dComplexArrayFree(MICARY_dComplexArray * array)
{
	if (!array){
		return;
	}
	
	if (array->da_cmpValue){
		free(array->da_cmpValue);
	}
	array->da_cmpValue = NULL;
	array->ha_numValue = 0;
// 2012/01/13 added
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = 0;
#endif
}
// ---------------------------------------------------------------------
// 2012/01/13 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void dComplexArrayClear(MICARY_dComplexArray * array)
{
	if (!array){
		return;
	}
	
	array->ha_numValue = 0;
}
#endif
// ---------------------------------------------------------------------
void dComplexArrayInit(MICARY_dComplexArray * array) 
{

	int k ;
	if(!array){
		return ;
	}
	
	// 2011/11/16 changed by N.Tanaka
	//for (k = 0 ; k < array->ha_numValue ; k++){
	//	array->da_cmpValue[k].re = 0.0 ;
	//	array->da_cmpValue[k].im = 0.0 ;
	//}
	DA_Complex *pValue;
	pValue = array->da_cmpValue;
	for (k = 0 ; k < array->ha_numValue ; k++, pValue++){
		pValue->re = 0.0 ;
		pValue->im = 0.0 ;
	}
	
	//memset(&array->ha_cmpValue[0].re,0,array->ha_numValue) ;
	//memset(&array->ha_cmpValue[0].im,0,array->ha_numValue) ;

	return ;
}
// ---------------------------------------------------------------------
void AllocdComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex)
{
	
	int i, iArraySize;

	if (!pcsSpec){
		return;
	}

	iArraySize = numCh*(iUpperIndex - iLowerIndex + 1);

	if (iArraySize == 0){
		dComplexArrayAlloc(&pcsSpec->micary_cmpaSpec, 0);
		pcsSpec->ha_numCh = 0;
		pcsSpec->ha_iLowerFreq = 0;
		pcsSpec->ha_iUpperFreq = 0;
// 2012/01/13 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		pcsSpec->ha_numReservedChSize = 0;
		pcsSpec->ha_numReservedFreqSize = 0;
#endif
		return;
	}

	// 数が同じ場合は初期化して返す
	if (pcsSpec->micary_cmpaSpec.da_cmpValue){
		if (pcsSpec->micary_cmpaSpec.ha_numValue == (HA_Num)iArraySize){
			for (i = 0 ; i < pcsSpec->micary_cmpaSpec.ha_numValue ; i++){
				pcsSpec->micary_cmpaSpec.da_cmpValue[i].re = 0.0;
				pcsSpec->micary_cmpaSpec.da_cmpValue[i].im = 0.0;
			}
			return;
		}
	}
			
	dComplexArrayAlloc(&pcsSpec->micary_cmpaSpec, iArraySize);
	pcsSpec->ha_numCh = numCh;
	pcsSpec->ha_iLowerFreq = iLowerIndex;
	pcsSpec->ha_iUpperFreq = iUpperIndex;
// 2012/01/13 added by N.Tanaka
	#ifdef __MICARY_def_USE_RESERVED_ARRAY
	pcsSpec->ha_numReservedChSize = numCh;
	pcsSpec->ha_numReservedFreqSize = iUpperIndex - iLowerIndex + 1;
#endif
}
// ---------------------------------------------------------------------
// 2012/01/13 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void AllocdComplexSpecArray_r(MICARY_dComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex,
						  int iReservedChSize,
						  int iReservedFreqSize)
{
	int i, iArraySize, iReservedSize;

	if (!pcsSpec){
		return;
	}
	
	iArraySize = numCh*(iUpperIndex - iLowerIndex + 1);
	iReservedSize = iReservedChSize * iReservedFreqSize;

	if (iArraySize == 0){
		dComplexArrayAlloc(&pcsSpec->micary_cmpaSpec, 0);
		pcsSpec->ha_numCh = 0;
		pcsSpec->ha_iLowerFreq = 0;
		pcsSpec->ha_iUpperFreq = 0;
		pcsSpec->ha_numReservedChSize = 0;
		pcsSpec->ha_numReservedFreqSize = 0;
		return;
	}

	// 数が同じ場合は初期化して返す
	if (pcsSpec->micary_cmpaSpec.da_cmpValue){
		if (pcsSpec->micary_cmpaSpec.ha_numValue == (HA_Num)iArraySize){
			/* 初期化しない
			for (i = 0 ; i < pcsSpec->micary_cmpaSpec.ha_numValue ; i++){
				pcsSpec->micary_cmpaSpec.ha_cmpValue[i].re = 0.0;
				pcsSpec->micary_cmpaSpec.ha_cmpValue[i].im = 0.0;
			}
			*/
			return;
		}
	}
	
	if (iReservedChSize < numCh){
		iReservedChSize = numCh;
	}
	if (iReservedFreqSize < iUpperIndex-iLowerIndex+1){
		iReservedFreqSize = iUpperIndex-iLowerIndex+1;
	}
			
	dComplexArrayAlloc_r(&pcsSpec->micary_cmpaSpec, iArraySize, iReservedSize);
	pcsSpec->ha_numCh = numCh;
	pcsSpec->ha_iLowerFreq = iLowerIndex;
	pcsSpec->ha_iUpperFreq = iUpperIndex;
	pcsSpec->ha_numReservedChSize = iReservedChSize;
	pcsSpec->ha_numReservedFreqSize = iReservedFreqSize;
}
#endif
// ---------------------------------------------------------------------
// 2008/07/01 added by S.Suzuki
void FreedComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec)
{

	if (!pcsSpec){
		return;
	}

    dComplexArrayFree(&pcsSpec->micary_cmpaSpec);
	pcsSpec->ha_numCh = 0;
	pcsSpec->ha_iLowerFreq = 0;
	pcsSpec->ha_iUpperFreq = 0;
// 2012/01/13 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	pcsSpec->ha_numReservedChSize = 0;
	pcsSpec->ha_numReservedFreqSize = 0;
#endif

}
// ---------------------------------------------------------------------
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void CleardComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec)
{
	if (!pcsSpec){
		return;
	}

	dComplexArrayClear(&pcsSpec->micary_cmpaSpec);
	pcsSpec->ha_numCh = 0;
	pcsSpec->ha_iLowerFreq = 0;
	pcsSpec->ha_iUpperFreq = 0;	
}
#endif
// ---------------------------------------------------------------------
int MICARY_Product_fCompMat(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2)
{
 	int index_o ,index1,index2, n, m, k ; 									 
	HA_Complex cMat ;

	if(!pcaOutMat)
		 return __FALSE__ ;

	if(pcaOutMat->ha_numValue != g1*r2)
		 return __FALSE__ ;

	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	ComplexArrayInit(pcaOutMat) ;

	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		 return __FALSE__ ;
	}

	index_o = 0 ;
	for(n = 0 ; n < r2 ; n++){
		for (k = 0 ; k < g1 ; k++){
			index1 = k ;
			index2 = n*g2 ;

			for(m = 0 ; m < r1 ; m++){
				cMat.re = (caInMat1.ha_cmpValue[index1].re * caInMat2.ha_cmpValue[index2].re)
				 -(caInMat1.ha_cmpValue[index1].im*caInMat2.ha_cmpValue[index2].im) ;

				cMat.im = (caInMat1.ha_cmpValue[index1].re * caInMat2.ha_cmpValue[index2].im)
				 +(caInMat1.ha_cmpValue[index1].im*caInMat2.ha_cmpValue[index2].re) ;

				
				pcaOutMat->ha_cmpValue[index_o].re += cMat.re ;
				pcaOutMat->ha_cmpValue[index_o].im += cMat.im ;

				// increment
				index1 = index1 + g1 ;
				index2 = index2 + 1 ;
			}
			index_o = index_o+1 ;
		}
	}

	return __TRUE__;
}

// ---------------------------------------------------------------------
int MICARY_Product_CompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2)
{
	int index_o ,index1,index2, n, m, k ; 									 
	DA_Complex cMat ;
	
	if(!pcaOutMat)
		 return __FALSE__ ;
	
	if(pcaOutMat->ha_numValue != g1*r2)
		 return __FALSE__ ;
	
	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	dComplexArrayInit(pcaOutMat) ;

	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		 return __FALSE__ ;
	}

	index_o = 0 ;
	for(n = 0 ; n < r2 ; n++){
		for (k = 0 ; k < g1 ; k++){
			index1 = k ;
			index2 = n*g2 ;

			for(m = 0 ; m < r1 ; m++){
				cMat.re = (caInMat1.da_cmpValue[index1].re * caInMat2.da_cmpValue[index2].re)
				 -(caInMat1.da_cmpValue[index1].im*caInMat2.da_cmpValue[index2].im) ;

				cMat.im = (caInMat1.da_cmpValue[index1].re * caInMat2.da_cmpValue[index2].im)
				 +(caInMat1.da_cmpValue[index1].im*caInMat2.da_cmpValue[index2].re) ;

				
				pcaOutMat->da_cmpValue[index_o].re += cMat.re ;
				pcaOutMat->da_cmpValue[index_o].im += cMat.im ;

				// increment
				index1 = index1 + g1 ;
				index2 = index2 + 1 ;
			}
			index_o = index_o+1 ;
		}
	}

	return __TRUE__;
 }
// ------------------------------------------------------------------------------------------------ 
int MICARY_Product_dfCompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2)
{

	int index_o ,index1,index2, n, m, k ; 									 
	DA_Complex cMat ;

	if(!pcaOutMat)
		 return __FALSE__  ;

	if(pcaOutMat->ha_numValue != g1*r2)
		 return __FALSE__  ;

	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	dComplexArrayInit(pcaOutMat) ;

	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		 return __FALSE__ ;
	}

	index_o = 0 ;
	for(n = 0 ; n < r2 ; n++){
		for (k = 0 ; k < g1 ; k++){
			index1 = k ;
			index2 = n*g2 ;

			for(m = 0 ; m < r1 ; m++){
				cMat.re = (caInMat1.da_cmpValue[index1].re * (double)caInMat2.ha_cmpValue[index2].re)
				 -(caInMat1.da_cmpValue[index1].im*(double)caInMat2.ha_cmpValue[index2].im) ;

				cMat.im = (caInMat1.da_cmpValue[index1].re * (double)caInMat2.ha_cmpValue[index2].im)
				 +(caInMat1.da_cmpValue[index1].im*(double)caInMat2.ha_cmpValue[index2].re) ;

				
				pcaOutMat->da_cmpValue[index_o].re += cMat.re ;
				pcaOutMat->da_cmpValue[index_o].im += cMat.im ;

				// increment
				index1 = index1 + g1 ;
				index2 = index2 + 1 ;
			}
			index_o = index_o+1 ;
		}
	}

	return __TRUE__;
 } 
// ---------------------------------------------------------------------
int MICARY_Abs_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat) 
{
	int i ;
	double dTmp ;
	if(!pcaOutMat){
		return __FALSE__ ;
	}

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue){
		return __FALSE__ ;
	}

	for(i = 0 ; i < caInMat.ha_numValue ; i++){
		dTmp = pow((double)caInMat.da_cmpValue[i].re,2.0) + pow((double)caInMat.da_cmpValue[i].im,2.0) ;
		dTmp = sqrt(dTmp) ;
		pcaOutMat->da_cmpValue[i].re = dTmp ;
		pcaOutMat->da_cmpValue[i].im = 0.0 ;
	}

	return __TRUE__ ;
}

// ---------------------------------------------------------------------------
int MICARY_Sub_fCompMat(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_ComplexArray caInMat2,int g2,int r2)
{

	int index ;
	index = 0 ;

	if(!pcaOutMat)
		return __FALSE__ ;
	if(pcaOutMat->ha_numValue != g1*r1)
		return __FALSE__ ;

	if(g1 != g2 || r1 != r2){ //同じサイズでなければならない
		return __FALSE__ ;
	}

	while(__TRUE__){
		if(index >= caInMat1.ha_numValue){
			break ;
		}

		pcaOutMat->ha_cmpValue[index].re = caInMat1.ha_cmpValue[index].re - caInMat2.ha_cmpValue[index].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat1.ha_cmpValue[index].im - caInMat2.ha_cmpValue[index].im ;

		index++ ;
	}
	return __TRUE__;							 
}
// ---------------------------------------------------------------------------
int MICARY_ConjTrans_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,int g,int r)
{
	int index,count_g,count_r ;
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;
	
	index = 0 ;
	count_g = 0 ;
	count_r = 0 ;

	while(__TRUE__){
		if(index >= caInMat.ha_numValue){
			break ;
		}
		
		pcaOutMat->da_cmpValue[index].re = caInMat.da_cmpValue[count_g+count_r*g].re ;
		pcaOutMat->da_cmpValue[index].im = caInMat.da_cmpValue[count_g+count_r*g].im *(-1)  ;
	
		index ++ ;
		count_r ++ ;
		if(count_r >= r){
			count_r = 0 ;
			count_g ++ ;
		}
	}
	return __TRUE__;

}
// ---------------------------------------------------------------------------
int MICARY_ConjTrans_dfCompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r)
{
	
	int index, count_g,count_r ;
	
	if(!pcaOutMat)
		return __FALSE__ ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return __FALSE__ ;
	
	index = 0 ;
	count_g = 0 ;
	count_r = 0 ;

	while(__TRUE__){
		if(index >= caInMat.ha_numValue){
			break ;
		}
		
		pcaOutMat->da_cmpValue[index].re = (double)caInMat.ha_cmpValue[count_g+count_r*g].re ;
		pcaOutMat->da_cmpValue[index].im = (double)caInMat.ha_cmpValue[count_g+count_r*g].im *(-1)  ;
	
		index ++ ;
		count_r ++ ;
		if(count_r >= r){
			count_r = 0 ;
			count_g ++ ;
		}
	}
	return __TRUE__ ;
}

// ---------------------------------------------------------------------
int MICARY_ConjTrans_fCompMat(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r)
{
	
	int index,count_g,count_r ;
	if(!pcaOutMat)
		return __FALSE__ ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return __FALSE__ ;
	
	index = 0 ;
	count_g = 0 ;
	count_r = 0 ;

	while(__TRUE__){
		if(index >= caInMat.ha_numValue){
			break ;
		}
		
		pcaOutMat->ha_cmpValue[index].re = caInMat.ha_cmpValue[count_g+count_r*g].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat.ha_cmpValue[count_g+count_r*g].im *(-1)  ;
	
		index ++ ;
		count_r ++ ;
		if(count_r >= r){
			count_r = 0 ;
			count_g ++ ;
		}
	}
	return __TRUE__;
}
// ---------------------------------------------------------------------------
int MICARY_Product_fdCompMat(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2)
{

	int index_o ,index1,index2, n, m, k ; 									 
	DA_Complex cMat ;

	if(!pcaOutMat)
		 return __FALSE__ ;

	if(pcaOutMat->ha_numValue != g1*r2)
		 return __FALSE__ ;

	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	dComplexArrayInit(pcaOutMat) ;

	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		 return __FALSE__ ;
	}

	index_o = 0 ;
	for(n = 0 ; n < r2 ; n++){
		for (k = 0 ; k < g1 ; k++){
			index1 = k ;
			index2 = n*g2 ;

			for(m = 0 ; m < r1 ; m++){
				cMat.re = ((double)caInMat1.ha_cmpValue[index1].re * caInMat2.da_cmpValue[index2].re)
				 -((double)caInMat1.ha_cmpValue[index1].im*caInMat2.da_cmpValue[index2].im) ;

				cMat.im = ((double)caInMat1.ha_cmpValue[index1].re * caInMat2.da_cmpValue[index2].im)
				 +((double)caInMat1.ha_cmpValue[index1].im*caInMat2.da_cmpValue[index2].re) ;

				pcaOutMat->da_cmpValue[index_o].re += cMat.re ;
				pcaOutMat->da_cmpValue[index_o].im += cMat.im ;

				// increment
				index1 = index1 + g1 ;
				index2 = index2 + 1 ;
			}
			index_o = index_o+1 ;
		}
	}
	return __TRUE__;
 } 
// ---------------------------------------------------------------------------
int MICARY_Sub_CompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2)
{
	int index ;
	index = 0 ;

	if(!pcaOutMat)
		return __FALSE__ ;
	if(pcaOutMat->ha_numValue != g1*r1)
		return __FALSE__ ;

	if(g1 != g2 || r1 != r2){ //同じサイズでなければならない
		return __FALSE__ ;
	}

	while(__TRUE__){
		if(index >= caInMat1.ha_numValue){
			break ;
		}

		pcaOutMat->da_cmpValue[index].re = caInMat1.da_cmpValue[index].re - caInMat2.da_cmpValue[index].re ;
		pcaOutMat->da_cmpValue[index].im = caInMat1.da_cmpValue[index].im - caInMat2.da_cmpValue[index].im ;

		index++ ;
	}
	return __TRUE__;				 
}
// ---------------------------------------------------------------------------
int MICARY_Diag_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat, int g , int r)
{

	int count ,count_g,count_r ;

	if(!pcaOutMat)
		return __FALSE__ ;

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return __FALSE__ ;

	if( g ==0 || r == 0)
		return __FALSE__ ;

	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(__TRUE__){
		if(count >= caInMat.ha_numValue){
			break ;
		}
		if (count_g == count_r){
			pcaOutMat->da_cmpValue[count].re = caInMat.da_cmpValue[count].re ;
			pcaOutMat->da_cmpValue[count].im = caInMat.da_cmpValue[count].im ;
		}
		else{
			pcaOutMat->da_cmpValue[count].re = 0.0 ;
			pcaOutMat->da_cmpValue[count].im = 0.0;
		}
		count++ ;
		count_g ++ ;

		if(count_g == g){
			count_g = 0 ;
			count_r ++ ;
		}
	}
	return __TRUE__ ;
}
// ---------------------------------------------------------------------------
int MICARY_Conj_CompMat(MICARY_dComplexArray *pcaOutMat, MICARY_dComplexArray caInMat)
{
	int index ;
	
	if(!pcaOutMat)
		return __FALSE__ ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return __FALSE__;

	for(index = 0 ; index < caInMat.ha_numValue ; index ++){
		pcaOutMat->da_cmpValue[index].re = caInMat.da_cmpValue[index].re ;
		pcaOutMat->da_cmpValue[index].im = caInMat.da_cmpValue[index].im * (-1) ;
	}
	return __TRUE__;
}
// ---------------------------------------------------------------------------
int MICARY_Dot_CompMat(MICARY_dComplexArray *pcaOut, MICARY_dComplexArray caIn1, MICARY_dComplexArray caIn2) 
{
	int i ;
	if(!pcaOut){
		return __FALSE__ ;
	}
	if(caIn1.ha_numValue != caIn2.ha_numValue){
		return __FALSE__ ;
	}
	for(i = 0 ; i < pcaOut->ha_numValue ; i++){
		pcaOut->da_cmpValue[i].re = caIn1.da_cmpValue[i].re * caIn2.da_cmpValue[i].re - (caIn1.da_cmpValue[i].im * caIn2.da_cmpValue[i].im ) ;
		pcaOut->da_cmpValue[i].im = caIn1.da_cmpValue[i].re * caIn2.da_cmpValue[i].im + (caIn1.da_cmpValue[i].im * caIn2.da_cmpValue[i].re ) ;
	}

	return __TRUE__ ;
}

// ----------------------------------------------------------------------------
int MICARY_Abs_dfCompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat) 
{

	int i ;
	double fTmp ;
	if(!pcaOutMat){
		return __FALSE__ ;
	}

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue){
		return __FALSE__ ;
	}

	for(i = 0 ; i < caInMat.ha_numValue ; i++){
		fTmp = pow((double)caInMat.ha_cmpValue[i].re,2.0) + pow((double)caInMat.ha_cmpValue[i].im,2.0) ;
		fTmp = sqrt(fTmp) ;
		pcaOutMat->da_cmpValue[i].re = fTmp ;
		pcaOutMat->da_cmpValue[i].im = 0.0 ;
	}

	return __TRUE__ ;
}
// ----------------------------------------------------------------------------------
int MICARY_Eye_CompMat(MICARY_dComplexArray *pcaOutMat,int g,int r)
{
	int count, count_g, count_r ;
	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(__TRUE__){
		if(count >= g*r){
			break ;
		}

		if (count_g == count_r){
			pcaOutMat->da_cmpValue[count].re = 1 ;
			pcaOutMat->da_cmpValue[count].im = 0 ;
		}
		else{
			pcaOutMat->da_cmpValue[count].re = 0 ;
			pcaOutMat->da_cmpValue[count].im= 0 ;

		}
		count++ ;
		count_g ++ ;

		if(count_g == g){
			count_g = 0 ;
			count_r ++ ;
		}
	}

	return __TRUE__ ;
}
// ------------------------------------------------------------------------
int MICARY_Multipl_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat, double a)
{
	int index ;

	if(!pcaOutMat)
		return __FALSE__ ;

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return __FALSE__ ;

	index = 0 ;

	while(__TRUE__){
		if(index >= caInMat.ha_numValue){
			break ;
		}

		pcaOutMat->da_cmpValue[index].re = caInMat.da_cmpValue[index].re * a ;
		pcaOutMat->da_cmpValue[index].im = caInMat.da_cmpValue[index].im * a ;

		index++ ;
	}

	return __TRUE__;						 
}	
//-------------------------------------------------------------------------------
int MICARY_Sqrt_CompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1) 
{
	int i ;
	double r , theta ;

	for(i = 0 ; i < caInMat1.ha_numValue ; i++){
		r = pow((double)caInMat1.da_cmpValue[i].re, (double)2.0) + pow((double)caInMat1.da_cmpValue[i].im, (double)2.0) ;
		r = sqrt(r) ;

		// 2008.9.1 
		// zero divide
		if(caInMat1.da_cmpValue[i].re != 0.0){
			theta = atan((double)caInMat1.da_cmpValue[i].im / (double)caInMat1.da_cmpValue[i].re) ;
		}
		else{
			theta = 0.0 ;
		}
		// 2008.9.1
		pcaOutMat->da_cmpValue[i].re = sqrt( (double)r) * cos(theta / (double)2.0) ;
		pcaOutMat->da_cmpValue[i].im = sqrt( (double)r) * sin(theta / (double)2.0) ;
	}

	return __TRUE__ ;
}
//-------------------------------------------------------------------------------
int MICARY_Divide_CompMat(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2)
{
	int i ;
	double dTmp ;

	if(!pcaOutMat){
		return __FALSE__ ;
	}

	if(caInMat1.ha_numValue != caInMat2.ha_numValue){
		return __FALSE__ ;
	}

	for(i = 0 ; i < pcaOutMat->ha_numValue ; i++){
		dTmp = pow((double)caInMat2.da_cmpValue[i].re, (double)2.0) + pow((double)caInMat2.da_cmpValue[i].im, (double)2.0) ;
		if(dTmp == 0){
		  dTmp = 0.0000001 ;
		}
		pcaOutMat->da_cmpValue[i].re = (caInMat1.da_cmpValue[i].re * caInMat2.da_cmpValue[i].re + caInMat1.da_cmpValue[i].im * caInMat2.da_cmpValue[i].im )/dTmp ;

		pcaOutMat->da_cmpValue[i].im = (caInMat1.da_cmpValue[i].im * caInMat2.da_cmpValue[i].re - (caInMat1.da_cmpValue[i].re * caInMat2.da_cmpValue[i].im)) /dTmp ;
	}

	return __TRUE__;
 }
// ---------------------------------------------------------------------------
int MICARY_Sum_CompMat(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2)
{
	int index ;

	if(!pcaOutMat)
		return __FALSE__ ;

	if(pcaOutMat->ha_numValue != g1 * r1)
		return __FALSE__ ;

	index = 0 ;

	if(g1 != g2 || r1 != r2){		//同じサイズでなければならない
		return __FALSE__ ;
	}

	while(__TRUE__){
		if(index >= caInMat1.ha_numValue){
			break ;
		}

		pcaOutMat->da_cmpValue[index].re = caInMat1.da_cmpValue[index].re + caInMat2.da_cmpValue[index].re ;
		pcaOutMat->da_cmpValue[index].im = caInMat1.da_cmpValue[index].im + caInMat2.da_cmpValue[index].im ;

		index++ ;
	}

	return __TRUE__ ;						 
}
// ----------------------------------------------------------------------------------
double MICARY_Max_CompMat(MICARY_dComplexArray caInMat)
{
	int i ;
	double fTmp ;

	fTmp = caInMat.da_cmpValue[0].re ;

	for(i = 0 ; i < caInMat.ha_numValue ; i++){
		if(fTmp < caInMat.da_cmpValue[i].re){
			fTmp = caInMat.da_cmpValue[i].re ;
		}
	}

	return(fTmp) ;
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
int MICARY_Diag_fCompMat(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat, int g , int r)
{
	int count ,count_g,count_r ;

	if(!pcaOutMat)
		return __FALSE__ ;

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return __FALSE__ ;

	if( g ==0 || r == 0)
		return __FALSE__ ;

	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(__TRUE__){
		if(count >= caInMat.ha_numValue){
			break ;
		}

		if (count_g == count_r){
			pcaOutMat->ha_cmpValue[count].re = caInMat.ha_cmpValue[count].re ;
			pcaOutMat->ha_cmpValue[count].im = caInMat.ha_cmpValue[count].im ;
		}
		else{
			pcaOutMat->ha_cmpValue[count].re = 0.0 ;
			pcaOutMat->ha_cmpValue[count].im = 0.0;
		}
		count++ ;
		count_g ++ ;

		if(count_g == g){
			count_g = 0 ;
			count_r ++ ;
		}
	}
	return __TRUE__ ;
}
// ----------------------------------------------------------------------------------
int MICARY_Multipl_fCompMat(MICARY_ComplexArray *pcaOutMat, MICARY_ComplexArray caInMat, float a)
{
	int index ;

	if(!pcaOutMat)
		return __FALSE__ ;

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return __FALSE__ ;

	index = 0 ;

	for(index = 0 ; index < caInMat.ha_numValue ; index++){
		pcaOutMat->ha_cmpValue[index].re = caInMat.ha_cmpValue[index].re * a ;
		pcaOutMat->ha_cmpValue[index].im = caInMat.ha_cmpValue[index].im * a ;
	}

	return __TRUE__;
}	
// ----------------------------------------------------------------------------
int MICARY_Conj_fCompMat(MICARY_ComplexArray *pcaOutMat, MICARY_ComplexArray caInMat)
{
	int index ;
	
	if(!pcaOutMat)
		return __FALSE__ ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return __FALSE__ ;

	for(index = 0 ; index < caInMat.ha_numValue ; index ++){
		pcaOutMat->ha_cmpValue[index].re = caInMat.ha_cmpValue[index].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat.ha_cmpValue[index].im * (-1) ;
	}

	return __TRUE__;
}

// ------------------------------------------------------------------------
int MICARY_Eye_fCompMat(MICARY_ComplexArray *pcaOutMat,int g,int r)
{	
	int count ,count_g,count_r ;
	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(__TRUE__){
		if(count >= g * r){
			break ;
		}

		if (count_g == count_r){
			pcaOutMat->ha_cmpValue[count].re = 1 ;
			pcaOutMat->ha_cmpValue[count].im = 0 ;
		}
		else{
			pcaOutMat->ha_cmpValue[count].re = 0 ;
			pcaOutMat->ha_cmpValue[count].im= 0 ;

		}
		count++ ;
		count_g ++ ;

		if(count_g == g){
			count_g = 0 ;
			count_r ++ ;
		}
	}

	return __TRUE__ ;
}
// -----------------------------------------------------------------------------
int MICARY_Trans_fCompMat(MICARY_ComplexArray *pcaOutMat, MICARY_ComplexArray caInMat,int g,int r)
{
	
	int index,count_g,count_r ;
	if(!pcaOutMat)
		return __FALSE__ ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return __FALSE__ ;

	index = 0 ;
	count_g = 0 ;
	count_r = 0 ;

	while(__TRUE__){
		if(index >= caInMat.ha_numValue){
			break ;
		}
		
		pcaOutMat->ha_cmpValue[index].re = caInMat.ha_cmpValue[count_g+count_r*g].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat.ha_cmpValue[count_g+count_r*g].im  ;
	
		index ++ ;
		count_r ++ ;
		if(count_r >= r){
			count_r = 0 ;
			count_g ++ ;
		}
	}
	return __TRUE__ ;
}

// ---------------------------------------------------------------------------








// ---------------------------------------------------------------------
//   ******************* boost/numeric/ublas/ を使用 *******************
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
//行列の初期化（0埋め）
bool _InitCompMat(cdMat *pcaOutMat)
{
	int row,col ;

	if(!pcaOutMat)  
		return false;


	for(col = 0; col < pcaOutMat->size2(); col++){
		for(row = 0; row < pcaOutMat->size1(); row++){
			
			complex<double> caTemp(0.0, 0.0);	
			pcaOutMat->insert_element(row, col, caTemp);
		}
	}
	return true;
}
// ---------------------------------------------------------------------
//単位行列の作成
bool _EyeCompMat(cdMat *pcaOutMat)
{
	int row, col;
	complex<double> caZero(0.0, 0.0);

	if(!pcaOutMat)  
		return false;

	if(pcaOutMat->size1() != pcaOutMat->size2()){
		return false;
	}

	for(col = 0; col < pcaOutMat->size2(); col++){
		for(row = 0; row < pcaOutMat->size1(); row++){
			if(row == col){
				complex<double> caTemp(1.0, 0.0);	
				pcaOutMat->insert_element(row, col, caTemp);
			}
			else{
				pcaOutMat->insert_element(row, col, caZero);
			}
		}
	}
	return true;
}
// ---------------------------------------------------------------------
//複素対角（正方）行列の作成
bool _DiagCompMat(cdMat *pcaOutMat, cdMat caInMat)
{
	int row, col;
	complex<double> caZero(0.0, 0.0);

	if(!pcaOutMat)  
		return false;

		//サイズが違っていたら，アウト
	if(pcaOutMat->size1() != caInMat.size1() ||  pcaOutMat->size2() !=  caInMat.size2()){
		return false;	//0929
	}

	for(col = 0; col < caInMat.size2(); col++){
		for(row = 0; row < caInMat.size1(); row++){

			if(row == col){
				pcaOutMat->insert_element(row, col, caInMat(row, col));
			}
			else{
				pcaOutMat->insert_element(row, col, caZero);
			}
		}
	}
	return true;
}
// ---------------------------------------------------------------------
//複素行列の絶対値の計算
bool _AbsCompMat(cdMat *pcaOutMat, cdMat caInMat)
{
	int row, col ;
	double dTemp ;
		
	if(!pcaOutMat)  
		return false;

		//サイズが違っていたら，アウト
	if(pcaOutMat->size1() != caInMat.size1() ||  pcaOutMat->size2() !=  caInMat.size2()){
		return false ;	
	}

	for(col = 0 ; col < caInMat.size2() ; col++){
		for(row = 0 ; row < caInMat.size1() ; row++){

			dTemp = pow((double)caInMat(row,col).real(), 2.0) + pow((double)caInMat(row,col).imag(), 2.0);
			
			complex<double> caTemp(sqrt(dTemp), 0.0);	
			pcaOutMat->insert_element(row, col, caTemp);
		}
	}
	//cout << "caOutMat = " << *pcaOutMat << endl;
	
	return true ;
}
// ---------------------------------------------------------------------
//複素行列(real part)の最大値の計算
double _MaxCompMat(cdMat caInMat)
{
	int row, col;
	double dRet;
	
	dRet = caInMat(0,0).real();

	for(col = 0; col < caInMat.size2(); col++){
		for(row = 0; row < caInMat.size1(); row++){
			if( dRet < caInMat(row, col).real() ){  
				dRet = caInMat(row, col).real();
			}
		}
	}

	return dRet;
}
// ---------------------------------------------------------------------
//複素行列の平方根の計算
bool _SqrtCompMat(cdMat *pcaOutMat ,cdMat caInMat)
{
	int row, col;
	double r, theta;

	if(!pcaOutMat)  
		return false;

	for(col = 0; col < caInMat.size2(); col++){
		for(row = 0; row < caInMat.size1(); row++){

			r = pow(caInMat(row, col).real(), 2.0) + pow(caInMat(row, col).imag(), 2.0);
			r = sqrt(r);

			if(caInMat(row, col).real() != 0.0){
				theta = atan(caInMat(row, col).imag() / caInMat(row, col).real());
			}
			else{
				theta = 0.0;
			}

			complex<double> caTemp(sqrt(r) * cos(theta / 2.0), sqrt(r) * sin(theta / 2.0));	
			pcaOutMat->insert_element(row, col, caTemp);
		
		}	
	}

	return true;
}
// ---------------------------------------------------------------------
//複素行列の整数倍
bool _MultiplCompMat(cdMat *pcaOutMat ,cdMat caInMat, double a)
{
	int row, col;
	double r, theta;

	if(!pcaOutMat)  
		return false;

	for(col = 0; col < caInMat.size2(); col++){
		for(row = 0; row < caInMat.size1(); row++){

			complex<double> caTemp(caInMat(row, col).real() * a , caInMat(row, col).imag() * a);	
			pcaOutMat->insert_element(row, col, caTemp);
		}
	}


	return true;
}
// ---------------------------------------------------------------------

// ==================================================================================================
//MICARY_ComplexSpecArray型をMICARY_cdMatSpecArray型(ublas::matrix<complex<double>)に変換
bool _Convert_ComplexSpec(cdMat *pcaOutMat ,
						  MICARY_ComplexArray caIn,
						  int Nrow,
						  int Ncol,
						  int iFreq)
{
	int row, col;
	int idx;	

	if(!pcaOutMat)  
		return false;

	//2010.09.01
	idx = iFreq * Nrow * Ncol ;	//読み込みたい配列の頭のインデックス


	for(col = 0; col < pcaOutMat->size2(); col++){
		for(row = 0; row < pcaOutMat->size1(); row++){
			complex<double> caTemp(caIn.ha_cmpValue[ idx ].re,	caIn.ha_cmpValue[ idx ].im) ;
			
			pcaOutMat->insert_element(row, col, caTemp); 
			idx++;
		}
	}
	return true;
}
//-------------------------------------------------------------------------------------------------
bool _Convert_Complex_InputSpec(cdMat *pcaOutMat ,MICARY_ComplexArray caIn,  int Nfreq, int iFreq)
{
	int row, col, idx;		//row = src, col = mic のつもり

	if(!pcaOutMat)  
		return false;
	
	idx = 0;	//入力された配列のインデックス
	for(col = 0; col < pcaOutMat->size2(); col++){
		for(row = 0; row < pcaOutMat->size1(); row++){
			complex<double> caTemp(caIn.ha_cmpValue[ iFreq  + idx * Nfreq ].re, 
								   caIn.ha_cmpValue[ iFreq  + idx * Nfreq ].im) ;
			
			pcaOutMat->insert_element(row, col, caTemp); 
			idx++;
		}
	}
	return true;
}

// ---------------------------------------------------------------------
//MICARY_dComplexSpecArray型をMICARY_cdMatSpecArray型(ublas::matrix<complex<double>)に変換
bool _Convert_dComplexSpec(cdMat *pcaOutMat ,MICARY_dComplexArray caIn, int Nrow, int Ncol, int iFreq)
{
	int row, col, idx;

	if(!pcaOutMat)  
		return false;

	//idx = (iFreq - iLowerFreq) * Nrow * Ncol;	//読み込みたい配列の頭のインデックス
	idx = iFreq * Nrow * Ncol;	//読み込みたい配列の頭のインデックス
	
	for(col = 0; col < pcaOutMat->size2(); col++){
		for(row = 0; row < pcaOutMat->size1(); row++){
			
			complex<double> caTemp(caIn.da_cmpValue[ idx ].re,	caIn.da_cmpValue[ idx ].im) ;
			
			pcaOutMat->insert_element(row, col, caTemp); 
			idx++;
		}
	}
	return true;
}
// ---------------------------------------------------------------------
bool _Update_ComplexSpec(MICARY_ComplexArray *caOut, cdMat caInMat, int Nrow, int Ncol, int iFreq)
{
	int row, col, idx;
	idx = iFreq * Nrow * Ncol;
	/*FILE *fp_out;
	fp_out = fopen("output_W.txt", "a");*/


	//printf(" W Size = %d %d\n", caInMat.size1(), caInMat.size2() );
	for(col = 0; col < caInMat.size2() ; col++){
		for(row = 0; row < caInMat.size1() ; row++){

			caOut->ha_cmpValue[ idx ].re = caInMat(row, col).real();
			caOut->ha_cmpValue[ idx ].im = caInMat(row, col).imag();
			
			//if(fp_out){
				//fprintf(fp_out, "%d %f\n", idx, caOut->ha_cmpValue[ idx ].re);
			//}
			//	printf("%f %f\n", caOut->ha_cmpValue[ idx ].re,caOut->ha_cmpValue[ idx ].im );
			idx++;
		}
	}

	/*if(fp_out){
		fclose(fp_out);
	}*/
	return true;
}
// ---------------------------------------------------------------------
bool _Update_dComplexSpec(MICARY_dComplexArray *caOut, cdMat caInMat, int Nrow, int Ncol, int iFreq)
{
	int row, col, idx;
	idx = iFreq * Nrow * Ncol;

	for(col = 0; col < caInMat.size2() ; col++){
		for(row = 0; row < caInMat.size1() ; row++){
			caOut->da_cmpValue[ idx ].re = caInMat(row, col).real();
			caOut->da_cmpValue[ idx ].im = caInMat(row, col).imag();
				//printf("%f %f\n", caOut->da_cmpValue[ idx ].re,caOut->da_cmpValue[ idx ].im );
			idx++;
		}
	}

	return true;
}
// ---------------------------------------------------------------------

