// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2011 Honda Research Institute Japan, Co., Ltd.

 \file ArrayInit.cpp
 \brief \~English Source file for initializing arrays (c++)(Microphone Array System)
 \brief \~Japanese 配列を含む構造体を初期化するソースファイル(c++)(Microphone Array System)
 \author Natsu Tanaka
 \date 16 Nov 2011
 \version $Id: ArrayInit.c 3538 2012-02-29 07:25:35Z tanaka $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>	// calloc, free
#include <string.h>// 2008/04/25 added by N.Tanaka
// ---------------------------------------------------------------------
#include "ArrayInit.h"
// ---------------------------------------------------------------------

void FloatArrayAlloc(MICARY_FloatArray * array, int iArraySize){

	if (!array){
		return;
	}
	
	if (iArraySize == 0){
		array->ha_fValue = NULL;
		array->ha_numValue = 0;		
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved = 0;
#endif
		return;
	}
/*
	if (array->ha_fValue)	{
		FloatArrayFree(array);
	}
*/	
	array->ha_fValue = (HA_Float *)calloc(iArraySize, sizeof(HA_Float));
	array->ha_numValue = iArraySize;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = iArraySize;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void FloatArrayAlloc_r(MICARY_FloatArray * array, int iArraySize, int iReservedSize)
{
	if (!array){
		return;
	}
	
	if (iArraySize == 0 && iReservedSize == 0){
		array->ha_fValue = NULL;
		array->ha_numValue = 0;		
		array->ha_numReserved = 0;
		return;
	}

	if (iArraySize > iReservedSize){
		iReservedSize = iArraySize;
	}
	
	if (array->ha_numReserved < iReservedSize){
		array->ha_fValue = (HA_Float *)calloc(iReservedSize, sizeof(HA_Float));
		array->ha_numReserved = iReservedSize;
	}
	
	array->ha_numValue = iArraySize;
}
#endif
// ---------------------------------------------------------------------

void FloatArrayFree(MICARY_FloatArray * array){
	if (!array){
		return;
	}
	
	if (array->ha_fValue){
		free(array->ha_fValue);
	}
	array->ha_fValue = NULL;
	array->ha_numValue = 0;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = 0;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void FloatArrayClear(MICARY_FloatArray * array)
{
	if (!array){
		return;
	}
	
	array->ha_numValue = 0;
}
#endif
// ---------------------------------------------------------------------

void IntArrayAlloc(MICARY_IntArray * array, int iArraySize){
	if (!array){
		return;
	}
	
	if (iArraySize == 0){
		array->ha_iValue = NULL;
		array->ha_numValue = 0;		
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved = 0;
#endif
		return;
	}
/*
	if (array->ha_iValue){
		IntArrayFree(array);
	}
*/	
	array->ha_iValue = (HA_Integer *)calloc(iArraySize, sizeof(HA_Integer));
	array->ha_numValue = iArraySize;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = iArraySize;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void IntArrayAlloc_r(MICARY_IntArray * array, int iArraySize, int iReservedSize)
{
	if (!array){
		return;
	}
	
	if (iArraySize == 0 && iReservedSize == 0){
		array->ha_iValue = NULL;
		array->ha_numValue = 0;		
		array->ha_numReserved = 0;
		return;
	}

	if (iArraySize > iReservedSize){
		iReservedSize = iArraySize;
	}
	
	if (array->ha_numReserved < iReservedSize){
		array->ha_iValue = (HA_Integer *)calloc(iReservedSize, sizeof(HA_Integer));
		array->ha_numReserved = iReservedSize;
	}
	
	array->ha_numValue = iArraySize;
}
#endif
// ---------------------------------------------------------------------

void IntArrayFree(MICARY_IntArray * array){
	if (!array){
		return;
	}
	
	if (array->ha_iValue){
		free(array->ha_iValue);
	}
	array->ha_iValue = NULL;
	array->ha_numValue = 0;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = 0;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void IntArrayClear(MICARY_IntArray * array)
{
	if (!array){
		return;
	}
	
	array->ha_numValue = 0;
}
#endif
// ---------------------------------------------------------------------
void ComplexArrayAlloc(MICARY_ComplexArray * array, int iArraySize){
	if (!array){
		return;
	}
	
	if (iArraySize == 0){
		array->ha_cmpValue = NULL;
		array->ha_numValue = 0;		
// 2011/11/16 added by N.Tanaka
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
	array->ha_cmpValue = (HA_Complex *)calloc(iArraySize, sizeof(HA_Complex));
	array->ha_numValue = iArraySize;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = iArraySize;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void ComplexArrayAlloc_r(MICARY_ComplexArray * array, int iArraySize, int iReservedSize)
{
	if (!array){
		return;
	}
	
	if (iArraySize == 0 && iReservedSize == 0){
		array->ha_cmpValue = NULL;
		array->ha_numValue = 0;		
		array->ha_numReserved = 0;
		return;
	}

	if (iArraySize > iReservedSize){
		iReservedSize = iArraySize;
	}
	
	if (array->ha_numReserved < iReservedSize){
		array->ha_cmpValue = (HA_Complex *)calloc(iReservedSize, sizeof(HA_Complex));
		array->ha_numReserved = iReservedSize;
	}
	
	array->ha_numValue = iArraySize;
}
#endif
// ---------------------------------------------------------------------

void ComplexArrayFree(MICARY_ComplexArray * array){
	if (!array){
		return;
	}
	
	if (array->ha_cmpValue){
		free(array->ha_cmpValue);
	}
	array->ha_cmpValue = NULL;
	array->ha_numValue = 0;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = 0;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void ComplexArrayClear(MICARY_ComplexArray * array)
{
	if (!array){
		return;
	}
	
	array->ha_numValue = 0;
}
#endif
// ---------------------------------------------------------------------

void LocationArrayAlloc(MICARY_LocationArray * array, int iArraySize){
  if (!array){
		return;
	}
	
	if (iArraySize == 0){
		array->micary_Location = NULL;
		array->ha_idLocation = NULL;
		array->ha_numLocation = 0;		
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved = 0;
#endif
		return;
	}
/*
	if (array->micary_Location || array->ha_idLocation){
		LocationArrayFree(array);
	}
*/

	array->micary_Location = (MICARY_Location *)calloc(iArraySize, sizeof(MICARY_Location));
	array->ha_idLocation = (HA_Id *)calloc(iArraySize, sizeof(HA_Id));
	array->ha_numLocation = iArraySize;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = iArraySize;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void LocationArrayAlloc_r(MICARY_LocationArray * array, int iArraySize, int iReservedSize)
{
	if (!array){
		return;
	}
	
	if (iArraySize == 0 && iReservedSize == 0){
		array->micary_Location = NULL;
		array->ha_idLocation = NULL;
		array->ha_numLocation = 0;		
		array->ha_numReserved = 0;
		return;
	}

	if (iArraySize > iReservedSize){
		iReservedSize = iArraySize;
	}
	
	if (array->ha_numReserved < iReservedSize){
		array->micary_Location = (MICARY_Location *)calloc(iReservedSize, sizeof(MICARY_Location));
		array->ha_idLocation = (HA_Id *)calloc(iReservedSize, sizeof(HA_Id));
		array->ha_numReserved = iReservedSize;
	}
	
	array->ha_numLocation = iArraySize;
}
#endif
// ---------------------------------------------------------------------

void LocationArrayFree(MICARY_LocationArray * array){
	if (!array){
		return;
	}
	
	if (array->micary_Location){
		free(array->micary_Location);
	}
	if (array->ha_idLocation){
		free(array->ha_idLocation);
	}
	array->micary_Location = NULL;
	array->ha_idLocation = NULL;
	array->ha_numLocation = 0;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = 0;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void LocationArrayClear(MICARY_LocationArray * array)
{
	if (!array){
		return;
	}
	
	array->ha_numLocation = 0;
}
#endif
// ---------------------------------------------------------------------

void BFDatabaseAlloc(MICARY_BFDatabase * BFD, int iFreqSize, int iLocSize, int iMicSize){
	if (!BFD){
		return;
	}
	
	if (iFreqSize * iLocSize * iMicSize == 0){
		ComplexArrayAlloc(&(BFD->micary_cmpaBF), 0);
		IntArrayAlloc(&(BFD->micary_iaFreqLine), 0);
		LocationArrayAlloc(&(BFD->micary_locaSource), 0);
		LocationArrayAlloc(&(BFD->micary_locaMic), 0);
		//BFD->ha_numMic = 0;
		return;
	}
/*	
	if ((BFD->micary_cmpaBF.ha_cmpValue) || (BFD->micary_iaFreqLine.ha_iValue) || (BFD->micary_locaSource.micary_Location)){
		BFDatabaseFree(BFD);
	}
*/	
	ComplexArrayAlloc(&(BFD->micary_cmpaBF), iFreqSize * iLocSize * iMicSize);
	IntArrayAlloc(&(BFD->micary_iaFreqLine), iFreqSize);
	LocationArrayAlloc(&(BFD->micary_locaSource), iLocSize);
	LocationArrayAlloc(&(BFD->micary_locaMic), iMicSize);
	
	//BFD->ha_numMic = iMicSize;
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void BFDatabaseAlloc_r(MICARY_BFDatabase * BFD, int iFreqSize, int iLocSize, int iMicSize, 
											  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize)
{
	if (!BFD){
		return;
	}
	
	if (iFreqSize * iLocSize * iMicSize == 0 &&
		iReservedFreqSize * iReservedLocSize * iReservedMicSize == 0){
		ComplexArrayAlloc(&(BFD->micary_cmpaBF), 0);
		IntArrayAlloc(&(BFD->micary_iaFreqLine), 0);
		LocationArrayAlloc(&(BFD->micary_locaSource), 0);
		LocationArrayAlloc(&(BFD->micary_locaMic), 0);
		//BFD->ha_numMic = 0;
		return;
	}
	ComplexArrayAlloc_r(&(BFD->micary_cmpaBF), iFreqSize * iLocSize * iMicSize, 
											 iReservedFreqSize * iReservedLocSize * iReservedMicSize);
	IntArrayAlloc_r(&(BFD->micary_iaFreqLine), iFreqSize, iReservedFreqSize);
	LocationArrayAlloc_r(&(BFD->micary_locaSource), iLocSize, iReservedLocSize);
	LocationArrayAlloc_r(&(BFD->micary_locaMic), iMicSize, iReservedMicSize);
}
#endif
// ---------------------------------------------------------------------

void BFDatabaseFree(MICARY_BFDatabase * BFD){
	if (!BFD){
		return;
	}

	ComplexArrayFree(&(BFD->micary_cmpaBF));
	IntArrayFree(&(BFD->micary_iaFreqLine));
	LocationArrayFree(&(BFD->micary_locaSource));
	LocationArrayFree(&(BFD->micary_locaMic));
	//BFD->ha_numMic = 0;
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void BFDatabaseClear(MICARY_BFDatabase * BFD)
{
	if (!BFD){
		return;
	}

	ComplexArrayClear(&(BFD->micary_cmpaBF));
	IntArrayClear(&(BFD->micary_iaFreqLine));
	LocationArrayClear(&(BFD->micary_locaSource));
	LocationArrayClear(&(BFD->micary_locaMic));
}
#endif
// ---------------------------------------------------------------------

void TFDatabaseAlloc(MICARY_TFDatabase * TFD, int iFreqSize, int iLocSize, int iMicSize){
	if (!TFD){
		return;
	}
	
	if (iFreqSize * iLocSize * iMicSize == 0){
		ComplexArrayAlloc(&(TFD->micary_cmpaTF), 0);
		IntArrayAlloc(&(TFD->micary_iaFreqLine), 0);
		LocationArrayAlloc(&(TFD->micary_locaSource), 0);
		LocationArrayAlloc(&(TFD->micary_locaMic), 0);
		//TFD->ha_numMic = 0;
		return;
	}
/*	
	if ((TFD->micary_cmpaTF.ha_cmpValue) || (TFD->micary_iaFreqLine.ha_iValue) || (TFD->micary_locaSource.micary_Location)){
		TFDatabaseFree(TFD);
	}
*/	
	ComplexArrayAlloc(&(TFD->micary_cmpaTF), iFreqSize * iLocSize * iMicSize);
	IntArrayAlloc(&(TFD->micary_iaFreqLine), iFreqSize);
	LocationArrayAlloc(&(TFD->micary_locaSource), iLocSize);
	LocationArrayAlloc(&(TFD->micary_locaMic), iMicSize);
	
	//TFD->ha_numMic = iMicSize;
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void TFDatabaseAlloc_r(MICARY_TFDatabase * TFD, int iFreqSize, int iLocSize, int iMicSize,
											  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize)
{
	if (!TFD){
		return;
	}
	
	if (iFreqSize * iLocSize * iMicSize == 0 &&
		iReservedFreqSize * iReservedLocSize * iReservedMicSize == 0){
		ComplexArrayAlloc(&(TFD->micary_cmpaTF), 0);
		IntArrayAlloc(&(TFD->micary_iaFreqLine), 0);
		LocationArrayAlloc(&(TFD->micary_locaSource), 0);
		LocationArrayAlloc(&(TFD->micary_locaMic), 0);
		return;
	}
	ComplexArrayAlloc_r(&(TFD->micary_cmpaTF), iFreqSize * iLocSize * iMicSize,
											 iReservedFreqSize * iReservedLocSize * iReservedMicSize);
	IntArrayAlloc_r(&(TFD->micary_iaFreqLine), iFreqSize, iReservedFreqSize);
	LocationArrayAlloc_r(&(TFD->micary_locaSource), iLocSize, iReservedLocSize);
	LocationArrayAlloc_r(&(TFD->micary_locaMic), iMicSize, iReservedMicSize);
}
#endif
// ---------------------------------------------------------------------

void TFDatabaseFree(MICARY_TFDatabase * TFD){
	if (!TFD){
		return;
	}

	// 2008/09/16 changed by N.Tanaka ???
	/*
	if(!&TFD->micary_cmpaTF){
		ComplexArrayFree(&(TFD->micary_cmpaTF));
	}
	if(!&TFD->micary_iaFreqLine){
		IntArrayFree(&(TFD->micary_iaFreqLine));
	}
	if(! &TFD->micary_locaSource){
		LocationArrayFree(&(TFD->micary_locaSource));
	}
	if(! &TFD->micary_locaMic){
		LocationArrayFree(&(TFD->micary_locaMic));
	}
	//TFD->ha_numMic = 0;
	*/
	ComplexArrayFree(&(TFD->micary_cmpaTF));
	IntArrayFree(&(TFD->micary_iaFreqLine));
	LocationArrayFree(&(TFD->micary_locaSource));
	LocationArrayFree(&(TFD->micary_locaMic));
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void TFDatabaseClear(MICARY_TFDatabase * TFD)
{
	if (!TFD){
		return;
	}

	ComplexArrayClear(&(TFD->micary_cmpaTF));
	IntArrayClear(&(TFD->micary_iaFreqLine));
	LocationArrayClear(&(TFD->micary_locaSource));
	LocationArrayClear(&(TFD->micary_locaMic));
}
#endif
// ---------------------------------------------------------------------

void IntArray2Alloc(MICARY_IntArray2 * array, int iArraySize1, int iArraySize2){

	int i;
	
	if (!array){
		return;
	}
	
	if (iArraySize1 == 0 || iArraySize2 == 0){
		array->ha_iValue = NULL;
		array->ha_numValue1 = 0;		
		array->ha_numValue2 = 0;		
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved1 = 0;		
		array->ha_numReserved2 = 0;		
#endif
		return;
	}
/*
	if (array->ha_iValue){
		IntArrayFree(array);
	}
*/	
	array->ha_iValue = (HA_Integer **)calloc(iArraySize1, sizeof(HA_Integer*));
	for (i = 0 ; i < iArraySize1 ; i++){
		array->ha_iValue[i] = (HA_Integer *)calloc(iArraySize2, sizeof(HA_Integer));
	}
	array->ha_numValue1 = iArraySize1;
	array->ha_numValue2 = iArraySize2;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved1 = iArraySize1;		
	array->ha_numReserved2 = iArraySize2;		
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void IntArray2Alloc_r(MICARY_IntArray2 * array, int iArraySize1, int iArraySize2,
											  int iReservedSize1, int iReservedSize2)
{
	int i;
	
	if (!array){
		return;
	}
	
	if (iArraySize1 == 0 || iArraySize2 == 0 ||
		iReservedSize1 == 0 || iReservedSize2 == 0){
		array->ha_iValue = NULL;
		array->ha_numValue1 = 0;		
		array->ha_numValue2 = 0;		
		array->ha_numReserved1 = 0;		
		array->ha_numReserved2 = 0;		
		return;
	}
	
	if (iArraySize1 > iReservedSize1){
		iReservedSize1 = iArraySize1;
	}
	if (iArraySize2 > iReservedSize2){
		iReservedSize2 = iArraySize2;
	}
	
	if (array->ha_numReserved1 < iReservedSize1 ||
		array->ha_numReserved2 < iReservedSize2){

		array->ha_iValue = (HA_Integer **)calloc(iReservedSize1, sizeof(HA_Integer*));
		for (i = 0 ; i < iReservedSize1 ; i++){
			array->ha_iValue[i] = (HA_Integer *)calloc(iReservedSize2, sizeof(HA_Integer));
		}
		array->ha_numReserved1 = iReservedSize1;		
		array->ha_numReserved2 = iReservedSize2;		
	}
	
	array->ha_numValue1 = iArraySize1;
	array->ha_numValue2 = iArraySize2;
}
#endif
// ---------------------------------------------------------------------

void IntArray2Free(MICARY_IntArray2 * array){
	
	int i;

	if (!array){
		return;
	}
	
	if (array->ha_iValue){
		for (i = 0 ; i < array->ha_numValue1 ; i++){
			free(array->ha_iValue[i]);
		}
		free(array->ha_iValue);
	}
	array->ha_iValue = NULL;
	array->ha_numValue1 = 0;
	array->ha_numValue2 = 0;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved1 = 0;		
	array->ha_numReserved2 = 0;		
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void IntArray2Clear(MICARY_IntArray2 * array)
{
	if (!array){
		return;
	}
	array->ha_numValue1 = 0;
	array->ha_numValue2 = 0;
}
#endif
// ---------------------------------------------------------------------
// 2006/10/12 added by N.Tanaka
void AllocComplexSpecArray(MICARY_ComplexSpecArray *pcsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex){
	
	int i, iArraySize;

	if (!pcsSpec){
		return;
	}

	iArraySize = numCh*(iUpperIndex - iLowerIndex + 1);

	if (iArraySize == 0){
		ComplexArrayAlloc(&pcsSpec->micary_cmpaSpec, 0);
		pcsSpec->ha_numCh = 0;
		pcsSpec->ha_iLowerFreq = 0;
		pcsSpec->ha_iUpperFreq = 0;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		pcsSpec->ha_numReservedChSize = 0;
		pcsSpec->ha_numReservedFreqSize = 0;
#endif
		return;
	}

	// 数が同じ場合は初期化して返す
	if (pcsSpec->micary_cmpaSpec.ha_cmpValue){
		if (pcsSpec->micary_cmpaSpec.ha_numValue == (HA_Num)iArraySize){
			for (i = 0 ; i < pcsSpec->micary_cmpaSpec.ha_numValue ; i++){
				pcsSpec->micary_cmpaSpec.ha_cmpValue[i].re = 0.0;
				pcsSpec->micary_cmpaSpec.ha_cmpValue[i].im = 0.0;
			}
			return;
		}
	}
			
	ComplexArrayAlloc(&pcsSpec->micary_cmpaSpec, iArraySize);
	pcsSpec->ha_numCh = numCh;
	pcsSpec->ha_iLowerFreq = iLowerIndex;
	pcsSpec->ha_iUpperFreq = iUpperIndex;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	pcsSpec->ha_numReservedChSize = numCh;
	pcsSpec->ha_numReservedFreqSize = iUpperIndex - iLowerIndex + 1;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void AllocComplexSpecArray_r(MICARY_ComplexSpecArray *pcsSpec,
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
		ComplexArrayAlloc(&pcsSpec->micary_cmpaSpec, 0);
		pcsSpec->ha_numCh = 0;
		pcsSpec->ha_iLowerFreq = 0;
		pcsSpec->ha_iUpperFreq = 0;
		pcsSpec->ha_numReservedChSize = 0;
		pcsSpec->ha_numReservedFreqSize = 0;
		return;
	}

	// 数が同じ場合は初期化して返す
	if (pcsSpec->micary_cmpaSpec.ha_cmpValue){
		if (pcsSpec->micary_cmpaSpec.ha_numValue == (HA_Num)iArraySize){
			for (i = 0 ; i < pcsSpec->micary_cmpaSpec.ha_numValue ; i++){
				pcsSpec->micary_cmpaSpec.ha_cmpValue[i].re = 0.0;
				pcsSpec->micary_cmpaSpec.ha_cmpValue[i].im = 0.0;
			}
			return;
		}
	}
	
	if (iReservedChSize < numCh){
		iReservedChSize = numCh;
	}
	if (iReservedFreqSize < iUpperIndex-iLowerIndex+1){
		iReservedFreqSize = iUpperIndex-iLowerIndex+1;
	}
			
	ComplexArrayAlloc_r(&pcsSpec->micary_cmpaSpec, iArraySize, iReservedSize);
	pcsSpec->ha_numCh = numCh;
	pcsSpec->ha_iLowerFreq = iLowerIndex;
	pcsSpec->ha_iUpperFreq = iUpperIndex;
	pcsSpec->ha_numReservedChSize = iReservedChSize;
	pcsSpec->ha_numReservedFreqSize = iReservedFreqSize;
}
#endif
// ---------------------------------------------------------------------
// 2006/10/12 added by N.Tanaka
void FreeComplexSpecArray(MICARY_ComplexSpecArray *pcsSpec){

	if (!pcsSpec){
		return;
	}

	ComplexArrayFree(&pcsSpec->micary_cmpaSpec);
	pcsSpec->ha_numCh = 0;
	pcsSpec->ha_iLowerFreq = 0;
	pcsSpec->ha_iUpperFreq = 0;	
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	pcsSpec->ha_numReservedChSize = 0;
	pcsSpec->ha_numReservedFreqSize = 0;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void ClearComplexSpecArray(MICARY_ComplexSpecArray *pcsSpec)
{
	if (!pcsSpec){
		return;
	}

	ComplexArrayClear(&pcsSpec->micary_cmpaSpec);
	pcsSpec->ha_numCh = 0;
	pcsSpec->ha_iLowerFreq = 0;
	pcsSpec->ha_iUpperFreq = 0;	
}
#endif
// ---------------------------------------------------------------------
// 2006/10/12 added by N.Tanaka
void AllocFloatSpecArray(MICARY_FloatSpecArray *pfsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex){

	int i, iArraySize;

	if (!pfsSpec){
		return;
	}

	iArraySize = numCh*(iUpperIndex - iLowerIndex + 1);

	if (iArraySize == 0){
		FloatArrayAlloc(&pfsSpec->micary_faSpec, 0);
		pfsSpec->ha_numCh = 0;
		pfsSpec->ha_iLowerFreq = 0;
		pfsSpec->ha_iUpperFreq = 0;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		pfsSpec->ha_numReservedChSize = 0;
		pfsSpec->ha_numReservedFreqSize = 0;
#endif
		return;
	}

	// 数が同じ場合は初期化して返す
	if (pfsSpec->micary_faSpec.ha_fValue){
		if (pfsSpec->micary_faSpec.ha_numValue == (HA_Num)iArraySize){
			for (i = 0 ; i < pfsSpec->micary_faSpec.ha_numValue ; i++){
				pfsSpec->micary_faSpec.ha_fValue[i] = 0.0;
			}
			return;
		}
	}
			
	FloatArrayAlloc(&pfsSpec->micary_faSpec, iArraySize);
	pfsSpec->ha_numCh = numCh;
	pfsSpec->ha_iLowerFreq = iLowerIndex;
	pfsSpec->ha_iUpperFreq = iUpperIndex;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	pfsSpec->ha_numReservedChSize = numCh;
	pfsSpec->ha_numReservedFreqSize = iUpperIndex - iLowerIndex + 1;
#endif

}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void AllocFloatSpecArray_r(MICARY_FloatSpecArray *pfsSpec,
						  HA_Num numCh,
						  HA_Integer iLowerIndex,
						  HA_Integer iUpperIndex,
						  int iReservedChSize,
						  int iReservedFreqSize)
{
	int i, iArraySize, iReservedSize;

	if (!pfsSpec){
		return;
	}
	
	iArraySize = numCh*(iUpperIndex - iLowerIndex + 1);
	iReservedSize = iReservedChSize * iReservedFreqSize;

	if (iArraySize == 0){
		FloatArrayAlloc(&pfsSpec->micary_faSpec, 0);
		pfsSpec->ha_numCh = 0;
		pfsSpec->ha_iLowerFreq = 0;
		pfsSpec->ha_iUpperFreq = 0;
		pfsSpec->ha_numReservedChSize = 0;
		pfsSpec->ha_numReservedFreqSize = 0;
		return;
	}

	// 数が同じ場合は初期化して返す
	if (pfsSpec->micary_faSpec.ha_fValue){
		if (pfsSpec->micary_faSpec.ha_numValue == (HA_Num)iArraySize){
			for (i = 0 ; i < pfsSpec->micary_faSpec.ha_numValue ; i++){
				pfsSpec->micary_faSpec.ha_fValue[i] = 0.0;
			}
			return;
		}
	}
	
	if (iReservedChSize < numCh){
		iReservedChSize = numCh;
	}
	if (iReservedFreqSize < iUpperIndex-iLowerIndex+1){
		iReservedFreqSize = iUpperIndex-iLowerIndex+1;
	}
			
	FloatArrayAlloc_r(&pfsSpec->micary_faSpec, iArraySize, iReservedSize);
	pfsSpec->ha_numCh = numCh;
	pfsSpec->ha_iLowerFreq = iLowerIndex;
	pfsSpec->ha_iUpperFreq = iUpperIndex;
	pfsSpec->ha_numReservedChSize = iReservedChSize;
	pfsSpec->ha_numReservedFreqSize = iReservedFreqSize;
}
#endif
// ---------------------------------------------------------------------
// 2006/10/12 added by N.Tanaka
void FreeFloatSpecArray(MICARY_FloatSpecArray *pfsSpec){

	if (!pfsSpec){
		return;
	}

	FloatArrayFree(&pfsSpec->micary_faSpec);
	pfsSpec->ha_numCh = 0;
	pfsSpec->ha_iLowerFreq = 0;
	pfsSpec->ha_iUpperFreq = 0;	
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	pfsSpec->ha_numReservedChSize = 0;
	pfsSpec->ha_numReservedFreqSize = 0;
#endif

}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void ClearFloatSpecArray(MICARY_FloatSpecArray *pfsSpec)
{
	if (!pfsSpec){
		return;
	}

	FloatArrayClear(&pfsSpec->micary_faSpec);
	pfsSpec->ha_numCh = 0;
	pfsSpec->ha_iLowerFreq = 0;
	pfsSpec->ha_iUpperFreq = 0;	
}
#endif
// ---------------------------------------------------------------------
// 2007/08/22 added by N.Tanaka
void StringAlloc(HA_String * array, int iArraySize){

  // 2008/04/25 changed by N.Tanaka
  // s : fixed length!
  /*
	if (!array){
		return;
	}
	
	if (iArraySize == 0){
		array->s = NULL;
		array->slen = 0;		
		return;
	}
/ *
	if (array->ha_fValue)	{
		FloatArrayFree(array);
	}
* /	
	array->s = (char *)calloc(iArraySize, sizeof(char));
	array->slen = iArraySize;
*/
  memset(array->s, 0, sizeof(char)*HA_STRING_MAXLEN);
}
// ---------------------------------------------------------------------
// 2007/08/22 added by N.Tanaka
void StringFree(HA_String * array){
  // 2008/04/25 deleted by N.Tanaka
  // s : fixed length
  /*
	if (!array){
		return;
	}
	
	if (array->s){
		free(array->s);
	}
	array->s = NULL;
	array->slen = 0;
  */
}
// ---------------------------------------------------------------------
