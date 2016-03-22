// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2007 Honda Research Institute Japan, Co., Ltd.

 \file CGHDSS_exec.cpp
 \brief \~English Source file for execute GHDSS. 
 \brief \~Japanese GHDSS実行クラスのソース(CGHDSS)
 \author Sachiko Suzuki
 \date 24 Dec 2009
 \version $Id: CGHDSS_exec.cpp 3538 2012-02-29 07:25:35Z tanaka $
*/
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;
// ---------------------------------------------------------------------
#include "Cexec.h"
#include "CGHDSS_exec.h"
// ---------------------------------------------------------------------
// コンストラクタ
CGHDSS_exec::CGHDSS_exec()
{
  //	Cexec::Cexec() ;
	
	m_pcsRxx = (MICARY_dComplexSpecArray *)calloc(1,sizeof(MICARY_dComplexSpecArray)) ;
	m_pcsRyy = (MICARY_dComplexSpecArray *)calloc(1,sizeof(MICARY_dComplexSpecArray)) ;
	AllocdComplexSpecArray(m_pcsRxx,0,0,0) ;
	AllocdComplexSpecArray(m_pcsRyy,0,0,0) ;
	
	// added 2012/03/07
	__SepMat_InitializeValues();
	
}

// ---------------------------------------------------------------------
// デストラクタ
CGHDSS_exec::~CGHDSS_exec()
{
  	if(m_pcsRxx){
        FreedComplexSpecArray(m_pcsRxx) ;
		free(m_pcsRxx) ;
		m_pcsRxx = NULL ;
	}
	if(m_pcsRyy){
		FreedComplexSpecArray(m_pcsRyy) ;
		free(m_pcsRyy) ;
		m_pcsRyy = NULL ;
	}

	// added 2012/03/07
	__SepMat_FreeValues(true, true);
}

// ---------------------------------------------------------------------
// GHDSSの実行（分離行列の更新) 
int CGHDSS_exec ::ExecuteGHDSS(MICARY_ComplexSpecArray pcsInputCSpec,
							  MICARY_ComplexSpecArray *pcsOutputCSpec)
{
	
	if(!&pcsInputCSpec){
		return 0 ;
	}
	if(!pcsOutputCSpec){
		return 0 ;
	}
	
	
	int iLower_f ,iUpper_f ;
	iLower_f = m_psSepMat->micary_tdTFDB.micary_iaFreqLine.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_tdTFDB.micary_iaFreqLine.ha_numValue + iLower_f -1 ;

	// サイズの確認
	// 2010.8.26
	
	/* 2013.1.26 Morio
	if(pcsInputCSpec.ha_iLowerFreq != iLower_f ){
		cout<<"Lower freq not matched!"<<endl;
		return 0 ;
	}
	
	if(pcsInputCSpec.ha_iUpperFreq != iUpper_f){
		cout<<"Upper freq not matched!"<<endl;
		return 0 ;
	}
	*/

	if(pcsInputCSpec.ha_numCh != m_psSepMat->micary_smSeparation.ha_numInput){
		cout<<"InputSpec ChNum and TFdata ChNum not matched!"<<endl;
		cout<<"InputSpec ChNum = "<<pcsInputCSpec.ha_numCh<<endl;
		cout<<"TFdata ChNum = "<<m_psSepMat->micary_smSeparation.ha_numInput<<endl;
		return 0 ;
	}

	if(pcsOutputCSpec->ha_iLowerFreq != iLower_f ||
		pcsOutputCSpec->ha_iUpperFreq != iUpper_f ||
		pcsOutputCSpec->ha_numCh  != m_psSepMat->micary_smSeparation.ha_numOutput)
	{
		// FreeComplexSpecArray(pcsOutputCSpec) ;	// 2012/01/19 deleted
		AllocComplexSpecArray_r(pcsOutputCSpec,m_psSepMat->micary_smSeparation.ha_numOutput,
		(HA_Integer)iLower_f,(HA_Integer)iUpper_f,
		__MICARY_def_MAX_NCH, __MICARY_def_MAX_NFL) ;
	}
	
	// 初期化後1番目のフレームはOCRAは無効
	bool bTmp = false;
	float fTmp ;
	int iTmp ;
/*	
if (pcsOutputCSpec->ha_numCh == 2){
	int ii;
	printf("m_psSepMat(size:%d):", m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_numValue);
	for (ii=0 ; ii < m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_numValue ; ii++){
		printf("%f+%fi,", m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii].re,
							m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii].im);
	}
	printf("\n");
}
*/	
	
	// GHDSSの実行と分離行列の更新
	// for New
	if(dUpdateSeparationMatGHDSS(m_psSepMat, *pcsOutputCSpec ,pcsInputCSpec,m_pcsRxx,m_pcsRyy,
		m_prmSep,m_fWMyu,m_fThreshold,m_iUpdate) == 0)
	
	{
		std ::cout << "Fail to execute GHDSS and update Separation matrix." << std::endl ;
		return 0 ;
	}
	
	//// GHDSSの実行と分離行列の更新
	//// for boost
	//if(dUpdateSeparationMatGHDSS(m_psSepMat, *pcsOutputCSpec ,pcsInputCSpec,
	//	m_prmSep,m_fWMyu,m_fThreshold,m_iUpdate) == 0)
	
	//{
	//	std ::cout << "Fail to execute GHDSS and update Separation matrix." << std::endl ;
	//	return 0 ;
	//}
	return 1;	
}

void CGHDSS_exec :: SetPrmGHDSS(HA_Integer ha_iSSMethod, HA_Float ha_fSSMyu , HA_Float ha_fSSScal,
							  	HA_Float ha_fNoiseFloor , HA_Integer ha_iLCConst ,
								HA_Integer ha_iLCMethod , HA_Float ha_fLCMyu) {
	
	m_prmSep.ha_fLCMyu = ha_fLCMyu ;
	m_prmSep.ha_fNoiseFloor = ha_fNoiseFloor ;
	m_prmSep.ha_fSSMyu = ha_fSSMyu ;
	m_prmSep.ha_iLCConst = ha_iLCConst ;
	m_prmSep.ha_iLCMethod = ha_iLCMethod ;
	m_prmSep.ha_iSSMethod = ha_iSSMethod ;
	m_prmSep.ha_fSSScal = ha_fSSScal ;

	return  ;

}

void CGHDSS_exec::Initialize_GHDSS(HA_Integer iNch,HA_Integer iNpos,HA_Integer iLower_freq,HA_Integer iUpper_freq )
{
	int i ;
	/*
	if(m_pcsRxx){
	  FreedComplexSpecArray(m_pcsRxx) ;
	}
	*/

	AllocdComplexSpecArray_r(m_pcsRxx,iNch*iNch,iLower_freq,iUpper_freq,
							 __MICARY_def_MAX_NCH*__MICARY_def_MAX_NCH, __MICARY_def_MAX_NFL) ;
	for(i = 0 ; i < m_pcsRxx->micary_cmpaSpec.ha_numValue ; i++){
		m_pcsRxx->micary_cmpaSpec.da_cmpValue[i].re = 1.0 ;
		m_pcsRxx->micary_cmpaSpec.da_cmpValue[i].im = 0.0 ;
	}

	/*
	if(m_pcsRyy){
	  FreedComplexSpecArray(m_pcsRyy) ;
	}
	*/

	AllocdComplexSpecArray_r(m_pcsRyy,iNpos*iNpos,iLower_freq,iUpper_freq,
							 (__MICARY_def_MAX_NSRC+1)*(__MICARY_def_MAX_NSRC+1), __MICARY_def_MAX_NFL) ;
	for(i = 0 ; i < m_pcsRyy->micary_cmpaSpec.ha_numValue ; i++){
		m_pcsRyy->micary_cmpaSpec.da_cmpValue[i].re = 1.0 ;
		m_pcsRyy->micary_cmpaSpec.da_cmpValue[i].im = 0.0 ;
	}
	
	m_bIni = true ;

	return ;
}
// ------------------------------------------------------------------------------
void CGHDSS_exec::SetPrmGHDSS2(HA_Float ha_fWmyu, HA_Float ha_fThreshold, HA_Integer ha_iUpdate)
{

	m_fWMyu = ha_fWmyu ;
	m_fThreshold = ha_fThreshold ;
	m_iUpdate = ha_iUpdate ;

}
// ------------------------------------------------------------------------
void CGHDSS_exec::SetbIni(bool bIni)
{
	m_bIni = bIni ;

}
