// HarkFileIO2.cpp: CHarkFileIO クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "HarkFileIO.h"

#ifndef ENABLE_HARKIO_NH

// Chaenged by Morio 2013/02/01
#ifdef ENABLE_HARKIO2
using namespace harkio2;
#include <iostream>
#include <fstream>
#include <boost/make_shared.hpp>
#elif ENABLE_HARKIO1
using namespace harkio;
#endif

using namespace std ;
#include <vector>


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CHarkFileIO::CHarkFileIO()
{
	
}

CHarkFileIO::~CHarkFileIO()
{
}

// Chaenged by Morio 2013/02/01 harkio1,2対応
#ifdef ENABLE_HARKIO2
// --------------------------------------------------------------------
bool CHarkFileIO::WriteData(const char* cFilename,
						MICARY_TFDBFileHeader *psTFDBHead,
						MICARY_ComplexArray micary_cmpaData,
						MICARY_IntArray	micary_iaFreqLine,
						MICARY_LocationArray micary_locaSource,
						MICARY_LocationArray micary_locaMic,
						const char* cDate,
						const char* cLocation,
						const char* cRobot,
						const char* cArrayType,
						const char * cCreator,
						const char* cMemo)
{	
	
	try{
		
		int nMic,nSrc,nFrq,nFFT,nFs ;
		int ii,jj,kk,index ;
		vector<int> *fi ;
		
		nMic = psTFDBHead->lNMic ;
		nSrc = psTFDBHead->lNSrc ;
		nFFT = psTFDBHead->lNFFT ;
		nFs = psTFDBHead->lFs ;
		nFrq = psTFDBHead->lNFreq ;

		//書き込み器の作成
		ofstream harkFileData(cFilename);
		boost::shared_ptr<hark::HarkFileWriter> writer(new hark::HarkFileBinaryWriter(harkFileData));
		boost::shared_ptr<hark::HarkFile> hark_f (new hark::HarkFile) ;    
    			
		//チャンクの作成
    	//auto fmt = boost::make_shared<hgtf::Fmt1_0>(); 
    	boost::shared_ptr<hark::hgtf::Fmt1_0> fmt(new hark::hgtf::Fmt1_0);  	
    	
    	fmt->SetFormatId( psTFDBHead->iFileType); // GTF or SM
    	fmt->SetNMic(nMic);
    	fmt->SetNFFT(nFFT);
    	fmt->SetNImpulseResponse(nFFT);// ? 現状通り
    	//fmt->SetNImpulseResponse();// こうでは？
    	fmt->SetFS(nFs);
    	
    	
    	std::vector<Vector3<HA_Float> > MicPositions(nMic);
    	
    	
	    for (ii = 0 ; ii < nMic ; ii++)
	    {
			harkio2::Vector3<HA_Float> tmp( micary_locaMic.micary_Location[ii].ha_fX, micary_locaMic.micary_Location[ii].ha_fY, micary_locaMic.micary_Location[ii].ha_fZ);
	        MicPositions[ii] =tmp;

	    }
    	
    	fmt->SetMicPositions(MicPositions);
				

		
       		//チャンクの作成
		if(psTFDBHead->iFileType==FORMAT_TYPE_TF){
				//このパターンないのでは
				//auto gtf = boost::make_shared<hgtf::GTF1_0>();
				boost::shared_ptr<hark::hgtf::GTF1_0> gtf(new hark::hgtf::GTF1_0);  
								
				gtf->SetNSource(nSrc);
				
				//音源位置座標をセット
				std::vector<Vector3<HA_Float> > SrcPositions(nSrc);				
				for (ii = 0 ; ii < nMic ; ii++)
			    {
					Vector3<HA_Float> tmp( micary_locaSource.micary_Location[ii].ha_fX, micary_locaSource.micary_Location[ii].ha_fY, micary_locaSource.micary_Location[ii].ha_fZ);
			        SrcPositions[ii]=tmp;
			    }				
		        gtf->SetSourcePositions(SrcPositions);
		        
		        //伝達関数の周波数インデックスからインデックスへのマップ
		        vector<unsigned int> findex_to_binindex(nFrq);
		        gtf->SetNFIndex(findex_to_binindex.size());
		        for (unsigned int i = 0; i < findex_to_binindex.size(); ++i)
		        {
		            findex_to_binindex[i] = micary_iaFreqLine.ha_iValue[i] ;
		        }
		        gtf->SetFIndexToBinIndexTable(findex_to_binindex);
		        
		        //伝達関数
		        
				std::vector<std::vector<std::vector<complex<HA_Float> > > > tf(nSrc,std::vector<std::vector<complex<HA_Float> > >(nMic,std::vector<complex<HA_Float> >(nFrq)));

				//データをセット(Src->Mic->Freq)
				index = 0 ;
				for(kk = 0 ; kk < nFrq ; kk++){			
					for(ii = 0 ; ii < nSrc ; ii++){
						for(jj = 0 ; jj < nMic ; jj++){
							tf[ii][jj][kk].real() = micary_cmpaData.ha_cmpValue[index].re ;
							tf[ii][jj][kk].imag() = micary_cmpaData.ha_cmpValue[index].im ;
							index++ ;
					      
						}
					}
				}
		        gtf->SetTFs(tf);
		        
		        hark_f->AddDataSet(gtf);  
		        
			
		}		
		else if(psTFDBHead->iFileType==FORMAT_TYPE_W){

				//auto sm = boost::make_shared<hgtf::SM1_0>();
				boost::shared_ptr<hark::hgtf::SM1_0> sm(new hark::hgtf::SM1_0);  
				//hgtf->addChunk(sm) ;


				sm->SetNSource(nSrc);
				//Chunkから音源位置座標コンテナを取得して，テストデータをセット
				//音源位置座標をセット
				
			
				std::vector<Vector3<float> > SrcPositions(nSrc);				
				for (ii = 0 ; ii < nSrc ; ii++)
			    {
					Vector3<float> tmp( micary_locaSource.micary_Location[ii].ha_fX, micary_locaSource.micary_Location[ii].ha_fY, micary_locaSource.micary_Location[ii].ha_fZ);
			        SrcPositions[ii]=tmp;
			       
			    }				
		        sm->SetSourcePositions(SrcPositions);

				//分離行列
				std::vector<vector<vector<complex<float> > > > sm_data(nSrc,vector<vector<complex<float> > >(nMic,vector<complex<float> >(nFrq)));

				//データをセット(Src->Mic->Freq)
				index = 0 ;
				for(kk = 0 ; kk < nFrq ; kk++){			
					for(jj = 0 ; jj < nMic ; jj++){
						for(ii = 0 ; ii < nSrc ; ii++){
							sm_data[ii][jj][kk].real() = micary_cmpaData.ha_cmpValue[index].re ;
							sm_data[ii][jj][kk].imag() = micary_cmpaData.ha_cmpValue[index].im ;
							index++ ;
						}
					}
				}
				
				sm->SetSMs(sm_data);

				//分離行列の周波数インデックスからインデックスへのマップ
		        //cout << "NFrq = " << nFrq << endl ;
		        vector<unsigned int> findex_to_binindex(nFrq);
		        sm->SetNFIndex(findex_to_binindex.size());
		        
		        //データをセット
		        for (unsigned int i = 0; i < findex_to_binindex.size(); ++i)
		        {
		            findex_to_binindex[i] = micary_iaFreqLine.ha_iValue[i] ;
		        }
		        sm->SetFIndexToBinIndexTable(findex_to_binindex);
		    
		        hark_f->AddDataSet(sm);
		
		}
		
		
		//共通　SPECチャンクの作成
		//auto spec = boost::make_shared<hgtf::Spec1_0>();
		boost::shared_ptr<hark::hgtf::Spec1_0> spec(new hark::hgtf::Spec1_0);  

		//SPECデータの作成
		spec->SetDate(cDate) ;
		spec->SetLocation(cLocation) ;
		spec->SetRobot(cRobot) ;
		spec->SetMicArrayType(cArrayType) ;
		spec->SetCreator(cCreator) ;
		spec->SetMemo(cMemo) ;
		
		hark_f->AddDataSet(fmt);
		hark_f->AddDataSet(spec);
		//書き込み
		writer->Write(hark_f);

	
	}
	catch(std::exception &ex){
	
		return false ;
	
	}

	//	cout << "Success Write Hark file" << endl ;
	return true ;
	

}

bool CHarkFileIO::ReadInfo(const char* cFilename,
					MICARY_TFDBFileHeader *psTFDBHead,
					MICARY_LocationArray * plocaSrcPos , 
					MICARY_LocationArray * plocaMicPos ,
			   MICARY_IntArray *piaFreqLine,
			   bool bInfo)
{
	int i ,j , k;
	vector<unsigned int> *fi ;
	
	try
	{
		ifstream harkFileData(cFilename);
		boost::shared_ptr<hark::HarkFileReader> reader(new hark::HarkFileBinaryReader(harkFileData));
		boost::shared_ptr<hark::HarkFile> hark = reader->Read();

		boost::shared_ptr<hark::hgtf::Fmt1_0> fmt = boost::dynamic_pointer_cast<hark::hgtf::Fmt1_0>(hark->GetDataSet("hgtf::Fmt"));
		if (fmt == 0)
		{
		  cout << "this is not include Fmt dataset version 1.0" << endl;
		  return false ;
		}

      	int slength = fmt->GetNFFT() / 2 + 1;
		int nmic = fmt->GetNMic();

    
	    // Mic数の確認
		if(plocaMicPos->ha_numLocation != fmt->GetNMic()){
			cout << "Mic number is mismatch !" << endl ;
			return false ;
		}
		
		psTFDBHead->iFileType = fmt->GetFormatId() ;
		psTFDBHead->lFs = fmt->GetFS() ;
		psTFDBHead->lNMic = fmt->GetNMic() ;
		psTFDBHead->lNFFT = fmt->GetNFFT() ;
		
		//std::vector<Vector3<float> >  *mic = fmt->GetMicPositions();
		
		float a;
		
		for(i = 0 ; i < fmt->GetNMic() ; i++){
			plocaMicPos->micary_Location[i].ha_fX = (HA_Float)fmt->GetMicPositions()[i].GetX() ;
			plocaMicPos->micary_Location[i].ha_fY = (HA_Float)fmt->GetMicPositions()[i].GetY() ;
			plocaMicPos->micary_Location[i].ha_fZ = (HA_Float)fmt->GetMicPositions()[i].GetZ() ;
			plocaMicPos->micary_Location[i].ha_idPos = 0;//dummy
			//plocaMicPos->ha_idLocation[i] = plocaSrcPos->micary_Location[i].ha_idPos ;
			//cout<<"mic"<<i<<","<<plocaMicPos->micary_Location[i].ha_fX <<","<<plocaMicPos->micary_Location[i].ha_fY <<","<<plocaMicPos->micary_Location[i].ha_fZ <<endl;
		}
		
		if(psTFDBHead->iFileType ==FORMAT_TYPE_TF){
			//GTFチャンクの読込み
		
			boost::shared_ptr<hark::hgtf::GTF1_0> gtf = boost::dynamic_pointer_cast<hark::hgtf::GTF1_0>(hark->GetDataSet("hgtf::GTF"));
			if (gtf == 0)
			{
			  cout << "this is not include GTF dataset version 1.0" << endl;
			  return false ;
			}
			
			//
			if(plocaSrcPos->ha_numLocation != gtf->GetNSource()){
				cout << "Source number is mismatch!" << endl ;
				return false ;
			}
			if(piaFreqLine->ha_numValue != gtf->GetNFIndex()){
				cout << "FreqLine number is mismatch!" << endl ;
				return false ;
			}
			psTFDBHead->lNSrc = gtf->GetNSource() ;
			psTFDBHead->lNFreq = gtf->GetNFIndex() ;
			
			//音源位置座標の読込み
			//std::vector<Vector3<float> > *src_tf gtf->GetSourcePositions();
			for(i = 0 ; i < gtf->GetNSource() ; i++){
				plocaSrcPos->micary_Location[i].ha_fX = (HA_Float)gtf->GetSourcePositions()[i].GetX() ;
				plocaSrcPos->micary_Location[i].ha_fY = (HA_Float)gtf->GetSourcePositions()[i].GetY() ;
				plocaSrcPos->micary_Location[i].ha_fZ = (HA_Float)gtf->GetSourcePositions()[i].GetZ() ;
				plocaSrcPos->micary_Location[i].ha_idPos = 0 ; //dummy
				plocaSrcPos->ha_idLocation[i] = plocaSrcPos->micary_Location[i].ha_idPos ;
				//cout<<"src"<<i<<","<<plocaSrcPos->micary_Location[i].ha_fX <<","<<plocaSrcPos->micary_Location[i].ha_fY <<","<<plocaSrcPos->micary_Location[i].ha_fZ <<endl;
			}

			//周波数ラインの読込み
			//fi = gtf->GetFIndexToBinIndexTable() ;
			for(i = 0 ; i < gtf->GetNFIndex() ; i++){
				piaFreqLine->ha_iValue[i] = gtf->GetFIndexToBinIndexTable()[i] ;
			}
			
		}
		else if(psTFDBHead->iFileType ==FORMAT_TYPE_W)
		{ 
			boost::shared_ptr<hark::hgtf::SM1_0> sm = boost::dynamic_pointer_cast<hark::hgtf::SM1_0>(hark->GetDataSet("hgtf::SM"));
			//バージョンチェック
			if (sm == 0)
			{
			  cout << "this is not include SM dataset version 1.0" << endl;
			  return false ;
			}
			
			if(plocaSrcPos->ha_numLocation != sm->GetNSource()){
				cout << "Source number is mismatch!" << endl ;
				return false ;
			}
			if(piaFreqLine->ha_numValue != sm->GetNFIndex()){
				cout << "FreqLine number is mismatch!" << endl ;
				return false ;
			}
			psTFDBHead->lNSrc = sm->GetNSource() ;
			psTFDBHead->lNFreq = sm->GetNFIndex() ;
			
			//音源位置座標の読込み
			//std::vector<Vector3<float> > *src = sm->GetSourcePositions() ;
			for(i = 0 ; i < sm->GetNSource(); i++){
				plocaSrcPos->micary_Location[i].ha_fX = (HA_Float)sm->GetSourcePositions()[i].GetX() ;
				plocaSrcPos->micary_Location[i].ha_fY = (HA_Float)sm->GetSourcePositions()[i].GetY() ;
				plocaSrcPos->micary_Location[i].ha_fZ = (HA_Float)sm->GetSourcePositions()[i].GetZ() ;
				plocaSrcPos->micary_Location[i].ha_idPos = 0;//dummy
				plocaSrcPos->ha_idLocation[i] = plocaSrcPos->micary_Location[i].ha_idPos ;
				
			}

			//周波数ラインの読込み
			//fi = sm->GetFIndexToBinIndexTable() ;
			for(i = 0 ; i < sm->GetNFIndex() ; i++){
				piaFreqLine->ha_iValue[i] = sm->GetFIndexToBinIndexTable()[i] ;
			}

		
		}
		//SPECチャンクの内容
		boost::shared_ptr<hark::hgtf::Spec1_0> spec = boost::dynamic_pointer_cast<hark::hgtf::Spec1_0>(hark->GetDataSet("hgtf::Spec"));
			//バージョンチェック
			if (spec == 0)
			{
			  cout << "this is not include SPEC dataset version 1.0" << endl;
			  return false ;
			}


		if(bInfo){
		cout << "Date = " << spec->GetDate() << endl ;
		cout << "Location = " << spec->GetLocation() << endl ;
		cout << "Robot = " << spec->GetRobot() << endl ;
		cout << "ArrayType = " << spec->GetMicArrayType() << endl ;
		cout << "Creator = " << spec->GetCreator() << endl ;
		cout << "Memo = " << spec->GetMemo() << endl ;
		}
	}
	catch(std::exception &ex){
		return false ;
	}
	return true ;



}
bool CHarkFileIO::ReadHeader(const char* cFilename,
				MICARY_TFDBFileHeader *psTFDBHead)
{
//	if(HARK::checkIdentifier(cFilename) != HARKIO_TRUE){
//		cout << "Fail to Read Hark File." << endl ;
//		return false ;
//	}

	int i ,j , k;

	try{
		//ファイル読み込み
		ifstream harkFileData(cFilename);
		boost::shared_ptr<hark::HarkFileReader> reader(new hark::HarkFileBinaryReader(harkFileData));
		boost::shared_ptr<hark::HarkFile> hark = reader->Read();

		boost::shared_ptr<hark::hgtf::Fmt1_0> fmt = boost::dynamic_pointer_cast<hark::hgtf::Fmt1_0>(hark->GetDataSet("hgtf::Fmt"));
		if (fmt == 0)
		{
		  cout << "this is not include Fmt dataset version 1.0" << endl;
		  return false ;
		}
		
		psTFDBHead->iFileType = fmt->GetFormatId() ;
		psTFDBHead->lFs = fmt->GetFS() ;
		psTFDBHead->lNMic = fmt->GetNMic() ;
		psTFDBHead->lNFFT = fmt->GetNFFT() ;
		
		if(psTFDBHead->iFileType==FORMAT_TYPE_TF){
			//GTFチャンクの読込み
			 boost::shared_ptr<hark::hgtf::GTF1_0> gtf = boost::dynamic_pointer_cast<hark::hgtf::GTF1_0>(hark->GetDataSet("hgtf::GTF"));
			if (gtf == 0)
			{
			  cout << "this is not include GTF dataset version 1.0" << endl;
			  return false ;
			}
				
			psTFDBHead->lNSrc = gtf->GetNSource() ;
			psTFDBHead->lNFreq = gtf->GetNFIndex() ;
				
	
		}
		if(psTFDBHead->iFileType == FORMAT_TYPE_W){
			//SMチャンクの読込み
			boost::shared_ptr<hark::hgtf::SM1_0> sm = boost::dynamic_pointer_cast<hark::hgtf::SM1_0>(hark->GetDataSet("hgtf::SM"));
			//バージョンチェック
			if (sm == 0)
			{
			  cout << "this is not include SM dataset version 1.0" << endl;
			  return false ;
			}
			
			psTFDBHead->lNSrc = sm->GetNSource() ;
			psTFDBHead->lNFreq = sm->GetNFIndex() ;
	   }
	}
	catch(std::exception &ex){
		return false ;
	}
	return true ;	

}
bool CHarkFileIO::ReadData(const char*cFilename,
					MICARY_ComplexArray *micary_cmpaData)
{
	/*
	if(HARK::checkIdentifier(cFilename) != HARKIO_TRUE){
		cout << "Fail to Read Hark File." << endl ;
		return false ;
	}
	*/

	int i ,j , k;
	int index;

	try{
		
		//ファイル読み込み
		ifstream harkFileData(cFilename);
		boost::shared_ptr<hark::HarkFileReader> reader(new hark::HarkFileBinaryReader(harkFileData));
		boost::shared_ptr<hark::HarkFile> hark = reader->Read();

		boost::shared_ptr<hark::hgtf::Fmt1_0> fmt = boost::dynamic_pointer_cast<hark::hgtf::Fmt1_0>(hark->GetDataSet("hgtf::Fmt"));
		if (fmt == 0)
		{
		  cout << "this is not include Fmt dataset version 1.0" << endl;
		  return false ;
		}
	
		if(fmt->GetFormatId() == FORMAT_TYPE_TF){
			std::cout<<"TF Conj file loaded by libharkio2" << std::endl;
			//GTFチャンクの読込み
				boost::shared_ptr<hark::hgtf::GTF1_0> gtf = boost::dynamic_pointer_cast<hark::hgtf::GTF1_0>(hark->GetDataSet("hgtf::GTF"));
			if (gtf == 0)
			{
			  cout << "this is not include GTF dataset version 1.0" << endl;
			  return false ;
			}
						
			//伝達関数データの読込み
			//tf = gtf->GetTFs() ;
			if(micary_cmpaData->ha_numValue != gtf->GetNFIndex()*gtf->GetNSource()*fmt->GetNMic()){
			  cout << "TF Data size is mismatch! Nbuf = " << micary_cmpaData->ha_numValue << " Nfile = " << gtf->GetNFIndex() * gtf->GetNSource() * fmt->GetNMic() << endl ;
				return false ;
			}
			//cout << "NF = " << gtf->getNFI() << "NS = " << gtf->getNSrc() << "NM = " << fmt->getNMic() << endl ;
 
		
			index = 0 ;
			for(k = 0 ; k < gtf->GetNFIndex() ; k++){
				for(j = 0 ; j < gtf->GetNSource() ; j ++){
					for(i = 0 ; i < fmt->GetNMic() ; i++){
					  micary_cmpaData->ha_cmpValue[index].re = gtf->GetTFs()[j][i][k].real() ;
					  micary_cmpaData->ha_cmpValue[index].im = gtf->GetTFs()[j][i][k].imag() ;
					  //cout << "index = " << index << micary_cmpaData->ha_cmpValue[index].re<< " , " << micary_cmpaData->ha_cmpValue[index].im ;
					  index++ ;
					}
				}
			}
			//	cout << endl ;

		}
		else if(fmt->GetFormatId() == FORMAT_TYPE_W){
			//std::cout<<"Separate Matrix file loaded by libharkio2" << std::endl;
			boost::shared_ptr<hark::hgtf::SM1_0> sm = boost::dynamic_pointer_cast<hark::hgtf::SM1_0>(hark->GetDataSet("hgtf::SM"));
			//バージョンチェック
			if (sm == 0)
			{
			  cout << "this is not include SM dataset version 1.0" << endl;
			  return false ;
			}
			
			//伝達関数データの読込み
			
			if(micary_cmpaData->ha_numValue != sm->GetNFIndex()*sm->GetNSource()*fmt->GetNMic()){
			  cout << "W Data size is mismatch! Nbuf = " << micary_cmpaData->ha_numValue << " Nfile = " << sm->GetNFIndex() * sm->GetNSource() * fmt->GetNMic() << endl ;
				return false ;
			}
		index = 0 ;
			for(k = 0 ; k < sm->GetNFIndex() ; k++){
				for(j = 0 ; j < fmt->GetNMic() ; j ++){
					for(i = 0 ; i < sm->GetNSource() ; i++){
					  micary_cmpaData->ha_cmpValue[index].re = sm->GetSMs()[i][j][k].real() ;
					  micary_cmpaData->ha_cmpValue[index].im = sm->GetSMs()[i][j][k].imag() ;
					  index++ ;
					}
				}
			}
		
		
		}
	}
	catch(std::exception &ex){
		return false ;
	}
	//cout << "Success Read Hark Data." << endl ;
	return true ;
}
bool CHarkFileIO::ConvertToGHDSS(const char* cOrgFilename,
						const char * cNewFilename)
{
	 MICARY_TFDBFileHeader *psH ;
	 MICARY_TFDatabase *pDBase ;
	 MICARY_PrmFrame pFrame ;

	 bool bRtn ;

     psH = new MICARY_TFDBFileHeader() ;
	 pDBase = new MICARY_TFDatabase() ;
	 TFDatabaseAlloc(pDBase,0,0,0) ;

	  bRtn =ReadHeader(cOrgFilename,psH) ;
	  if(!bRtn){
			cout << "Fail to Read HARK Header" << endl ;
			if(pDBase){
			  delete pDBase ;
			  pDBase = NULL ;
			}
			if(psH){
				delete psH ;
				psH = NULL ;
			}
			return false;
	  }

	  if(pDBase->micary_cmpaTF.ha_numValue != psH->lNFreq * psH->lNMic * psH->lNSrc){
		  TFDatabaseAlloc(pDBase,psH->lNFreq,psH->lNSrc,psH->lNMic) ;
	  }

	  //ヘッダとその他周辺データの読込み
		bRtn = ReadInfo(cOrgFilename,
					psH,
					&pDBase->micary_locaSource , 
					&pDBase->micary_locaMic,
					&pDBase->micary_iaFreqLine);
	  if(!bRtn){
		cout << "Fail to Read HARK Header" << endl ;
		if(pDBase){
			delete pDBase ;
			pDBase = NULL ;
		}
		if(psH){
		  delete [] psH ;
		  psH = NULL ;
		}
		return false;
	}


	  //データの読込み
	bRtn = ReadData(cOrgFilename,&pDBase->micary_cmpaTF)  ;

	if(!bRtn){
		cout << "Fail to Read HARK Data" << endl ;
		if(pDBase){
			delete [] pDBase ;
			pDBase = NULL ;
		}
		if(psH){
		  delete [] psH ;
		  psH = NULL ;
		}
		return false;
	}
	
	cout << "Data" << endl ; 
	for(int i = 0 ; i < pDBase->micary_cmpaTF.ha_numValue ; i++){
	  cout << "i = "<< i << "/" << pDBase->micary_cmpaTF.ha_cmpValue[i].re << " , " << pDBase->micary_cmpaTF.ha_cmpValue[i].im ; //<< endl;
	}
	cout << endl ;
	

	return true ;
	 
}

#elif ENABLE_HARKIO1

// --------------------------------------------------------------------
bool CHarkFileIO::WriteData(const char* cFilename,
						MICARY_TFDBFileHeader *psTFDBHead,
						MICARY_ComplexArray micary_cmpaData,
						MICARY_IntArray	micary_iaFreqLine,
						MICARY_LocationArray micary_locaSource,
						MICARY_LocationArray micary_locaMic,
						const char* cDate,
						const char* cLocation,
						const char* cRobot,
						const char* cArrayType,
						const char * cCreator,
						const char* cMemo)
{
	// HGTF Dataオブジェクトの生成
	HARK hark;
	Data *hgtf = NULL;
	FMT *fmt = NULL ;
	GTF *gtf = NULL ;
	SM *sm = NULL ; 
	SPEC *spec = NULL ;
		
	try{
		int nMic,nSrc,nFrq,nFFT,nFs ;
		int ii,jj,kk,index ;
		vector<int> *fi ;
		GTF::SRC_LIST_TYPE *src_tf ;
		GTF::TF_LIST_TYPE *tf ; 
		SM::SRC_LIST_TYPE *src;
		SM::SM_LIST_TYPE *sm_data;
		
		
		hgtf = new HGTF() ;
		// mHarkにdataの追加
		hark.addData(hgtf) ;
	
		nMic = psTFDBHead->lNMic ;
		nSrc = psTFDBHead->lNSrc ;
		nFFT = psTFDBHead->lNFFT ;
		nFs = psTFDBHead->lFs ;
		nFrq = psTFDBHead->lNFreq ;

		//FMTチャンクの作成
		if(psTFDBHead->iFileType == FORMAT_TYPE_TF){ 
			fmt = new FMT(FMT::V1_0,FMT::GTF,nMic,nFFT,nFFT,nFs) ;
		}
		else if(psTFDBHead->iFileType == FORMAT_TYPE_W){
			fmt = new FMT(FMT::V1_0,FMT::SM,nMic,nFFT,nFFT,nFs) ;
		}
		hgtf->addChunk(fmt) ;
	
		// FMT Chunkへマイク座標コンテナを生成して，サンプルデータをセット
		FMT::MIC_LIST_TYPE *mic = fmt->newMic() ;	//マイク座標コンテナの生成
	
		//データセット
		for(ii = 0 ; ii < nMic ; ii++){
			(*mic)[ii].x = micary_locaMic.micary_Location[ii].ha_fX ;
			(*mic)[ii].y = micary_locaMic.micary_Location[ii].ha_fY ;
			(*mic)[ii].z = micary_locaMic.micary_Location[ii].ha_fZ ;
			
		}
		
       		//チャンクの作成
		switch(psTFDBHead->iFileType){
		case FORMAT_TYPE_TF :
				gtf = new GTF(GTF::V1_0,fmt,nSrc,nFrq) ;
			     	hgtf->addChunk(gtf) ;
				
				//Chunkから音源位置座標コンテナを取得して，テストデータをセット
				src_tf = gtf->getSrc() ;
			
				//データセット
				for(ii = 0 ; ii < nSrc ; ii++){
					(*src_tf)[ii].x = micary_locaSource.micary_Location[ii].ha_fX ;
					(*src_tf)[ii].y = micary_locaSource.micary_Location[ii].ha_fY ;
					(*src_tf)[ii].z = micary_locaSource.micary_Location[ii].ha_fZ ;
			     

				}

				//Chunkから伝達関数コンテナを取得して，テストデータをセット
				tf = gtf->getTF() ;
			
				//データをセット(Src->Mic->Freq)
				index = 0 ;
				for(kk = 0 ; kk < nFrq ; kk++){			
					for(ii = 0 ; ii < nSrc ; ii++){
						for(jj = 0 ; jj < nMic ; jj++){
							(*tf)[ii][jj][kk].real() = micary_cmpaData.ha_cmpValue[index].re ;
							(*tf)[ii][jj][kk].imag() = micary_cmpaData.ha_cmpValue[index].im ;
							index++ ;
					      
						}
					}
				}

			
				//伝達関数の周波数インデックスからインデックスへのマップ
				//コンテナを取得
				fi = gtf->getFI() ;
				cout << "NFrq = " << nFrq << endl ;
			
				//データをセット
				for(ii = 0 ; ii < nFrq ; ii++){
					(*fi)[ii] = micary_iaFreqLine.ha_iValue[ii] ;
			      	}
			       
				break ;
		case FORMAT_TYPE_W :
				sm = new SM(SM::V1_0,fmt,nSrc,nFrq) ;
				hgtf->addChunk(sm) ;

				//Chunkから音源位置座標コンテナを取得して，テストデータをセット
				src = sm->getSrc() ;
		
				//データセット
				for(ii = 0 ; ii < nSrc ; ii++){
					(*src)[ii].x = micary_locaSource.micary_Location[ii].ha_fX ;
					(*src)[ii].y = micary_locaSource.micary_Location[ii].ha_fY ;
					(*src)[ii].z = micary_locaSource.micary_Location[ii].ha_fZ ;
				}

				//Chunkから伝達関数コンテナを取得して，テストデータをセット
				sm_data = sm->getSM() ;

				//データをセット(Src->Mic->Freq)
				index = 0 ;
				for(kk = 0 ; kk < nFrq ; kk++){			
					for(jj = 0 ; jj < nMic ; jj++){
						for(ii = 0 ; ii < nSrc ; ii++){
							(*sm_data)[ii][jj][kk].real() = micary_cmpaData.ha_cmpValue[index].re ;
							(*sm_data)[ii][jj][kk].imag() = micary_cmpaData.ha_cmpValue[index].im ;
							index++ ;
						}
					}
				}

				//分離行列の周波数インデックスからインデックスへのマップ
				//コンテナを取得
				fi = sm->getFI() ;

				//データをセット
				for(ii = 0 ; ii < nFrq ; ii++){
					(*fi)[ii] = micary_iaFreqLine.ha_iValue[ii] ;
				}
				break ;
		
		}
		
		//共通　SPECチャンクの作成
		spec = new SPEC(SPEC::V1_0) ;
		
		hgtf->addChunk(spec) ;
		

		//SPECデータの作成
		spec->setDate(cDate) ;
		spec->setLocation(cLocation) ;
		spec->setRobot(cRobot) ;
		spec->setArrayType(cArrayType) ;
		spec->setCreator(cCreator) ;
		spec->setMemo(cMemo) ;

		//harkを書き込み
		hark.writeFile(cFilename) ;
		
	}
	catch(std::exception &ex){
	
		return false ;
	
	}

	//	cout << "Success Write Hark file" << endl ;
	return true ;
}

bool CHarkFileIO::ReadInfo(const char* cFilename,
					MICARY_TFDBFileHeader *psTFDBHead,
					MICARY_LocationArray * plocaSrcPos , 
					MICARY_LocationArray * plocaMicPos ,
			   MICARY_IntArray *piaFreqLine,
			   bool bInfo)
{
	if(HARK::checkIdentifier(cFilename) != HARKIO_TRUE){
		cout << "Fail to Read Hark File." << endl ;
		return false ;
	}
	HARK hark ;
	FMT *fmt ;
	GTF *gtf ;
	SM *sm ;
	int i ,j , k;
	GTF::SRC_LIST_TYPE *src_tf ;
	vector<int> *fi ;
	SM::SRC_LIST_TYPE *src ;
	FMT::MIC_LIST_TYPE *mic ;

	try{
		//ファイル読み込み
		hark.readFile(cFilename) ;
		
		Chunk::VERSION_CHECK version_check ;
		// FMTチャンク
		fmt = hark.getData()->getChunk<FMT>() ;

		//バージョンチェック
		version_check = fmt->checkVersion(FMT::V1_0) ;
		if(version_check == Chunk::MISMATCH){
			return false ;
		}
		else if(version_check == Chunk::SAFE_MATCH){
			cout << harkio::Error::getInstance().getMsg() << endl ;
		}
		// Mic数,Freq数の確認
		if(plocaMicPos->ha_numLocation != fmt->getNMic()){
			cout << "Mic number is mismatch !" << endl ;
			return false ;
		}
		
		psTFDBHead->iFileType = fmt->getFormatId() ;
		psTFDBHead->lFs = fmt->getNFS() ;
		psTFDBHead->lNMic = fmt->getNMic() ;
		psTFDBHead->lNFFT = fmt->getNFFT() ;
		psTFDBHead->lFs = fmt->getNFS() ;

		mic = fmt->getMic() ;
		for(i = 0 ; i < fmt->getNMic() ; i++){
			plocaMicPos->micary_Location[i].ha_fX = (*mic)[i].x ;
			plocaMicPos->micary_Location[i].ha_fY = (*mic)[i].y ;
			plocaMicPos->micary_Location[i].ha_fZ = (*mic)[i].z ;
			plocaMicPos->micary_Location[i].ha_idPos = 0;//dummy
			//plocaMicPos->ha_idLocation[i] = plocaSrcPos->micary_Location[i].ha_idPos ;
		}


		switch(psTFDBHead->iFileType){
		case FORMAT_TYPE_TF :	//GTFチャンクの読込み
			gtf = hark.getData()->getChunk<GTF>() ;
			//バージョンチェック
			version_check = gtf->checkVersion(GTF::V1_0) ;
			if(version_check == Chunk::MISMATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
				return false ;
			}
			else if(version_check == Chunk::SAFE_MATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
			}
			if(plocaSrcPos->ha_numLocation != gtf->getNSrc()){
				cout << "Source number is mismatch!" << endl ;
				return false ;
			}
			if(piaFreqLine->ha_numValue != gtf->getNFI()){
				cout << "FreqLine number is mismatch!" << endl ;
				return false ;
			}
			psTFDBHead->lNSrc = gtf->getNSrc() ;
			psTFDBHead->lNFreq = gtf->getNFI() ;
			
			//音源位置座標の読込み
			src_tf = gtf->getSrc() ;
			for(i = 0 ; i < src_tf->size() ; i++){
				plocaSrcPos->micary_Location[i].ha_fX = (*src_tf)[i].x ;
				plocaSrcPos->micary_Location[i].ha_fY = (*src_tf)[i].y ;
				plocaSrcPos->micary_Location[i].ha_fZ = (*src_tf)[i].z ;
				plocaSrcPos->micary_Location[i].ha_idPos = 0 ; //dummy
				plocaSrcPos->ha_idLocation[i] = plocaSrcPos->micary_Location[i].ha_idPos ;
			}

			//周波数ラインの読込み
			fi = gtf->getFI() ;
			for(i = 0 ; i < fi->size() ; i++){
				piaFreqLine->ha_iValue[i] = (*fi)[i] ;
			}
		break ;

		case FORMAT_TYPE_W:
			sm = hark.getData()->getChunk<SM>() ;
			//バージョンチェック
			version_check = sm->checkVersion(SM::V1_0) ;
			
			if(version_check == Chunk::MISMATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
				return false ;
			}
			else if(version_check == Chunk::SAFE_MATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
			}
			if(plocaSrcPos->ha_numLocation != sm->getNSrc()){
				cout << "Source number is mismatch!" << endl ;
				return false ;
			}
			if(piaFreqLine->ha_numValue != sm->getNFI()){
				cout << "FreqLine number is mismatch!" << endl ;
				return false ;
			}
			psTFDBHead->lNSrc = sm->getNSrc() ;
			psTFDBHead->lNFreq = sm->getNFI() ;
			
			//音源位置座標の読込み
			src = sm->getSrc() ;
			for(i = 0 ; i < src->size() ; i++){
				plocaSrcPos->micary_Location[i].ha_fX = (*src)[i].x ;
				plocaSrcPos->micary_Location[i].ha_fY = (*src)[i].y ;
				plocaSrcPos->micary_Location[i].ha_fZ = (*src)[i].z ;
				plocaSrcPos->micary_Location[i].ha_idPos = 0;//dummy
				plocaSrcPos->ha_idLocation[i] = plocaSrcPos->micary_Location[i].ha_idPos ;
			}

			//周波数ラインの読込み
			fi = sm->getFI() ;
			for(i = 0 ; i < fi->size() ; i++){
				piaFreqLine->ha_iValue[i] = (*fi)[i] ;
			}

		break ;
	}
	//SPECチャンクの内容
	SPEC *spec = hark.getData()->getChunk<SPEC>() ;

	//バージョンチェック
	version_check = spec->checkVersion(SPEC::V1_0) ;
	if(version_check == Chunk::MISMATCH){
		cout << harkio::Error::getInstance().getMsg() << endl ;
		return false ;
	}
	else if(version_check == Chunk::SAFE_MATCH){
		cout << harkio::Error::getInstance().getMsg() << endl ;
	}
	
	if(bInfo){
	cout << "Date = " << spec->getDate() << endl ;
	cout << "Location = " << spec->getLocation() << endl ;
	cout << "Robot = " << spec->getRobot() << endl ;
	cout << "ArrayType = " << spec->getArrayType() << endl ;
	cout << "Creator = " << spec->getCreator() << endl ;
	cout << "Memo = " << spec->getMemo() << endl ;
	}
	}
	catch(std::exception &ex){
		return false ;
	}
	return true ;

}
bool CHarkFileIO::ReadHeader(const char* cFilename,
				MICARY_TFDBFileHeader *psTFDBHead)
{
	if(HARK::checkIdentifier(cFilename) != HARKIO_TRUE){
		cout << "Fail to Read Hark File." << endl ;
		return false ;
	}
	HARK hark ;
	FMT *fmt ;
	GTF *gtf ;
	SM *sm ;
	int i ,j , k;

	try{
		//ファイル読み込み
		hark.readFile(cFilename) ;
		
		Chunk::VERSION_CHECK version_check ;
		// FMTチャンク
		fmt = hark.getData()->getChunk<FMT>() ;

		//バージョンチェック
		version_check = fmt->checkVersion(FMT::V1_0) ;
		if(version_check == Chunk::MISMATCH){
			return false ;
		}
		else if(version_check == Chunk::SAFE_MATCH){
			cout << harkio::Error::getInstance().getMsg() << endl ;
		}
		
		psTFDBHead->iFileType = fmt->getFormatId() ;
		psTFDBHead->lFs = fmt->getNFS() ;
		psTFDBHead->lNMic = fmt->getNMic() ;
		psTFDBHead->lNFFT = fmt->getNFFT() ;
		psTFDBHead->lFs = fmt->getNFS() ;
		
		switch(psTFDBHead->iFileType){
		case FORMAT_TYPE_TF :	//GTFチャンクの読込み
			gtf = hark.getData()->getChunk<GTF>() ;
			//バージョンチェック
			version_check = gtf->checkVersion(GTF::V1_0) ;
			if(version_check == Chunk::MISMATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
				return false ;
			}
			else if(version_check == Chunk::SAFE_MATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
			}
			psTFDBHead->lNSrc = gtf->getNSrc() ;
			psTFDBHead->lNFreq = gtf->getNFI() ;
		break ;

		case FORMAT_TYPE_W:
			sm = hark.getData()->getChunk<SM>() ;
			//バージョンチェック
			version_check = sm->checkVersion(SM::V1_0) ;
			
			if(version_check == Chunk::MISMATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
				return false ;
			}
			else if(version_check == Chunk::SAFE_MATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
			}
			psTFDBHead->lNSrc = sm->getNSrc() ;
			psTFDBHead->lNFreq = sm->getNFI() ;
			

		break ;
	   }
	}
	catch(std::exception &ex){
		return false ;
	}
	return true ;	

}
bool CHarkFileIO::ReadData(const char*cFilename,
					MICARY_ComplexArray *micary_cmpaData)
{
	if(HARK::checkIdentifier(cFilename) != HARKIO_TRUE){
		cout << "Fail to Read Hark File." << endl ;
		return false ;
	}
	HARK hark ;
	FMT *fmt ;
	GTF *gtf ;
	SM *sm ;
	int i ,j , k;
	int index;
	GTF::TF_LIST_TYPE *tf ;
	SM::SM_LIST_TYPE *sm_data ;

	try{
		//ファイル読み込み
		hark.readFile(cFilename) ;
		
		Chunk::VERSION_CHECK version_check ;
		// FMTチャンク
		fmt = hark.getData()->getChunk<FMT>() ;

		//バージョンチェック
		version_check = fmt->checkVersion(FMT::V1_0) ;
		if(version_check == Chunk::MISMATCH){
			return false ;
		}
		else if(version_check == Chunk::SAFE_MATCH){
			cout << harkio::Error::getInstance().getMsg() << endl ;
		}
	
		switch(fmt->getFormatId()){
		case FORMAT_TYPE_TF :	//GTFチャンクの読込み
			std::cout<<"TF Conj file loaded by libharkio1" << std::endl;
			gtf = hark.getData()->getChunk<GTF>() ;
			//バージョンチェック
			version_check = gtf->checkVersion(GTF::V1_0) ;
			if(version_check == Chunk::MISMATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
				return false ;
			}
			else if(version_check == Chunk::SAFE_MATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
			}
						
			//伝達関数データの読込み
			tf = gtf->getTF() ;
			if(micary_cmpaData->ha_numValue != gtf->getNFI()*gtf->getNSrc()*fmt->getNMic()){
			  cout << "TF Data size is mismatch! Nbuf = " << micary_cmpaData->ha_numValue << " Nfile = " << gtf->getNFI()*gtf->getNSrc() *fmt->getNMic() << endl ;
				return false ;
			}
			//cout << "NF = " << gtf->getNFI() << "NS = " << gtf->getNSrc() << "NM = " << fmt->getNMic() << endl ;
 
		
			index = 0 ;
			for(k = 0 ; k < gtf->getNFI() ; k++){
				for(j = 0 ; j < gtf->getNSrc() ; j ++){
					for(i = 0 ; i < fmt->getNMic() ; i++){
					  micary_cmpaData->ha_cmpValue[index].re =(HA_Float) (*tf)[j][i][k].real() ;
					  micary_cmpaData->ha_cmpValue[index].im =(HA_Float) (*tf)[j][i][k].imag() ;
					  //cout << "index = " << index << micary_cmpaData->ha_cmpValue[index].re<< " , " << micary_cmpaData->ha_cmpValue[index].im ;
					  index++ ;
					}
				}
			}
			//	cout << endl ;
		break ;

		case FORMAT_TYPE_W:
			std::cout<<"Separated Matrix file loaded by libharkio1" << std::endl;
			sm = hark.getData()->getChunk<SM>() ;
			//バージョンチェック
			version_check = sm->checkVersion(SM::V1_0) ;
			
			if(version_check == Chunk::MISMATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
				return false ;
			}
			else if(version_check == Chunk::SAFE_MATCH){
				cout << harkio::Error::getInstance().getMsg() << endl ;
			}
			
			//伝達関数データの読込み
			sm_data = sm->getSM() ;
			if(micary_cmpaData->ha_numValue != sm->getNFI()*sm->getNSrc()*fmt->getNMic()){
			  cout << "W Data size is mismatch! Nbuf = " << micary_cmpaData->ha_numValue << " Nfile = " << sm->getNFI()*sm->getNSrc() *fmt->getNMic() << endl ;
				return false ;
			}
		index = 0 ;
			for(k = 0 ; k < sm->getNFI() ; k++){
				for(j = 0 ; j < fmt->getNMic() ; j ++){
					for(i = 0 ; i < sm->getNSrc() ; i++){
					  micary_cmpaData->ha_cmpValue[index].re = (HA_Float)(*sm_data)[i][j][k].real() ;
					  micary_cmpaData->ha_cmpValue[index].im = (HA_Float)(*sm_data)[i][j][k].imag() ;
					  index++ ;
					}
				}
			}
		
			break ;
		}
	}
	catch(std::exception &ex){
		return false ;
	}
	//cout << "Success Read Hark Data." << endl ;
	return true ;
}

bool CHarkFileIO::ConvertToGHDSS(const char* cOrgFilename,
						const char * cNewFilename)
{
	 MICARY_TFDBFileHeader *psH ;
	 MICARY_TFDatabase *pDBase ;
	 MICARY_PrmFrame pFrame ;

	 bool bRtn ;

     psH = new MICARY_TFDBFileHeader() ;
	 pDBase = new MICARY_TFDatabase() ;
	 TFDatabaseAlloc(pDBase,0,0,0) ;

	  bRtn =ReadHeader(cOrgFilename,psH) ;
	  if(!bRtn){
			cout << "Fail to Read HARK Header" << endl ;
			if(pDBase){
			  delete pDBase ;
			  pDBase = NULL ;
			}
			if(psH){
				delete psH ;
				psH = NULL ;
			}
			return false;
	  }

	  if(pDBase->micary_cmpaTF.ha_numValue != psH->lNFreq * psH->lNMic * psH->lNSrc){
		  TFDatabaseAlloc(pDBase,psH->lNFreq,psH->lNSrc,psH->lNMic) ;
	  }

	  //ヘッダとその他周辺データの読込み
		bRtn = ReadInfo(cOrgFilename,
					psH,
					&pDBase->micary_locaSource , 
					&pDBase->micary_locaMic,
					&pDBase->micary_iaFreqLine);
	  if(!bRtn){
		cout << "Fail to Read HARK Header" << endl ;
		if(pDBase){
			delete [] pDBase ;
			pDBase = NULL ;
		}
		if(psH){
		  delete [] psH ;
		  psH = NULL ;
		}
		return false;
	}


	  //データの読込み
	bRtn = ReadData(cOrgFilename,&pDBase->micary_cmpaTF)  ;

	if(!bRtn){
		cout << "Fail to Read HARK Data" << endl ;
		if(pDBase){
			delete [] pDBase ;
			pDBase = NULL ;
		}
		if(psH){
		  delete [] psH ;
		  psH = NULL ;
		}
		return false;
	}
	
	cout << "Data" << endl ; 
	for(int i = 0 ; i < pDBase->micary_cmpaTF.ha_numValue ; i++){
	  cout << "i = "<< i << "/" << pDBase->micary_cmpaTF.ha_cmpValue[i].re << " , " << pDBase->micary_cmpaTF.ha_cmpValue[i].im ; //<< endl;
	}
	cout << endl ;
	

	return true ;
	 
}


bool CHarkFileIO::LoadPositionData(const char* cFilename,
					MICARY_LocationArray * plocaPos)
{
  int type ;
  int Npos ;
  unsigned int i ;
  harkio::TextData files ;
  harkio::TextBase *file ;

  file = files.readFile(cFilename);
  type = files.checkFileIdentifier(cFilename) ;
  
  switch(type){
  case harkio::TextData::TYPE::MICARYLocationFile :
    // マイクファイルの場合
    harkio::MICARYLocationFile* micfile ;
    file = files.readFile(cFilename) ;
    micfile = ((harkio::MICARYLocationFile*) file) ;
    micfile->atLeastVersion(harkio::MICARYLocationFile::V1_0) ;
    
    Npos = micfile->mic_lists.size() ;
    if(Npos != plocaPos->ha_numLocation){
      if(plocaPos){
	LocationArrayFree(plocaPos) ;
      }
      LocationArrayAlloc(plocaPos,Npos) ;		 
    }  
  
    for(i = 0 ; i < Npos ;i++){
      plocaPos->micary_Location[i].ha_fX = micfile->mic_lists[i].x ;
      plocaPos->micary_Location[i].ha_fY = micfile->mic_lists[i].y ;
      plocaPos->micary_Location[i].ha_fZ = micfile->mic_lists[i].z ;
      // 自動割り振り
      plocaPos->micary_Location[i].ha_idPos =i ;
      plocaPos->ha_idLocation[i] = i ;
    }
    delete micfile ;
    break ;
  case harkio::TextData::TYPE::NoiseLocationFile :
    // ノイズファイルの場合
    harkio::NoiseLocationFile* noisefile ;
    file = files.readFile(cFilename) ;
    noisefile = ((harkio::NoiseLocationFile*) file) ;
    noisefile->atLeastVersion(harkio::NoiseLocationFile::V1_0) ;
   
    Npos = noisefile->noise_lists.size() ;
    if(Npos != plocaPos->ha_numLocation){
      if(plocaPos){
	LocationArrayFree(plocaPos) ;
      }
      LocationArrayAlloc(plocaPos,Npos) ;		 
    }  
  
    for(i = 0 ; i < Npos ;i++){
      plocaPos->micary_Location[i].ha_fX = noisefile->noise_lists[i].x ;
      plocaPos->micary_Location[i].ha_fY = noisefile->noise_lists[i].y ;
      plocaPos->micary_Location[i].ha_fZ = noisefile->noise_lists[i].z ;
      // 自動割り振り
      plocaPos->micary_Location[i].ha_idPos = i ;
      plocaPos->ha_idLocation[i] = i ;
    }
    delete noisefile ;
    break ;
  default :
    cout << "Unknown File Type:" << cFilename << endl ;
    break ;
  }
  
  return true ;
}

#endif

bool CHarkFileIO::LoadPositionData(const char* cFilename,
					MICARY_LocationArray * plocaPos,int iFileType)
{
 	if(!cFilename || !plocaPos){
		return false ;
	}
	
	string TAG_TITLE ;
	// file open
	std::ifstream ifs(cFilename) ;

	if(ifs.fail()){
		cout << "Can't open Position file " << endl ;
		return false ;
	}

	if(iFileType == __FILE_TYPE_MIC){
		TAG_TITLE = "MICARY-LocationFile" ;
	}
	else{
		TAG_TITLE = "Noise-LocationFile" ;
	}
	string TAG_VERSION = "Version" ;
	string TAG_NUM_POSITION = "NumOfPosition" ;
	string TAG_POSITION	= "Position" ;

	const int BUF = 1024 ;
        char buf[BUF] ;
	
	int Npos ;
	int index = 0 ;

	memset(buf,'\0',BUF) ;

	string tag ;
	string line_val ;
	string line ;
	int offset ;

        cout << "Read File = " << cFilename << endl ;

	while(!ifs.eof()){
	 
	  // 2010.7.9
	  // ifs.getline(buf,1024,'\r\n') ;
	  //  line = buf ;
	  ifs.getline(buf,1024) ;
	  line = buf ;
	 	  
		// コメント
		offset = line.find_first_of("#");
		if(offset == 0){
		  cout << line << endl ;
			continue ;
		}else{
		  break;
		}
		
	}	

	// TAG_TITLE
	// ifs.getline(buf,1024) ;
	// line = buf ;

	cout << "File_type = " << buf << endl ;

	//if(line != TAG_TITLE){
	if(line.find(TAG_TITLE) == -1){
		cout << "File format is different." << endl ;
		ifs.close() ;
		return false ;
	}
	

	// TAG_VERSION
	ifs.getline(buf,1024) ;
	line = buf ;
	offset = line.find(TAG_VERSION) ;

	if(offset == -1){
		cout << "File version is different." << endl ;
		ifs.close() ;
		return false ;
	}
	else{
		cout << line  << endl ;
	}

	while(!ifs.eof()){
	 
	  // 2010.7.9
	  // ifs.getline(buf,1024,'\r\n') ;
	  //  line = buf ;
	  ifs.getline(buf,1024) ;
	  line = buf ;
	 	  
		// コメント
		offset = line.find_first_of("#");
		if(offset == 0){
		  cout << line << endl ;
			continue ;
		}

		// TAG_NUMOFPOSITION
		offset = line.find_first_of("=") ;
		// TAGと値の分割
		if(offset != -1){	//NumOfPosition
			tag = line.substr(0,offset) ;
			line_val = line.substr(offset+1) ;
			if(tag.find_first_of(" ")!=-1) tag = tag.substr(0,tag.find_first_of(" "));
			if(tag == TAG_NUM_POSITION){
				Npos = atoi(line_val.c_str()) ;
				cout << "NumberOfPosition = " << line_val <<  endl ;
				if(Npos != plocaPos->ha_numLocation){
				   if(plocaPos){
				    LocationArrayFree(plocaPos) ;
				  }
				   cout << "alloc" << endl ;
				  LocationArrayAlloc(plocaPos,Npos) ;
				 
				}  
				
				continue ;
			}
		}
		
		// Position
		//cout << line << endl ;
		offset = line.find_first_of(' ') ;
	
		if(offset != -1){
			tag = line.substr(0,offset) ;
			line_val = line.substr(offset+1) ;
			
			if(tag == TAG_POSITION){
				string line_new ;
				int offset_new = 0 ;
				float pos[3] ;
				int id ;
				for(int i = 0 ; i <= 3 ; i++){
				  if(i == 3){
				  	  pos[i-1] = atof(line_val.c_str()) ;
					  break ;
				  }  
				  offset_new = line_val.find_first_of(",") ;
				  if(offset_new == -1){
					break ;
				  }
				  line_new = line_val.substr(0,offset_new) ;
				  if(i == 0){
					id = atoi(line_new.c_str()) ;
				  }
				  else{
					pos[i-1] = atof(line_new.c_str()) ;
				  }
				  line_val = line_val.substr(offset_new+1) ;
				}
				//if(index < Npos){
				if(index < Npos){
			            plocaPos->micary_Location[index].ha_fX = pos[0] ;
				    plocaPos->micary_Location[index].ha_fY = pos[1] ;
				    plocaPos->micary_Location[index].ha_fZ = pos[2] ;
				    plocaPos->micary_Location[index].ha_idPos =id ;
				    plocaPos->ha_idLocation[index] = id ;
				    //index++ ;
				  
				  cout << "position " << id << " " << plocaPos->micary_Location[index].ha_fX << "," << plocaPos->micary_Location[index].ha_fY << "," << plocaPos->micary_Location[index].ha_fZ << endl ;
				  index++ ;
				} 
						  
			}
		}
	}
 
	return true ;
  

}
#endif // ENABLE_HARKIO_NH
