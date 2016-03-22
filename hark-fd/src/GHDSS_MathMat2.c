// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file	GHDSS_MathMat2.c
 \brief	\~English Source file for Matrix operation. (GHDSS program)
 \brief	\~Japanese 行列演算のためのソースファイル(GHDSS program)
 \author	Sachiko Suzuki
 \date	23 Oct 2007
 \version	$Id: GHDSS_MathMat2.c 456 2009-04-09 02:24:03Z suzuki $
*/

// ---------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ArrayInit.h"
#include "MICARY_struct.h"
#include "GHDSS_MathMat2.h"
// ---------------------------------------------------------------------
int ProductCompMat(MICARY_ComplexArray2 *pcaOutMat,MICARY_ComplexArray2 caInMat1,
								 MICARY_ComplexArray2 caInMat2){
 
 int igyou1 ,iretsu1,igyou2 ,iretsu2 ;									 
 HA_Complex cMat ;
 if(!pcaOutMat)
	 return 0;
 //matrixのサイズが合わなければ計算できない
 if(pcaOutMat->ha_numValue1 != caInMat1.ha_numValue1 || pcaOutMat->ha_numValue2 !=caInMat2.ha_numValue2)
	return 0;

 for(igyou1 = 0 ; igyou1 < caInMat1.ha_numValue1 ; igyou1 ++){
	 for(iretsu1 = 0 ; iretsu1< caInMat1.ha_numValue2 ; iretsu1 ++){
		 for(iretsu2 = 0 ;iretsu2 < caInMat2.ha_numValue2 ; iretsu2 ++){
			for (igyou2 = 0 ; igyou2 < caInMat2.ha_numValue1 ; igyou2 ++){
				cMat.re = (caInMat1.ha_cmpValue[igyou1][iretsu1].re * caInMat2.ha_cmpValue[igyou2][iretsu2].re)
				 -(caInMat1.ha_cmpValue[igyou1][iretsu1].im*caInMat2.ha_cmpValue[igyou2][iretsu2].im) ;

				cMat.im = (caInMat1.ha_cmpValue[igyou1][iretsu1].re * caInMat2.ha_cmpValue[igyou2][iretsu2].im)
				 +(caInMat1.ha_cmpValue[igyou1][iretsu1].im*caInMat2.ha_cmpValue[igyou2][iretsu2].re) ;
				pcaOutMat->ha_cmpValue[igyou1][iretsu2].re += cMat.re ;
				pcaOutMat->ha_cmpValue[igyou1][iretsu2].im += cMat.im ;
			}
		 }
	 }
 }
return 1 ;
 } 

// ---------------------------------------------------------------------
int ProductFloatMat(MICARY_FloatArray2 *pcaOutMat,MICARY_FloatArray2 caInMat1,
								MICARY_FloatArray2 caInMat2){

	int igyou1 ,iretsu1,igyou2 ,iretsu2 ;									 
	HA_Float cMat ;
	if(!pcaOutMat)
		return 0 ;
		//matrixのサイズが合わなければ計算できない
	if(pcaOutMat->ha_numValue1 != caInMat1.ha_numValue1 || pcaOutMat->ha_numValue2 != caInMat2.ha_numValue2)
		return 0 ;

	if(caInMat1.ha_numValue2 != caInMat2.ha_numValue1){
		return 0 ;
	}

	for(igyou1 = 0 ; igyou1 < caInMat1.ha_numValue1 ; igyou1 ++){
		for(iretsu1 = 0 ; iretsu1< caInMat1.ha_numValue2 ; iretsu1 ++){
			for(iretsu2 = 0 ;iretsu2 < caInMat2.ha_numValue2 ; iretsu2 ++){
				for (igyou2 = 0 ; igyou2 < caInMat2.ha_numValue1 ; igyou2 ++){
					cMat = (caInMat1.ha_fValue[igyou1][iretsu1] * caInMat2.ha_fValue[igyou2][iretsu2]);
					pcaOutMat->ha_fValue[igyou1][iretsu2] += cMat;
				}
			}
		}
	}
	return 1 ;
 } 
 
// ---------------------------------------------------------------------
int ConjCompMat(MICARY_ComplexArray2 *pcaOutMat ,MICARY_ComplexArray2 caInMat){

	int igyou , iretsu ;
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue1 != caInMat.ha_numValue1 || pcaOutMat->ha_numValue2 != caInMat.ha_numValue2)
		return 0 ;

	for(igyou = 0 ; igyou < caInMat.ha_numValue1 ; igyou ++){
		for(iretsu = 0 ;iretsu < caInMat.ha_numValue2 ; iretsu ++){
			pcaOutMat->ha_cmpValue[igyou][iretsu].re = caInMat.ha_cmpValue[igyou][iretsu].re ;
			pcaOutMat->ha_cmpValue[igyou][iretsu].im = caInMat.ha_cmpValue[igyou][iretsu].im *(-1) ;
		}
	}
	return 1 ;

}

// ------------------------------------------------------------------------
int DiagCompMat(MICARY_ComplexArray2 *pcaOutMat,MICARY_ComplexArray2 caInMat){
	int i ;
	 
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue1 != caInMat.ha_numValue1 || pcaOutMat->ha_numValue2 != caInMat.ha_numValue2)
		return 0 ;

	if (caInMat.ha_numValue1 != caInMat.ha_numValue2){
		return 0 ;
	}

	for (i = 0 ; i < caInMat.ha_numValue1 ; i++){
		pcaOutMat->ha_cmpValue[i][i].re = caInMat.ha_cmpValue[i][i].re ;
		pcaOutMat->ha_cmpValue[i][i].im = caInMat.ha_cmpValue[i][i].im ;
	}
	return 1 ;
}
 
// -------------------------------------------------------------------------
int DiagFloatMat(MICARY_FloatArray2 *pcaOutMat,MICARY_FloatArray2 caInMat){
	int i ;
	
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue1 != caInMat.ha_numValue1 || pcaOutMat->ha_numValue2 != caInMat.ha_numValue2)
		return 0 ;

	if (caInMat.ha_numValue1 != caInMat.ha_numValue2){
		return 0 ;
	}

	for (i = 0 ; i < caInMat.ha_numValue1 ; i++){
		pcaOutMat->ha_fValue[i][i] = caInMat.ha_fValue[i][i] ;
	}
	return 1 ;
}

// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
// 1dim.

int ProductCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
									MICARY_ComplexArray caInMat2,int g2,int r2){
 

	int index_o ,index1,index2, n, m, k ; 									 
	HA_Complex cMat ;
	if(!pcaOutMat)
		 return 0 ;
	if(pcaOutMat->ha_numValue != g1*r2)
		 return 0 ;
	//ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8
	ComplexArrayInit(pcaOutMat) ;

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

	return 1;


// -----------------------------------------------------------------------
/*
 int index_o , count_g1,count_r1,count_g2,count_r2 ,count ,index1,index2;									 
 HA_Complex cMat ;
 if(!pcaOutMat)
	 return 0 ;
 if(pcaOutMat->ha_numValue != g1*r2)
	 return 0 ;

 ComplexArrayFree(pcaOutMat) ;
 ComplexArrayAlloc(pcaOutMat,g1*r2) ;	//零埋め2008/5/8

 count_g1 = 0 ;
 count_r1 = 0 ;
 count_g2 = 0 ;
 count_r2 = 0 ;
 count = 0 ;
 index_o = 0 ;
 index1 = 0 ;
 index2 = 0 ;
 
 //matrixのサイズが合わなければ計算できない
 if(r1 != g2){
	 return 0 ;
 }

 while(1){
	if(index_o >= g1*r2){
		break ;
	}
	index1 = count_g1*r1+count ;
	index2 = count*r2+count_r2 ;

	cMat.re = (caInMat1.ha_cmpValue[index1].re * caInMat2.ha_cmpValue[index2].re)
				 -(caInMat1.ha_cmpValue[index1].im*caInMat2.ha_cmpValue[index2].im) ;

	cMat.im = (caInMat1.ha_cmpValue[index1].re * caInMat2.ha_cmpValue[index2].im)
				 +(caInMat1.ha_cmpValue[index1].im*caInMat2.ha_cmpValue[index2].re) ;

	pcaOutMat->ha_cmpValue[index_o].re += cMat.re ;
	pcaOutMat->ha_cmpValue[index_o].im += cMat.im ;
	
	count++ ;
	
	if(count >= r1){
		count = 0 ;
		if(count_r2 < r2 -1){
			count_r2 ++ ;
		}
		else if(count_r2 == r2 -1){
			count_r2 = 0 ;
			count_g1 ++ ;
		}
	
		index_o ++ ;
		
	}
 }
 return 1 ;
 */
 } 

// ---------------------------------------------------------------------
int ProductFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat1,int g1,int r1,
								MICARY_FloatArray caInMat2,int g2,int r2){

	int index_o , count_g1,count_r1,count_g2,count_r2 ,count ,index1,index2;									 
	HA_Float cMat ;
	if(!pcaOutMat)
		return 0 ;

	if(pcaOutMat->ha_numValue != g1*r2)
		return 0 ;

	count_g1 = 0 ;
	count_r1 = 0 ;
	count_g2 = 0 ;
	count_r2 = 0 ;
	count = 0 ;
	index_o = 0 ;
	index1 = 0 ;
	index2 = 0 ;
 
	//matrixのサイズが合わなければ計算できない
	if(r1 != g2){
		return 0 ;
	}

	while(1){
		if(index_o >= g1*r2){
			break ;
		}
		index1 = count_g1*r1+count ;
		index2 = count*r2+count_r2 ;

		cMat = (caInMat1.ha_fValue[index1] * caInMat2.ha_fValue[index2]) ;
		pcaOutMat->ha_fValue[index_o] += cMat ;
		
		count++ ;
	
		if(count >= r1){
			count = 0 ;
			if(count_g1 < g1-1 ){
				count_g1 ++ ;
			}
			else if( count_g1 == g1-1){
				count_g1 = 0 ;
				count_r2 ++ ;
			}
			index_o ++ ;
		
		}
	}
	return 1;
 } 

 
// ---------------------------------------------------------------------
int ConjCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat){
	int index ;
	
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0;

	for(index = 0 ; index < caInMat.ha_numValue ; index ++){
		pcaOutMat->ha_cmpValue[index].re = caInMat.ha_cmpValue[index].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat.ha_cmpValue[index].im * (-1) ;
	}
	return 1;

}

// ------------------------------------------------------------------------
int DiagCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat, int g , int r){

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
	return 1 ;
}

 
// -------------------------------------------------------------------------
int DiagFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat,int g,int r){

	int count ,count_g,count_r ;
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;

	if(g != r)
		return 0 ;

	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;
	
	while(1){
		if(count >= caInMat.ha_numValue){
			break ;
		}
		if (count_g == count_r){
			pcaOutMat->ha_fValue[count] = caInMat.ha_fValue[count] ;
			pcaOutMat->ha_fValue[count] = caInMat.ha_fValue[count] ;
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
int ConjTransCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r){
	
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
		
		pcaOutMat->ha_cmpValue[index].re = caInMat.ha_cmpValue[count_g+count_r*g].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat.ha_cmpValue[count_g+count_r*g].im *(-1)  ;
	
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
int TransCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,int g,int r)
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
		
		pcaOutMat->ha_cmpValue[index].re = caInMat.ha_cmpValue[count_g+count_r*g].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat.ha_cmpValue[count_g+count_r*g].im  ;
	
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
int SumCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_ComplexArray caInMat2,int g2,int r2){

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

		pcaOutMat->ha_cmpValue[index].re = caInMat1.ha_cmpValue[index].re + caInMat2.ha_cmpValue[index].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat1.ha_cmpValue[index].im + caInMat2.ha_cmpValue[index].im ;

		index++ ;
	}
	return 1 ;
							 
}

// ----------------------------------------------------------------------------
 int SumFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat1,int g1,int r1,
									MICARY_FloatArray caInMat2,int g2,int r2){

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

		pcaOutMat->ha_fValue[index] = caInMat1.ha_fValue[index] + caInMat2.ha_fValue[index] ;
		index++ ;
	}
	return 1 ;
							 
}
 // ---------------------------------------------------------------------------
int SubCompMat1(MICARY_ComplexArray *pcaOutMat,MICARY_ComplexArray caInMat1,int g1,int r1,
							 MICARY_ComplexArray caInMat2,int g2,int r2){

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

		pcaOutMat->ha_cmpValue[index].re = caInMat1.ha_cmpValue[index].re - caInMat2.ha_cmpValue[index].re ;
		pcaOutMat->ha_cmpValue[index].im = caInMat1.ha_cmpValue[index].im - caInMat2.ha_cmpValue[index].im ;

		index++ ;
	}
	return 1;
							 
}

// ----------------------------------------------------------------------------
 int SubFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat1,int g1,int r1,
									MICARY_FloatArray caInMat2,int g2,int r2){

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

		pcaOutMat->ha_fValue[index] = caInMat1.ha_fValue[index] - caInMat2.ha_fValue[index] ;
		index++ ;
	}
	return 1 ;
							 
}
 // ---------------------------------------------------------------------------
int MultiplCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat,float a){

	int index ;
	//	printf("alpha = %10.10f\n",a) ;
	
	/*
	printf("a: %e\n", a);
	 {
			    int i;
			    printf("Multi W: ");
			    for (i = 0; i < caInMat.ha_numValue; i++) {
				printf("%e ", caInMat.ha_cmpValue[i].re);
			    }
			    printf("\n");
			    }
	*/
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != caInMat.ha_numValue)
		return 0 ;
	index = 0 ;
	/*
	while(1){
		if(index >= caInMat.ha_numValue){
			break ;
		}
	*/
	//printf("alpha = %10.10f\n",a) ;

	for(index = 0 ; index < caInMat.ha_numValue ;index++){
	  /*
	  printf("ind = %d \n",index) ;
	  printf("Before in = %10.10f , %10.10f / ",caInMat.ha_cmpValue[index].re , caInMat.ha_cmpValue[index].im) ;
	  printf("out = %10.10f , %10.10f /",pcaOutMat->ha_cmpValue[index].re , pcaOutMat->ha_cmpValue[index].im) ;
	  printf("alpha = %f \n",a) ;
	  */
	 
		pcaOutMat->ha_cmpValue[index].re = caInMat.ha_cmpValue[index].re * a ;
		pcaOutMat->ha_cmpValue[index].im = caInMat.ha_cmpValue[index].im * a ;
		 // 2010.3.18
		/*
	  printf("After in = %10.10f , %10.10f / ",caInMat.ha_cmpValue[index].re , caInMat.ha_cmpValue[index].im) ;
	  printf("out = %10.10f , %10.10f /",pcaOutMat->ha_cmpValue[index].re , pcaOutMat->ha_cmpValue[index].im) ;
	  printf("alpha = %f \n",a) ;
	  // end 2010.3.18
	  */
		//	index++ ;
	}
	return 1;
							 
}	
// ----------------------------------------------------------------------------
int MultiplFloatMat1(MICARY_FloatArray *pcaOutMat ,MICARY_FloatArray caInMat,float a){
	
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

		pcaOutMat->ha_fValue[index] = caInMat.ha_fValue[index] * a ;
		
		index++ ;
	}
	return 1;
							 
}
//-------------------------------------------------------------------------------
int EyeCompMat1(MICARY_ComplexArray *pcaOutMat,int g,int r){

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
		pcaOutMat->ha_cmpValue[ind].re = 1.0 ;
	}
	
/*
	int count ,count_g,count_r ;
	MICARY_ComplexArray caOutMat ;
	ComplexArrayAlloc(&caOutMat,g*r) ;

	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(1){
		if(count >= g*r){
			break ;
		}
		if (count_g == count_r){
			caOutMat.ha_cmpValue[count].re = 1 ;
		}
		count++ ;
		count_g ++ ;

		if(count_g == g){
			count_g = 0 ;
			count_r ++ ;
		}
	}
*/
	return 1 ;
}
// ------------------------------------------------------------
// 2007/10/23
int EyeCompMat1v2(MICARY_ComplexArray *pcaOutMat,int g,int r){
	
	int count ,count_g,count_r ;
	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(1){
		if(count >= g*r){
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

	/*
	int n,min_n ,ind;
	if(!caOutMat)
		return 0 ;
	if(caOutMat->ha_numValue != g*r)
		return 0 ;
	
	if(g >=r)
		min_n = r ;
	else
		min_n = g ;
	for(n = 0; n < min_n;n++){
		ind = n*min_n+n ;
		caOutMat->ha_cmpValue[ind].re = 1.0 ;
	}
	*/
	return 1 ;
}
  // -----------------------------------------------------------------------------
int EyeFloatMat1(MICARY_FloatArray *pcaOutMat ,	int g,int r){

	int count ,count_g,count_r ;
	
	if(!pcaOutMat)
		return 0 ;
	if(pcaOutMat->ha_numValue != g*r)
		return 0 ;
	
	count_g = 0 ;
	count_r = 0 ;
	count = 0 ;

	while(1){
		if(count >= g*r){
			break ;
		}
		if (count_g == count_r){
			pcaOutMat->ha_fValue[count] = 1 ;
		}
		count++ ;
		count_g ++ ;

		if(count_g == g){
			count_g = 0 ;
			count_r ++ ;
		}
	}

	return 1;
}

// ----------------------------------------------------------------------------------
int AbsCompMat1(MICARY_ComplexArray *pcaOutMat ,MICARY_ComplexArray caInMat) 
{

	int i ;
	HA_Float fTmp ;
	if(!pcaOutMat){
		return 0 ;
	}

	if(pcaOutMat->ha_numValue != caInMat.ha_numValue){
		return 0 ;
	}

	for(i = 0 ; i < caInMat.ha_numValue ; i++){
		fTmp = pow(caInMat.ha_cmpValue[i].re,2.0) + pow(caInMat.ha_cmpValue[i].im,2.0) ;
		fTmp = sqrt(fTmp) ;
		pcaOutMat->ha_cmpValue[i].re = fTmp ;
		pcaOutMat->ha_cmpValue[i].im = 0.0 ;
	}

	return 1 ;
}
// ----------------------------------------------------------------------------------
HA_Float MaxCompMat1(MICARY_ComplexArray caInMat)
{
	int i ;
	HA_Float fTmp ;

	fTmp = caInMat.ha_cmpValue[0].re ;
	for(i = 0 ; i < caInMat.ha_numValue ;i++){
		if(fTmp > caInMat.ha_cmpValue[i].re){
			fTmp = caInMat.ha_cmpValue[i].re ;
		}
	}
	return(fTmp) ;
}
// ----------------------------------------------------------------------------------
int DotcCompMat1(MICARY_ComplexArray *pcaOut ,MICARY_ComplexArray caIn1 ,MICARY_ComplexArray caIn2) 
{
	int i ;
	if(!pcaOut){
		return 0 ;
	}
	if(caIn1.ha_numValue != caIn2.ha_numValue){
		return 0 ;
	}
	for(i = 0 ; i < pcaOut->ha_numValue ; i++){
		pcaOut->ha_cmpValue[i].re = caIn1.ha_cmpValue[i].re * caIn2.ha_cmpValue[i].re - (caIn1.ha_cmpValue[i].im * caIn2.ha_cmpValue[i].im ) ;
		pcaOut->ha_cmpValue[i].im = caIn1.ha_cmpValue[i].re * caIn2.ha_cmpValue[i].im + (caIn1.ha_cmpValue[i].im * caIn2.ha_cmpValue[i].re ) ;
	}

	return 1 ;
}


