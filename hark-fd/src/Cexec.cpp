// ---------------------------------------------------------------------
/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
using namespace std;
// ---------------------------------------------------------------------
#include "Cexec.h"
#ifndef ENABLE_HARKIO_NH

// ---------------------------------------------------------------------
// �R���X�g���N�^

Cexec::Cexec()
{
	m_psSepMat = (MICARY_SavedSeparation *)calloc(1,sizeof(MICARY_SavedSeparation)) ;	//�����s��ۑ��l
	SavedSeparationAlloc(m_psSepMat,0,0,0) ;
		

	LocationArrayAlloc(&locaSrcFixedNoise,0) ;
	LocationArrayAlloc(&locaSrcFixedNoise,1) ;
	m_psTFDB = NULL ;	
	m_psSMDB = NULL ;

	m_file = NULL ;
	m_file_FixedNoise = NULL ;
	m_file_sm = NULL ;
	m_file_tf = NULL ;
	
	LocationArrayAlloc(&locaMic,0) ;
	
	m_iFlgtf = 0 ;
	m_bFlgFixedNoise = false ;
	m_iFlgsm = 0 ;

	iMicPosShift = 1 ;	//default : FIX
	
	//m_updateW�̊m��
	m_updateW = (MICARY_UpdateSeparationMat *)calloc(1,sizeof(MICARY_UpdateSeparationMat)) ;
	UpdateSeparationMatAlloc(m_updateW,0,0,0) ;

	// m_laFrontSrcLoc�̊m��
	m_laFrontSrcLoc = (MICARY_LocationArray *)calloc(1,sizeof(MICARY_LocationArray)) ;
	LocationArrayAlloc(m_laFrontSrcLoc,0) ;

	// m_iUpdateMethodWex�̊m��
	m_iUpdateMethodWex = (MICARY_IntArray *)calloc(1,sizeof(MICARY_IntArray)) ;
	IntArrayAlloc(m_iUpdateMethodWex,0) ;

	// default value
	m_iFormat_TF_CONJ = __FILE_FORMAT_MICARY ;
	m_iFormat_InitW = __FILE_FORMAT_MICARY ;
	m_iFormat_ExportW = __FILE_FORMAT_MICARY ;
      m_iFormat_MicPos = __FILE_FORMAT_MICARY ;
	m_iFormat_FixedNoise = __FILE_FORMAT_MICARY ;

	// 2012/01/19 added
	LocationArrayAlloc(&m_plaSrcPosIni,0) ;
	ComplexArrayAlloc(&m_tmp_cmpaTFDB,0) ;

}

// ---------------------------------------------------------------------
// �f�X�g���N�^
Cexec::~Cexec()
{
	if(m_psSepMat){
		SavedSeparationFree(m_psSepMat) ;
		free(m_psSepMat) ;
		m_psSepMat = NULL ;
	}
	if(&locaSrcFixedNoise){
       	LocationArrayFree(&locaSrcFixedNoise) ;
	}
	if(m_psTFDB){
		TFDatabaseFree(m_psTFDB) ;
		free(m_psTFDB) ;
		m_psTFDB = NULL ;
	}
	if(m_psSMDB){
		SMDatabaseFree(m_psSMDB) ;
		free(m_psSMDB) ;
		m_psSMDB = NULL ;
	}
	if(m_file){
		free(m_file) ;
		m_file = NULL ;
	}
	if(m_file_FixedNoise){
		free(m_file_FixedNoise) ;
		m_file_FixedNoise = NULL ;
	}
	if(m_file_sm){
		free(m_file_sm) ;
		m_file_sm = NULL ;
	}
	if(m_file_tf){
		free(m_file_tf) ;
		m_file_tf = NULL ;
	}
	if(&locaMic){
		LocationArrayFree(&locaMic) ;
	}
	m_iFlgtf = 0 ;
	m_bFlgFixedNoise = false ;
	m_iFlgsm = 0 ;

	if(m_updateW){
		UpdateSeparationMatFree(m_updateW) ;
		free(m_updateW) ;
		m_updateW = NULL ;
	}
	if(m_laFrontSrcLoc){
		LocationArrayFree(m_laFrontSrcLoc) ;
		free(m_laFrontSrcLoc) ;
		m_laFrontSrcLoc = NULL ;
	}

	if(m_iUpdateMethodWex){
		IntArrayFree(m_iUpdateMethodWex) ;
		free(m_iUpdateMethodWex) ;
		m_iUpdateMethodWex = NULL ;
	}

	// 2012/01/19 added
	LocationArrayFree(&m_plaSrcPosIni) ;
	ComplexArrayFree(&m_tmp_cmpaTFDB) ;
	
}

// ---------------------------------------------------------------------
// �����s��ۑ��l�̃N���A
int Cexec::Clear()
{
	SavedSeparationFree(m_psSepMat) ;
	return 1 ;
}

// ---------------------------------------------------------------------
//�����s��ۑ��l�A���g�����C���A�}�C�N�A�������W�z��̊m�ۂƍ쐬
int Cexec::Initialize(const char *filename,float fspeed_sound,
							int iFlgTf,const char *filename_tf ,
							bool bFlgFixedNoise, const char *filename_FixedNoise,
							int iFlgSm,const char *filename_sm)
{
	this->Clear() ;
	//this->m_PrmStepSize.ha_fStepSizePrm=fStepPrm ;
 
	//�}�C�N���W�t�@�C��
	if(iFlgTf == 0){	//Calculation 
		this->m_file = (char *)malloc(sizeof(char)*strlen(filename)+1 ) ;
		strcpy(this->m_file,filename) ;
		if(Cexec ::ReadMicPos() == 0){
			std::cout << "Fail to open mic file." << std::endl ;
			m_iFlgtf = 0 ;
			m_bFlgFixedNoise = false ;
			m_iFlgsm = 0 ;
			
			return 0 ;
		}
		std::cout << "Read Mic position file." << std::endl ;
			
	}
	
	this->m_fspeed_sound = fspeed_sound ;
	//TF�}�g���N�X�t�@�C��
	if(iFlgTf == 1){	// Read data file
		this->m_file_tf = (char *)malloc(sizeof(char)*strlen(filename_tf)+1) ;
		strcpy(this->m_file_tf,filename_tf) ;
		this->m_iFlgtf = 1 ;
	}
	else{
		this->m_iFlgtf = 0 ;	//Calculation
	}
	this->m_bFlgFixedNoise = bFlgFixedNoise ;
	if(this->m_bFlgFixedNoise){
		if(!filename_FixedNoise){
			std::cout << "You must input FixedNoise position file." << std::endl ;
			m_iFlgtf = 0 ;
			m_bFlgFixedNoise = false ;
			m_iFlgsm = 0 ;
			return 0 ;
		}
		this->m_file_FixedNoise = (char *)malloc(sizeof(char)*strlen(filename_FixedNoise)+1) ;
		strcpy(this->m_file_FixedNoise,filename_FixedNoise) ;
		//�ŏ��ɓǂݍ���ł���
		int iMm,iMXm,iMYm,iMZm ;
	
		if(m_iFormat_FixedNoise == __FILE_FORMAT_HARK){
			if(!cHark.LoadPositionData(m_file_FixedNoise,&locaSrcFixedNoise,__FILE_TYPE_NOISE)){
		    		std ::cout << "Fail to Read FixedNoise position file." << std ::endl ;
				LocationArrayFree(&locaSrcFixedNoise) ;
				m_iFlgtf = 0 ;
				m_bFlgFixedNoise = false ;
				m_iFlgsm = 0 ;
				return 0 ;
			}
		}
			  
		std::cout << "Read FixedNoise position file." << std::endl ;
	}

	//�����s�񏉊��l(Calc or Read)
	if(iFlgSm == 1){
		this->m_file_sm = (char *)malloc(sizeof(char)*strlen(filename_sm)+1) ;
		strcpy(this->m_file_sm,filename_sm) ;
		this->m_iFlgsm = 1 ;
	}
	else{
		this->m_iFlgsm = 0 ;
	}
	return 1 ;
}
// ---------------------------------------------------------------------
// �t���[���p�����[�^�̐ݒ�
int Cexec ::SetPrmFrame(MICARY_PrmFrame *psPrmFrame ,int iFs ,
							int iLength,int iOverlap,int enumWin)
{
	psPrmFrame->ha_fFs = (float)iFs ;
	psPrmFrame->ha_iShift = (int)(iLength - iOverlap) ;
	psPrmFrame->ha_numNFFT = (HA_Num)iLength ;
	psPrmFrame->ha_numWindow = (HA_Num)iLength ;
	psPrmFrame->micary_nwnWindow = (MICARY_WindowEnum)enumWin ;
	return 1 ;
}
// ---------------------------------------------------------------------
//�}�C�N���W�t�@�C���̓ǂݍ��݂Ɣz��̍쐬
int Cexec ::ReadMicPos()
{
  	int iMm,iMXm,iMYm,iMZm ;
	if(m_iFormat_MicPos == __FILE_FORMAT_HARK){
		cout << "File open as HARK = " << m_file << endl ; 
	  	if(!cHark.LoadPositionData(m_file,&locaMic,__FILE_TYPE_MIC)){
	    		std ::cout << "Fail to Read mic position file." << std ::endl ;
			return 0 ;
	  	}
	}
	return 1 ;
}
// --------------------------------------------------------------------
// �`�B�֐��̍쐬(���R����)
int Cexec :: MakeTFMatrix(int iUpper_freq,MICARY_PrmFrame *psPrmFrame)
{
 
   if(CalcTFFreeH(&m_psSepMat->micary_tdTFDB,iUpper_freq,*psPrmFrame,m_psSepMat->micary_tdTFDB.micary_locaSource,
		m_psSepMat->micary_tdTFDB.micary_locaMic,(HA_Float)this->m_fspeed_sound) == 0)
	{
		std ::cout << "Fail to calculate TF-Matrix." << std::endl ;
		return 0 ;
	}

	return 1 ;
}

// --------------------------------------------------------------------
// �`�B�֐��̍쐬
int Cexec :: MakeTFMatrix2(MICARY_PrmFrame *psPrmFrame, int idsrc)
{
 
   if(CalcTFFreeH2(&m_psSepMat->micary_tdTFDB,*psPrmFrame,idsrc,(HA_Float)this->m_fspeed_sound) == 0)
	{
		std ::cout << "Fail to calculate TF-Matrix." << std::endl ;
		return 0 ;
	}

	return 1 ;
}

// --------------------------------------------------------------------
// �`�B�֐��t�@�C���̓ǂݍ���(Original�t�@�C���d�l)
int Cexec :: ReadTFMatrix(MICARY_PrmFrame psPrmFrame)
{
	HA_String TFfile ;
	// Read
	TFfile.s = (HA_Char *)calloc(strlen(this->m_file_tf)+1,sizeof(char)) ;
	strcpy(TFfile.s,this->m_file_tf) ;
	TFfile.slen = strlen(this->m_file_tf) ;
	
	if(m_iFormat_TF_CONJ == __FILE_FORMAT_HARK){
		MICARY_TFDBFileHeader *psH ;
		psH = new MICARY_TFDBFileHeader() ;
		if(!cHark.ReadHeader(TFfile.s,psH)){
			std ::cout << "Fail to open TF data file." << std::endl ;
			if(TFfile.s){
				free(TFfile.s) ;
				TFfile.s = NULL ;
			}
			if(psH){
				delete psH ;
				psH = NULL ;
			}
			return 0 ;
		}
		if(m_psTFDB->micary_cmpaTF.ha_numValue != psH->lNSrc * psH->lNMic * psH->lNFreq){
		  TFDatabaseFree(m_psTFDB) ;
		  TFDatabaseAlloc(m_psTFDB,psH->lNFreq,psH->lNSrc,psH->lNMic) ;
		}

		if(!cHark.ReadInfo(TFfile.s,psH,
							&m_psTFDB->micary_locaSource ,
							&m_psTFDB->micary_locaMic,
							&m_psTFDB->micary_iaFreqLine)){
			std ::cout << "Fail to read TF data file." << std::endl ;
			if(TFfile.s){
				free(TFfile.s) ;
				TFfile.s = NULL ;
			}
			if(psH){
				delete psH ;
				psH = NULL ;
			}
			return 0 ;
		}
	

		if(!cHark.ReadData(TFfile.s,&m_psTFDB->micary_cmpaTF)){
			std ::cout << "Fail to read TF data file." << std::endl ;
			if(TFfile.s){
				free(TFfile.s) ;
				TFfile.s = NULL ;
			}
			if(psH){
				delete psH ;
				psH = NULL ;
			}
			return 0 ;
		}
		if(psH){
		  delete psH ;
		  psH = NULL ;
		}
	}
	if(TFfile.s){
		free(TFfile.s) ;
		TFfile.s = NULL ;
	}
	
	return 1 ;
}	
//-----------------------------------------------------------------------------
// TF Matrix�̏�����
//TF�f�[�^�x�[�X�A�����s�񏉊��l�f�[�^�x�[�X�̃t�@�C���ǂݍ���
int Cexec :: InitTFMatrix(MICARY_PrmFrame *psPrmFrame)
{
	int iLower_f,iUpper_f,iNch,iNpos,iNfreq ;
	iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue +iLower_f -1 ;
	
		if(!this->m_psTFDB){	//Initial�t���[�������i�f�[�^�x�[�X̧�ق̓ǂݍ���)
			
			m_psTFDB = (MICARY_TFDatabase *)calloc(1,sizeof(MICARY_TFDatabase)) ;	//TF�f�[�^�x�[�X
 			TFDatabaseAlloc(m_psTFDB,0,0,0) ;

			MICARY_TFDBFileHeader * psTFDBHead ;	//TF�f�[�^�x�[�X�w�b�_
			psTFDBHead = (MICARY_TFDBFileHeader *)calloc(1,sizeof(MICARY_TFDBFileHeader)) ;
	
			HA_String file ; // TF �t�@�C����
			file.s = (HA_Char *)calloc(strlen(this->m_file_tf)+1,sizeof(char)) ;
			strcpy(file.s,this->m_file_tf) ;
			file.slen = strlen(this->m_file_tf) ;
	
			if(m_iFormat_TF_CONJ == __FILE_FORMAT_HARK){
				if(cHark.ReadHeader(file.s,psTFDBHead) == 0) //�t�@�C���̃w�b�_�����擾
				{
					printf("Can not read TF Database.\n") ;
					if(psTFDBHead){
						free(psTFDBHead) ;
						psTFDBHead = NULL ;
					}
					m_iFlgtf = 0 ;
					m_bFlgFixedNoise = false ;
					m_iFlgsm = 0 ;
					if(file.s){
						free(file.s) ;
						file.s = NULL ;
					}
					return 0 ;
				}
			}
		       
		     TFDatabaseAlloc(m_psTFDB,(int)psTFDBHead->lNFreq,(int)psTFDBHead->lNSrc,(int)psTFDBHead->lNMic) ;
		     cout << "NF = " << psTFDBHead->lNFreq << " NSrc = " << psTFDBHead->lNSrc << " NMic = " << psTFDBHead->lNMic << endl ;
		     if(Cexec ::ReadTFMatrix(*psPrmFrame) == 0){
				free(psTFDBHead)  ;
				psTFDBHead = NULL ;
				m_iFlgtf = 0 ;
				m_bFlgFixedNoise = false ;
				m_iFlgsm = 0 ;
				if(file.s){
					free(file.s) ;
					file.s = NULL ;
				}
				return 0;
			}
			iNch = psTFDBHead->lNMic ;
		      free(psTFDBHead)  ;
			psTFDBHead = NULL ;
			if(file.s){
				free(file.s) ;
				file.s = NULL ;
			}
			
			/*
			cout << "----------file open--------------"<<endl;
			cout << "X " << m_psTFDB->micary_locaSource.micary_Location[32].tfindex[0] << " " << m_psTFDB->micary_locaSource.micary_Location[32].ha_fX <<endl;
			cout << "Y " << m_psTFDB->micary_locaSource.micary_Location[32].tfindex[1] << " " << m_psTFDB->micary_locaSource.micary_Location[32].ha_fY <<endl;
			cout << "Z " << m_psTFDB->micary_locaSource.micary_Location[32].tfindex[2] << " " << m_psTFDB->micary_locaSource.micary_Location[32].ha_fZ  <<endl;
			
			cout << "X " << m_psTFDB->micary_locaSource.micary_Location[40].tfindex[0] << " " << m_psTFDB->micary_locaSource.micary_Location[40].ha_fX <<endl;
			cout << "Y " << m_psTFDB->micary_locaSource.micary_Location[40].tfindex[1] << " " << m_psTFDB->micary_locaSource.micary_Location[40].ha_fY <<endl;
			cout << "Z " << m_psTFDB->micary_locaSource.micary_Location[40].tfindex[2] << " " << m_psTFDB->micary_locaSource.micary_Location[40].ha_fZ  <<endl;
			*/
			
			//�����̕t�����i�t�@�C���ɂȂ����j
			if(m_iSrc_Compare_mode == __COMPARE_MODE_DEG){	
				//add by Morio 2013/01/20 �p�x�����i�[
				//�p�x�Z�o�̌��_��(0,0,0)�Ƃ���
				float pi = acos(-1.0) ;
				float x,y,z;

				for(unsigned int i = 0; i < m_psTFDB->micary_locaSource.ha_numLocation; i++)
				{
					x = m_psTFDB->micary_locaSource.micary_Location[i].ha_fX;
					y = m_psTFDB->micary_locaSource.micary_Location[i].ha_fY;
					z = m_psTFDB->micary_locaSource.micary_Location[i].ha_fZ;
					m_psTFDB->micary_locaSource.micary_Location[i].ha_fTheta = (float)(atan2(y,x) * 180 / pi);
					m_psTFDB->micary_locaSource.micary_Location[i].ha_fPhi = (float)(atan2(z,sqrt(x*x+y*y)) * 180 / pi);
					
					
				}
			}
			else if(m_iSrc_Compare_mode == __COMPARE_MODE_TFINDEX){
				
				//�������i�K�ł̓t�@�C������ǂݍ��߂Ȃ��̂ŁC��ʂƕ����̓`�B�֐����������Ƀt�@�C���Ɋi�[����Ă���Ƃ���Index�����߂���
				//������ʂ�ꍇ�͉�����0�͂��肦�Ȃ��̂ŁC�����s��̍ŏ��̗v�f��TFIndex��min,max���擾�ł���B
				for(unsigned int i = 0; i < m_psTFDB->micary_locaSource.ha_numLocation; i++){
					m_psTFDB->micary_locaSource.micary_Location[i].tfindex_min[0] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[0].tfindex_min[0];
					m_psTFDB->micary_locaSource.micary_Location[i].tfindex_min[1] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[0].tfindex_min[1];
					m_psTFDB->micary_locaSource.micary_Location[i].tfindex_min[2] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[0].tfindex_min[2];
					m_psTFDB->micary_locaSource.micary_Location[i].tfindex_max[0] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[0].tfindex_max[0];
					m_psTFDB->micary_locaSource.micary_Location[i].tfindex_max[1] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[0].tfindex_max[1];
					m_psTFDB->micary_locaSource.micary_Location[i].tfindex_max[2] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[0].tfindex_max[2]; 
					
				}
				
				
				//min�Cmax�͏ꏊ�ɂ���ĕs�ς̂͂�
				int ih = m_psTFDB->micary_locaSource.micary_Location[0].tfindex_max[0] - m_psTFDB->micary_locaSource.micary_Location[0].tfindex_min[0]; 
				int id = m_psTFDB->micary_locaSource.micary_Location[0].tfindex_max[1] - m_psTFDB->micary_locaSource.micary_Location[0].tfindex_min[1]; 
				int ir = m_psTFDB->micary_locaSource.micary_Location[0].tfindex_max[2] - m_psTFDB->micary_locaSource.micary_Location[0].tfindex_min[2]; 
				
				// �`�B�֐��̓_���m�F
				if ( ((ih+1) * (id+1) * (ir+1)) != m_psTFDB->micary_locaSource.ha_numLocation){
					cout << "TF file not match ! at GHDSS Cexec :: InitTFMatrix" << endl;
					cout << "TF file for Localize  : Sorce Num = " << ((ih+1) * (id+1) * (ir+1)) << endl;
					cout << "TF file for Separation: Sorce Num = " << m_psTFDB->micary_locaSource.ha_numLocation << endl;
					return 0;
				}
				
				unsigned int m;
				m=0;
				for (unsigned int i = 0; i< ih + 1; i++ )
				{
					for  (unsigned int j = 0; j< id + 1; j++ )
					{
						for  (unsigned int k = 0; k< ir + 1; k++ ) 	
						{
							m_psTFDB->micary_locaSource.micary_Location[m].tfindex[0]=i;
							m_psTFDB->micary_locaSource.micary_Location[m].tfindex[1]=j; 
							m_psTFDB->micary_locaSource.micary_Location[m].tfindex[2]=k; 
							m = m+1;
						}
					}
				}
				
				if(m_bCheck_distance == true)
				{
					bool bcheck;
					int kk;
					for (int i = 0; i < m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation; i++)
					{
						kk = GetNearestPos(m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i],&m_psTFDB->micary_locaSource);
						
						m_iSrc_Compare_mode = __COMPARE_MODE_DEG;
						bcheck = _IsThreshPos(m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i],m_psTFDB->micary_locaSource.micary_Location[kk]);
						
						
						
						m_iSrc_Compare_mode = __COMPARE_MODE_TFINDEX;
						if(bcheck == false){
							cout << "CHECK_TFINDEX_DISTANCE: Not match Transfer function file localization and separation !!  "<< endl;
							cout << "---------Check_distance----------"<<endl;
							cout << "X   input src = " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].tfindex[0] << " TFCONJ = " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].ha_fX << " " <<m_psTFDB->micary_locaSource.micary_Location[kk].ha_fX <<endl;
							cout << "Y   input src = " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].tfindex[1] << " TFCONJ = " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].ha_fX << " " <<m_psTFDB->micary_locaSource.micary_Location[kk].ha_fX <<endl;
							cout << "Z   input src = " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].tfindex[2] << " TFCONJ = " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].ha_fX << " " <<m_psTFDB->micary_locaSource.micary_Location[kk].ha_fX <<endl;
						
						
						}
					}
				}				
			}	 
		}
		
		/*
		cout << "-------------input tfindex------------"<<endl;
		cout << "X " << m_psTFDB->micary_locaSource.micary_Location[32].tfindex[0] << " " << m_psTFDB->micary_locaSource.micary_Location[32].ha_fX <<endl;
		cout << "Y " << m_psTFDB->micary_locaSource.micary_Location[32].tfindex[1] << " " << m_psTFDB->micary_locaSource.micary_Location[32].ha_fY <<endl;
		cout << "Z " << m_psTFDB->micary_locaSource.micary_Location[32].tfindex[2] << " " << m_psTFDB->micary_locaSource.micary_Location[32].ha_fZ  <<endl;
			
		cout << "X " << m_psTFDB->micary_locaSource.micary_Location[40].tfindex[0] << " " << m_psTFDB->micary_locaSource.micary_Location[40].ha_fX <<endl;
		cout << "Y " << m_psTFDB->micary_locaSource.micary_Location[40].tfindex[1] << " " << m_psTFDB->micary_locaSource.micary_Location[40].ha_fY <<endl;
		cout << "Z " << m_psTFDB->micary_locaSource.micary_Location[40].tfindex[2] << " " << m_psTFDB->micary_locaSource.micary_Location[40].ha_fZ  <<endl;
		*/
		
		// Initialize
		iNfreq = iUpper_f - iLower_f +1 ;
		iNpos = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
		// not Init frame 
		iNch = m_psTFDB->micary_locaMic.ha_numLocation ;
		 
		 
		 
		 
		// �T�C�Y���Ⴆ�ΐ؂�Ȃ���
		if(m_psSepMat->micary_tdTFDB.micary_locaMic.ha_numLocation != iNch){
			LocationArrayFree(&m_psSepMat->micary_tdTFDB.micary_locaMic) ;   
			LocationArrayAlloc(&m_psSepMat->micary_tdTFDB.micary_locaMic,iNch) ;
		}
		// �}�C�N���W
		int ii ;
		//�}�C�N�Ɋւ��Ă͊p�x�CTFIndex�͎g��Ȃ�
		for(ii = 0 ; ii < iNch ; ii++){
			m_psSepMat->micary_tdTFDB.micary_locaMic.ha_idLocation[ii] = m_psTFDB->micary_locaMic.ha_idLocation[ii] ;
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_fPhi = m_psTFDB->micary_locaMic.micary_Location[ii].ha_fPhi ;
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_fTheta = m_psTFDB->micary_locaMic.micary_Location[ii].ha_fTheta ;
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_fX = m_psTFDB->micary_locaMic.micary_Location[ii].ha_fX ;
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_fY = m_psTFDB->micary_locaMic.micary_Location[ii].ha_fY ;
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_fZ = m_psTFDB->micary_locaMic.micary_Location[ii].ha_fZ ;
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_idDir = m_psTFDB->micary_locaMic.micary_Location[ii].ha_idDir ;
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_idPos = m_psTFDB->micary_locaMic.micary_Location[ii].ha_idPos ;
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].micary_LocationFlag = m_psTFDB->micary_locaMic.micary_Location[ii].micary_LocationFlag ;

		}


		if(m_psSepMat->micary_tdTFDB.micary_iaFreqLine.ha_numValue != iNfreq){
			IntArrayFree(&m_psSepMat->micary_tdTFDB.micary_iaFreqLine) ;   
			IntArrayAlloc(&m_psSepMat->micary_tdTFDB.micary_iaFreqLine,iNfreq) ;
		}
		
		for(ii = 0 ; ii < iNfreq ; ii++){
		  if(ii < m_psTFDB->micary_iaFreqLine.ha_numValue){
			m_psSepMat->micary_tdTFDB.micary_iaFreqLine.ha_iValue[ii] = m_psTFDB->micary_iaFreqLine.ha_iValue[ii] ;
		  }
		  else{
		    m_psSepMat->micary_tdTFDB.micary_iaFreqLine.ha_iValue[ii] = iNfreq ;
		  }
		    
		}

		if(m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_numValue != iNpos *iNch * iNfreq ){
		  //ComplexArrayFree(&m_psSepMat->micary_tdTFDB.micary_cmpaTF) ;
		  ComplexArrayAlloc_r(&m_psSepMat->micary_tdTFDB.micary_cmpaTF,iNch*iNfreq*iNpos,
		  					__MICARY_def_MAX_NFL*(__MICARY_def_MAX_NSRC+1)*__MICARY_def_MAX_NCH) ;                 		
		}
		
		//�K�v�ȏ������o��
		if(Cexec::GetNearestTFMatrixFromDB(&m_psSepMat->micary_tdTFDB,m_psTFDB) == 0){
				m_iFlgtf = 0 ;
				m_bFlgFixedNoise = false ;
				m_iFlgsm = 0 ;
				return 0;
		}


	return 1 ;
}
// -------------------------------------------------------------------------------------
// 2012/01/13 added
// TF�f�[�^�x�[�X�A�����s�񏉊��l�f�[�^�x�[�X�̃t�@�C���ǂݍ���
int Cexec :: LoadInitialTFMatrix(MICARY_PrmFrame *psPrmFrame, int iSrcIndex)
{
	int iLower_f,iUpper_f,iNch,iNpos,iNfreq ;
	iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue +iLower_f -1 ;
	
	

		// Initialize
		iNfreq = iUpper_f - iLower_f +1 ;
		iNpos = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
		// not Init frame 
		iNch = m_psTFDB->micary_locaMic.ha_numLocation ;
		
		//�K�v�ȏ������o��
		if(Cexec::GetNearestTFMatrixFromDB2(&m_psSepMat->micary_tdTFDB,m_psTFDB, iSrcIndex) == 0){
				m_iFlgtf = 0 ;
				m_bFlgFixedNoise = false ;
				m_iFlgsm = 0 ;
				return 0;
		}



	return 1 ;
}
// -------------------------------------------------------------------------------------
int Cexec :: InitSepMatrix(MICARY_PrmFrame *psPrmFrame)
{

	//�����s��̏�����
	if(this->m_iFlgsm == 0){	// Read TF
	 
		if(MakeWMatGHDSS(m_psSepMat->micary_tdTFDB,&m_psSepMat->micary_smSeparation) == 0){
			std ::cout << "Fail to initialize sepatation matrix." << std::endl ;
			m_iFlgtf = 0 ;
			m_bFlgFixedNoise = false ;
			m_iFlgsm = 0 ;
			return 0 ;
		}
		// TF�t�@�C����conj�Œ�`
	
	}
	else if(this->m_iFlgsm == 1){	// Read intW data file
		if(!m_psSMDB){	//Initial frame process
			
			MICARY_TFDBFileHeader * psSMDBHead ;	//�f�[�^�x�[�X�w�b�_
			psSMDBHead = (MICARY_TFDBFileHeader *)calloc(1,sizeof(MICARY_TFDBFileHeader)) ;
	
			HA_String file ; // �t�@�C����
			file.s = (HA_Char *)calloc(strlen(this->m_file_sm)+1,sizeof(char)) ;
			strcpy(file.s,this->m_file_sm) ;
			file.slen = strlen(this->m_file_sm) ;
	
			if(m_iFormat_InitW == __FILE_FORMAT_HARK){
				if(cHark.ReadHeader(file.s,psSMDBHead) == 0) //�t�@�C���̃w�b�_�����擾
				{
					printf("Can not read Separation matrix Database.\n") ;
					if(psSMDBHead){
						free(psSMDBHead) ;
						psSMDBHead = NULL ;
					}
					m_iFlgtf = 0 ;
					m_bFlgFixedNoise = false ;
					m_iFlgsm = 0 ;
					if(file.s){
						free(file.s) ;
						file.s = NULL ;
					}
					return 0 ;
				}

			}
			m_psSMDB = (MICARY_SMDatabase *)calloc(1,sizeof(MICARY_SMDatabase)) ;	//�����s��ۑ��l
			SMDatabaseAlloc(m_psSMDB,0,0,0) ;

			SMDatabaseAlloc(m_psSMDB,(int)psSMDBHead->lNFreq,(int)psSMDBHead->lNSrc,(int)psSMDBHead->lNMic) ;
			
			free(psSMDBHead)  ;
			psSMDBHead = NULL ;

			// Separation matrix�f�[�^�̓ǂݍ���
			if(Cexec ::ReadSepMatrix(*psPrmFrame) == 0){
				m_iFlgtf = 0 ;
				m_bFlgFixedNoise = false ;
				m_iFlgsm = 0 ;
				if(file.s){
					free(file.s) ;
					file.s = NULL ;
				}
				return 0;
			}
			if(file.s){
				free(file.s) ;
				file.s = NULL ;
			}
		} //End initial frame process
		
		// ��ԋ߂����W�����o��
		int iLower_f,iUpper_f,iNch,iNpos,iNfreq ;
		iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
		iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue +iLower_f -1 ;
		
		iNch = m_psSepMat->micary_tdTFDB.micary_locaMic.ha_numLocation ;
       	iNfreq = iUpper_f - iLower_f +1 ;
       	iNpos = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	
       	if(m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_numValue != iNpos *iNch * iNfreq ){
	 		//ComplexArrayFree(&m_psSepMat->micary_smSeparation.micary_cmpaSeparation) ;	// 2012/01/19 deleted
	 	  	ComplexArrayAlloc_r(&m_psSepMat->micary_smSeparation.micary_cmpaSeparation,iNch*iNfreq*iNpos,
	 	  		__MICARY_def_MAX_NCH*(__MICARY_def_MAX_NSRC+1)*__MICARY_def_MAX_NFL) ;
		  	m_psSepMat->micary_smSeparation.ha_numInput = iNch ;
		  	m_psSepMat->micary_smSeparation.ha_numOutput= iNpos ;
		}
	//2010.09.29 assign freq line index
	for(int i = 0 ; i < m_psSMDB->micary_smSeparation.micary_iaFreqLinex.ha_numValue ; i++){
	  m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[i] =  m_psSMDB->micary_smSeparation.micary_iaFreqLinex.ha_iValue[i]  ;
	}
		
		//�K�v�ȏ������o��
		if(Cexec::GetNearestSepMatrixFromDB(m_psSepMat,m_psSMDB) == 0){
			m_iFlgtf = 0 ;
			m_bFlgFixedNoise = false ;
			m_iFlgsm = 0 ;
			return 0;
		}
		
	}
	return 1;
}
// ---------------------------------------------------------------------
// 2012/01/13 added
// �����C���f�b�N�X�ŕ����s�񏉊��l��ǂݍ���
int Cexec :: LoadInitialSepMatrix(MICARY_PrmFrame *psPrmFrame, int iSrcIndex)
{
	//�����s��̏�����
	if(this->m_iFlgsm == 0){	// Calculation
		if(MakeWMatGHDSS2(m_psSepMat->micary_tdTFDB, &m_psSepMat->micary_smSeparation, iSrcIndex) == 0){
			std ::cout << "Fail to initialize sepatation matrix." << std::endl ;
			m_iFlgtf = 0 ;
			m_bFlgFixedNoise = false ;
			m_iFlgsm = 0 ;
			return 0 ;
		}
		// TF�t�@�C����conj�Œ�`
	
	}
	else if(this->m_iFlgsm == 1){	// Read data file
		// ��ԋ߂����W�����o��
		int iLower_f,iUpper_f,iNch,iNpos,iNfreq ;
		iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
		iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue +iLower_f -1 ;
		
		iNch = m_psSepMat->micary_tdTFDB.micary_locaMic.ha_numLocation ;
       	iNfreq = iUpper_f - iLower_f +1 ;
       	iNpos = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
		
		//�K�v�ȏ������o��
		if(Cexec::GetNearestSepMatrixFromDB2(m_psSepMat,m_psSMDB,iSrcIndex) == 0){
			m_iFlgtf = 0 ;
			m_bFlgFixedNoise = false ;
			m_iFlgsm = 0 ;
			return 0;
		}
		
	}
	return 1;
}
// ---------------------------------------------------------------------
// �������W�z��̑��
int Cexec :: AssignSrcPos(MICARY_LocationArray locaSrcPos)
{
	
	int n ;
	if(!&locaSrcPos){
		return 0 ;
	}

	if(this->m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation != locaSrcPos.ha_numLocation)
	{
	  //LocationArrayFree(&m_psSepMat->micary_tdTFDB.micary_locaSource) ;	
	  LocationArrayAlloc_r(&m_psSepMat->micary_tdTFDB.micary_locaSource,locaSrcPos.ha_numLocation,
	  						__MICARY_def_MAX_NSRC+1) ;
	}

	for(n = 0 ; n < locaSrcPos.ha_numLocation ; n ++){
		m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[n] = locaSrcPos.ha_idLocation[n] ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fX = locaSrcPos.micary_Location[n].ha_fX ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fY = locaSrcPos.micary_Location[n].ha_fY ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fZ = locaSrcPos.micary_Location[n].ha_fZ ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_idPos = locaSrcPos.micary_Location[n].ha_idPos ;

		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].micary_LocationFlag = locaSrcPos.micary_Location[n].micary_LocationFlag ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fTheta = locaSrcPos.micary_Location[n].ha_fTheta ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fPhi = locaSrcPos.micary_Location[n].ha_fPhi ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_idDir = 0;
		
		//TFIndex
		for (int jj=0; jj < 3 ; jj++ ){
			m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].tfindex[jj]  = locaSrcPos.micary_Location[n].tfindex[jj] ;
			m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].tfindex_min[jj]  = locaSrcPos.micary_Location[n].tfindex_min[jj] ;
			m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].tfindex_max[jj]  = locaSrcPos.micary_Location[n].tfindex_max[jj] ;
		}
	}

/*
		cout << "---------input m_psSepMat----------"	 <<endl;
		for (int i=0; i<locaSrcPos.ha_numLocation; i++){
cout << "X " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].tfindex[0] << " " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].ha_fX <<endl;
cout << "Y " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].tfindex[1] << " " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].ha_fY <<endl;
cout << "Z " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].tfindex[2] << " " << m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[i].ha_fZ <<endl;
}
*/
	return 1 ;
}


// FixedNoise_noiseTF�̍쐬�p�������W�̍쐬
int Cexec::MakeFixedNoise_Pos(MICARY_LocationArray &locaSrc)
{
	int iNpos = locaSrc.ha_numLocation ;
	
	if (locaSrc.ha_numLocation+1 > locaSrc.ha_numReserved){
		// ���ۂɂ͂�����̃��[�g��ʂ邱�Ƃ͂قڂȂ�
		int i ;	
		MICARY_LocationArray locaSrcTmp ; //,locaSrcFixedNoise;
	
		//FixedNoise�ȊO���o�b�N�A�b�v
		LocationArrayAlloc(&locaSrcTmp,0) ;
		LocationArrayAlloc(&locaSrcTmp,iNpos) ;
		for(i = 0 ; i < iNpos ; i++){
			locaSrcTmp.ha_idLocation[i] = locaSrc.ha_idLocation[i] ;
			locaSrcTmp.micary_Location[i].ha_fX = locaSrc.micary_Location[i].ha_fX ;
			locaSrcTmp.micary_Location[i].ha_fY = locaSrc.micary_Location[i].ha_fY ;
			locaSrcTmp.micary_Location[i].ha_fZ = locaSrc.micary_Location[i].ha_fZ ;
			locaSrcTmp.micary_Location[i].ha_idPos = locaSrc.micary_Location[i].ha_idPos ;
			locaSrcTmp.micary_Location[i].ha_fPhi = locaSrc.micary_Location[i].ha_fPhi ;
			locaSrcTmp.micary_Location[i].ha_fTheta = locaSrc.micary_Location[i].ha_fTheta ;
			locaSrcTmp.micary_Location[i].micary_LocationFlag = locaSrc.micary_Location[i].micary_LocationFlag ;
		}
	
		LocationArrayAlloc_r(&locaSrc, iNpos+1, __MICARY_def_MAX_NSRC+1) ;
	
		for(i = 0 ; i < iNpos ; i++){
			locaSrc.ha_idLocation[i] = locaSrcTmp.ha_idLocation[i] ;
			locaSrc.micary_Location[i].ha_fX = locaSrcTmp.micary_Location[i].ha_fX ;
			locaSrc.micary_Location[i].ha_fY = locaSrcTmp.micary_Location[i].ha_fY ;
			locaSrc.micary_Location[i].ha_fZ = locaSrcTmp.micary_Location[i].ha_fZ ;
			locaSrc.micary_Location[i].ha_idPos = locaSrcTmp.micary_Location[i].ha_idPos ;
			locaSrc.micary_Location[i].ha_fPhi = locaSrcTmp.micary_Location[i].ha_fPhi ;
			locaSrc.micary_Location[i].ha_fTheta = locaSrcTmp.micary_Location[i].ha_fTheta ;
			locaSrc.micary_Location[i].micary_LocationFlag = locaSrcTmp.micary_Location[i].micary_LocationFlag ;	
		}

		LocationArrayFree(&locaSrcTmp) ;
	}
	else {
		locaSrc.ha_numLocation += 1;
	}
	
	locaSrc.micary_Location[iNpos].ha_fX = locaSrcFixedNoise.micary_Location[0].ha_fX ;
	locaSrc.micary_Location[iNpos].ha_fY = locaSrcFixedNoise.micary_Location[0].ha_fY ;
	locaSrc.micary_Location[iNpos].ha_fZ = locaSrcFixedNoise.micary_Location[0].ha_fZ ;
	locaSrc.micary_Location[iNpos].ha_fTheta = locaSrcFixedNoise.micary_Location[0].ha_fTheta ;
	locaSrc.micary_Location[iNpos].ha_fPhi = locaSrcFixedNoise.micary_Location[0].ha_fPhi ;
	locaSrc.micary_Location[iNpos].micary_LocationFlag = locaSrcFixedNoise.micary_Location[0].micary_LocationFlag ;
	// FixedNoise source��ID = -1
	locaSrc.ha_idLocation[iNpos] = -1 ;
	locaSrc.micary_Location[iNpos].ha_idPos = -1 ;
		
	return 1 ;
}

//Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~
bool Cexec::CheckFixedNoise_noise(MICARY_LocationArray locaSrc)
{
	
	bool bFixedNoise_Src = false ;
	int i ,iNpos;
	float fSrc[3], fFixedNoise[3];
	float ha_fNorm,ha_fP ;

	// fixednoise�̏���
	ha_fNorm = (float)(pow((double)locaSrcFixedNoise.micary_Location->ha_fX,(double)2.0) + pow((double)locaSrcFixedNoise.micary_Location->ha_fY,(double)2.0)+pow((double)locaSrcFixedNoise.micary_Location->ha_fZ,(double)2.0)) ;
	ha_fNorm = (float)(pow((double)ha_fNorm,(double)0.5)) ;
	
	fFixedNoise[0] = locaSrcFixedNoise.micary_Location->ha_fX / ha_fNorm ;
	fFixedNoise[1] = locaSrcFixedNoise.micary_Location->ha_fY / ha_fNorm ;
	fFixedNoise[2] = locaSrcFixedNoise.micary_Location->ha_fZ / ha_fNorm ;
		

	iNpos = locaSrc.ha_numLocation ;
	for(i = 0 ; i < iNpos ; i++){
		//Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~
		//if(locaSrc.micary_Location[i].ha_fZ < 0){ // Z<0�ł����FixedNoise_noise
		//	bFixedNoise_Src = true ;
		//}
		//else{
		  	ha_fNorm = (float)pow((double)locaSrc.micary_Location[i].ha_fX,(double)2.0) + pow((double)locaSrc.micary_Location[i].ha_fY,(double)2.0)+pow((double)locaSrc.micary_Location[i].ha_fZ,(double)2.0) ;
		  	ha_fNorm = (float)pow((double)ha_fNorm,(double)0.5) ;
	
		  	fSrc[0] = locaSrc.micary_Location[i].ha_fX / ha_fNorm ;
			fSrc[1] = locaSrc.micary_Location[i].ha_fY / ha_fNorm ;
			fSrc[2] = locaSrc.micary_Location[i].ha_fZ / ha_fNorm ;

			ha_fP = _InnerProductVec(fSrc,fFixedNoise) ;
			
			if(fabs(ha_fP) > 0.999){
		  		locaSrc.ha_idLocation[i]= -1 ;
		  		locaSrc.micary_Location[i].ha_idPos = -1 ;
		  		bFixedNoise_Src = true ;
			}
		//}
	}
	return bFixedNoise_Src ;
	
}

// ------------------------------------------------------------------
// �����s��f�[�^�x�[�X�t�@�C���̓ǂݍ���
int Cexec::ReadSepMatrix(MICARY_PrmFrame psPrmFrame)
{
	HA_String SMfile ;
	SMfile.s = (HA_Char *)calloc(strlen(this->m_file_sm)+1,sizeof(char)) ;
	strcpy(SMfile.s,this->m_file_sm) ;
	SMfile.slen = strlen(this->m_file_sm) ;

	MICARY_TFDBFileHeader * psSMDBHead ;	//TF�f�[�^�x�[�X�w�b�_(���ʃt�H�[�}�b�g)
	psSMDBHead = (MICARY_TFDBFileHeader *)calloc(1,sizeof(MICARY_TFDBFileHeader)) ;
	
	if(m_iFormat_InitW == __FILE_FORMAT_HARK){
		if(!cHark.ReadInfo(SMfile.s,psSMDBHead,
							&m_psSMDB->micary_locaSource ,
							&m_psSMDB->micary_locaMic,
							&m_psSMDB->micary_smSeparation.micary_iaFreqLinex)){
			printf("Can not read SM Database.\n") ;
			if(psSMDBHead){
				free(psSMDBHead) ;
				psSMDBHead = NULL ;
			}
			if(SMfile.s){
				free(SMfile.s) ;
				SMfile.s = NULL ;
			}

			return 0 ;
		}
		
		if(!cHark.ReadData(SMfile.s,
			&m_psSMDB->micary_smSeparation.micary_cmpaSeparation)){
				printf("Can not read SM Database.\n") ;
			if(psSMDBHead){
				free(psSMDBHead) ;
				psSMDBHead = NULL ;
			}
			if(SMfile.s){
				free(SMfile.s) ;
				SMfile.s = NULL ;
			}

			return 0 ;
		}
	}
	
		//add by Morio 2013/01/20 �p�x�����i�[
		//�p�x�Z�o�̌��_��(0,0,0)�Ƃ���
		float pi = acos(-1.0) ;
		float x,y,z;
		if(m_iSrc_Compare_mode == __COMPARE_MODE_DEG){
			for(unsigned int i = 0; i < m_psSMDB->micary_locaSource.ha_numLocation; i++)
			{
				x = m_psSMDB->micary_locaSource.micary_Location[i].ha_fX;
				y = m_psSMDB->micary_locaSource.micary_Location[i].ha_fY;
				z = m_psSMDB->micary_locaSource.micary_Location[i].ha_fZ;
				m_psSMDB->micary_locaSource.micary_Location[i].ha_fTheta = (float)(atan2(y,x) * 180 / pi);
				m_psSMDB->micary_locaSource.micary_Location[i].ha_fPhi = (float)(atan2(z,sqrt(x*x+y*y)) * 180 / pi);
									
				//cout<<"src"<<i<<"_ang,"<<m_psTFDB->micary_locaSource.micary_Location[i].ha_fTheta<<","<<m_psTFDB->micary_locaSource.micary_Location[i].ha_fPhi <<endl;
				
			}	
		}

	if(psSMDBHead){
		free(psSMDBHead) ;
		psSMDBHead = NULL ;
	}
	if(SMfile.s){
		free(SMfile.s) ;
		SMfile.s = NULL ;
	}
	
	return 1 ;

}

// ----------------------------------------------------------------
bool Cexec::Initialize_Change(int iNch, int iNpos, int iLower_f, int iUpper_f)
{

 	// �������ύX���̔z��̏�����
	int iSize ;
	iSize = (iUpper_f - iLower_f +1)*iNpos*iNch ;
	if(iSize != m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_numValue){	
		// 2012/01/13
		/*
	  	SavedSeparationFree(m_psSepMat) ;
	  	SavedSeparationAlloc(m_psSepMat,iUpper_f-iLower_f+1,iNpos,iNch) ;*/
	  	SavedSeparationAlloc_r(m_psSepMat,iUpper_f-iLower_f+1,iNpos,iNch,
	  							__MICARY_def_MAX_NFL, __MICARY_def_MAX_NSRC+1, __MICARY_def_MAX_NCH) ;
	}

	//���g�����C���z��̍쐬
	if(iUpper_f-iLower_f >= 1){
		for(int k =0 ; k < iUpper_f-iLower_f +1 ; k++ ){
			m_psSepMat->micary_tdTFDB.micary_iaFreqLine.ha_iValue[k] = iLower_f + k ;
		}
	}

	return true ;
}
// ----------------------------------------------------------------
// �x�N�g���̓��ς̌v�Z
HA_Float Cexec::_InnerProductVec(MICARY_FloatArray micary_faVec1,MICARY_FloatArray micary_faVec2)
{
	int k ;
	HA_Float ha_fRet,tmp ;

	if(micary_faVec1.ha_numValue != micary_faVec2.ha_numValue){
			return -1.0 ;
	}
	
	ha_fRet = 0.0 ;

	for(k = 0 ; k < micary_faVec1.ha_numValue ; k++){
		tmp = micary_faVec1.ha_fValue[k] * micary_faVec2.ha_fValue[k] ;
		ha_fRet = ha_fRet + tmp ;
	}

	return ha_fRet ;

}
// 2011/11/16 added by N.Tanaka
HA_Float Cexec::_InnerProductVec(float fVec1[3],float fVec2[3])
{
	int k ;
	HA_Float ha_fRet;
	ha_fRet = 0.0 ;
	for(k = 0 ; k < 3 ; k++){
		ha_fRet += fVec1[k] * fVec2[k];
	}
	return ha_fRet ;
}
// -----------------------------------------------------------------
// DB�����ԋ߂�����Index���擾
/*
HA_Integer Cexec::GetNearestPos(MICARY_Location micary_loCur, MICARY_LocationArray *micary_laDB)
{
	
	HA_Float ha_fMaxP ,ha_fP,ha_fNorm ;
	HA_Integer ha_iMax ;
	MICARY_FloatArray laCurN , laPosN ;
	int k ;

	ha_fMaxP = -32768.0 ;
	FloatArrayAlloc(&laCurN,0) ;
	FloatArrayAlloc(&laPosN,0) ;

	FloatArrayAlloc(&laCurN,3) ;
	FloatArrayAlloc(&laPosN,3) ;

	// ���K��
	ha_fNorm = pow((double)micary_loCur.ha_fX,2.0) + pow((double)micary_loCur.ha_fY,2.0)+pow((double)micary_loCur.ha_fZ,2.0) ;
	ha_fNorm = pow((double)ha_fNorm,0.5) ;
	
	laCurN.ha_fValue[0] = micary_loCur.ha_fX / ha_fNorm ;
	laCurN.ha_fValue[1] = micary_loCur.ha_fY / ha_fNorm ;
	laCurN.ha_fValue[2] = micary_loCur.ha_fZ / ha_fNorm ;
	
	//DB���̊e�x�N�g���Ɠ��ς��Ƃ�
	for(k = 0 ; k < micary_laDB->ha_numLocation ; k++){
		// ���K��
	  ha_fNorm = pow((double)micary_laDB->micary_Location[k].ha_fX,2.0) + pow((double)micary_laDB->micary_Location[k].ha_fY,2.0)+pow((double)micary_laDB->micary_Location[k].ha_fZ,2.0) ;
	  ha_fNorm = pow((double)ha_fNorm,0.5) ;
		
		laPosN.ha_fValue[0] = micary_laDB->micary_Location[k].ha_fX/ha_fNorm ;
		laPosN.ha_fValue[1] = micary_laDB->micary_Location[k].ha_fY/ha_fNorm ;
		laPosN.ha_fValue[2] = micary_laDB->micary_Location[k].ha_fZ/ha_fNorm ;

		ha_fP = _InnerProductVec(laPosN,laCurN) ;
		if(ha_fP > ha_fMaxP){
			ha_fMaxP = ha_fP ;
			ha_iMax = k ;
		}
	}

	FloatArrayFree(&laCurN) ;
	FloatArrayFree(&laPosN) ;
	// add 2009.7.9
	if(ha_fMaxP <= -32768.0){
		ha_iMax = __INVALID_SRC_ID ;
	}
	return ha_iMax ;
	
}
*/

HA_Integer Cexec::GetNearestPos(MICARY_Location micary_loCur, MICARY_LocationArray *micary_laDB)
{
	
	if (m_iSrc_Compare_mode == __COMPARE_MODE_DEG)
	{
		HA_Float ha_fMaxP ,ha_fP,ha_fNorm ;
		HA_Integer ha_iMax ;
		int k ;
		float fCurN[3], fPosN[3];

		ha_fMaxP = -32768.0 ;

		// ���K��
		ha_fNorm = pow((double)micary_loCur.ha_fX,2.0) + pow((double)micary_loCur.ha_fY,2.0)+pow((double)micary_loCur.ha_fZ,2.0) ;
		ha_fNorm = pow((double)ha_fNorm,0.5) ;
		
		fCurN[0] = micary_loCur.ha_fX / ha_fNorm ;
		fCurN[1] = micary_loCur.ha_fY / ha_fNorm ;
		fCurN[2] = micary_loCur.ha_fZ / ha_fNorm ;



		//DB���̊e�x�N�g���Ɠ��ς��Ƃ�
		for(k = 0 ; k < micary_laDB->ha_numLocation ; k++)
		{
			//���ʊp�Ő��� Changed by Morio 2013/01/20
			if(((360.0 - abs(micary_loCur.ha_fTheta - micary_laDB->micary_Location[k].ha_fTheta)) < m_fSearch_azimuth) || (abs(micary_loCur.ha_fTheta - micary_laDB->micary_Location[k].ha_fTheta) < m_fSearch_azimuth))
			{
				//�p�Ő��� Changed by Morio 2013/01/20
				if(abs(micary_loCur.ha_fPhi - micary_laDB->micary_Location[k].ha_fPhi) < m_fSearch_elevation)
				{
					// ���K��
				  ha_fNorm = pow((double)micary_laDB->micary_Location[k].ha_fX,2.0) + pow((double)micary_laDB->micary_Location[k].ha_fY,2.0)+pow((double)micary_laDB->micary_Location[k].ha_fZ,2.0) ;
				  ha_fNorm = pow((double)ha_fNorm,0.5) ;
					
					fPosN[0] = micary_laDB->micary_Location[k].ha_fX/ha_fNorm ;
					fPosN[1] = micary_laDB->micary_Location[k].ha_fY/ha_fNorm ;
					fPosN[2] = micary_laDB->micary_Location[k].ha_fZ/ha_fNorm ;

					ha_fP = _InnerProductVec(fPosN,fCurN) ;
					if(ha_fP > ha_fMaxP){
						if ((ha_fP - ha_fMaxP) > 0.0001 )
						{
							ha_fMaxP = ha_fP ;
							ha_iMax = k ;
						}
						
					}
				}
			}

		}

		// add 2009.7.9 
		// Changed by Morio
		if(ha_fMaxP <= -32767.999){

			ha_iMax = __INVALID_SRC_ID ;
		}
		return ha_iMax ;
	}
	else
	{
		HA_Integer ha_iMin;
		HA_Float ha_fMinD;
	    ha_fMinD = 32768.0 ;
	    
	    for(int k = 0; k < micary_laDB->ha_numLocation; k++)
	    {
	      int maxdist = 0;
	      for(int i=0; i<3; i++)
	      {
	        int tmpdist;
	        if(micary_loCur.tfindex[i] > micary_laDB->micary_Location[k].tfindex[i])
	        {
	          tmpdist = min(abs(micary_loCur.tfindex[i] - micary_laDB->micary_Location[k].tfindex[i]),
	                        abs(micary_loCur.tfindex[i] - micary_laDB->micary_Location[k].tfindex_max[i]) + abs(micary_laDB->micary_Location[k].tfindex[i] - micary_laDB->micary_Location[k].tfindex_min[i]));
	        }else{
	          tmpdist = min(abs(micary_loCur.tfindex[i] - micary_laDB->micary_Location[k].tfindex[i]),
	                        abs(micary_loCur.tfindex[i] - micary_laDB->micary_Location[k].tfindex_min[i]) + abs(micary_laDB->micary_Location[k].tfindex[i] - micary_laDB->micary_Location[k].tfindex_max[i]));
	        }
	        maxdist = (maxdist < tmpdist)?tmpdist:maxdist;
	      }
	      if (maxdist < ha_fMinD)
	      {
			ha_fMinD = maxdist;
			ha_iMin = k;
		  }

	    }
	    
	    if(ha_fMinD > 32767.999){

			ha_iMin = __INVALID_SRC_ID ;
		}
		
		/*
		if(m_bCheck_distance == true)
		{
			bool bcheck;
			m_iSrc_Compare_mode = __COMPARE_MODE_DEG;
			bcheck = _IsThreshPos(micary_loCur,micary_laDB->micary_Location[ha_iMin]);
			
			cout << "---------Check_distance----------"<<endl;
			cout << "X " << micary_loCur.tfindex[0] << " " << micary_loCur.ha_fX << " " << micary_laDB->micary_Location[ha_iMin].ha_fX <<endl;
			cout << "Y " << micary_loCur.tfindex[1] << " " << micary_loCur.ha_fY << " " << micary_laDB->micary_Location[ha_iMin].ha_fY <<endl;
			cout << "Z " << micary_loCur.tfindex[2] << " " << micary_loCur.ha_fZ << " " << micary_laDB->micary_Location[ha_iMin].ha_fZ <<endl;
			
			m_iSrc_Compare_mode = __COMPARE_MODE_TFINDEX;
			if(bcheck == false)
				cout << "CHECK_TFINDEX_DISTANCE: Not match Transfer function file localization and separation !!  "<< endl;
		}
		*/
		
		return ha_iMin ;
	}
}



// 2013/01/23 Morio �������c���Ă��邩�ǂ����̔���ɗp����
HA_Integer Cexec::GetNearestPos_ALLSearch(MICARY_Location micary_loCur, MICARY_LocationArray *micary_laDB)
{
	if(m_iSrc_Compare_mode == __COMPARE_MODE_DEG){
		HA_Float ha_fMaxP ,ha_fP,ha_fNorm ;
		HA_Integer ha_iMax ;
		int k ;
		float fCurN[3], fPosN[3];

		ha_fMaxP = -32768.0 ;

		// ���K��
		ha_fNorm = pow((double)micary_loCur.ha_fX,2.0) + pow((double)micary_loCur.ha_fY,2.0)+pow((double)micary_loCur.ha_fZ,2.0) ;
		ha_fNorm = pow((double)ha_fNorm,0.5) ;
		
		fCurN[0] = micary_loCur.ha_fX / ha_fNorm ;
		fCurN[1] = micary_loCur.ha_fY / ha_fNorm ;
		fCurN[2] = micary_loCur.ha_fZ / ha_fNorm ;



		//DB���̊e�x�N�g���Ɠ��ς��Ƃ�
		for(k = 0 ; k < micary_laDB->ha_numLocation ; k++)
		{
		
					// ���K��
				  ha_fNorm = pow((double)micary_laDB->micary_Location[k].ha_fX,2.0) + pow((double)micary_laDB->micary_Location[k].ha_fY,2.0)+pow((double)micary_laDB->micary_Location[k].ha_fZ,2.0) ;
				  ha_fNorm = pow((double)ha_fNorm,0.5) ;
					
					fPosN[0] = micary_laDB->micary_Location[k].ha_fX/ha_fNorm ;
					fPosN[1] = micary_laDB->micary_Location[k].ha_fY/ha_fNorm ;
					fPosN[2] = micary_laDB->micary_Location[k].ha_fZ/ha_fNorm ;

					ha_fP = _InnerProductVec(fPosN,fCurN) ;
					if(ha_fP > ha_fMaxP){
						ha_fMaxP = ha_fP ;
						ha_iMax = k ;
					}

		}

		// add 2009.7.9 
		// Changed by Morio
		if(ha_fMaxP <= -32767.999){

			ha_iMax = __INVALID_SRC_ID ;
		}
		return ha_iMax ;
	}
	else
	{
		HA_Integer inearest_id ;
		inearest_id = GetNearestPos(micary_loCur,micary_laDB);
		return inearest_id ;
	}
	
}

// --------------------------------------------------------------------------------
// DB����f�[�^�̎擾(�ł��߂����W��ǂ�)
int Cexec::GetNearestTFMatrixFromDB(MICARY_TFDatabase *OutTF,MICARY_TFDatabase *TFDB)
{
	// OutTF�ɂ͎��g�����C���C�������W�z��͓����Ă���
	int i ,m,k,l,Npos,Nmic,NposD,Nfreq,NfreqD,iCount;
	bool bFixedNoise_check ;
	MICARY_Location loctmp ;
	int iDBpos ;

	bFixedNoise_check = false ;
	Npos = OutTF->micary_locaSource.ha_numLocation ;
	NposD = TFDB->micary_locaSource.ha_numLocation ;
	Nmic = TFDB->micary_locaMic.ha_numLocation ;
	NfreqD = TFDB->micary_iaFreqLine.ha_numValue ;
	Nfreq = OutTF->micary_iaFreqLine.ha_numValue ;
	
	if(OutTF->micary_cmpaTF.ha_numValue != Npos*Nmic*Nfreq){
	  cout << "OutTF-size = " << OutTF->micary_cmpaTF.ha_numValue << endl ;
	  cout << "Srcsize = " << Npos << endl ;
	  cout << "freqsize = " << Nfreq << endl ;
	  cout << "micsize = " << Nmic << endl ;
	  cout << "Size error." << endl ;
	  return 0 ;
	}

	iCount = 0 ;

	for(i = 0 ; i < Npos ; i++){
		
		memcpy(&loctmp,&OutTF->micary_locaSource.micary_Location[i],sizeof(MICARY_Location)) ;
	//Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~
		//if(OutTF->micary_locaSource.ha_idLocation[i] == -1){
		//	bFixedNoise_check = true ;
		//}
		
		/*
//		if(loctmp.ha_fZ < 0 && OutTF->micary_locaSource.ha_idLocation[i] != -1){
				OutTF->micary_locaSource.ha_idLocation[i] = -1 ;	//FixedNoise�m�C�Y��ID = -1
		}
//		if(loctmp.ha_fZ < 0 || OutTF->micary_locaSource.ha_idLocation[i] == -1){
			bFixedNoise_check = true ;
		}
		*/

		// ��ԋ߂�ID��DB����擾

		iDBpos = GetNearestPos(loctmp,&TFDB->micary_locaSource) ;

	
		if(iDBpos != __INVALID_SRC_ID){
		       for(k = 0 ; k < Nfreq ; k++){
				for (l = 0 ; l < NfreqD ; l++){
					if(OutTF->micary_iaFreqLine.ha_iValue[k] == TFDB->micary_iaFreqLine.ha_iValue[l]){
						for(m = 0 ; m < Nmic ; m++){
							// TF�z���freq->src->mic�̏��ɕ���ł���
							OutTF->micary_cmpaTF.ha_cmpValue[m+(Npos*Nmic)*k+i*Nmic].re = 
									TFDB->micary_cmpaTF.ha_cmpValue[m+(NposD*Nmic)*l+iDBpos*Nmic].re ;
								OutTF->micary_cmpaTF.ha_cmpValue[m+(Npos*Nmic)*k+i*Nmic].im = 
									TFDB->micary_cmpaTF.ha_cmpValue[m+(NposD*Nmic)*l+iDBpos*Nmic].im ;
						}
						break ;
					}
				}
		       }
		       iCount++ ;
		}
	}

	//Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~
	//if(this->m_bFlgFixedNoise){
	//	if(bFixedNoise_check == false){	//��������͂��Ȃ̂Ƀt�@�C���ɂȂ���΃G���[
	//	  cout << "Cannot find FixedNoise TF data." << endl ;
	//		return 0 ;
	//	}
	//}

	//Changed by Morio 2013/01/23 
	if(iCount != Npos){
	  cout << "Can't find TF data! Please check GHDSS Update Parameters.";
	  cout << "Detect size = "<<iCount << "/ Source size = "<< Npos <<endl ;
		return 0 ; //������������Ȃ���΃G���[
	}
	else {
		return 1 ;
	}
}

// --------------------------------------------------------------------------
// DB����f�[�^�̎擾(�ł��߂����W��ǂ�)
int Cexec::GetNearestTFMatrixFromDB2(MICARY_TFDatabase *OutTF,MICARY_TFDatabase *TFDB, int idsrc)
{
	// OutTF�ɂ͎��g�����C���C�������W�z��͓����Ă���
	int i ,m,k,l,Npos,Nmic,NposD,Nfreq,NfreqD;
	bool bFixedNoise_check ;
	MICARY_Location loctmp ;
	int iDBpos ;

	bFixedNoise_check = false ;
	Npos = OutTF->micary_locaSource.ha_numLocation ;
	NposD = TFDB->micary_locaSource.ha_numLocation ;
	Nmic = TFDB->micary_locaMic.ha_numLocation ;
	NfreqD = TFDB->micary_iaFreqLine.ha_numValue ;
	Nfreq = OutTF->micary_iaFreqLine.ha_numValue ;
	
	if(OutTF->micary_cmpaTF.ha_numValue != Npos*Nmic*Nfreq){
	  cout << "OutTF-size = " << OutTF->micary_cmpaTF.ha_numValue << endl ;
	  cout << "Srcsize = " << Npos << endl ;
	  cout << "freqsize = " << Nfreq << endl ;
	  cout << "micsize = " << Nmic << endl ;
	  cout << "Size error." << endl ;
	  return 0 ;
	}

	//Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~

	//if(OutTF->micary_locaSource.micary_Location[idsrc].ha_fZ < 0 && OutTF->micary_locaSource.ha_idLocation[idsrc] != -1){
	//	OutTF->micary_locaSource.ha_idLocation[idsrc] = -1 ;	//FixedNoise�m�C�Y��ID = -1
	//}
	//if(OutTF->micary_locaSource.micary_Location[idsrc].ha_fZ < 0 || OutTF->micary_locaSource.ha_idLocation[idsrc] == -1){
	//	bFixedNoise_check = true ;
	//}

	// ��ԋ߂�ID��DB����擾
	iDBpos = GetNearestPos(OutTF->micary_locaSource.micary_Location[idsrc],&TFDB->micary_locaSource) ;
	if(iDBpos != __INVALID_SRC_ID){
		for(k = 0 ; k < Nfreq ; k++){
			for (l = 0 ; l < NfreqD ; l++){
				if(OutTF->micary_iaFreqLine.ha_iValue[k] == TFDB->micary_iaFreqLine.ha_iValue[l]){
					for(m = 0 ; m < Nmic ; m++){
						// TF�z���freq->src->mic�̏��ɕ���ł���
						OutTF->micary_cmpaTF.ha_cmpValue[m+(Npos*Nmic)*k+idsrc*Nmic].re = 
							TFDB->micary_cmpaTF.ha_cmpValue[m+(NposD*Nmic)*l+iDBpos*Nmic].re ;
						OutTF->micary_cmpaTF.ha_cmpValue[m+(Npos*Nmic)*k+idsrc*Nmic].im = 
							TFDB->micary_cmpaTF.ha_cmpValue[m+(NposD*Nmic)*l+iDBpos*Nmic].im ;
					}
					break ;
				}
			}
		}
	}


	//Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~
	//if(this->m_bFlgFixedNoise){
	//	if(bFixedNoise_check == false){	//��������͂��Ȃ̂Ƀt�@�C���ɂȂ���΃G���[
	//	  cout << "Cannot find FixedNoise TF data." << endl ;
	//		return 0 ;
	//	}
	//}

	return 1 ;
}


// --------------------------------------------------------------------------
void Cexec::SetiMicPosShift(int ivalue)
{
	iMicPosShift = ivalue ;
	return ;
}

// --------------------------------------------------------------------------
int Cexec::GetNearestSepMatrixFromDB(MICARY_SavedSeparation *OutSM, MICARY_SMDatabase *SMDB)
{

	int i ,m,k,l,Npos,Nmic,NposD,Nfreq,NfreqD,iCount;
	MICARY_Location loctmp ;
	int iDBpos ;
	bool bFixedNoise_check ;
	bFixedNoise_check = false ;

	Npos = OutSM->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	NposD = SMDB->micary_locaSource.ha_numLocation ;
	Nmic = SMDB->micary_locaMic.ha_numLocation ;
	NfreqD = SMDB->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;
	Nfreq = OutSM->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;

	if(OutSM->micary_smSeparation.micary_cmpaSeparation.ha_numValue != Npos*Nmic*Nfreq){
	  cout << "OutSM-size = " << OutSM->micary_smSeparation.micary_cmpaSeparation.ha_numValue << endl ;
	  cout << "Srcsize = " << Npos << endl ;
	  cout << "freqsize = " << Nfreq << endl ;
	  cout << "micsize = " << Nmic << endl ;
	  cout << "Size error." << endl ;
	return 0 ;
	}

	iCount = 0 ;

	for(i = 0 ; i < Npos ; i++){
		memcpy(&loctmp,&OutSM->micary_tdTFDB.micary_locaSource.micary_Location[i],sizeof(MICARY_Location)) ;
	
		//Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~

		//if(loctmp.ha_fZ < 0 && OutSM->micary_tdTFDB.micary_locaSource.ha_idLocation[i] != -1){
		//		OutSM->micary_tdTFDB.micary_locaSource.ha_idLocation[i] = -1 ;	//FixedNoise�m�C�Y��ID = -1
		//}
		//if(loctmp.ha_fZ < 0 || OutSM->micary_tdTFDB.micary_locaSource.ha_idLocation[i] == -1){
		//	bFixedNoise_check = true ;
		//}

		// ��ԋ߂�ID��DB����擾
		iDBpos = GetNearestPos(loctmp,&SMDB->micary_locaSource) ;
		if(iDBpos != __INVALID_SRC_ID){
		       for(k = 0 ; k < Nfreq ; k++){
				for (l = 0 ; l < NfreqD ; l++){
					if(OutSM->micary_tdTFDB.micary_iaFreqLine.ha_iValue[k] == SMDB->micary_smSeparation.micary_iaFreqLinex.ha_iValue[l]){
						
						for(m = 0 ; m < Nmic ; m++){
							
							// SepMat�z���freq->mic->src�̏��ɕ���ł���
							OutSM->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[m*Npos+(Npos*Nmic)*k+i].re = 
									SMDB->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[m*NposD+(NposD*Nmic)*l+iDBpos].re ;
							OutSM->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[m*Npos+(Npos*Nmic)*k+i].im = 
									SMDB->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[m*NposD+(NposD*Nmic)*l+iDBpos].im ;

						}
						break ;
					}
				}
		       }
	
		       iCount++ ;
		}
	}

	//Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~
	//if(this->m_bFlgFixedNoise){
	//	if(bFixedNoise_check == false){	//��������͂��Ȃ̂Ƀt�@�C���ɂȂ���΃G���[
	//	  cout << "Cannot find FixedNoise TF data." << endl ;
	//		return 0 ;
	//	}
	//}
	if(iCount != Npos){
	  cout << "Cannot find the same number of TF data.";
	  cout << "Detect size = "<<iCount << "/ Source size = "<< Npos <<endl ;
		return 0 ; //������������Ȃ���΃G���[
	}
	else {
	 
		return 1 ;
	}
	
}
// ----------------------------------------------------------------------------------
// 2012/01/13 added
int Cexec::GetNearestSepMatrixFromDB2(MICARY_SavedSeparation *OutSM, MICARY_SMDatabase *SMDB, int idsrc)
{
	int i ,m,k,l,Npos,Nmic,NposD,Nfreq,NfreqD,iCount;
	MICARY_Location loctmp ;
	int iDBpos ;
	bool bFixedNoise_check ;
	bFixedNoise_check = false ;

	Npos = OutSM->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	NposD = SMDB->micary_locaSource.ha_numLocation ;
	Nmic = SMDB->micary_locaMic.ha_numLocation ;
	NfreqD = SMDB->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;
	Nfreq = OutSM->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;

	if(OutSM->micary_smSeparation.micary_cmpaSeparation.ha_numValue != Npos*Nmic*Nfreq){
	  cout << "OutSM-size = " << OutSM->micary_smSeparation.micary_cmpaSeparation.ha_numValue << endl ;
	  cout << "Srcsize = " << Npos << endl ;
	  cout << "freqsize = " << Nfreq << endl ;
	  cout << "micsize = " << Nmic << endl ;
	  cout << "Size error." << endl ;
	return 0 ;
	}

	iCount = 0 ;

	//Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~

	//if(OutSM->micary_tdTFDB.micary_locaSource.micary_Location[idsrc].ha_fZ < 0 && OutSM->micary_tdTFDB.micary_locaSource.ha_idLocation[i] != -1){
	//		OutSM->micary_tdTFDB.micary_locaSource.ha_idLocation[i] = -1 ;	//FixedNoise�m�C�Y��ID = -1
	//}
	//if(OutSM->micary_tdTFDB.micary_locaSource.micary_Location[idsrc].ha_fZ < 0 || OutSM->micary_tdTFDB.micary_locaSource.ha_idLocation[i] == -1){
	//	bFixedNoise_check = true ;
	//}

	// ��ԋ߂�ID��DB����擾
	iDBpos = GetNearestPos(OutSM->micary_tdTFDB.micary_locaSource.micary_Location[idsrc],&SMDB->micary_locaSource) ;
	
	if(iDBpos != __INVALID_SRC_ID){
		for(k = 0 ; k < Nfreq ; k++){
			for (l = 0 ; l < NfreqD ; l++){
				if(OutSM->micary_tdTFDB.micary_iaFreqLine.ha_iValue[k] == SMDB->micary_smSeparation.micary_iaFreqLinex.ha_iValue[l]){
					for(m = 0 ; m < Nmic ; m++){
						// SepMat�z���freq->mic->src�̏��ɕ���ł���
						OutSM->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[m*Npos+(Npos*Nmic)*k+idsrc].re = 
								SMDB->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[m*NposD+(NposD*Nmic)*l+iDBpos].re ;
						OutSM->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[m*Npos+(Npos*Nmic)*k+idsrc].im = 
								SMDB->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[m*NposD+(NposD*Nmic)*l+iDBpos].im ;
					}
					break ;
				}
			}
		}
		iCount++ ;
	}

	//�@Changed by Morio 2013/01/23�@GHDSS�ł�Fixed Noise�͔p�~
	//if(this->m_bFlgFixedNoise){
	//	if(bFixedNoise_check == false){	//��������͂��Ȃ̂Ƀt�@�C���ɂȂ���΃G���[
	//	  cout << "Cannot find FixedNoise TF data." << endl ;
	//		return 0 ;
	//	}
	//}


	// changed 2012/02/29
	/*
	if(iCount != Npos){
		cout << "Cannot find the same number of TF data.";
	  cout << "Detect size = "<<iCount << "/ Source size = "<< Npos <<endl ;
		return 0 ; //������������Ȃ���΃G���[
	}
	else {
	 
		return 1 ;
	}
	*/
	if(iCount != 1){
		cout << "Cannot find the same number of TF data.";
		return 0 ; //������������Ȃ���΃G���[
	}
	else {
	 
		return 1 ;
	}
	
}


// ----------------------------------------------------------------------------------
bool Cexec::SetPrmUpdate(int iUpdateMTF_CONJ, int iUpdateMW, int iSrc_Compare_mode, float fThred_angle, int iUpdate_tfindex_distance, float fSearch_azimuth ,float fSearch_elevation, bool bCheck_distance)
{
	// INITW�쐬���@ = CALC����UpdateMethod->ID
	m_iUpdateMethodTF_CONJ = iUpdateMTF_CONJ ;
	m_iUpdateMethodW = iUpdateMW ;
	m_iSrc_Compare_mode = iSrc_Compare_mode ; // add
	m_fThred_angle = fThred_angle ;
	m_fThred_TFIndex = iUpdate_tfindex_distance; // add
	m_fSearch_azimuth = fSearch_azimuth ;
	m_fSearch_elevation = fSearch_elevation ;
	m_bCheck_distance = bCheck_distance;  // add
	return true;
}
// ----------------------------------------------------------------------------------
int Cexec::InitExportW(const char *cFilename,MICARY_PrmFrame *psPrmFrame)
{
	int ii ;

	if(!m_psSepMat){
		return 0 ;
	}

	//int iNLoc = 360 / __SAVED_LOC_THETA_PITCH+1 ;
	int iNmic = m_psSepMat->micary_smSeparation.ha_numInput ;
	int iNfreq = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;

	//�o�̓t�@�C���̐ݒ�
	if(cFilename){
		HA_String file ; // TF �t�@�C����
		file.s = (HA_Char *)calloc(strlen(cFilename)+1,sizeof(char)) ;
		strcpy(file.s,cFilename) ;
		file.slen = strlen(cFilename) ;
		
		MICARY_SMDatabase  *psDum ;	//�_�~�[�f�[�^
		
		psDum = (MICARY_SMDatabase *)calloc(1,sizeof(MICARY_SMDatabase)) ;
		SMDatabaseAlloc(psDum,0,0,0) ;
		SMDatabaseAlloc(psDum,iNfreq,1,iNmic) ;

		//�}�C�N���W
		for(ii = 0 ; ii < iNmic ; ii++){
			psDum->micary_locaMic.ha_idLocation[ii]=
				m_updateW->micary_dbSeparation.micary_locaMic.ha_idLocation[ii] ;
			psDum->micary_locaMic.micary_Location[ii].ha_fX = 
				m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[ii].ha_fX ;
			psDum->micary_locaMic.micary_Location[ii].ha_fY = 
				m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[ii].ha_fY ;
			psDum->micary_locaMic.micary_Location[ii].ha_fZ = 
				m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[ii].ha_fZ ;
			psDum->micary_locaMic.micary_Location[ii].ha_idPos = 
				m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[ii].ha_idPos ;
		}
		//���g�����C��
		for(ii = 0 ; ii < iNfreq ; ii++){
			psDum->micary_smSeparation.micary_iaFreqLinex.ha_iValue[ii] = 
				m_updateW->micary_dbSeparation.micary_smSeparation.micary_iaFreqLinex.ha_iValue[ii] ;
		}

		// dummy source
		psDum->micary_locaSource.ha_idLocation[0]=
				m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[0] ;
		psDum->micary_locaSource.micary_Location[0].ha_fX = 
		               	m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[0].ha_fX ;
		  
		psDum->micary_locaSource.micary_Location[0].ha_fY = 
		                m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[0].ha_fY ;

		psDum->micary_locaSource.micary_Location[0].ha_fZ =
		                  m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[0].ha_fZ ;

		psDum->micary_locaSource.micary_Location[0].ha_idPos = 
				  m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[0].ha_idPos ;

		for(ii = 0 ; ii < psDum->micary_smSeparation.micary_cmpaSeparation.ha_numValue ; ii++){
		  psDum->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii].re = 0.0 ;
		  psDum->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii].im = 0.0 ;
		}

		if(m_iFormat_ExportW == __FILE_FORMAT_HARK){
		  	MICARY_TFDBFileHeader * psH ;	//�f�[�^�x�[�X�w�b�_
			psH = (MICARY_TFDBFileHeader *)calloc(1,sizeof(MICARY_TFDBFileHeader)) ;
			psH->lFs = (int)psPrmFrame->ha_fFs ;
			psH->lNFFT = psPrmFrame->ha_numNFFT ;
			psH->lNMic =iNmic ;
		        psH->lNSrc = 1 ;
			psH->lNFreq = iNfreq ;
			psH->iVersion = 1 ;
			psH->iFileType = FORMAT_TYPE_W ;

			if(!cHark.WriteData(file.s,psH,
					    psDum->micary_smSeparation.micary_cmpaSeparation,
					    psDum->micary_smSeparation.micary_iaFreqLinex,
					    psDum->micary_locaSource,
					    psDum->micary_locaMic,
					    "",
					    "",
					    "",
					    "",
					    "",
					    "GHDSS_EXPORT")){
			  if(psH){
			      free(psH) ;
			      psH = NULL ;
			  }
			  if(file.s){
			      free(file.s) ;
			      file.s = NULL ;
			 }
			  SMDatabaseFree(psDum) ;
			  free(psDum) ;
			  psDum = NULL ;
			  cout << "Fail to Export file as HARK format." << endl ;
			  return false ;
		      }
		}

	
		if(file.s){
			free(file.s) ;
			file.s = NULL ;
		}
		SMDatabaseFree(psDum) ;
		
		free(psDum) ;
		psDum = NULL ;
		
	}

	return 1 ;
	
}
// ----------------------------------------------------------------------------------
int Cexec::InitUpdateW()
{
	// Changed by Morio. 3�������Ή� 2013/01/17
	int ii ;

	if(!m_psSepMat){
		return 0 ;
	}
	int iNLoc = m_psTFDB->micary_locaSource.ha_numLocation ; //�`�B�֐��̔z�u�Ɠ�����
	int iNmic = m_psSepMat->micary_smSeparation.ha_numInput ;
	int iNfreq = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;

	UpdateSeparationMatAlloc(m_updateW,iNfreq,iNLoc,iNmic) ;

	//�X�V�����̏�����
	for(ii = 0 ; ii < m_updateW->micary_iaUpdate.ha_numValue; ii++){
		m_updateW->micary_iaUpdate.ha_iValue[ii] = __INVALID_SRC_ID ;
	}

	//�}�C�N�ʒu�̑��
	for(ii = 0 ; ii < iNmic ; ii++){
		m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[ii].ha_fX =
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_fX ;
		m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[ii].ha_fY =
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_fY ;
		m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[ii].ha_fZ =
			m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_fZ ;
		m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[ii].ha_idPos =
		  m_psSepMat->micary_tdTFDB.micary_locaMic.micary_Location[ii].ha_idPos; 
		m_updateW->micary_dbSeparation.micary_locaMic.ha_idLocation[ii] =
			m_psSepMat->micary_tdTFDB.micary_locaMic.ha_idLocation[ii] ;
	}

	// ���g�����C���̑��
	for(ii = 0 ; ii < iNfreq; ii++){
		m_updateW->micary_dbSeparation.micary_smSeparation.micary_iaFreqLinex.ha_iValue[ii] =
				m_psSepMat->micary_tdTFDB.micary_iaFreqLine.ha_iValue[ii] ;
	}


	//Changed by Morio. �����ʒu��TF�Ɠ����Ƃ���.�p�x�������Ŏ擾


	for(ii = 0 ; ii < iNLoc ; ii++){
		m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fX = m_psTFDB->micary_locaSource.micary_Location[ii].ha_fX ;
		m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fY = m_psTFDB->micary_locaSource.micary_Location[ii].ha_fY ;
		m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fZ = m_psTFDB->micary_locaSource.micary_Location[ii].ha_fZ ;
		m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fTheta = m_psTFDB->micary_locaSource.micary_Location[ii].ha_fTheta ;
		m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fPhi = m_psTFDB->micary_locaSource.micary_Location[ii].ha_fPhi ;
		
		//TFIndex
		for (int jj=0; jj < 3 ; jj++ ){
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].tfindex[jj] = m_psTFDB->micary_locaSource.micary_Location[ii].tfindex[jj]  ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].tfindex_min[jj] = m_psTFDB->micary_locaSource.micary_Location[ii].tfindex_min[jj]  ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].tfindex_max[jj] = m_psTFDB->micary_locaSource.micary_Location[ii].tfindex_max[jj]  ;
		}
		
		//m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].micary_LocationFlag = m_psTFDB->micary_locaSource.micary_Location.micary_LocationFlag ;
		//m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_idPos = m_psTFDB->micary_locaSource.micary_Location.ha_idPos ;
		//m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_idDir = m_psTFDB->micary_locaSource.micary_Location.ha_idDir ;
		//cout<<"upW"<<ii<<"_ang,"<<m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fTheta<<","<<m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fPhi <<endl;
	}
	

	//����ID�̏�����
	for(ii = 0 ; ii < iNLoc ; ii++){
		m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[ii] = ii ;//__INVALID_SRC_ID ;
		m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_idPos = ii ;
	}
	return 1 ;
	
}	
// ---------------------------------------------------------------------------------------
// 2012/01/13 added
// TF_CONJ��W�̈����p��
int Cexec::ChangeSrcPos_TF_CONJ_W(MICARY_LocationArray plaSrcPosCur, MICARY_PrmFrame psPrmFrame)
{
	int i, k, j, m,  n ;
	int Npos_ini,Npos_cur,Nmic,Nfreq,iLower_f,iUpper_f ;
	//MICARY_ComplexArray tmp_cmpaTFDB ;
	//MICARY_LocationArray plaSrcPosIni ;
	bool bFound, bRtnPos, bRtnID ;
	int iRtn = 2 ;
	int iInherit_TF_CONJ = 0;
	int iInherit_W = 0;

	Nmic = m_psSepMat->micary_smSeparation.ha_numInput ;
	Npos_ini = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue + iLower_f -1 ;
	Nfreq = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;
	Npos_cur = plaSrcPosCur.ha_numLocation ;
	
	// �o�b�N�A�b�v
		// �����ʒu
	//LocationArrayAlloc(&plaSrcPosIni,0) ;
	LocationArrayAlloc_r(&m_plaSrcPosIni,Npos_ini,__MICARY_def_MAX_NSRC+1) ;
	for(n = 0 ; n < Npos_ini ; n++){
	   m_plaSrcPosIni.ha_idLocation[n] = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[n] ;
	   m_plaSrcPosIni.micary_Location[n].ha_fX = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fX ;
	   m_plaSrcPosIni.micary_Location[n].ha_fY = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fY ;
	   m_plaSrcPosIni.micary_Location[n].ha_fZ = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fZ ;
	   m_plaSrcPosIni.micary_Location[n].ha_idPos = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_idPos ;
	   m_plaSrcPosIni.micary_Location[n].ha_fTheta = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fTheta ;
	   m_plaSrcPosIni.micary_Location[n].ha_fPhi = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fPhi ;
	   m_plaSrcPosIni.micary_Location[n].micary_LocationFlag = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].micary_LocationFlag ;
	   
	   //TFIndex
	   for (int jj=0; jj < 3 ; jj++ ){
			m_plaSrcPosIni.micary_Location[n].tfindex[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].tfindex[jj]  ;
			m_plaSrcPosIni.micary_Location[n].tfindex_min[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].tfindex_min[jj]  ;
			m_plaSrcPosIni.micary_Location[n].tfindex_max[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].tfindex_max[jj]  ;
		}
	   
	}
	
		// TF_CONJ
	//ComplexArrayAlloc(&tmp_cmpaTFDB,0) ;
	ComplexArrayAlloc_r(&m_tmp_cmpaTFDB,(int)Nfreq*Npos_ini*Nmic, __MICARY_def_MAX_NCH*(__MICARY_def_MAX_NSRC+1)*__MICARY_def_MAX_NFL) ;	
	for (n = 0 ; n < m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_numValue;n++){
		m_tmp_cmpaTFDB.ha_cmpValue[n].re = m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[n].re ;
		m_tmp_cmpaTFDB.ha_cmpValue[n].im = m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[n].im ;
	}

	// �z�񐔂̐؂�Ȃ���
	if(Npos_ini != Npos_cur){
		Initialize_Change(Nmic,Npos_cur,iLower_f,iUpper_f) ; // �����Œ��g�̓N���A����Ȃ�
		iRtn = 1 ;
	}

	// �������̍X�V
	Cexec ::AssignSrcPos(plaSrcPosCur) ;

	// TF_CONJ�̍X�V
	if (m_iUpdateMethodTF_CONJ == __UPDATE_METHOD_ID){
		for (i = 0 ; i < Npos_cur ; i++){
			bFound = false;
			for (j = 0 ; j < Npos_ini ; j++){
				if (plaSrcPosCur.ha_idLocation[i] == m_plaSrcPosIni.ha_idLocation[j]){
					// changed 2012/02/29
					// 1�����z��Ȃ̂ŁA���������قȂ�ꍇ�̓R�s�[����K�v������
//					if (i != j){	// �����C���f�b�N�X�ł���΃R�s�[����K�v�͂Ȃ�
//						// �O���ID�̓������͈̂����p��
//						for(k = 0 ; k < Nfreq ; k++){
//							for(m = 0 ; m < Nmic ; m++){
//								m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[m+i*Nmic+k*Nmic*Npos_cur].re 
//									= m_tmp_cmpaTFDB.ha_cmpValue[m+j*Nmic+k*Nmic*Npos_ini].re ;
//		   						m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[m+i*Nmic+k*Nmic*Npos_cur].im 
//									= m_tmp_cmpaTFDB.ha_cmpValue[m+j*Nmic+k*Nmic*Npos_ini].im ;
//							}
//						}
//					}
					if (i != j || // �����C���f�b�N�X�ł���΃R�s�[����K�v�͂Ȃ�
						Npos_ini != Npos_cur){	
						// �O���ID�̓������͈̂����p��
						for(k = 0 ; k < Nfreq ; k++){
							for(m = 0 ; m < Nmic ; m++){
								m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[m+i*Nmic+k*Nmic*Npos_cur].re 
									= m_tmp_cmpaTFDB.ha_cmpValue[m+j*Nmic+k*Nmic*Npos_ini].re ;
		   						m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[m+i*Nmic+k*Nmic*Npos_cur].im 
									= m_tmp_cmpaTFDB.ha_cmpValue[m+j*Nmic+k*Nmic*Npos_ini].im ;
							}
						}
					}
					iInherit_TF_CONJ++ ;
					bFound = true;
					break;
				}
			}
			if (!bFound){
				// TF_CONJ�����l��ǂݏo��
				LoadInitialTFMatrix(&psPrmFrame, i);
			}
		}
	}
	else {
		Cexec::InitTFMatrix(&psPrmFrame);
	}
	
	// W�̍X�V
	for(i = 0 ; i < Npos_cur ; i++){
		
		// added 2012/02/29
		bRtnID = false;
		bRtnPos = false;
		
		m_iUpdateMethodWex->ha_iValue[i] = m_iUpdateMethodW ;
		
		if(m_iUpdateMethodW == __UPDATE_METHOD_ID 
				|| m_iUpdateMethodW == __UPDATE_METHOD_ID_POS){
			// changed 2012/02/29
			// 1�����z��Ȃ̂ŁA���������قȂ�ꍇ�̓R�s�[����K�v������
//			// ID�x�[�X�̊m�F
//			if (i < Npos_ini){
//				if (plaSrcPosCur.ha_idLocation[i] == m_plaSrcPosIni.ha_idLocation[i]){
//					bRtnID = true;	// �R�s�[������K�v�Ȃ�
//				}
//			}
			if (i < Npos_ini &&
				Npos_ini == Npos_cur){
				if (plaSrcPosCur.ha_idLocation[i] == m_plaSrcPosIni.ha_idLocation[i]){
					bRtnID = true;	// �R�s�[������K�v�Ȃ�
				}
			}
			if (!bRtnID){
				bRtnID = _CheckSrcID_W(i) ;
			}
		}
		// ���ۂ̍X�V��ID or POS
		if(m_iUpdateMethodW == __UPDATE_METHOD_ID_POS){
			if(bRtnID == false){
				m_iUpdateMethodWex->ha_iValue[i] = __UPDATE_METHOD_POS ;
			}
			else{
				m_iUpdateMethodWex->ha_iValue[i] = __UPDATE_METHOD_ID ;
			}
		}
		if(m_iUpdateMethodWex->ha_iValue[i] == __UPDATE_METHOD_POS){
			// POS�x�[�X�̊m�F
			bRtnPos = _CheckSrcPos_W(i) ;
			if(!bRtnPos){
			  if(m_iUpdateMethodW == __UPDATE_METHOD_ID_POS){
			    m_iUpdateMethodWex->ha_iValue[i] = __UPDATE_METHOD_ID ;
			  }
			}
			
		}
		
		if(bRtnID || bRtnPos){
			iInherit_W++ ;
		}
		else {
			// SepMat�̏����l
			LoadInitialSepMatrix(&psPrmFrame, i);
		}
	}

	if(iRtn == 1){	// �����ύX
		return 1 ;
	}
	else{
		if (iInherit_TF_CONJ != Npos_ini) { // �ʒu���ύX�A���͕s��
			return 2 ;
		}
		else {
			return 0 ;
		}
	}
}
// ---------------------------------------------------------------------------------------
int Cexec::ChangeSrcPos_TF_CONJ(MICARY_LocationArray plaSrcPosCur, MICARY_PrmFrame psPrmFrame)
{
	int n ;
	int Npos_ini,Npos_cur,Nmic,Nfreq,iLower_f,iUpper_f ;
	MICARY_ComplexArray tmp_cmpaTFDB ;
	MICARY_LocationArray plaSrcPosIni ;
	bool bRtn ;
	int iRtn = 2 ;

	Nmic = m_psSepMat->micary_smSeparation.ha_numInput ;
	Npos_ini = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue + iLower_f -1 ;
	Nfreq = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;
	Npos_cur = plaSrcPosCur.ha_numLocation ;
	
	// �o�b�N�A�b�v
	LocationArrayAlloc(&plaSrcPosIni,0) ;
	LocationArrayAlloc(&plaSrcPosIni,Npos_ini) ;
	for(n = 0 ; n < Npos_ini ; n++){
	   plaSrcPosIni.ha_idLocation[n] = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[n] ;
	   plaSrcPosIni.micary_Location[n].ha_fX = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fX ;
	   plaSrcPosIni.micary_Location[n].ha_fY = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fY ;
	   plaSrcPosIni.micary_Location[n].ha_fZ = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fZ ;
	   plaSrcPosIni.micary_Location[n].ha_idPos = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_idPos ;
	   plaSrcPosIni.micary_Location[n].ha_fTheta = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fTheta ;
	   plaSrcPosIni.micary_Location[n].ha_fPhi = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].ha_fPhi ;
	   plaSrcPosIni.micary_Location[n].micary_LocationFlag = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].micary_LocationFlag ;
	   for (int jj=0; jj < 3 ; jj++ ){
			m_plaSrcPosIni.micary_Location[n].tfindex[jj] =  m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].tfindex[jj]  ;
			m_plaSrcPosIni.micary_Location[n].tfindex_min[jj] =  m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].tfindex_min[jj]  ;
			m_plaSrcPosIni.micary_Location[n].tfindex_max[jj] =  m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[n].tfindex_max[jj]  ;
			
	   }
	}
	

	ComplexArrayAlloc(&tmp_cmpaTFDB,0) ;
    	ComplexArrayAlloc(&tmp_cmpaTFDB,(int)Nfreq*Npos_ini*Nmic) ;	
    	for (n = 0 ; n < m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_numValue;n++){
		tmp_cmpaTFDB.ha_cmpValue[n].re = m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[n].re ;
		tmp_cmpaTFDB.ha_cmpValue[n].im = m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[n].im ;
	}

	//�z�񐔂̐؂�Ȃ���
	if(Npos_ini != Npos_cur){
		Initialize_Change(Nmic,Npos_cur,iLower_f,iUpper_f) ;
		iRtn = 1 ;
	}

	//�������ETF_CONJ�̍X�V
	Cexec ::AssignSrcPos(plaSrcPosCur) ;	

	//TF_CONJ�̍X�V
	Cexec :: InitTFMatrix(&psPrmFrame) ;
	bRtn = false ; 
	if(m_iUpdateMethodTF_CONJ == __UPDATE_METHOD_ID){
		bRtn = _CheckSrcID_TF_CONJ(tmp_cmpaTFDB,plaSrcPosIni) ;
	}
	
	//W�X�V�p�Ɉ�x���ɖ߂�
	Cexec :: AssignSrcPos(plaSrcPosIni) ;
	
	ComplexArrayFree(&tmp_cmpaTFDB) ;
	LocationArrayFree(&plaSrcPosIni) ;

	if(iRtn == 1){	//�����ύX
		return 1 ;
	}
	else{
		if(bRtn == false){ //�ʒu���ύX�A���͕s��
			return 2 ;
		}
		else{	//�ʒu�C���ύX�Ȃ�
			return 0 ;
		}

	}
	
}
// ---------------------------------------------------------------------------------------
bool Cexec::_CheckSrcID_TF_CONJ(MICARY_ComplexArray caTFDBInit,MICARY_LocationArray laSrcPosInit)	
{
	int i,j,k,m,Npos_cur,Npos_init,Nmic,Nfreq,iLower_f,iUpper_f ;
	int  iInherit ;
	
	Npos_cur = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	Npos_init = laSrcPosInit.ha_numLocation ;
	Nmic = m_psSepMat->micary_smSeparation.ha_numInput ;
	iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue + iLower_f -1 ;
	Nfreq = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;
	
	iInherit = 0 ;
	//TF_CONJ�f�[�^�̈��p��/������
	for(i = 0 ; i < Npos_cur ; i++){
	  for(j = 0 ; j < Npos_init ; j++){
	    // �����̈ʒu������(���p��)
	    if(m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[i] == laSrcPosInit.ha_idLocation[j]){
			for(k = 0 ; k < Nfreq ; k++){
				for(m = 0 ; m < Nmic ; m++){
					m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[m+i*Nmic+k*Nmic*Npos_cur].re 
						= caTFDBInit.ha_cmpValue[m+j*Nmic+k*Nmic*Npos_init].re ;
		   			m_psSepMat->micary_tdTFDB.micary_cmpaTF.ha_cmpValue[m+i*Nmic+k*Nmic*Npos_cur].im 
						= caTFDBInit.ha_cmpValue[m+j*Nmic+k*Nmic*Npos_init].im ;
				}
			}
			iInherit++ ;
			
			break ;
		}
	  } 

	}
	if(iInherit == Npos_cur){	//�����ύX�Ȃ�
		return true ;
	}
	else{	//�����ύX����
		return false ;
	}
	
}
//-------------------------------------------------------------------------------------------
int Cexec::ChangeSrcPos_W(MICARY_LocationArray plaSrcPosCur, MICARY_PrmFrame psPrmFrame)
{
	int i ;
	int Npos_ini,Npos_cur,Nmic,Nfreq,iLower_f,iUpper_f ;
	bool bRtn ,bRtnID;
	int iRtn = 2 ;

	Nmic = m_psSepMat->micary_smSeparation.ha_numInput ;
	Npos_ini = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue + iLower_f -1 ;
	Nfreq = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;
	Npos_cur = plaSrcPosCur.ha_numLocation ;
	
			
	//�z�񐔂̐؂�Ȃ���
	if(Npos_ini != Npos_cur){
		iRtn = 1 ;
	}
	if(m_iUpdateMethodWex->ha_numValue != Npos_cur){
		IntArrayFree(m_iUpdateMethodWex) ;
		IntArrayAlloc(m_iUpdateMethodWex,Npos_cur) ;
	}
	
	// Change_Src_Pos_TF_CONJ�Ő؂蒼����Ă���͂�
	if(m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_numValue != Npos_cur * Nfreq*Nmic){
		SeparationMatFree(&m_psSepMat->micary_smSeparation) ;
		SeparationMatAlloc(&m_psSepMat->micary_smSeparation,Nfreq,Npos_cur,Nmic) ;
	}

	//�������̍X�V
	Cexec ::AssignSrcPos(plaSrcPosCur) ;
	// �����s��̍X�V
	Cexec ::InitSepMatrix(&psPrmFrame) ;
	
	bRtn = false ; 
	bRtnID = false ;
	int iInherit ;
	iInherit = 0 ;
	//�����s��̈��p���m�F
	for(i = 0 ; i < Npos_cur ; i++){
		m_iUpdateMethodWex->ha_iValue[i] = m_iUpdateMethodW ;
		
		if(m_iUpdateMethodW == __UPDATE_METHOD_ID 
				|| m_iUpdateMethodW == __UPDATE_METHOD_ID_POS){
			// ID�x�[�X�̊m�F
			bRtnID = _CheckSrcID_W(i) ;
			if(bRtnID == true){
				iInherit++ ;
			}
		}
		//���ۂ̍X�V��ID or POS
		if(m_iUpdateMethodW == __UPDATE_METHOD_ID_POS){
			if(bRtnID == false){
				m_iUpdateMethodWex->ha_iValue[i] = __UPDATE_METHOD_POS ;
			}
			else{
				m_iUpdateMethodWex->ha_iValue[i] = __UPDATE_METHOD_ID ;
			}
		}

		if(m_iUpdateMethodWex->ha_iValue[i] == __UPDATE_METHOD_POS){
			//POS�x�[�X�̊m�F
			bRtn = _CheckSrcPos_W(i) ;
			if(bRtn == true){
				iInherit++ ;
			}

			else{
			  if(m_iUpdateMethodW == __UPDATE_METHOD_ID_POS){
			    m_iUpdateMethodWex->ha_iValue[i] = __UPDATE_METHOD_ID ;
			  }
			}

			
		}
		
	}
	if(iRtn == 1){	//�������A�ύX
		return 1 ;
	}
	else{
		if(iInherit == Npos_cur){	//�S�ĕύX�Ȃ�
			return 0 ;
		}
		else{	//�ꕔ�ύX
			return 2 ;
		}
	}
		
}

// ----------------------------------------------------------------
bool Cexec::_CheckSrcID_W(int iIdcur)
{
	int k,m,Npos_cur,Npos_init,Nmic,Nfreq,iLower_f,iUpper_f ;
	int iInherit ;
	int iIdsave ;

	Npos_cur = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	Npos_init = m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ;
	Nmic = m_psSepMat->micary_smSeparation.ha_numInput ;
	iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue + iLower_f -1 ;
	Nfreq = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;
	
	//�O�t���[���̉���ID�Ɣ�r(Create or Exist)
	iIdsave = _CheckFrontSrcLoc(m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur],__UPDATE_METHOD_ID) ;
	if(iIdsave == __INVALID_SRC_ID){
		return false ;
	}
	iInherit = 0 ;
	if(m_updateW->micary_iaUpdate.ha_iValue[iIdsave] == __SRCID_FRONT_EXIST){	
	//�����s��̈��p��
		for(k = 0 ; k < Nfreq ; k++){
			for(m = 0 ; m < Nmic ; m++){
				m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iIdcur+m*Npos_cur+k*Nmic*Npos_cur].re 
					= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iIdsave+m*Npos_init+k*Nmic*Npos_init].re ;
		   		m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iIdcur+m*Npos_cur+k*Nmic*Npos_cur].im 
					= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iIdsave+m*Npos_init+k*Nmic*Npos_init].im ;
		   	
			}
		}
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fX 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIdsave].ha_fX ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fY 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIdsave].ha_fY ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fZ 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIdsave].ha_fZ ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fTheta 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIdsave].ha_fTheta ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fPhi 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIdsave].ha_fPhi ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_idPos
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIdsave].ha_idPos ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[iIdcur] 
			= m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[iIdsave] ;	
		
		//TFIndex
		for (int jj=0; jj < 3 ; jj++ ){
			m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].tfindex[jj]
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIdsave].tfindex[jj] ;
			m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].tfindex_min[jj]
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIdsave].tfindex_min[jj] ;
			m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].tfindex_max[jj]
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIdsave].tfindex_max[jj] ;
	    }
		
		iInherit++ ;
	}
	

	
	if(iInherit == 1){	//�����ύX�Ȃ�(�O�t���[��������p)
		return true ;
	}
	else{
		return false ;
	}
}

// -----------------------------------------------------------------
bool Cexec::_CheckSrcPos_W(int iIdcur)
{
	int k,m,Npos_cur,Npos_init,Nmic,Nfreq,iLower_f,iUpper_f ;
	bool btmp;
	int iIDpos,iInherit ;
	//MICARY_Location locPos ;


	Npos_cur = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	Npos_init = m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ;
	Nmic = m_psSepMat->micary_smSeparation.ha_numInput ;
	iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue + iLower_f -1 ;
	Nfreq = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;

	//�O�t���[���̉���ID�Ɣ�r(Create or Exist)
	iIDpos = _CheckFrontSrcLoc(m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur],__UPDATE_METHOD_POS) ;
	if(iIDpos == __INVALID_SRC_ID){

		return false ;
	}
	
	iInherit = 0 ;

	if(m_updateW->micary_iaUpdate.ha_iValue[iIDpos] == __SRCID_FRONT_EXIST){
		for(k = 0 ; k < Nfreq ; k++){
			for(m = 0 ; m < Nmic ; m++){
				m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iIdcur+m*Npos_cur+k*Nmic*Npos_cur].re 
						= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iIDpos+m*Npos_init+k*Nmic*Npos_init].re ;
		   		m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iIdcur+m*Npos_cur+k*Nmic*Npos_cur].im 
						= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iIDpos+m*Npos_init+k*Nmic*Npos_init].im ;
		   	
			}
		}
		
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fX 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIDpos].ha_fX ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fY 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIDpos].ha_fY ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fZ 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIDpos].ha_fZ ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fTheta 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIDpos].ha_fTheta ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_fPhi 
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIDpos].ha_fPhi ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].ha_idPos
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIDpos].ha_idPos ;
		m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[iIdcur] 
			= m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[iIDpos] ;	
		
		//TFIndex
		for (int jj=0; jj < 3 ; jj++ ){
			m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].tfindex[jj]
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIDpos].tfindex[jj] ;
			m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].tfindex_min[jj]
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIDpos].tfindex_min[jj] ;
			m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[iIdcur].tfindex_max[jj]
			= m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iIDpos].tfindex_max[jj] ;
	    }
	    
		iInherit++ ;
	
	}
	
	if(iInherit == 1){	//�����ύX�Ȃ�
		return true ;
	}
	else{
		return false ;
	}

}

// ----------------------------------------------------------------------------------------
bool Cexec::_IsThreshPos(MICARY_Location laRefPos,MICARY_Location laObjPos)
{
	if (m_iSrc_Compare_mode==__COMPARE_MODE_DEG){
		//Changed by Morio. 2013/01/20 ���_��(0,0,0)��
		float fPObj;


		// �ړI�p�x�̓��ς��v�Z
		fPObj = _InnerProductLoc(laRefPos,laObjPos) ;

		if(fPObj>1 && fPObj<1.001){ //����1�ɑ���
			return true ;
		}

		// �ړI�p�x���p�x��臒l�ȉ��Ȃ�true
		if(m_fThred_angle >= 180/acos(-1.0)*abs(acos(fPObj))){
			return true ;
		}
		else{
			return false ;
		}
	}
	else
	{
		HA_Integer tfDist = _tfindexDistance(laRefPos, laObjPos);
		if (m_fThred_TFIndex >= tfDist)
		{
			return true ;
		}
		else
		{
			return false ;
		}
		
	}

}

// ----------------------------------------------------------------------------------------
HA_Integer Cexec::_tfindexDistance(MICARY_Location laRefPos,MICARY_Location laObjPos)
{
  int maxdist = 0;
  for(int i=0; i<3; i++){
    int tmpdist;
    if(laRefPos.tfindex[i] > laObjPos.tfindex[i]){
      tmpdist = min(abs(laRefPos.tfindex[i] - laObjPos.tfindex[i]),
                    abs(laRefPos.tfindex[i] - laObjPos.tfindex_max[i]) + abs(laObjPos.tfindex[i] - laRefPos.tfindex_min[i]));
    }else{
      tmpdist = min(abs(laRefPos.tfindex[i] - laObjPos.tfindex[i]),
                    abs(laRefPos.tfindex[i] - laRefPos.tfindex_min[i]) + abs(laObjPos.tfindex[i] - laRefPos.tfindex_max[i]));
    }
    maxdist = (maxdist < tmpdist)?tmpdist:maxdist;
  }
  return maxdist;
}

// ------------------------------------------------------------------------------------
void Cexec::_Ang2Pos(MICARY_Location *plocPos)
{
	float ftheta,fphi ;
	float pi ;

	pi = acos(-1.0) ;
	ftheta = plocPos->ha_fTheta*pi/180.0 ;
	fphi = plocPos->ha_fPhi*pi/180.0 ;

	plocPos->ha_fX = cos(ftheta)*cos(fphi) ;
	plocPos->ha_fY = sin(ftheta)*cos(fphi) ;
	plocPos->ha_fZ = sin(fphi) ;

	return ;

}
// -----------------------------------------------------------------------------------
HA_Float Cexec::_InnerProductLoc(MICARY_Location micary_Loc1, MICARY_Location micary_Loc2)
{
	HA_Float ha_fNorm,ha_fP ;
	MICARY_FloatArray laVec1 , laVec2 ;
	
	FloatArrayAlloc(&laVec1,0) ;
	FloatArrayAlloc(&laVec2,0) ;

	FloatArrayAlloc(&laVec1,3) ;
	FloatArrayAlloc(&laVec2,3) ;

	// ���K��
	ha_fNorm = pow((double)micary_Loc1.ha_fX,2.0) + pow((double)micary_Loc1.ha_fY,2.0)+pow((double)micary_Loc1.ha_fZ,2.0) ;
	ha_fNorm = pow((double)ha_fNorm,0.5) ;
	
	laVec1.ha_fValue[0] = micary_Loc1.ha_fX / ha_fNorm ;
	laVec1.ha_fValue[1] = micary_Loc1.ha_fY / ha_fNorm ;
	laVec1.ha_fValue[2] = micary_Loc1.ha_fZ / ha_fNorm ;


	// ���K��
	ha_fNorm = pow((double)micary_Loc2.ha_fX,2.0) + pow((double)micary_Loc2.ha_fY,2.0)+pow((double)micary_Loc2.ha_fZ,2.0) ;
	ha_fNorm = pow((double)ha_fNorm,0.5) ;
	
	laVec2.ha_fValue[0] = micary_Loc2.ha_fX / ha_fNorm ;
	laVec2.ha_fValue[1] = micary_Loc2.ha_fY / ha_fNorm ;
	laVec2.ha_fValue[2] = micary_Loc2.ha_fZ / ha_fNorm ;

	ha_fP = _InnerProductVec(laVec1,laVec2) ;
	
	FloatArrayFree(&laVec1) ;
	FloatArrayFree(&laVec2) ;
	
	return ha_fP ;
	
}
// -------------------------------------------------------
bool Cexec::ExportUpdateW(MICARY_PrmFrame *psPrmFrame,const char *cFilename)
{
  int i,iNvalidSize,iNmic,iNfreq,iNpos,iiv,k,m ;
	
		
	iNvalidSize = _GetNvalidUpdateW() ;
	if(iNvalidSize == 0){
		return false ;
	}
	iNmic = m_updateW->micary_dbSeparation.micary_smSeparation.ha_numInput ;
	iNpos = m_updateW->micary_dbSeparation.micary_smSeparation.ha_numOutput ;
	iNfreq = m_updateW->micary_dbSeparation.micary_smSeparation.micary_iaFreqLinex.ha_numValue ;
	MICARY_SMDatabase *psValidW ;
	psValidW = (MICARY_SMDatabase *)calloc(1,sizeof(MICARY_SMDatabase)) ;
	SMDatabaseAlloc(psValidW,0,0,0) ;
	SMDatabaseAlloc(psValidW,iNfreq ,iNvalidSize,iNmic) ;

	//�}�C�N�ʒu�̑��
	for(i = 0 ; i < iNmic ; i++){
		psValidW->micary_locaMic.micary_Location[i].ha_fX = m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[i].ha_fX ;
		psValidW->micary_locaMic.micary_Location[i].ha_fY =m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[i].ha_fY ;
		psValidW->micary_locaMic.micary_Location[i].ha_fZ = m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[i].ha_fZ;
		psValidW->micary_locaMic.micary_Location[i].ha_idPos = m_updateW->micary_dbSeparation.micary_locaMic.micary_Location[i].ha_idPos;
		psValidW->micary_locaMic.ha_idLocation[i] = m_updateW->micary_dbSeparation.micary_locaMic.ha_idLocation[i];

	}


	// ���g�����C���̑��
	for(i = 0 ; i < iNfreq; i++){
		psValidW->micary_smSeparation.micary_iaFreqLinex.ha_iValue[i] = m_updateW->micary_dbSeparation.micary_smSeparation.micary_iaFreqLinex.ha_iValue[i];
	}

	iiv = 0 ;
	
	for(i = 0; i < iNpos ; i++){
		if(m_updateW->micary_iaUpdate.ha_iValue[i] == __SRCID_FRONT_CREATE || m_updateW->micary_iaUpdate.ha_iValue[i] == __SRCID_FRONT_EXIST){
			psValidW->micary_locaSource.ha_idLocation[iiv] = m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[i];
			psValidW->micary_locaSource.micary_Location[iiv].ha_fX = m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[i].ha_fX ;
			psValidW->micary_locaSource.micary_Location[iiv].ha_fY =m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[i].ha_fY ;
			psValidW->micary_locaSource.micary_Location[iiv].ha_fZ = m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[i].ha_fZ;
			psValidW->micary_locaSource.micary_Location[iiv].ha_idPos = m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[i].ha_idPos ;
		
			
			for(k = 0 ; k < iNfreq ; k++){
				for(m = 0 ; m < iNmic ; m++){
					psValidW->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iiv+m*iNvalidSize+k*iNmic*iNvalidSize].re 
							= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[i+m*iNpos+k*iNmic*iNpos].re ;
		   			psValidW->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iiv+m*iNvalidSize+k*iNmic*iNvalidSize].im 
							= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[i+m*iNpos+k*iNmic*iNpos].im ;
		   	
				}
			}
			iiv++ ;
			if(iiv == iNvalidSize){
			  break ;
			}
		}
	}
	
	HA_String file ; // TF �t�@�C����
	file.s = (HA_Char *)calloc(strlen(cFilename)+1,sizeof(char)) ;
	strcpy(file.s,cFilename) ;
	file.slen = strlen(cFilename) ;
	if(m_iFormat_ExportW == __FILE_FORMAT_HARK){
		MICARY_TFDBFileHeader *psH ;
		psH = new MICARY_TFDBFileHeader() ;
		psH->lFs = (int)psPrmFrame->ha_fFs ;
		psH->lNFFT = psPrmFrame->ha_numNFFT ;
		psH->lNMic = iNmic ;
		psH->lNSrc = iNpos ;
		psH->lNFreq = iNfreq ;
		psH->iVersion = 1 ;
		psH->iFileType = FORMAT_TYPE_W ;

		if(!cHark.WriteData(file.s,psH,
					psValidW->micary_smSeparation.micary_cmpaSeparation,
					psValidW->micary_smSeparation.micary_iaFreqLinex,
					psValidW->micary_locaSource,
					psValidW->micary_locaMic,
					"",
					"",
					"",
					"",
					"",
				    "GHDSS_EXPORT")){
			if(file.s){
				free(file.s) ;
				file.s = NULL ;
			}
			SMDatabaseFree(psValidW) ;
			free(psValidW);
			psValidW = NULL ;
			if(psH){
				delete [] psH ;
				psH = NULL ;
			}
			return false ;
		}

	}


	if(file.s){
		free(file.s) ;
		file.s = NULL ;
	}
	SMDatabaseFree(psValidW) ;
	free(psValidW) ;
	psValidW = NULL ;
	return true ;

}
// -------------------------------------------------------
int Cexec::_GetNvalidUpdateW()
{
	int iRtn,i ;
	iRtn = 0 ;
	//for(i = 0 ; i < m_updateW->micary_iaUpdate.ha_numValue ; i++){
	for(i = 0 ; i < m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ; i++){
		if(m_updateW->micary_iaUpdate.ha_iValue[i] != __INVALID_SRC_ID){
			iRtn++ ;
		}
	}
	return iRtn ;

}
// ---------------------------------------------------------
void Cexec::SetUpdateW()
{
	int ii,jj,k,m,iPos,Nmic,Npos_cur,Npos_all,iLower_f,iUpper_f,Nfreq;

	Nmic = m_psSepMat->micary_smSeparation.ha_numInput ;
	Npos_cur = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ;
	Npos_all = m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ;
	iLower_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_iValue[0] ;
	iUpper_f = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue + iLower_f -1 ;
	Nfreq = m_psSepMat->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;

	if(m_iUpdateMethodWex->ha_numValue == 0){
		// 2011/11/16 changed by N.Tanaka
		//IntArrayFree(m_iUpdateMethodWex) ;
		//IntArrayAlloc(m_iUpdateMethodWex,Npos_cur) ;
		IntArrayClear(m_iUpdateMethodWex) ;
		IntArrayAlloc_r(m_iUpdateMethodWex, Npos_cur, __MICARY_def_MAX_NSRC) ;
		for(ii = 0 ; ii < Npos_cur ; ii++){
			m_iUpdateMethodWex->ha_iValue[ii] = m_iUpdateMethodW ;
		}
	}

	for(ii = 0 ; ii < Npos_cur ; ii++){
		switch (m_iUpdateMethodWex->ha_iValue[ii]){
		case __UPDATE_METHOD_POS :
			iPos = GetNearestPos(m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii],
				&(m_updateW->micary_dbSeparation.micary_locaSource)) ;
			if(iPos == __INVALID_SRC_ID){
			  for(jj = 0 ; jj < Npos_all ; jj++){
			    if(m_updateW->micary_iaUpdate.ha_iValue[jj] == __INVALID_SRC_ID){
					iPos = jj ;
					break ;
			    }
			  }
			}	  
			for(k = 0 ; k < Nfreq ; k++){
				for(m = 0 ; m < Nmic ; m++){
					m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iPos+m*Npos_all+k*Npos_all*Nmic].re=
						m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_cur+k*Npos_cur*Nmic].re ;
					m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iPos+m*Npos_all+k*Npos_all*Nmic].im = 
						m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_cur+k*Npos_cur*Nmic].im  ;
				}
			}
			//�������̍X�V
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fX = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fX ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fY = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fY ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fZ = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fZ ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_idPos = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_idPos ;
			
			//�p�x
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fTheta = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fTheta ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fPhi = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fPhi ;
			
			//TFIndex
			for (jj=0; jj < 3 ; jj++ ){
				m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].tfindex[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].tfindex[jj] ;
				m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].tfindex_min[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].tfindex_min[jj] ;
				m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].tfindex_max[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].tfindex_max[jj] ;
    		
    		}
    		
    		
			
			//����ID���X�V
			m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[iPos]= m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[ii] ;
			//�X�V�����̍X�V
			m_updateW->micary_iaUpdate.ha_iValue[iPos] = __SRCID_FRONT_EXIST ;
				

			break ;
		case __UPDATE_METHOD_ID :
		case __UPDATE_METHOD_ID_POS :
		  	m_iUpdateMethodWex->ha_iValue[ii] = __UPDATE_METHOD_ID;
		  	iPos = Npos_all ;
			for(jj = 0; jj < Npos_all; jj++){
				if(m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[ii] ==
					m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[jj]){
						iPos = jj ;
						break ;
				}
			}
			if(iPos >= Npos_all){
				for(jj = 0 ; jj < Npos_all ; jj++){
					if(m_updateW->micary_iaUpdate.ha_iValue[jj] == __INVALID_SRC_ID){
						iPos = jj ;
						break ;
					}
				}
			}
		

			for(k = 0 ; k < Nfreq ; k++){
				for(m = 0 ; m < Nmic ; m++){
					m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iPos+m*Npos_all+k*Npos_all*Nmic].re=
						m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_cur+k*Npos_cur*Nmic].re ;
					m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[iPos+m*Npos_all+k*Npos_all*Nmic].im = 
						m_psSepMat->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_cur+k*Npos_cur*Nmic].im  ;
				}
			}
					
			//�������̍X�V
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fX = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fX ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fY = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fY ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fZ = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fZ ;

			//�p�x
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fTheta = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fTheta ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_fPhi = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fPhi ;
				
			//TFIndex
			for (jj=0; jj < 3 ; jj++ ){
				m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].tfindex[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].tfindex[jj] ;
				m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].tfindex_min[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].tfindex_min[jj] ;
				m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].tfindex_max[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].tfindex_max[jj] ;
    		}
    		
			//����ID���X�V
			m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[iPos] = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[ii] ;
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[iPos].ha_idPos = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_idPos ;

			//�X�V�����̍X�V
			m_updateW->micary_iaUpdate.ha_iValue[iPos] = __SRCID_FRONT_EXIST ;

		break ;
		default :
		break ;
		}
	}
	_SetRemoveSrc() ;

	return ;

}
// -----------------------------------------------------------------------------------------------
void Cexec::_SetRemoveSrc()
{
	//�������ł̊m�F
	int ii,jj,kk ;
	bool bRemove = true ;

	for(ii = 0 ; ii < m_laFrontSrcLoc->ha_numLocation ; ii++){

  		switch(m_laFrontSrcLoc->micary_Location[ii].micary_LocationFlag){
		case __UPDATE_METHOD_POS :
	
			// Changed by Morio 2013/01/23 
			// �����́C���������邩�ǂ������d�v�Ȃ̂ŋ߂��Ȃ��l�ł��E��Ȃ���΂Ȃ�Ȃ� 2013/01/23 �X��
			// 1�t���[���O�ɉ����������Ă��C���ꂪ�����p���͈͊O�ł��邩���m�F���Ă���B
			jj = GetNearestPos_ALLSearch(m_laFrontSrcLoc->micary_Location[ii],&m_psSepMat->micary_tdTFDB.micary_locaSource) ;
			if(jj != __INVALID_SRC_ID){
				bRemove = _IsThreshPos(m_laFrontSrcLoc->micary_Location[ii],m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[jj]) ;
			
				if(!bRemove){
					kk = GetNearestPos(m_laFrontSrcLoc->micary_Location[ii],&m_updateW->micary_dbSeparation.micary_locaSource) ;
					if(kk!=__INVALID_SRC_ID){
						m_updateW->micary_iaUpdate.ha_iValue[kk] = __SRCID_FRONT_REMOVE;
						#ifdef __DEBUG_UPDATEW
						cout << "Src REMOVE " << endl ;
						#endif
					
					}
				}
			}
			
		       
			break ;
		case __UPDATE_METHOD_ID :

	
			bRemove = true ;
			for(jj = 0 ; jj < m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ; jj++){
				if(m_laFrontSrcLoc->ha_idLocation[ii] == m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[jj]){
					bRemove = false ;
					break ;
				}
			}
			if(bRemove){	//�O�t���[��ID������ID�ɂȂ�=���ł����ꍇ
				for(kk = 0 ; kk < m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ; kk++){
					if(m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[kk] == m_laFrontSrcLoc->ha_idLocation[ii]){
						m_updateW->micary_iaUpdate.ha_iValue[kk] = __SRCID_FRONT_REMOVE;
					
						break ;
					}
				}
			}
			break ;
		case __UPDATE_METHOD_ID_POS :

			bRemove = true ;
			for(jj = 0 ; jj < m_psSepMat->micary_tdTFDB.micary_locaSource.ha_numLocation ; jj++){
				if(m_laFrontSrcLoc->ha_idLocation[ii] == m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[jj]){
					bRemove = false ;
					break ;
				}
			}
			
				
			if(bRemove){
				jj = GetNearestPos(m_laFrontSrcLoc->micary_Location[ii],&m_psSepMat->micary_tdTFDB.micary_locaSource) ;
				if(jj != __INVALID_SRC_ID){
					bool btmp=_IsThreshPos(m_laFrontSrcLoc->micary_Location[ii],m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[jj]) ;
					if(btmp){
						bRemove = false ;
					}
				}
			}
			if(bRemove){	//����
				kk = GetNearestPos(m_laFrontSrcLoc->micary_Location[ii],&m_updateW->micary_dbSeparation.micary_locaSource) ;
				if(kk!=__INVALID_SRC_ID){
					m_updateW->micary_iaUpdate.ha_iValue[kk] = __SRCID_FRONT_REMOVE;
				}
			}

			
		
			break ;
		}
	}
	
	// �z��T�C�Y�̐؂�Ȃ���
	if(m_laFrontSrcLoc->ha_numLocation != m_psSepMat->micary_smSeparation.ha_numOutput){
		// 2011/11/16 changed by N.Tanaka
		//LocationArrayFree(m_laFrontSrcLoc) ;
		//LocationArrayAlloc(m_laFrontSrcLoc,m_psSepMat->micary_smSeparation.ha_numOutput) ;
		LocationArrayClear(m_laFrontSrcLoc) ;
		LocationArrayAlloc_r(m_laFrontSrcLoc,m_psSepMat->micary_smSeparation.ha_numOutput, __MICARY_def_MAX_NSRC) ;
	}
	for(ii = 0 ; ii < m_laFrontSrcLoc->ha_numLocation ; ii++){
		m_laFrontSrcLoc->ha_idLocation[ii] = m_psSepMat->micary_tdTFDB.micary_locaSource.ha_idLocation[ii] ;
		m_laFrontSrcLoc->micary_Location[ii].ha_fX = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fX ;
		m_laFrontSrcLoc->micary_Location[ii].ha_fY = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fY ;
		m_laFrontSrcLoc->micary_Location[ii].ha_fZ = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fZ ;
		m_laFrontSrcLoc->micary_Location[ii].ha_idPos = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_idPos;
		
		//DEG
		m_laFrontSrcLoc->micary_Location[ii].ha_fTheta = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fTheta ;
		m_laFrontSrcLoc->micary_Location[ii].ha_fPhi = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].ha_fPhi ;
		
		//TFIndex
		for (jj=0; jj < 3 ; jj++ ){
			m_laFrontSrcLoc->micary_Location[ii].tfindex[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].tfindex[jj] ;
			m_laFrontSrcLoc->micary_Location[ii].tfindex_min[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].tfindex_min[jj] ;
			m_laFrontSrcLoc->micary_Location[ii].tfindex_max[jj] = m_psSepMat->micary_tdTFDB.micary_locaSource.micary_Location[ii].tfindex_max[jj] ;
		}
		
		
		// Update���@���X�V
		m_laFrontSrcLoc->micary_Location[ii].micary_LocationFlag = m_iUpdateMethodWex->ha_iValue[ii] ;

	}
/*	
			cout << "---------input m_laFrontSrcLoc----------"	 <<endl;
		for (int i=0; i<m_laFrontSrcLoc->ha_numLocation; i++){
cout << "X " << m_laFrontSrcLoc->micary_Location[i].tfindex[0] << " " << m_laFrontSrcLoc->micary_Location[i].ha_fX <<endl;
cout << "Y " << m_laFrontSrcLoc->micary_Location[i].tfindex[1] << " " << m_laFrontSrcLoc->micary_Location[i].ha_fY <<endl;
cout << "Z " << m_laFrontSrcLoc->micary_Location[i].tfindex[2] << " " << m_laFrontSrcLoc->micary_Location[i].ha_fZ <<endl;
}
*/
	
}

// -----------------------------------------------------------------------------------------------
// �O�t���[���̉����Ɣ�r
int Cexec::_CheckFrontSrcLoc(MICARY_Location lcCurLoc,int iUpdateMethodW)
{
	int ii,jj ;
	bool btmp ;
	switch (iUpdateMethodW){
	case __UPDATE_METHOD_POS :
		//���t���[�������ƂP�t���[���O������Position������

		// Changed by Morio 2013/01/23 
		// �����́C���������邩�ǂ������d�v�Ȃ̂ŋ߂��Ȃ��l�ł��E��Ȃ���΂Ȃ�Ȃ� 2013/01/23 �X��
		ii = GetNearestPos_ALLSearch(lcCurLoc,m_laFrontSrcLoc) ;
		if(ii != __INVALID_SRC_ID){	//���������ꍇ
			//���e�͈͓����m�F
			btmp = _IsThreshPos(lcCurLoc,m_laFrontSrcLoc->micary_Location[ii]) ;
			if(btmp == true){	//���e�͈͓�(���݂��Ă���)�Ȃ�΁C�ۑ��l�f�[�^�Ō���
				// 1�t���[���O�̉����ʒu�Ō���
				jj = GetNearestPos(m_laFrontSrcLoc->micary_Location[ii],&m_updateW->micary_dbSeparation.micary_locaSource) ;
				if(jj != __INVALID_SRC_ID){
					m_updateW->micary_iaUpdate.ha_iValue[jj] = __SRCID_FRONT_EXIST ;
					//�ۑ��l�ް������t���[�������ʒu�ɍX�V
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fX = lcCurLoc.ha_fX ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fY = lcCurLoc.ha_fY ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fZ = lcCurLoc.ha_fZ ;
					m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[jj] = m_laFrontSrcLoc->micary_Location[ii].ha_idPos ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_idPos = m_laFrontSrcLoc->micary_Location[ii].ha_idPos ;
					
					//DEG
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fTheta = lcCurLoc.ha_fTheta ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fPhi = lcCurLoc.ha_fPhi ;
					
					//TFIndex
					for (int kk=0; kk < 3 ; kk++ ){
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex[kk] = lcCurLoc.tfindex[kk] ;
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_min[kk] = lcCurLoc.tfindex_min[kk] ;
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_max[kk] = lcCurLoc.tfindex_max[kk] ;
					}
					
					return jj ;
				}
			}
			//1�t���[���O�̉����Ƃ͈قȂ�ꍇ�C�ȑO�ɏo�������ʒu���H�i�ۑ��l�f�[�^�Ō����j
			for(jj = 0 ; jj < m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ; jj++){
				if(m_updateW->micary_iaUpdate.ha_iValue[jj] != __INVALID_SRC_ID){
			
					btmp = _IsThreshPos(lcCurLoc,m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj]) ;
						if(btmp == true){	// �ۑ��l�f�[�^�Ō��������ꍇ�C�X�V
							m_updateW->micary_iaUpdate.ha_iValue[jj] = __SRCID_FRONT_EXIST ; //����
							m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fX = lcCurLoc.ha_fX ;
							m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fY = lcCurLoc.ha_fY ;
							m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fZ = lcCurLoc.ha_fZ ;
							
							//DEG
							m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fTheta = lcCurLoc.ha_fTheta ;
							m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fPhi = lcCurLoc.ha_fPhi ;
					
							//TFIndex
							for (int kk=0; kk < 3 ; kk++ ){
								m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex[kk] = lcCurLoc.tfindex[kk] ;
								m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_min[kk] = lcCurLoc.tfindex_min[kk] ;
								m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_max[kk] = lcCurLoc.tfindex_max[kk] ;
							}
							
							return jj ;
						}
				}
			}
			
			// 1�t���[���O�ɂ͂Ȃ�����=CREATE�Ȃ�΁A�V�K�ɕۑ�����ꏊ�̔ԍ���Ԃ�
			for(jj = 0 ; jj < m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ; jj++){
				if(m_updateW->micary_iaUpdate.ha_iValue[jj] == __INVALID_SRC_ID){
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fX = lcCurLoc.ha_fX ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fY = lcCurLoc.ha_fY ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fZ = lcCurLoc.ha_fZ ;
					m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[jj] = lcCurLoc.ha_idPos ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_idPos = lcCurLoc.ha_idPos ;
					
					//DEG
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fTheta = lcCurLoc.ha_fTheta ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fPhi = lcCurLoc.ha_fPhi ;
					
					//TFIndex
					for (int kk=0; kk < 3 ; kk++ ){
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex[kk] = lcCurLoc.tfindex[kk] ;
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_min[kk] = lcCurLoc.tfindex_min[kk] ;
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_max[kk] = lcCurLoc.tfindex_max[kk] ;
					}
					
					m_updateW->micary_iaUpdate.ha_iValue[jj] = __SRCID_FRONT_CREATE ;
				
					return jj ;
				}
			}
		}
		return __INVALID_SRC_ID ;

	break ;
	case __UPDATE_METHOD_ID :
		for(ii = 0 ; ii < m_laFrontSrcLoc->ha_numLocation ; ii++){
			if(m_laFrontSrcLoc->ha_idLocation[ii] == lcCurLoc.ha_idPos){ //1�t���[���O�̉����ƈ�v���Ă���
			  for(jj = 0 ; jj < m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ; jj++){//�ۑ��l�f�[�^�̔ԍ���Ԃ�
			    	if(m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[jj] == lcCurLoc.ha_idPos){
			    	  if(m_updateW->micary_iaUpdate.ha_iValue[jj] == __SRCID_FRONT_CREATE || 
							m_updateW->micary_iaUpdate.ha_iValue[jj] == __SRCID_FRONT_EXIST){
										
							m_updateW->micary_iaUpdate.ha_iValue[jj] = __SRCID_FRONT_EXIST ;
							return jj ;
						}
					}
				}
			}
		}
		if(m_updateW == __UPDATE_METHOD_ID){	//add 2009.7.29
			// 1�t���[���O�̉����ɂ͂Ȃ�����=CREATE�Ȃ�΁A�V�K�ɕۑ�����ꏊ�̔ԍ���Ԃ�
			for(jj = 0 ; jj < m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ; jj++){
				if(m_updateW->micary_iaUpdate.ha_iValue[jj] == __INVALID_SRC_ID){
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fX = lcCurLoc.ha_fX ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fY = lcCurLoc.ha_fY ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fZ = lcCurLoc.ha_fZ ;
					
					m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[jj] = lcCurLoc.ha_idPos ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_idPos = lcCurLoc.ha_idPos ;
					m_updateW->micary_iaUpdate.ha_iValue[jj] = __SRCID_FRONT_CREATE ;
					
					//DEG
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fTheta = lcCurLoc.ha_fTheta ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fPhi = lcCurLoc.ha_fPhi ;
					
					//TFIndex
					for (int kk=0; kk < 3 ; kk++ ){
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex[kk] = lcCurLoc.tfindex[kk] ;
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_min[kk] = lcCurLoc.tfindex_min[kk] ;
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_max[kk] = lcCurLoc.tfindex_max[kk] ;
					}
					
					return jj ;
				}
			}
		}
		return	__INVALID_SRC_ID ; // error code
	break ;

	case __UPDATE_METHOD_ID_POS :
		for(ii = 0 ; ii < m_laFrontSrcLoc->ha_numLocation ; ii++){
			if(m_laFrontSrcLoc->ha_idLocation[ii] == lcCurLoc.ha_idPos){ //1�t���[���O�̉����ƈ�v���Ă���
				for(jj = 0 ; jj < m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ; jj++){//�ۑ��l�f�[�^�̔ԍ���Ԃ�
					if(m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[jj] == lcCurLoc.ha_idPos){
						m_updateW->micary_iaUpdate.ha_iValue[jj] = __SRCID_FRONT_EXIST ;
						return jj ;
					}
				}
			}
		}
		// 1�t���[���O�̉����ɂ͂Ȃ�����=CREATE�Ȃ�΁APOS�Ō���
		ii = GetNearestPos(lcCurLoc,m_laFrontSrcLoc) ;
		//���e�͈͓����̊m�F
		if(ii != __INVALID_SRC_ID){
			btmp = _IsThreshPos(lcCurLoc,m_laFrontSrcLoc->micary_Location[ii]) ;
			if(btmp == true){	//���e�͈͓�(���݂��Ă���)�Ȃ�΁C�ۑ��l�f�[�^�Ō���
				// 1�t���[���O�̉����ʒu�Ō���
				jj = GetNearestPos(m_laFrontSrcLoc->micary_Location[ii],&m_updateW->micary_dbSeparation.micary_locaSource) ;
				if(jj != __INVALID_SRC_ID){
					m_updateW->micary_iaUpdate.ha_iValue[jj] = __SRCID_FRONT_EXIST ;
				}
				return jj ;
			}
			// 1�t���[���O�̉����ɂ͂Ȃ�����=CREATE�Ȃ�΁A�V�K�ɕۑ�����ꏊ�̔ԍ���Ԃ�
			for(jj = 0 ; jj < m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation ; jj++){
				if(m_updateW->micary_iaUpdate.ha_iValue[jj] == __INVALID_SRC_ID){
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fX = lcCurLoc.ha_fX ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fY = lcCurLoc.ha_fY ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fZ = lcCurLoc.ha_fZ ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_idPos = lcCurLoc.ha_idPos ;
					m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[jj] = lcCurLoc.ha_idPos ;
					m_updateW->micary_iaUpdate.ha_iValue[jj] = __SRCID_FRONT_CREATE ;
					
					//DEG
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fTheta = lcCurLoc.ha_fTheta ;
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].ha_fPhi = lcCurLoc.ha_fPhi ;
					
					//TFIndex
					for (int kk=0; kk < 3 ; kk++ ){
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex[kk] = lcCurLoc.tfindex[kk] ;
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_min[kk] = lcCurLoc.tfindex_min[kk] ;
						m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[jj].tfindex_max[kk] = lcCurLoc.tfindex_max[kk] ;
					}
					
					return jj ;
				}
			}
		}
		return __INVALID_SRC_ID ;

	break ;
	}

}
// ---------------------------------------------------------------------------
bool Cexec::AddUpdateW(MICARY_PrmFrame *psPrmFrame,const char *cFilename,bool bExpW)
{
	int ii ,jj ,Npos,Nmic,Nfreq,k,m,Npos_all,idtmp;
	bool bUpdate ;
	bool bRtn ;

	Npos_all = m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation;
	Nmic = m_updateW->micary_dbSeparation.micary_locaMic.ha_numLocation ;
	Nfreq = m_updateW->micary_dbSeparation.micary_smSeparation.micary_iaFreqLinex.ha_numValue ;

	for(ii = 0 ; ii < Npos_all ;ii++){
		if(m_updateW->micary_iaUpdate.ha_iValue[ii] == __SRCID_FRONT_REMOVE && 
			m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_idPos != __INVALID_SRC_ID){
		 if(!bExpW){
		    	//�t���O�̍X�V
		   if(m_iUpdateMethodW == __UPDATE_METHOD_ID || m_iUpdateMethodW == __UPDATE_METHOD_ID_POS){
			       m_updateW->micary_iaUpdate.ha_iValue[ii]=__INVALID_SRC_ID;
			    for(k = 0 ; k < Nfreq ; k++){
				 	for(m = 0 ; m < Nmic ; m++){
				    		m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_all+k*Nmic*Npos_all].re = 0.0 ;
				    		m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_all+k*Nmic*Npos_all].im = 0.0;
		   	    		}
			  	}
			}
		 }
		 else{ //ExportW
		   // 2010.8.11
		   cout << "Export file : " << cFilename <<" src = " << m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_idPos << endl ;

			MICARY_SMDatabase *pstmpW ;
			// �t�@�C������Ǎ���
			MICARY_TFDBFileHeader * psH ;	//�f�[�^�x�[�X�w�b�_
			psH = (MICARY_TFDBFileHeader *)calloc(1,sizeof(MICARY_TFDBFileHeader)) ;
	
			HA_String file ; // �t�@�C����
			file.s = (HA_Char *)calloc(strlen(cFilename)+1,sizeof(char)) ;
			strcpy(file.s,cFilename) ;
			file.slen = strlen(cFilename) ;
	
			if(m_iFormat_ExportW == __FILE_FORMAT_HARK){
				if(!cHark.ReadHeader(file.s,psH)) //�t�@�C���̃w�b�_�����擾
				{
					cout << "Can not read export file."<< endl ;
					if(psH){
						free(psH) ;
						psH = NULL ;
					}
					if(file.s){
						free(file.s) ;
						file.s = NULL ;
					}
					return false ;
				}
				pstmpW = (MICARY_SMDatabase *)calloc(1,sizeof(MICARY_SMDatabase)) ;	//�����s��e���|����

				SMDatabaseAlloc(pstmpW,0,0,0) ;
				SMDatabaseAlloc(pstmpW,(int)psH->lNFreq,(int)psH->lNSrc,(int)psH->lNMic) ;
			
				// Read Header
				if(!cHark.ReadInfo(file.s,psH ,&pstmpW->micary_locaSource,
						   &pstmpW->micary_locaMic,&pstmpW->micary_smSeparation.micary_iaFreqLinex,false))
				{
					cout << "Can not read export file."<< endl ;
					if(psH){
						free(psH) ;
						psH = NULL ;
					}
					if(file.s){
						free(file.s) ;
						file.s = NULL ;
					}
					SMDatabaseFree(pstmpW) ;
					free(pstmpW) ;
					pstmpW = NULL ;

					return false ;
				}
				Npos = pstmpW->micary_locaSource.ha_numLocation ;
				Nmic = pstmpW->micary_locaMic.ha_numLocation ;
				Nfreq = pstmpW->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;

				if(!cHark.ReadData(file.s,&pstmpW->micary_smSeparation.micary_cmpaSeparation)){
					std ::cout << "Fail to read export file." << std::endl ;
					if(psH){
						free(psH) ;
						psH = NULL ;
					}
					if(file.s){
						free(file.s) ;
						file.s = NULL ;
					}
					SMDatabaseFree(pstmpW) ;
					free(pstmpW) ;
					pstmpW = NULL ;

					return false ;
				}
				//add by morio �����p�Ɋp�x�����߂�im_updateW�̉������͑傫���C�t�@�C������ǂݏo����pstmpW�̉��������傫���ƍl������j
				
				if(m_iSrc_Compare_mode == __COMPARE_MODE_DEG)
				{
					float x,y,z;
                    float pi = acos(-1.0) ;
                    for(unsigned int i = 0; i < pstmpW->micary_locaSource.ha_numLocation; i++){
						x = pstmpW->micary_locaSource.micary_Location[i].ha_fX;
						y = pstmpW->micary_locaSource.micary_Location[i].ha_fY;
						z = pstmpW->micary_locaSource.micary_Location[i].ha_fZ;
						pstmpW->micary_locaSource.micary_Location[i].ha_fTheta = (float)(atan2(y,x) * 180 / pi);
						pstmpW->micary_locaSource.micary_Location[i].ha_fPhi = (float)(atan2(z,sqrt(x*x+y*y)) * 180 / pi);
											
						//cout<<"src"<<i<<"_ang,"<<m_psTFDB->micary_locaSource.micary_Location[i].ha_fTheta<<","<<m_psTFDB->micary_locaSource.micary_Location[i].ha_fPhi <<endl;
					}
				}	
		
			}
			// ����ID������΁A�X�V
			// �|�W�V�����Ō������ăt�@�C���f�[�^�x�[�X�X�V�ł͂Ȃ����H�H
			bUpdate = false ;

		      idtmp = GetNearestPos(m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii],&pstmpW->micary_locaSource) ;
			if(idtmp != __INVALID_SRC_ID){
				bool btmp = _IsThreshPos(m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii],
								pstmpW->micary_locaSource.micary_Location[idtmp]) ;
				if(btmp){
				  /*
				  cout << "Current Pos = " << m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_idPos << " , " <<  m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fX << " , " <<  m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fY << " , " <<  m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fZ << endl ;
				   cout << "DBfile Pos = " << pstmpW->micary_locaSource.micary_Location[idtmp].ha_idPos << " , " <<  pstmpW->micary_locaSource.micary_Location[idtmp].ha_fX << " , " <<  pstmpW->micary_locaSource.micary_Location[idtmp].ha_fY << " , " <<  pstmpW->micary_locaSource.micary_Location[idtmp].ha_fZ << endl ;
				  */

				//�����s��̈��p��
			  		for(k = 0 ; k < Nfreq ; k++){
			    			for(m = 0 ; m < Nmic ; m++){
			      			pstmpW->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[idtmp+m*Npos+k*Nmic*Npos].re 
										= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_all+k*Nmic*Npos_all].re ;
		   	      			pstmpW->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[idtmp+m*Npos+k*Nmic*Npos].im 
										= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_all+k*Nmic*Npos_all].im ;
		   	    			}
			  		}
			  		//SrcPos���X�V
			  		pstmpW->micary_locaSource.micary_Location[idtmp].ha_fX = m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fX ;
			  		pstmpW->micary_locaSource.micary_Location[idtmp].ha_fY = m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fY ;
			  		pstmpW->micary_locaSource.micary_Location[idtmp].ha_fZ = m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fZ ;
			  		bUpdate = true ;
				}
			}
		
			if(bUpdate){
				if(m_iFormat_ExportW == __FILE_FORMAT_HARK){
				  psH->lFs = (int)psPrmFrame->ha_fFs ;
				  psH->lNFFT = psPrmFrame->ha_numNFFT ;
				 // 2010.8.11
				  psH->lNSrc = pstmpW->micary_locaSource.ha_numLocation ;
				  psH->lNMic =  pstmpW->micary_locaMic.ha_numLocation ;
				  psH->lNFreq = pstmpW->micary_smSeparation.micary_iaFreqLinex.ha_numValue ;
				  psH->iVersion = 1 ;
				  psH->iFileType = FORMAT_TYPE_W ;
			
				  /*	 
					cout << "File info : Nmic = " << psH->lNMic << " Npos = " << psH->lNSrc << " Nfreq = " << psH->lNFreq << " Version = " << psH->iVersion << " FileType = " << psH->iFileType << endl ;
				  */
				 


					if(!cHark.WriteData(file.s,psH,
							pstmpW->micary_smSeparation.micary_cmpaSeparation,
							pstmpW->micary_smSeparation.micary_iaFreqLinex,
							pstmpW->micary_locaSource,
							pstmpW->micary_locaMic,
							"",
							"",
							"",
							"",
							"",
							    "GHDSS_EXPORT")){
						if(psH){
							free(psH) ;
							psH = NULL ;
						}
						if(file.s){
							free(file.s) ;
							file.s = NULL ;
						}
						SMDatabaseFree(pstmpW) ;
						free(pstmpW) ;
						pstmpW = NULL ;

						return false ;
					}				
				}
			}
			else{	//�X�V���鉹�����Ȃ���Βǉ�
				MICARY_SMDatabase *pstmpW2 ;
				pstmpW2 = (MICARY_SMDatabase *)calloc(1,sizeof(MICARY_SMDatabase)) ;	//�����s��e���|����
				SMDatabaseAlloc(pstmpW2,0,0,0) ;
				SMDatabaseAlloc(pstmpW2,Nfreq,Npos+1,Nmic) ;

				//�}�C�N���W
				for(jj = 0 ; jj < Nmic ; jj++){
					pstmpW2->micary_locaMic.ha_idLocation[jj] = 
						pstmpW->micary_locaMic.ha_idLocation[jj] ;
					pstmpW2->micary_locaMic.micary_Location[jj].ha_fX = 
						pstmpW->micary_locaMic.micary_Location[jj].ha_fX ;

					pstmpW2->micary_locaMic.micary_Location[jj].ha_fY = 
						pstmpW->micary_locaMic.micary_Location[jj].ha_fY ;

					pstmpW2->micary_locaMic.micary_Location[jj].ha_fZ = 
						pstmpW->micary_locaMic.micary_Location[jj].ha_fZ ;

					pstmpW2->micary_locaMic.micary_Location[jj].ha_idPos = 
						pstmpW->micary_locaMic.micary_Location[jj].ha_idPos ;

				}

				//���g�����C��
				for(jj = 0 ; jj < Nfreq ; jj++){
					pstmpW2->micary_smSeparation.micary_iaFreqLinex.ha_iValue[jj] = 
						pstmpW->micary_smSeparation.micary_iaFreqLinex.ha_iValue[jj] ;
				}

				//�������W
				for(jj = 0 ; jj < Npos ; jj++){
					pstmpW2->micary_locaSource.ha_idLocation[jj] = 
						pstmpW->micary_locaSource.ha_idLocation[jj] ;
					pstmpW2->micary_locaSource.micary_Location[jj].ha_fX = 
						pstmpW->micary_locaSource.micary_Location[jj].ha_fX ;
					pstmpW2->micary_locaSource.micary_Location[jj].ha_fY = 
						pstmpW->micary_locaSource.micary_Location[jj].ha_fY ;
					pstmpW2->micary_locaSource.micary_Location[jj].ha_fZ = 
						pstmpW->micary_locaSource.micary_Location[jj].ha_fZ ;
					pstmpW2->micary_locaSource.micary_Location[jj].ha_idPos = 
						pstmpW->micary_locaSource.micary_Location[jj].ha_idPos ;

				}
	
				pstmpW2->micary_locaSource.ha_idLocation[Npos] = 
					m_updateW->micary_dbSeparation.micary_locaSource.ha_idLocation[ii] ;
				pstmpW2->micary_locaSource.micary_Location[Npos].ha_fX = 
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fX ;
				pstmpW2->micary_locaSource.micary_Location[Npos].ha_fY = 
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fY ;
				pstmpW2->micary_locaSource.micary_Location[Npos].ha_fZ = 
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_fZ ;
				pstmpW2->micary_locaSource.micary_Location[Npos].ha_idPos = 
					m_updateW->micary_dbSeparation.micary_locaSource.micary_Location[ii].ha_idPos ;
	
				/*	
				cout << "Add Pos = " << pstmpW2->micary_locaSource.micary_Location[Npos].ha_idPos << " , " <<  pstmpW2->micary_locaSource.micary_Location[Npos].ha_fX << " , " <<  pstmpW2->micary_locaSource.micary_Location[Npos].ha_fY << " , " <<  pstmpW2->micary_locaSource.micary_Location[Npos].ha_fZ << endl ;
				*/
 
				//�t�@�C���f�[�^��ǂݍ���(2009/8/5)
				for(jj = 0 ; jj < Npos +1 ; jj++){								
					for(k = 0 ; k < Nfreq ; k++){
						for(m = 0 ; m < Nmic ; m++){
							if(jj < Npos){
								pstmpW2->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[jj+m*(Npos+1)+k*Nmic*(Npos+1)].re 
									= pstmpW->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[jj+m*Npos+k*Nmic*Npos].re ;
		   						pstmpW2->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[jj+m*(Npos+1)+k*Nmic*(Npos+1)].im 
									= pstmpW->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[jj+m*Npos+k*Nmic*Npos].im ;
							
							}
							else{
								pstmpW2->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[jj+m*(Npos+1)+k*Nmic*(Npos+1)].re 
									= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_all+k*Nmic*Npos_all].re ;
		   						pstmpW2->micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[jj+m*(Npos+1)+k*Nmic*(Npos+1)].im 
									= m_updateW->micary_dbSeparation.micary_smSeparation.micary_cmpaSeparation.ha_cmpValue[ii+m*Npos_all+k*Nmic*Npos_all].im ;
							}
						}
					}
				}

				// Export File 
				if(m_iFormat_ExportW == __FILE_FORMAT_HARK){
				  psH->lFs = (int)psPrmFrame->ha_fFs ;
				  psH->lNFFT = psPrmFrame->ha_numNFFT ;
				  // 2010.8.11
				  psH->lNSrc = pstmpW2->micary_locaSource.ha_numLocation ;
				  psH->lNMic =  pstmpW2->micary_locaMic.ha_numLocation ;
				  psH->lNFreq = pstmpW2->micary_smSeparation.micary_iaFreqLinex.ha_numValue ; 
				  psH->iVersion = 1 ;
				  psH->iFileType = FORMAT_TYPE_W ;
				  
				  /*
				cout << "File info : Nmic = " << psH->lNMic << " Npos = " << psH->lNSrc << " Nfreq = " << psH->lNFreq << " Version = " << psH->iVersion << " FileType = " << psH->iFileType << endl ;
				  */


				  if(!cHark.WriteData(file.s,psH,
					pstmpW2->micary_smSeparation.micary_cmpaSeparation,
					pstmpW2->micary_smSeparation.micary_iaFreqLinex,
					pstmpW2->micary_locaSource,
					pstmpW2->micary_locaMic,
					"",
					"",
					"",
					"",
					"",
					"GHDSS_EXPORT")){
					if(psH){
					  free(psH) ;
					  psH = NULL ;
					}
					if(file.s){
					  free(file.s) ;
					  file.s = NULL ;
					}
					SMDatabaseFree(pstmpW) ;
					free(pstmpW) ;
					pstmpW = NULL ;
					
				        SMDatabaseFree(pstmpW2) ;
					free(pstmpW2) ;
					pstmpW2 = NULL ;
						cout << "Fail to Export file as HARK2 format." << endl ;
					return false ;
				}
			
			     }


				SMDatabaseFree(pstmpW2) ;
				free(pstmpW2) ;
				pstmpW2 = NULL ;

			}
			//�t���O�̍X�V
			if(m_iUpdateMethodW == __UPDATE_METHOD_ID){
				m_updateW->micary_iaUpdate.ha_iValue[ii]=__INVALID_SRC_ID;
			}

			if(psH){
				free(psH) ;
				psH = NULL ;
			}
			if(file.s){
				free(file.s) ;
				file.s = NULL ;
			}
			SMDatabaseFree(pstmpW) ;
			free(pstmpW) ;
			pstmpW = NULL ;
			
		  }
		}		
	}
	
}
//--------------------------------------------------------------------------
bool Cexec::ExportAllUpdateW(MICARY_PrmFrame *psPrmFrame,const char *cFilename)
{
	int Npos_all,ii ;
	bool bRtn ;

	Npos_all = m_updateW->micary_dbSeparation.micary_locaSource.ha_numLocation;
	cout<< "Npos_all = " << Npos_all << endl;
	for(ii = 0 ; ii < Npos_all ;ii++){
		if(m_updateW->micary_iaUpdate.ha_iValue[ii] == __SRCID_FRONT_CREATE	|| 
			m_updateW->micary_iaUpdate.ha_iValue[ii] == __SRCID_FRONT_EXIST)
		{
			m_updateW->micary_iaUpdate.ha_iValue[ii] = __SRCID_FRONT_REMOVE ;
		
		}
	}

	bRtn = AddUpdateW(psPrmFrame,cFilename,true) ;
	return bRtn ;
}
void Cexec::SetTF_CONJ_FileFormat(int iFormat)
{
	m_iFormat_TF_CONJ = iFormat ;
}
void Cexec::SetInitW_FileFormat(int iFormat)
{
	m_iFormat_InitW = iFormat ;
}
void Cexec::SetExportW_FileFormat(int iFormat)
{
	m_iFormat_ExportW = iFormat ;
}
void Cexec::SetMicPos_FileFormat(int iFormat)
{
	m_iFormat_MicPos = iFormat ;
}
void Cexec::SetFixedNoisePos_FileFormat(int iFormat)
{
	m_iFormat_FixedNoise = iFormat ;
}


#endif // ENABLE_HARKIO_NH