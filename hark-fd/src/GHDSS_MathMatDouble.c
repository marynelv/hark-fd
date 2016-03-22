// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_MathMat2Double.c
 \brief	\~English Source file for Matrix operation[type double]. (GHDSS program)
 \brief	\~Japanese 行列演算のためのソースファイル[type double](GHDSS program)
 \author	Sachiko Suzuki
 \date	27 Jun 2008
 \version	$Id: GHDSS_MathMatDouble.c 456 2009-04-09 02:24:03Z suzuki $
*/

// ---------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ArrayInit.h"
#include "MICARY_struct.h"
#include "GHDSS_MathMatDouble.h"
// ---------------------------------------------------------------------
void dComplexArrayAlloc(MICARY_dComplexArray * array, int iArraySize){
	if (!array){
		return;
	}
	
	if (iArraySize == 0){
		array->da_cmpValue = NULL;
		array->ha_numValue = 0;		
		return;
	}
/*
	if (array->ha_cmpValue){
		ComplexArrayFree(array);
	}
*/	
	array->da_cmpValue = (DA_Complex *)calloc(iArraySize, sizeof(DA_Complex));
	array->ha_numValue = iArraySize;
}
// ---------------------------------------------------------------------

void dComplexArrayFree(MICARY_dComplexArray * array){
	if (!array){
		return;
	}
	
	if (array->da_cmpValue){
		free(array->da_cmpValue);
	}
	array->da_cmpValue = NULL;
	array->ha_numValue = 0;
}
// ---------------------------------------------------------------------
void dComplexArrayInit(MICARY_dComplexArray * array) {

	int k ;
	if(!array){
		return ;
	}
	
	for (k = 0 ; k < array->ha_numValue ; k++){
		array->da_cmpValue[k].re = 0.0 ;
		array->da_cmpValue[k].im = 0.0 ;
	}

	//memset(&array->ha_cmpValue[0].re,0,array->ha_numValue) ;
	//memset(&array->ha_cmpValue[0].im,0,array->ha_numValue) ;

	return ;
}
// ---------------------------------------------------------------------
void AllocdComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex){
	
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
}
// ---------------------------------------------------------------------
// 2008/07/01 added by S.Suzuki
void FreedComplexSpecArray(MICARY_dComplexSpecArray *pcsSpec){

	if (!pcsSpec){
		return;
	}

       	    dComplexArrayFree(&pcsSpec->micary_cmpaSpec);
	    pcsSpec->ha_numCh = 0;
	    pcsSpec->ha_iLowerFreq = 0;
	    pcsSpec->ha_iUpperFreq = 0;
       
}
// ---------------------------------------------------------------------



// ---------------------------------------------------------------------

// 1dim.
int ProductdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2){
 

	int index_o ,index1,index2, n, m, k ; 									 
	DA_Complex cMat ;
	if(!pcaOutMat)
		 return 0 ;
	if(pcaOutMat->ha_numValue != g1*r2)
		 return 0 ;
	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	dComplexArrayInit(pcaOutMat) ;

	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		 return 0 ;
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

	return 1;
 }
// ------------------------------------------------------------------------------------------------ 
int ProductdfCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2)
{

	int index_o ,index1,index2, n, m, k ; 									 
	DA_Complex cMat ;
	if(!pcaOutMat)
		 return 0 ;
	if(pcaOutMat->ha_numValue != g1*r2)
		 return 0 ;
	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	dComplexArrayInit(pcaOutMat) ;

	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		 return 0 ;
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

	return 1;
 } 
// --------------------------------------------------------------------------------
int ProductdffCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2)
{

	int index_o ,index1,index2, n, m, k ; 									 
	DA_Complex cMat ;
	if(!pcaOutMat)
		 return 0 ;
	if(pcaOutMat->ha_numValue != g1*r2)
		 return 0 ;
	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	dComplexArrayInit(pcaOutMat) ;

	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		 return 0 ;
	}

	index_o = 0 ;
	for(n = 0 ; n < r2 ; n++){
		for (k = 0 ; k < g1 ; k++){
			index1 = k ;
			index2 = n*g2 ;

			for(m = 0 ; m < r1 ; m++){
				cMat.re = ((double)caInMat1.ha_cmpValue[index1].re * (double)caInMat2.ha_cmpValue[index2].re)
				 -((double)caInMat1.ha_cmpValue[index1].im*(double)caInMat2.ha_cmpValue[index2].im) ;

				cMat.im = ((double)caInMat1.ha_cmpValue[index1].re * (double)caInMat2.ha_cmpValue[index2].im)
				 +((double)caInMat1.ha_cmpValue[index1].im*(double)caInMat2.ha_cmpValue[index2].re) ;

				
				pcaOutMat->da_cmpValue[index_o].re += cMat.re ;
				pcaOutMat->da_cmpValue[index_o].im += cMat.im ;

				// increment
				index1 = index1 + g1 ;
				index2 = index2 + 1 ;
			}
			index_o = index_o+1 ;
		}
	}

	return 1;
 } 

// ---------------------------------------------------------------------
int ProductfdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2)
{

	int index_o ,index1,index2, n, m, k ; 									 
	DA_Complex cMat ;
	if(!pcaOutMat)
		 return 0 ;
	if(pcaOutMat->ha_numValue != g1*r2)
		 return 0 ;
	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	dComplexArrayInit(pcaOutMat) ;

	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		 return 0 ;
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

	return 1;
 } 
// -------------------------------------------------------------------------------------
int ProductfddCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2){
 

	int index_o ,index1,index2, n, m, k ; 									 
	DA_Complex cMat ,cMattmp;
	if(!pcaOutMat)
		 return 0 ;
	if(pcaOutMat->ha_numValue != g1*r2)
		 return 0 ;
	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	ComplexArrayInit(pcaOutMat) ;
	cMattmp.re = 0.0 ;
	cMattmp.im = 0.0 ;
	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		 return 0 ;
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

				cMattmp.re += cMat.re ;
				cMattmp.im += cMat.im ;

				//pcaOutMat->ha_cmpValue[index_o].re += cMat.re ;
				//pcaOutMat->ha_cmpValue[index_o].im += cMat.im ;
				pcaOutMat->ha_cmpValue[index_o].re = (HA_Float)cMattmp.re ;
				pcaOutMat->ha_cmpValue[index_o].im = (HA_Float)cMattmp.im ;

				// increment
				index1 = index1 + g1 ;
				index2 = index2 + 1 ;
			}
			index_o = index_o+1 ;
		}
	}

	return 1;
 }
// ------------------------------------------------------------------------------------------------ 

// ---------------------------------------------------------------------

int ConjdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat){
	int index ;
	
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0;

	for(index = 0 ; index < caInMat.ha_numValue ; index ++){
		pcaOutMat->da_cmpValue[index].re = caInMat.da_cmpValue[index].re ;
		pcaOutMat->da_cmpValue[index].im = caInMat.da_cmpValue[index].im * (-1) ;
	}
	return 1;

}

// ------------------------------------------------------------------------
int DiagdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat, int g , int r){

	int count ,count_g,count_r ;

	if(!pcaOutMat)
		return 0 ;

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;

	if( g ==0 || r == 0)
		return 0 ;

	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(1){
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
	return 1 ;
}

 
// ---------------------------------------------------------------------------
int DiagdfCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat, int g , int r){

	int count ,count_g,count_r ;

	if(!pcaOutMat)
		return 0 ;

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;

	if( g ==0 || r == 0)
		return 0 ;

	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(1){
		if(count >= caInMat.ha_numValue){
			break ;
		}
		if (count_g == count_r){
			pcaOutMat->da_cmpValue[count].re = (double)caInMat.ha_cmpValue[count].re ;
			pcaOutMat->da_cmpValue[count].im = (double)caInMat.ha_cmpValue[count].im ;
		}
		else{
			pcaOutMat->da_cmpValue[count].re = (double)0.0 ;
			pcaOutMat->da_cmpValue[count].im = (double)0.0;
		}
		count++ ;
		count_g ++ ;

		if(count_g == g){
			count_g = 0 ;
			count_r ++ ;
		}
	}
	return 1 ;
}

 
// ---------------------------------------------------------------------------

int ConjTransdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,int g,int r){
	
	int index,count_g,count_r ;
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;
	
	index = 0 ;
	count_g = 0 ;
	count_r = 0 ;

	while(1){
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
	return 1 ;

}
// ---------------------------------------------------------------------------
int ConjTransdfCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r){
	
	int index,count_g,count_r ;
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;
	
	index = 0 ;
	count_g = 0 ;
	count_r = 0 ;

	while(1){
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
	return 1 ;

}
// ---------------------------------------------------------------------------

int TransdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,int g,int r)
{
	
	int index,count_g,count_r ;
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;

	index = 0 ;
	count_g = 0 ;
	count_r = 0 ;

	while(1){
		if(index >= caInMat.ha_numValue){
			break ;
		}
		
		pcaOutMat->da_cmpValue[index].re = caInMat.da_cmpValue[count_g+count_r*g].re ;
		pcaOutMat->da_cmpValue[index].im = caInMat.da_cmpValue[count_g+count_r*g].im  ;
	
		index ++ ;
		count_r ++ ;
		if(count_r >= r){
			count_r = 0 ;
			count_g ++ ;
		}
	}
	return 1 ;

}

// ---------------------------------------------------------------------------
int SumdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2){

	int index ;
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != g1*r1)
		return 0 ;

	index = 0 ;

	if(g1 != g2 || r1 != r2){ //同じサイズでなければならない
		return 0 ;
	}

	while(1){
		if(index >= caInMat1.ha_numValue){
			break ;
		}

		pcaOutMat->da_cmpValue[index].re = caInMat1.da_cmpValue[index].re + caInMat2.da_cmpValue[index].re ;
		pcaOutMat->da_cmpValue[index].im = caInMat1.da_cmpValue[index].im + caInMat2.da_cmpValue[index].im ;

		index++ ;
	}
	return 1 ;
							 
}

int SumffdCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2){

	int index ;
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != g1*r1)
		return 0 ;

	index = 0 ;

	if(g1 != g2 || r1 != r2){ //同じサイズでなければならない
		return 0 ;
	}

	while(1){
		if(index >= caInMat1.ha_numValue){
			break ;
		}

		pcaOutMat->ha_cmpValue[index].re = caInMat1.ha_cmpValue[index].re + caInMat2.da_cmpValue[index].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat1.ha_cmpValue[index].im + caInMat2.da_cmpValue[index].im ;

		index++ ;
	}
	return 1 ;
							 
}

 // ---------------------------------------------------------------------------
int SubdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2){

	int index ;
	index = 0 ;

	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != g1*r1)
		return 0 ;

	if(g1 != g2 || r1 != r2){ //同じサイズでなければならない
		return 0 ;
	}

	while(1){
		if(index >= caInMat1.ha_numValue){
			break ;
		}

		pcaOutMat->da_cmpValue[index].re = caInMat1.da_cmpValue[index].re - caInMat2.da_cmpValue[index].re ;
		pcaOutMat->da_cmpValue[index].im = caInMat1.da_cmpValue[index].im - caInMat2.da_cmpValue[index].im ;

		index++ ;
	}
	return 1;
							 
}

// ----------------------------------------------------------------------------
int SubfdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_dComplexArray caInMat2,int g2,int r2){

	int index ;
	index = 0 ;

	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != g1*r1)
		return 0 ;

	if(g1 != g2 || r1 != r2){ //同じサイズでなければならない
		return 0 ;
	}

	while(1){
		if(index >= caInMat1.ha_numValue){
			break ;
		}

		pcaOutMat->da_cmpValue[index].re = (double)caInMat1.ha_cmpValue[index].re - caInMat2.da_cmpValue[index].re ;
		pcaOutMat->da_cmpValue[index].im = (double)caInMat1.ha_cmpValue[index].im - caInMat2.da_cmpValue[index].im ;

		index++ ;
	}
	return 1;
							 
}

// ----------------------------------------------------------------------------

 int MultipldCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,double a){

	int index ;

	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;
	index = 0 ;

	while(1){
		if(index >= caInMat.ha_numValue){
			break ;
		}

		pcaOutMat->da_cmpValue[index].re = caInMat.da_cmpValue[index].re * a ;
		pcaOutMat->da_cmpValue[index].im = caInMat.da_cmpValue[index].im * a ;

		index++ ;
	}
	return 1;
							 
}	
//-------------------------------------------------------------------------------
int EyedCompMat1(MICARY_dComplexArray *pcaOutMat,int g,int r){

	int n,min_n ,ind;
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != g*r)
		return 0 ;
	
	if(g >=r)
		min_n = r ;
	else
		min_n = g ;
	for(n = 0; n < min_n;n++){
		ind = n*min_n+n ;
		pcaOutMat->da_cmpValue[ind].re = 1.0 ;
	}
	
	return 1 ;
}
// ------------------------------------------------------------
// 2007/10/23
int EyedCompMat1v2(MICARY_dComplexArray *pcaOutMat,int g,int r){
	
	int count ,count_g,count_r ;
	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(1){
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

	return 1 ;
}

// ----------------------------------------------------------------------------------
int AbsdCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat) 
{

	int i ;
	double fTmp ;
	if(!pcaOutMat){
		return 0 ;
	}

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue){
		return 0 ;
	}

	for(i = 0 ; i < caInMat.ha_numValue ; i++){
		fTmp = pow(caInMat.da_cmpValue[i].re,2.0) + pow(caInMat.da_cmpValue[i].im,2.0) ;
		fTmp = sqrt(fTmp) ;
		pcaOutMat->da_cmpValue[i].re = fTmp ;
		pcaOutMat->da_cmpValue[i].im = 0.0 ;
	}

	return 1 ;
}
// ----------------------------------------------------------------------------------
int AbsdfCompMat1(MICARY_dComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat) 
{

	int i ;
	double fTmp ;
	if(!pcaOutMat){
		return 0 ;
	}

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue){
		return 0 ;
	}

	for(i = 0 ; i < caInMat.ha_numValue ; i++){
		fTmp = pow((double)caInMat.ha_cmpValue[i].re,2.0) + pow((double)caInMat.ha_cmpValue[i].im,2.0) ;
		fTmp = sqrt(fTmp) ;
		pcaOutMat->da_cmpValue[i].re = fTmp ;
		pcaOutMat->da_cmpValue[i].im = 0.0 ;
	}

	return 1 ;
}
// ----------------------------------------------------------------------------------

double MaxdCompMat1(MICARY_dComplexArray caInMat)
{
	int i ;
	double fTmp ;

	fTmp = caInMat.da_cmpValue[0].re ;
	for(i = 0 ; i < caInMat.ha_numValue ;i++){
		if(fTmp < caInMat.da_cmpValue[i].re){
			fTmp = caInMat.da_cmpValue[i].re ;

		}
	}
	return(fTmp) ;
}
// ----------------------------------------------------------------------------------
int DotcdCompMat1(MICARY_dComplexArray *pcaOut ,MICARY_dComplexArray caIn1 ,MICARY_dComplexArray caIn2) 
{
	int i ;
	if(!pcaOut){
		return 0 ;
	}
	if(caIn1.ha_numValue != caIn2.ha_numValue){
		return 0 ;
	}
	for(i = 0 ; i < pcaOut->ha_numValue ; i++){
		pcaOut->da_cmpValue[i].re = caIn1.da_cmpValue[i].re * caIn2.da_cmpValue[i].re - (caIn1.da_cmpValue[i].im * caIn2.da_cmpValue[i].im ) ;
		pcaOut->da_cmpValue[i].im = caIn1.da_cmpValue[i].re * caIn2.da_cmpValue[i].im + (caIn1.da_cmpValue[i].im * caIn2.da_cmpValue[i].re ) ;
	}

	return 1 ;
}

// ----------------------------------------------------------------------------
 int MultiplfdCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_dComplexArray caInMat,double a){

	int index ;
	double DMAX ;
	DMAX = 3.4028E+038 ;

	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;
	index = 0 ;

	while(1){
		if(index >= caInMat.ha_numValue){
			break ;
		}
		if(caInMat.da_cmpValue[index].re * a > DMAX 
				&& caInMat.da_cmpValue[index].im * (double)a > DMAX){
			printf("Data is deverged !! \n") ;
			return 0 ;
		}
		pcaOutMat->ha_cmpValue[index].re = (float)(caInMat.da_cmpValue[index].re * a) ;
		pcaOutMat->ha_cmpValue[index].im = (float)(caInMat.da_cmpValue[index].im * a) ;

		index++ ;
	}
	return 1;
							 
}	
//-------------------------------------------------------------------------------
// 1dim.
int DividedCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1,int g1,int r1,
									MICARY_dComplexArray caInMat2,int g2,int r2){
 

	int i ;
	double dTmp ;

	if(!pcaOutMat){
		return 0 ;
	}
	if(caInMat1.ha_numValue != caInMat2.ha_numValue){
		return 0 ;
	}
	for(i = 0 ; i < pcaOutMat->ha_numValue ; i++){
		dTmp = pow(caInMat2.da_cmpValue[i].re,2.0) + pow(caInMat2.da_cmpValue[i].im,2.0) ;
		if(dTmp == 0){
		  dTmp = 0.0000001 ;
		}
		pcaOutMat->da_cmpValue[i].re = (caInMat1.da_cmpValue[i].re * caInMat2.da_cmpValue[i].re + caInMat1.da_cmpValue[i].im * caInMat2.da_cmpValue[i].im )/dTmp ;

		pcaOutMat->da_cmpValue[i].im = (caInMat1.da_cmpValue[i].im * caInMat2.da_cmpValue[i].re - (caInMat1.da_cmpValue[i].re * caInMat2.da_cmpValue[i].im)) /dTmp ;
	}

	return 1;
 }

int SqrtdCompMat1(MICARY_dComplexArray *pcaOutMat,MICARY_dComplexArray caInMat1) {

	int i ;
	double r , theta ;

	for(i = 0 ; i < caInMat1.ha_numValue ; i++){
		r = pow(caInMat1.da_cmpValue[i].re,2.0) + pow(caInMat1.da_cmpValue[i].im,2.0) ;
		r = sqrt(r) ;
		// 2008.9.1 
		// zero divide
		if(caInMat1.da_cmpValue[i].re != 0.0){
		  theta = atan(caInMat1.da_cmpValue[i].im/caInMat1.da_cmpValue[i].re) ;
		}
		else{
		  theta = 0.0 ;
		}
		// 2008.9.1
		pcaOutMat->da_cmpValue[i].re = sqrt(r)*cos(theta/2.0) ;
		pcaOutMat->da_cmpValue[i].im = sqrt(r)*sin(theta/2.0) ;

	}

	return 1 ;
}

