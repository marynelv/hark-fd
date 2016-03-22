// ---------------------------------------------------------------------
/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#include <stdlib.h>	// calloc, free
// ---------------------------------------------------------------------
#include "MICARY_GHDSS_struct.h"
// ---------------------------------------------------------------------

void FloatArray2Alloc(MICARY_FloatArray2 * array, int iArraySize1, int iArraySize2){

	int i;
	
	if (!array){
		return;
	}
	
	if (iArraySize1 == 0 || iArraySize2 == 0){
		array ->ha_fValue = NULL ;
		array->ha_numValue1 = 0;		
		array->ha_numValue2 = 0;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved1 = 0;		
		array->ha_numReserved2 = 0;		
#endif
		return;
	}
	array->ha_fValue = (HA_Float **)calloc(iArraySize1, sizeof(HA_Float*));
	for (i = 0 ; i < iArraySize1 ; i++){
		array->ha_fValue[i] = (HA_Float *)calloc(iArraySize2, sizeof(HA_Float));
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
void FloatArray2Alloc_r(MICARY_FloatArray2 * array, int iArraySize1, int iArraySize2,
												  int iReservedSize1, int iReservedSize2)
{
	int i;
	
	if (!array){
		return;
	}
	
	if (iArraySize1 == 0 || iArraySize2 == 0 ||
		iReservedSize1 == 0 || iReservedSize2 == 0){
		array->ha_fValue = NULL;
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

		array->ha_fValue = (HA_Float **)calloc(iReservedSize1, sizeof(HA_Float*));
		for (i = 0 ; i < iReservedSize1 ; i++){
			array->ha_fValue[i] = (HA_Float *)calloc(iReservedSize2, sizeof(HA_Float));
		}
		array->ha_numReserved1 = iReservedSize1;		
		array->ha_numReserved2 = iReservedSize2;		
	}
	
	array->ha_numValue1 = iArraySize1;
	array->ha_numValue2 = iArraySize2;
}
#endif
// ---------------------------------------------------------------------

void FloatArray2Free(MICARY_FloatArray2 * array){
	
	int i;

	if (!array){
		return;
	}
	
	if (array->ha_fValue){
		for (i = 0 ; i < array->ha_numValue1 ; i++){
			free(array->ha_fValue[i]);
		}
		free(array->ha_fValue);
	}
	array->ha_fValue = NULL;
	array->ha_numValue1 = 0;
	array->ha_numValue2 = 0;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved1 = 0;		
	array->ha_numReserved2 = 0;		
#endif
}
// ----------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void FloatArray2Clear(MICARY_FloatArray2 * array)
{
	if (!array){
		return;
	}
	array->ha_numValue1 = 0;
	array->ha_numValue2 = 0;
}
#endif
// ----------------------------------------------------------------------
void ComplexArray2Alloc(MICARY_ComplexArray2 * array, int iArraySize1, int iArraySize2){

	int i;
	
	if (!array){
		return;
	}
	
	if (iArraySize1 == 0 || iArraySize2 == 0){
		array ->ha_cmpValue = NULL ;
		array->ha_numValue1 = 0;		
		array->ha_numValue2 = 0;		
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved1 = 0;		
		array->ha_numReserved2 = 0;		
#endif
		return;
	}
	array->ha_cmpValue = (HA_Complex **)calloc(iArraySize1, sizeof(HA_Complex*));
	for (i = 0 ; i < iArraySize1 ; i++){
		array->ha_cmpValue[i] = (HA_Complex *)calloc(iArraySize2, sizeof(HA_Complex));
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
void ComplexArray2Alloc_r(MICARY_ComplexArray2 * array, int iArraySize1, int iArraySize2,
													  int iReservedSize1, int iReservedSize2) 
{
	int i;
	
	if (!array){
		return;
	}
	
	if (iArraySize1 == 0 || iArraySize2 == 0 ||
		iReservedSize1 == 0 || iReservedSize2 == 0){
		array->ha_cmpValue = NULL;
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

		array->ha_cmpValue = (HA_Complex **)calloc(iReservedSize1, sizeof(HA_Complex*));
		for (i = 0 ; i < iReservedSize1 ; i++){
			array->ha_cmpValue[i] = (HA_Complex *)calloc(iReservedSize2, sizeof(HA_Complex));
		}
		array->ha_numReserved1 = iReservedSize1;		
		array->ha_numReserved2 = iReservedSize2;		
	}
	
	array->ha_numValue1 = iArraySize1;
	array->ha_numValue2 = iArraySize2;
}
#endif
// ---------------------------------------------------------------------

void ComplexArray2Free(MICARY_ComplexArray2 * array){
	
	int i;

	if (!array){
		return;
	}
	
	if (array->ha_cmpValue){
		for (i = 0 ; i < array->ha_numValue1 ; i++){
			free(array->ha_cmpValue[i]);
		}
		free(array->ha_cmpValue);
	}
	array->ha_cmpValue = NULL;
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
void ComplexArray2Clear(MICARY_ComplexArray2 * array)
{
	if (!array){
		return;
	}
	array->ha_numValue1 = 0;
	array->ha_numValue2 = 0;
}
#endif
// ----------------------------------------------------------------------
// alloc & free
void SeparationMatAlloc(MICARY_SeparationMat * SPM, int iFreqSize, int iLocSize, int iMicSize){
	if (!SPM){
		return;
	}
	
	SPM -> ha_numInput = iMicSize ;
	SPM -> ha_numOutput = iLocSize ;
		
	if (iFreqSize * iLocSize * iMicSize == 0){
		IntArrayAlloc(&(SPM ->micary_iaFreqLinex),0) ;
		ComplexArrayAlloc(&(SPM -> micary_cmpaSeparation),0) ;
		SPM -> ha_numInput = 0 ;
		SPM -> ha_numOutput = 0 ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		SPM -> ha_numReservedInput = 0;
		SPM -> ha_numReservedOutput = 0;
#endif
		return ;
	}

	IntArrayAlloc(&(SPM ->micary_iaFreqLinex),iFreqSize) ;
	ComplexArrayAlloc(&(SPM ->micary_cmpaSeparation),iFreqSize*iLocSize*iMicSize) ;
	
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	SPM -> ha_numReservedInput = iMicSize;
	SPM -> ha_numReservedOutput = iLocSize;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void SeparationMatAlloc_r(MICARY_SeparationMat * SPM, int iFreqSize, int iLocSize, int iMicSize,
													int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) 
{
	if (!SPM){
		return;
	}
	
	
	if (iFreqSize * iLocSize * iMicSize == 0 &&
		iReservedFreqSize * iReservedLocSize * iReservedMicSize == 0){
		IntArrayAlloc(&(SPM ->micary_iaFreqLinex),0) ;
		ComplexArrayAlloc(&(SPM -> micary_cmpaSeparation),0) ;
		SPM -> ha_numInput = 0 ;
		SPM -> ha_numOutput = 0 ;
		SPM -> ha_numReservedInput = 0;
		SPM -> ha_numReservedOutput = 0;
		return ;
	}

	IntArrayAlloc_r(&(SPM ->micary_iaFreqLinex),iFreqSize, iReservedFreqSize) ;
	ComplexArrayAlloc_r(&(SPM ->micary_cmpaSeparation),iFreqSize*iLocSize*iMicSize, 
													 iReservedFreqSize*iReservedLocSize*iReservedMicSize) ;
	SPM -> ha_numInput = iMicSize ;
	SPM -> ha_numOutput = iLocSize ;
	SPM -> ha_numReservedInput = iReservedMicSize;
	SPM -> ha_numReservedOutput = iReservedLocSize;
}
#endif
// ---------------------------------------------------------------------

void SeparationMatFree(MICARY_SeparationMat * SPM){
	if (!SPM){
		return;
	}

	ComplexArrayFree(&(SPM->micary_cmpaSeparation));
	IntArrayFree(&(SPM->micary_iaFreqLinex));

// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	SPM -> ha_numInput = 0 ;
	SPM -> ha_numOutput = 0 ;
	SPM -> ha_numReservedInput = 0;
	SPM -> ha_numReservedOutput = 0;
#endif
	
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void SeparationMatClear(MICARY_SeparationMat * SPM) 
{
	if (!SPM){
		return;
	}
	SPM -> ha_numInput = 0 ;
	SPM -> ha_numOutput = 0 ;
}
#endif
// ---------------------------------------------------------------------
void SavedSeparationAlloc(MICARY_SavedSeparation * SSP ,int iFreqSize, int iLocSize, int iMicSize){
	
	//Init 2008.10.31
	int iArraySize ;
	iArraySize = iFreqSize * iLocSize * iMicSize ;
	if(iArraySize == 0){
		TFDatabaseAlloc(&(SSP->micary_tdTFDB),0,0,0) ;
		SeparationMatAlloc(&(SSP ->micary_smSeparation),0,0,0) ;
		return ;
    }

	TFDatabaseAlloc(&(SSP ->micary_tdTFDB),iFreqSize,iLocSize,iMicSize) ;
	SeparationMatAlloc(&(SSP ->micary_smSeparation),iFreqSize,iLocSize,iMicSize) ;
	
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void SavedSeparationAlloc_r(MICARY_SavedSeparation * SSP ,int iFreqSize, int iLocSize, int iMicSize,
						  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize) 
{
	TFDatabaseAlloc_r(&(SSP ->micary_tdTFDB),iFreqSize,iLocSize,iMicSize,
										   iReservedFreqSize, iReservedLocSize, iReservedMicSize) ;
	SeparationMatAlloc_r(&(SSP ->micary_smSeparation),iFreqSize,iLocSize,iMicSize,
										   iReservedFreqSize, iReservedLocSize, iReservedMicSize) ;
}
#endif
// ---------------------------------------------------------------------

void SavedSeparationFree(MICARY_SavedSeparation * SSP){
	
	if(!SSP){
		return ;
	}
	if(&(SSP->micary_tdTFDB)){
		TFDatabaseFree(&(SSP ->micary_tdTFDB)) ;
	}
	if(&(SSP->micary_smSeparation.micary_cmpaSeparation)){
		ComplexArrayFree(&(SSP->micary_smSeparation.micary_cmpaSeparation));
		IntArrayFree(&(SSP->micary_smSeparation.micary_iaFreqLinex)) ;
	}
	//SeparationMatFree(&(SSP ->micary_smSeparation)) ;

}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void SavedSeparationClear(MICARY_SavedSeparation * SSP) 
{
	if(!SSP){
		return ;
	}
	if(&(SSP->micary_tdTFDB)){
		TFDatabaseClear(&(SSP ->micary_tdTFDB)) ;
	}
	if(&(SSP->micary_smSeparation.micary_cmpaSeparation)){
		ComplexArrayClear(&(SSP->micary_smSeparation.micary_cmpaSeparation));
		IntArrayClear(&(SSP->micary_smSeparation.micary_iaFreqLinex)) ;
	}
}
#endif
// ---------------------------------------------------------------------
void LocationIdAlloc(MICARY_GHDSSLocationId * array, int iArraySize){
	if (!array){
		return;
	}
	
	if (iArraySize == 0){
		array->ha_idExistence = NULL;
		array->ha_idLocation = NULL;
		array->ha_numLocation = 0;
		array->micary_MoveFlg = 0 ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved = 0;
#endif
		return;
	}
	array ->ha_idExistence = (HA_Id *)calloc(iArraySize, sizeof(HA_Id));
	array->ha_idLocation = (HA_Id *)calloc(iArraySize, sizeof(HA_Id));
	array->ha_numLocation = iArraySize;
	array->micary_MoveFlg = 0 ;
	// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = iArraySize;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void LocationIdAlloc_r(MICARY_GHDSSLocationId * array, int iArraySize, int iReservedSize)
{
	if (!array){
		return;
	}
	
	if (iArraySize == 0){
		array->ha_idExistence = NULL;
		array->ha_idLocation = NULL;
		array->ha_numLocation = 0;
		array->micary_MoveFlg = 0 ;
		array->ha_numReserved = 0;
		return;
	}
	if (iReservedSize < iArraySize){
		iReservedSize = iArraySize;
	}
	
	array ->ha_idExistence = (HA_Id *)calloc(iReservedSize, sizeof(HA_Id));
	array->ha_idLocation = (HA_Id *)calloc(iReservedSize, sizeof(HA_Id));
	array->ha_numLocation = iArraySize;
	array->micary_MoveFlg = 0 ;
	array->ha_numReserved = iReservedSize;
}
#endif
// ---------------------------------------------------------------------

void LocationIdFree(MICARY_GHDSSLocationId * array){
	if (!array){
		return;
	}
	
	if (array->ha_idLocation){
		free(array->ha_idLocation);
	}
	if (array->ha_idExistence){
		free(array->ha_idExistence);
	}
	array->ha_idLocation = NULL;
	array->ha_idExistence = NULL ;
	array->ha_numLocation = 0;
	array->micary_MoveFlg = 0 ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = 0;
#endif
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void LocationIdClear(MICARY_GHDSSLocationId * array) 
{
	if (!array){
		return;
	}
	
	array->ha_numLocation = 0;
	array->micary_MoveFlg = 0 ;
}
#endif
// ---------------------------------------------------------------------
void FreqLineAlloc(MICARY_GHDSSFreqLine * array, int iLowerFreqLine , int iUpperFreqLine)
{
	int ind ,size ;

	if(!array){
		return  ;
	}

	if(iLowerFreqLine > iUpperFreqLine){
//		array->ha_iLowerFreq = 0 ;
//		array->ha_iUpperFreq = 0 ;
//		array->micary_iaFreqLine = NULL ;
		return  ;
	}
	size = iUpperFreqLine - iLowerFreqLine +1 ;
	// Initialize 2008.10.31
	if(size == 0){
		IntArrayAlloc(array->micary_iaFreqLine,0) ;
		array ->micary_iaFreqLine->ha_numValue = 0 ;
		array->ha_iLowerFreq = 0 ;
		array->ha_iUpperFreq = 0 ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
		array->ha_numReserved = 0;
#endif
	  return ;
	}

	array->micary_iaFreqLine = (MICARY_IntArray *)calloc(1,sizeof(MICARY_IntArray)) ;
	array->micary_iaFreqLine->ha_iValue = (HA_Integer *)calloc(size,sizeof(HA_Integer)) ;
	array ->micary_iaFreqLine->ha_numValue = size ;
	
	//IntArrayAlloc(&array->micary_iaFreqLine,size) ;

	for(ind = 0 ; ind < size ; ind ++){
		array->micary_iaFreqLine->ha_iValue[ind] = iLowerFreqLine +ind ;
	}
	array->ha_iLowerFreq = iLowerFreqLine ;
	array->ha_iUpperFreq = iUpperFreqLine ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = iUpperFreqLine - iLowerFreqLine + 1;
#endif
	
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void FreqLineAlloc_r(MICARY_GHDSSFreqLine * array, int iLowerFreqLine , int iUpperFreqLine, int iReservedSize) 
{
	int ind ,size ;

	if(!array){
		return  ;
	}

	if(iLowerFreqLine > iUpperFreqLine){
//		array->ha_iLowerFreq = 0 ;
//		array->ha_iUpperFreq = 0 ;
//		array->micary_iaFreqLine = NULL ;
		return  ;
	}
	size = iUpperFreqLine - iLowerFreqLine +1 ;
	// Initialize 2008.10.31
	if(size == 0 &&
	   iReservedSize == 0){
		IntArrayAlloc(array->micary_iaFreqLine,0) ;
		array ->micary_iaFreqLine->ha_numValue = 0 ;
		array->ha_iLowerFreq = 0 ;
		array->ha_iUpperFreq = 0 ;
		array->ha_numReserved = 0;
	  return ;
	}
	if (iReservedSize < size){
		iReservedSize = size;
	}

	array->micary_iaFreqLine = (MICARY_IntArray *)calloc(1,sizeof(MICARY_IntArray)) ;
	array->micary_iaFreqLine->ha_iValue = (HA_Integer *)calloc(size,sizeof(HA_Integer)) ;
	array ->micary_iaFreqLine->ha_numValue = size ;
	
	IntArrayAlloc_r(array->micary_iaFreqLine, size, iReservedSize) ;

	for(ind = 0 ; ind < size ; ind ++){
		array->micary_iaFreqLine->ha_iValue[ind] = iLowerFreqLine +ind ;
	}
	
	array->ha_iLowerFreq = iLowerFreqLine ;
	array->ha_iUpperFreq = iUpperFreqLine ;
	array->ha_numReserved = iReservedSize;
}
#endif
// ---------------------------------------------------------------------

void FreqLineFree(MICARY_GHDSSFreqLine * array){
	if (!array){
		return  ;
	}
	
	if (array->micary_iaFreqLine->ha_numValue){
		IntArrayFree(array->micary_iaFreqLine) ;
	}
	array->ha_iLowerFreq = 0;
	array->ha_iUpperFreq = 0 ;
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
	array->ha_numReserved = 0;
#endif
	return  ;
}
// ----------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void FreqLineClear(MICARY_GHDSSFreqLine * array) 
{
	if (!array){
		return;
	}
	array->ha_iLowerFreq = 0;
	array->ha_iUpperFreq = 0 ;
}
#endif
// ----------------------------------------------------------------------
// SMDatabe‚É‘Î‰ž(2008/01/10 Sachiko.suzuki)
void SMDatabaseAlloc(MICARY_SMDatabase * SMD, int iFreqSize, int iLocSize, int iMicSize){
	if (!SMD){
		return;
	}
	
	if (iFreqSize * iLocSize * iMicSize == 0){
		SeparationMatAlloc(&(SMD->micary_smSeparation), 0,0,0);
		LocationArrayAlloc(&(SMD->micary_locaSource), 0);
		LocationArrayAlloc(&(SMD->micary_locaMic), 0);
		//TFD->ha_numMic = 0;
		return;
	}
	SeparationMatAlloc(&(SMD->micary_smSeparation), iFreqSize , iLocSize , iMicSize);
	LocationArrayAlloc(&(SMD->micary_locaSource), iLocSize);
	LocationArrayAlloc(&(SMD->micary_locaMic), iMicSize);
	
	return ;
	//TFD->ha_numMic = iMicSize;
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void SMDatabaseAlloc_r(MICARY_SMDatabase * SMD, int iFreqSize, int iLocSize, int iMicSize,
											  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize)
{
	if (!SMD){
		return;
	}
	
	if (iFreqSize * iLocSize * iMicSize == 0){
		SeparationMatAlloc(&(SMD->micary_smSeparation), 0,0,0);
		LocationArrayAlloc(&(SMD->micary_locaSource), 0);
		LocationArrayAlloc(&(SMD->micary_locaMic), 0);
		return;
	}
	SeparationMatAlloc_r(&(SMD->micary_smSeparation), iFreqSize , iLocSize , iMicSize,
													iReservedFreqSize, iReservedLocSize, iReservedMicSize);
	LocationArrayAlloc_r(&(SMD->micary_locaSource), iLocSize, iReservedLocSize);
	LocationArrayAlloc_r(&(SMD->micary_locaMic), iMicSize, iReservedMicSize);
	
	return ;
}
#endif
// ---------------------------------------------------------------------

void SMDatabaseFree(MICARY_SMDatabase * SMD){
	if (!SMD){
		return;
	}

	SeparationMatFree(&(SMD->micary_smSeparation));

	LocationArrayFree(&(SMD->micary_locaSource));

        LocationArrayFree(&(SMD->micary_locaMic));

	return ;
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void SMDatabaseClear(MICARY_SMDatabase * SMD) 
{
	if (!SMD){
		return;
	}
	SeparationMatClear(&(SMD->micary_smSeparation));
	LocationArrayClear(&(SMD->micary_locaSource));
	LocationArrayClear(&(SMD->micary_locaMic));
}
#endif
// ---------------------------------------------------------------------
void ComplexArrayInit(MICARY_ComplexArray * array) {

        int k ;
	if(!array){
		return ;
	}      
	for (k = 0 ; k < array->ha_numValue ; k++){
		array->ha_cmpValue[k].re = 0.0 ;
		array->ha_cmpValue[k].im = 0.0 ;
	}

	//memset(&array->ha_cmpValue[0].re,0,array->ha_numValue) ;
	//memset(&array->ha_cmpValue[0].im,0,array->ha_numValue) ;

	return ;
}
// ---------------------------------------------------------------------
// alloc & free
void UpdateSeparationMatAlloc(MICARY_UpdateSeparationMat * SPM, 
							  int iFreqSize, int iLocSize,int iMicSize)
{
	if (!SPM){
		return;
	}

	if (iFreqSize * iLocSize * iMicSize == 0){
		SMDatabaseAlloc(&(SPM->micary_dbSeparation),0,0,0) ;
		IntArrayAlloc(&(SPM->micary_iaUpdate),0) ;
		return;
	}

		SMDatabaseAlloc(&(SPM->micary_dbSeparation),iFreqSize,iLocSize,iMicSize) ;
	  IntArrayAlloc(&(SPM->micary_iaUpdate),iLocSize) ;
		
	return ;
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void UpdateSeparationMatAlloc_r(MICARY_UpdateSeparationMat * SPM, int iFreqSize, int iLocSize,int iMicSize,
							  int iReservedFreqSize, int iReservedLocSize, int iReservedMicSize)
{
	if (!SPM){
		return;
	}

	if (iFreqSize * iLocSize * iMicSize == 0 &&
		iReservedFreqSize * iReservedLocSize * iReservedMicSize == 0){
		SMDatabaseAlloc(&(SPM->micary_dbSeparation),0,0,0) ;
		IntArrayAlloc(&(SPM->micary_iaUpdate),0) ;
		return;
	}

	SMDatabaseAlloc_r(&(SPM->micary_dbSeparation),iFreqSize,iLocSize,iMicSize,
												iReservedFreqSize, iReservedLocSize, iReservedMicSize) ;
	IntArrayAlloc_r(&(SPM->micary_iaUpdate),iLocSize, iReservedLocSize) ;
		
	return ;
}
#endif
// ---------------------------------------------------------------------

void UpdateSeparationMatFree(MICARY_UpdateSeparationMat * SPM){

	if (!SPM){
		return;
	}
	SMDatabaseFree(&(SPM->micary_dbSeparation)) ;
	IntArrayFree(&(SPM->micary_iaUpdate)) ;
	
	return ;
	
}
// ---------------------------------------------------------------------
// 2011/11/16 added by N.Tanaka
#ifdef __MICARY_def_USE_RESERVED_ARRAY
void UpdateSeparationMatClear(MICARY_UpdateSeparationMat * SPM)
{
	if (!SPM){
		return;
	}
	SMDatabaseClear(&(SPM->micary_dbSeparation)) ;
	IntArrayClear(&(SPM->micary_iaUpdate)) ;
	
	return ;
}
#endif
// ---------------------------------------------------------------------
