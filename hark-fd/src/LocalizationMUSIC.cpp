/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#define ENABLE_TIMER 0

#include "../config.h"

#ifdef ENABLE_HARKIO
#ifndef ENABLE_HARKIO_NH

#include "LocalizationMUSIC.hpp"

#include <iostream>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "evd.hpp"
#include "svd.hpp"
// #include "svdold.hpp" 

#ifdef ENABLE_HARKIO2
#include <harkio2/harkio.hpp>
#elif ENABLE_HARKIO1
#include <libharkio/libharkio.h> 
#endif

using namespace std;
using namespace boost;
using namespace boost::numeric;
using namespace boost::posix_time; 

#ifdef ENABLE_HARKIO2
using namespace harkio2;
#elif ENABLE_HARKIO1
using namespace harkio;
#endif

struct less_for_vec {
  bool operator()(vector<float>& v, int i, int j) { return v[i] < v[j]; }
};

LocalizationMUSIC::LocalizationMUSIC(int num_mic, int sampling_rate,
                                     int tlength, int shift, string peak_search_alg, int maxn_out_peaks) :
  NumMicrophones(num_mic), TimeLength(tlength), Shift(shift),
  SamplingRate(sampling_rate), R(num_mic, num_mic), V(num_mic, num_mic),
  RN(num_mic, num_mic), VN(num_mic, num_mic),
  TempCalc1(num_mic, num_mic), TempCalc2(num_mic, num_mic),
  TempCalc3(num_mic, num_mic), TempCalc4(num_mic, num_mic),
  TempCalc5(num_mic, num_mic), TempCalc6(num_mic, num_mic),
  TempCalc7(num_mic, num_mic), TempCalc8(num_mic, num_mic),
  TempCalc9(num_mic, num_mic),
  NumSource(num_mic)
{
  FreqLength = TimeLength / 2 + 1;

  UsesFreq.resize(FreqLength);

  psearch_alg = peak_search_alg;
  maxnum_output_peaks = maxn_out_peaks;

  EigenValue.resize(FreqLength);
  CEigenValue.resize(NumMicrophones);
  REigenValue.resize(NumMicrophones);
  MaxRxx.resize(FreqLength);
  MaxRxxN.resize(FreqLength);  
  E.resize(FreqLength);
  EN.resize(FreqLength);
  for (int k = 0; k < FreqLength; k++) {
    EigenValue[k].resize(NumMicrophones);
    E[k].resize(NumMicrophones, NumMicrophones);
    EN[k].resize(NumMicrophones, NumMicrophones);
  }

  FoundHeight.resize(NumSource);
  FoundDirection.resize(NumSource);
  FoundRange.resize(NumSource);

  FoundHeight_Itpl.resize(NumSource);
  FoundDirection_Itpl.resize(NumSource);
  FoundRange_Itpl.resize(NumSource);
  FoundPower_Itpl.resize(NumSource);

}

void LocalizationMUSIC::SetUsedFrequency(int lo, int up)
{
  int lower = (int)((float)lo * TimeLength / SamplingRate);
  int upper = (int)((float)up * TimeLength / SamplingRate);
  for (int k = lower; k < upper; k++) {
    UsesFreq[k] = true;
  }
}

void LocalizationMUSIC::Localize(vector<fcmatrix>& Rxx, vector<fcmatrix>& RxxN, 
				  int mindeg, int maxdeg, 
				  string music_alg, 
				  ublas::vector<float>& MSSW,
				  bool enable_eigenvalue_weight,
				  bool enable_hierarchical)
{

#if ENABLE_TIMER
  ptime start_time;
  ptime end_time;
  start_time = microsec_clock::local_time();
#endif

  for (int k = 0; k < UsesFreq.size(); k++) {

    MaxRxx[k] = 1.0;
    MaxRxxN[k] = 1.0;

    if (!UsesFreq[k]) {
      continue;
    }

    float max = MaxOfAbsValue(Rxx[k]);
    noalias(R) = Rxx[k] / (max + 1.0f);
    MaxRxx[k] = max + 1.0f;

    if(music_alg != "SEVD"){
      float maxN = MaxOfAbsValue(RxxN[k]);
      noalias(RN) = RxxN[k] / (maxN);
      MaxRxxN[k] = maxN;
    }

    //============= GEVD ==============
    //printf("%d ", k);
    
    if(music_alg == "SEVD"){
      evd(R, EigenValue[k], V);
    }
    else if (music_alg == "GEVD"){
      invMAT(RN,TempCalc1);
      evd(TempCalc1, REigenValue, VN);
      for(int i = 0; i < REigenValue.size(); i++){ 
	if(REigenValue[i] < 0)
	  REigenValue[i] = -1.0f * pow(-1.0f*REigenValue[i],(float)0.5);
	else	
	  REigenValue[i] = pow(REigenValue[i], (float)0.5);    
      }
      invMAT(VN,TempCalc2);
      for(int i = 0; i < TempCalc3.size1(); i++) {
	for(int j = 0; j < TempCalc3.size2(); j++) {
	  TempCalc3(i,j) = 0.0;
	  if(i==j) TempCalc3(i,j) = REigenValue[i];
	}
      }
      noalias(TempCalc6) = prod(TempCalc3,TempCalc2);
      noalias(TempCalc4) = prod(VN, TempCalc6);
      noalias(TempCalc7) = prod(R,TempCalc4);
      noalias(TempCalc5) = prod(TempCalc4, TempCalc7);
      evd(TempCalc5, EigenValue[k], V);
    }
    else if (music_alg == "GSVD"){
      invMAT(RN,TempCalc1);
      noalias(TempCalc2) = prod(TempCalc1, R);
      svd(TempCalc2, REigenValue, TempCalc4, TempCalc3);
      sort_eig_fvec_ascending(REigenValue, TempCalc4, EigenValue[k], V);
    }
    
    noalias(E[k]) = V;    
        
  }

  //printf("\n");
  
  
  CalcAveragePower(EigenValue, E, A, UsesFreq, NumSource, Pavg, Pavg_bin, MSSW, enable_eigenvalue_weight);

  for (int ih = 0; ih < SearchHeight.size(); ih++) {
    for (int id = 0; id < SearchDirection.size(); id++) {
      for (int ir = 0; ir < SearchRange.size(); ir++) {
	Pavg_sum[ih][id][ir] = 10 * log10(Pavg[ih][id][ir] + 1.0f);
      }
    }
  }
  
  if(maxnum_output_peaks < 0) maxnum_output_peaks = NumSource;
  FoundSourceCount = SearchLocation(Pavg_sum, Pavg, maxnum_output_peaks,
                                    FoundHeight, FoundDirection, FoundRange, SearchDirection, mindeg, maxdeg);

  // High resolution localization by interpolation

  if(enable_hierarchical){

#if 0
    cout << "FoundSourceCount : " << FoundSourceCount << endl;
#endif

    for(int cnt = 0; cnt < FoundSourceCount; cnt++){    

      // Search two closest points in each axis

      int height_id_c = FoundHeight[cnt];
      int height_id_l = (height_id_c == 0 ? 0 : height_id_c - 1);
      int height_id_h = (height_id_c == SearchHeight.size() - 1 ? SearchHeight.size() - 1 : height_id_c + 1);      
      float limz_l = (float) SearchHeight[height_id_l];
      float limz_h = (float) SearchHeight[height_id_h];

      int  azimuth_id_c = FoundDirection[cnt];
      int  azimuth_id_l = (azimuth_id_c == 0 ? SearchDirection.size() - 1 : azimuth_id_c - 1);      
      int  azimuth_id_h = (azimuth_id_c == SearchDirection.size() - 1 ? 0 : azimuth_id_c + 1);      
      float limx_l = (float) SearchDirection[azimuth_id_l];
      float limx_h = (float) SearchDirection[azimuth_id_h];
            
      int range_id_c = FoundRange[cnt];
      int range_id_l = (range_id_c == 0 ? 0 : range_id_c - 1);
      int range_id_h = (range_id_c == SearchRange.size() - 1 ? SearchRange.size() - 1 : range_id_c + 1);      
      float limy_l = (float) SearchRange[range_id_l];
      float limy_h = (float) SearchRange[range_id_h];
  
      // Trim the search range

      for (int ih = 0; ih < SearchHeight_Itpl.size(); ih++) {
	if((SearchHeight_Itpl[ih] >= limz_l)&&(SearchHeight_Itpl[ih] <= limz_h))
	  SearchHeightFlag_Itpl[ih] = true;
	else
	  SearchHeightFlag_Itpl[ih] = false;
      }

      for (int id = 0; id < SearchDirection_Itpl.size(); id++) {
	if(limx_l <= limx_h){
	  if((SearchDirection_Itpl[id] >= limx_l)&&(SearchDirection_Itpl[id] <= limx_h))
	    SearchDirectionFlag_Itpl[id] = true;
	  else
	    SearchDirectionFlag_Itpl[id] = false;
	}
	if(limx_l > limx_h){
	  if((SearchDirection_Itpl[id] >= limx_l)||(SearchDirection_Itpl[id] <= limx_h))
	    SearchDirectionFlag_Itpl[id] = true;
	  else
	    SearchDirectionFlag_Itpl[id] = false;
	}
      }

      for (int ir = 0; ir < SearchRange_Itpl.size(); ir++) {
	if((SearchRange_Itpl[ir] >= limy_l)&&(SearchRange_Itpl[ir] <= limy_h))
	  SearchRangeFlag_Itpl[ir] = true;
	else
	  SearchRangeFlag_Itpl[ir] = false;
      }
     
#if 0

      for (int ih = 0; ih < SearchHeightFlag_Itpl.size(); ih++) {
	cout << SearchHeightFlag_Itpl[ih] << " ";
      }      
      cout << endl;

      for (int id = 0; id < SearchDirectionFlag_Itpl.size(); id++) {
	cout << SearchDirectionFlag_Itpl[id] << " ";
      }      
      cout << endl;

      for (int ir = 0; ir < SearchRangeFlag_Itpl.size(); ir++) {
	cout << SearchRangeFlag_Itpl[ir] << " ";
      }      
      cout << endl;

#endif

      // Localization
   
      CalcAveragePower(EigenValue, E, A_Itpl, UsesFreq, NumSource, Pavg_Itpl, Pavg_bin_Itpl, MSSW, enable_eigenvalue_weight,
		       SearchHeightFlag_Itpl, SearchDirectionFlag_Itpl, SearchRangeFlag_Itpl);
      
      for (int ih = 0; ih < SearchHeight_Itpl.size(); ih++) {
	for (int id = 0; id < SearchDirection_Itpl.size(); id++) {
	  for (int ir = 0; ir < SearchRange_Itpl.size(); ir++) {
	    Pavg_sum_Itpl[ih][id][ir] = 10 * log10(Pavg_Itpl[ih][id][ir] + 1.0f);
	  }
	}
      }
      
      vector<int> FoundHeight_Itpl_tmp;
      FoundHeight_Itpl_tmp.resize(1);
      vector<int> FoundDirection_Itpl_tmp;
      FoundDirection_Itpl_tmp.resize(1);
      vector<int> FoundRange_Itpl_tmp;
      FoundRange_Itpl_tmp.resize(1);
    
      //int FoundSourceCount_Itpl = SearchLocation(Pavg_sum_Itpl, Pavg_Itpl, 1, 
      //					 FoundHeight_Itpl_tmp, FoundDirection_Itpl_tmp, FoundRange_Itpl_tmp, 
      //					 SearchDirection_Itpl, limx_l, limx_h);
      int FoundSourceCount_Itpl = SearchLocation(Pavg_sum_Itpl, Pavg_Itpl, 1, 
						 FoundHeight_Itpl_tmp, FoundDirection_Itpl_tmp, FoundRange_Itpl_tmp, 
						 SearchDirection_Itpl, -180, 180);
            
      if(FoundSourceCount_Itpl != 1) {
	cout << "Interpolation generated a not appropriate size output : " << FoundSourceCount_Itpl << endl;
	exit(1);
      }
      
      FoundDirection_Itpl[cnt] = FoundDirection_Itpl_tmp[0];
      FoundRange_Itpl[cnt] = FoundRange_Itpl_tmp[0];
      FoundHeight_Itpl[cnt] = FoundHeight_Itpl_tmp[0];
      FoundPower_Itpl[cnt] = Pavg_sum_Itpl[FoundHeight_Itpl_tmp[0]][FoundDirection_Itpl_tmp[0]][FoundRange_Itpl_tmp[0]]; 

    }
    
  }

#if ENABLE_TIMER
  end_time = microsec_clock::local_time();
  cout << end_time - start_time << endl;
#endif

}

void LocalizationMUSIC::ReadTransferFunction(const char* filename,
					      vector<int>& ch,
					      bool enable_interpolation,
					      bool enable_hierarchical,
					      string interpolation_type,
					      vector<float>& resolution)
{

#ifdef ENABLE_HARKIO2

  try
    {
      ifstream harkFileData(filename);
      shared_ptr<hark::HarkFileReader> reader(new hark::HarkFileBinaryReader(harkFileData));
      shared_ptr<hark::HarkFile> hark = reader->Read();
      
      shared_ptr<hark::hgtf::Fmt1_0> fmt = dynamic_pointer_cast<hark::hgtf::Fmt1_0>(hark->GetDataSet("hgtf::Fmt"));
      if (fmt == 0)
	{
	  cout << "this is not include Fmt dataset version 1.0" << endl;
	  exit(1);
	}
      shared_ptr<hark::hgtf::M2PG1_0> m2pg = dynamic_pointer_cast<hark::hgtf::M2PG1_0>(hark->GetDataSet("hgtf::M2PG"));
      if (m2pg == 0)
	{
	  cout << "this is not include M2PG dataset version 1.0" << endl;
	  exit(1);
	}
      
      int slength = fmt->GetNFFT() / 2 + 1;
      int nmic = fmt->GetNMic();
      int nmic2 = ch.size();
      for (int c = 0; c < ch.size(); c++) {
	if (!(ch[c] >= 0 && ch[c] < nmic)) {
	cerr << "LocalizationMUSIC. Nonexistent channel is selected." << endl;
	exit(1);
	}
      }

// if ND means number of TFs
#if 1

      // Calculate the number of heights
      vector<float> allHeight(m2pg->GetND());
      for (int ih = 0; ih < m2pg->GetND(); ih++) {
	allHeight[ih] = (float)m2pg->GetSourcePositions()[ih][0].GetZ();
      }
      sort(allHeight.begin(),allHeight.end());
      int numHeight = 1;
      for (int ih = 1; ih < allHeight.size(); ih++) {
	if(allHeight[ih] - allHeight[ih-1] >= 0.001) numHeight++;
      }
      int m2pgGetNR;
      m2pgGetNR = numHeight;
      int m2pgGetND;
      m2pgGetND = floor(m2pg->GetND() / numHeight);

      A.resize(extents[m2pgGetNR][m2pgGetND][1]); // This should include radius
      for (int ih = 0; ih < m2pgGetNR; ih++) {
	for (int id = 0; id < m2pgGetND; id++) {
	  for (int ir = 0; ir < 1; ir++) {
	    A[ih][id][ir].resize(slength, nmic2);
	  }
	}
      }

      SearchHeight.resize(m2pgGetNR);
      for (int ih = 0; ih < m2pgGetNR; ih++) {
	SearchHeight[ih] = (float)atan2(m2pg->GetSourcePositions()[ih][0].GetZ(),
					sqrt(m2pg->GetSourcePositions()[ih][0].GetX() * m2pg->GetSourcePositions()[ih][0].GetX() +
					     m2pg->GetSourcePositions()[ih][0].GetY() * m2pg->GetSourcePositions()[ih][0].GetY())) * 180.0 / M_PI;
      }
      SearchDirection.resize(m2pgGetND);
      for (int id = 0; id < m2pgGetND; id++) {
	SearchDirection[id] = (float)atan2(m2pg->GetSourcePositions()[id * m2pgGetNR][0].GetY(),
						 m2pg->GetSourcePositions()[id * m2pgGetNR][0].GetX()) * 180.0 / M_PI;
      }
      SearchRange.resize(1);
      for (int ir = 0; ir < 1; ir++) {
	SearchRange[ir] = (float)sqrt(m2pg->GetSourcePositions()[0][0].GetX() * m2pg->GetSourcePositions()[0][0].GetX() +
				    m2pg->GetSourcePositions()[0][0].GetY() * m2pg->GetSourcePositions()[0][0].GetY() +
				    m2pg->GetSourcePositions()[0][0].GetZ() * m2pg->GetSourcePositions()[0][0].GetZ());
      }
      
      for (int ih = 0; ih < m2pgGetNR; ih++) {
	for (int id = 0; id < m2pgGetND; id++) {
	  for (int ir = 0; ir < 1; ir++) {
	    for (int m = 0; m < nmic2; m++) {
	      for (int k = 0; k < slength; k++) {
		A[ih][id][ir](k, m) = m2pg->GetTFs()[m2pgGetNR * id + ih][ir][ch[m]][k];
	      }
	    }
	  }
	}
      }

// if NR means radius
#else

      A.resize(extents[1][m2pg->GetND()][m2pg->GetNR()]); // This should include height
      for (int ih = 0; ih < 1; ih++) {
	for (int id = 0; id < m2pg->GetND(); id++) {
	  for (int ir = 0; ir < m2pg->GetNR(); ir++) {
	    A[ih][id][ir].resize(slength, nmic2);
	  }
	}
      }

      SearchHeight.resize(1);
      for (int ih = 0; ih < 1; ih++) {
	SearchHeight[ih] = (float)atan2(m2pg->GetSourcePositions()[0][0].GetZ(),
					sqrt(m2pg->GetSourcePositions()[0][0].GetX() * m2pg->GetSourcePositions()[0][0].GetX() +
					     m2pg->GetSourcePositions()[0][0].GetY() * m2pg->GetSourcePositions()[0][0].GetY())) * 180.0 / M_PI;
      }
      SearchDirection.resize(m2pg->GetND());
      for (int id = 0; id < m2pg->GetND(); id++) {
	SearchDirection[id] = (float)atan2(m2pg->GetSourcePositions()[id][0].GetY(),
					   m2pg->GetSourcePositions()[id][0].GetX()) * 180 / M_PI;
      }
      SearchRange.resize(m2pg->GetNR());
      for (int ir = 0; ir < m2pg->GetNR(); ir++) {
	SearchRange[ir] = (float)sqrt(m2pg->GetSourcePositions()[0][ir].GetX() * m2pg->GetSourcePositions()[0][ir].GetX() +
				    m2pg->GetSourcePositions()[0][ir].GetY() * m2pg->GetSourcePositions()[0][ir].GetY() +
				    m2pg->GetSourcePositions()[0][ir].GetZ() * m2pg->GetSourcePositions()[0][ir].GetZ());
      }
      
      for (int ih = 0; ih < 1; ih++) {
	for (int id = 0; id < m2pg->GetND(); id++) {
	  for (int ir = 0; ir < m2pg->GetNR(); ir++) {
	    for (int m = 0; m < nmic2; m++) {
	      for (int k = 0; k < slength; k++) {
		A[ih][id][ir](k, m) = m2pg->GetTFs()[id][ir][ch[m]][k];
	      }
	    }
	  }
	}
      }

#endif
      cerr << "TF was loaded by libharkio2." << endl;

    }
  catch (const harkio2::AnalyzeError& e)
    {
      cerr << "Try to read " << filename << " as header-less MUSIC transfer function format.\n";    
      ::ReadTransferFunction(filename, A, SearchHeight, SearchDirection, SearchRange, ch);
    }

#elif ENABLE_HARKIO1

  if (HARK::checkIdentifier(filename) == HARKIO_TRUE) {
    HARK hark_read;
    hark_read.readFile(filename);

    cerr << "Try to read " << filename << " as HARK format." << endl;

    Chunk::VERSION_CHECK version_check;
    FMT* fmt = hark_read.getData()->getChunk<FMT>();
    // TODO: version check
    if (fmt->getFormatId() != FMT::M2PG) {
      // TODO: エラー処理
    }

    M2PG* m2pg = hark_read.getData()->getChunk<M2PG>();
    int slength = fmt->getNFFT() / 2 + 1;
    int nmic = fmt->getNMic();
    int nmic2 = ch.size(); 
    for (int c = 0; c < ch.size(); c++) { 
      if (!(ch[c] >= 0 && ch[c] < nmic)) { 
	cerr << "LocalizationMUSIC. Nonexistent channel is selected." << endl; 
	exit(1); 
      } 
    } 

// if ND means number of TFs
#if 1

    // Calculate the number of heights
    vector<float> allHeight(m2pg->getND());
    M2PG::SRC_LIST_TYPE* srch = m2pg->getSrc();
    for (int ih = 0; ih < m2pg->getND(); ih++) {
      allHeight[ih] = (float)(*srch)[ih][0].z;
    }
    sort(allHeight.begin(),allHeight.end());
    int numHeight = 1;
    for (int ih = 1; ih < allHeight.size(); ih++) {
      if(allHeight[ih] - allHeight[ih-1] >= 0.001) numHeight++;
    }
    int m2pggetNR;
    m2pggetNR = numHeight;
    int m2pggetND;
    m2pggetND = floor(m2pg->getND() / numHeight);
      
    A.resize(extents[m2pggetNR][m2pggetND][1]); // This should include radius
    for (int ih = 0; ih < m2pggetNR; ih++) {
      for (int id = 0; id < m2pggetND; id++) {
	for (int ir = 0; ir < 1; ir++) {
	  A[ih][id][ir].resize(slength, nmic2);
	}
      }
    }
    M2PG::SRC_LIST_TYPE* src = m2pg->getSrc();

    SearchHeight.resize(m2pggetNR);
    for (int ih = 0; ih < m2pggetNR; ih++) {
      SearchHeight[ih] = (float)atan2((*src)[ih][0].z, 
				      sqrt((*src)[ih][0].x * (*src)[ih][0].x +
					   (*src)[ih][0].y * (*src)[ih][0].y)) * 180 / M_PI;
    }
    SearchDirection.resize(m2pggetND);
    for (int id = 0; id < m2pggetND; id++) {
      SearchDirection[id] = (float)atan2((*src)[m2pggetNR * id][0].y,
					 (*src)[m2pggetNR * id][0].x) * 180 / M_PI;
    }
    SearchRange.resize(1);
    for (int ir = 0; ir < 1; ir++) {
      SearchRange[ir] = (float)sqrt((*src)[0][0].x * (*src)[0][0].x +
                                  (*src)[0][0].y * (*src)[0][0].y +
                                  (*src)[0][0].z * (*src)[0][0].z);
    }

    M2PG::TF_LIST_TYPE* tf = m2pg->getTF();
    for (int ih = 0; ih < m2pggetNR; ih++) {
      for (int id = 0; id < m2pggetND; id++) {
	for (int ir = 0; ir < 1; ir++) {
	  for (int m = 0; m < nmic2; m++) {
	    for (int k = 0; k < slength; k++) {
	      A[ih][id][ir](k, m) = (*tf)[m2pggetNR * id + ih][ir][ch[m]][k];
	    }
	  }
	}
      }
    }

// if NR means radius
#else

    A.resize(extents[1][m2pg->getND()][m2pg->getNR()]); // This should include height
    for (int ih = 0; ih < 1; ih++) {
      for (int id = 0; id < m2pg->getND(); id++) {
	for (int ir = 0; ir < m2pg->getNR(); ir++) {
	  A[ih][id][ir].resize(slength, nmic2);
	}
      }
    }
    M2PG::SRC_LIST_TYPE* src = m2pg->getSrc();

    SearchHeight.resize(1);
    for (int ih = 0; ih < 1; ih++) {
      SearchHeight[ih] = (float)atan2((*src)[0][0].z,
				      sqrt((*src)[0][0].x * (*src)[0][0].x +
					   (*src)[0][0].y * (*src)[0][0].y)) * 180 / M_PI;
    }
    SearchDirection.resize(m2pg->getND());
    for (int id = 0; id < m2pg->getND(); id++) {
      SearchDirection[id] = (float)atan2((*src)[id][0].y,
					 (*src)[id][0].x) * 180 / M_PI;
    }
    SearchRange.resize(m2pg->getNR());
    for (int ir = 0; ir < m2pg->getNR(); ir++) {
      SearchRange[ir] = (float)sqrt((*src)[0][ir].x * (*src)[0][ir].x +
				    (*src)[0][ir].y * (*src)[0][ir].y +
				    (*src)[0][ir].z * (*src)[0][ir].z);
    }

    M2PG::TF_LIST_TYPE* tf = m2pg->getTF();
    for (int ih = 0; ih < 1; ih++) {
      for (int id = 0; id < m2pg->getND(); id++) {
	for (int ir = 0; ir < m2pg->getNR(); ir++) {
	  for (int m = 0; m < nmic2; m++) {
	    for (int k = 0; k < slength; k++) {
	      A[ih][id][ir](k, m) = (*tf)[id][ir][ch[m]][k];
	    }
	  }
	}
      }
    }

#endif
      cerr << "TF was loaded by libharkio1." << endl;
    
  }
  else {
    cerr << harkio::Error::getInstance().getMsg() << endl;
    cerr << "Try to read " << filename << " as header-less MUSIC transfer function format.\n";    
    ::ReadTransferFunction(filename, A, SearchHeight, SearchDirection, SearchRange, ch);
  }

#endif

#if 0

    cout << "Height [" << SearchHeight.size() << "] : ";
    for(int j = 0; j < SearchHeight.size(); j++){
      cout << SearchHeight[j] << " ";
    }
    cout << endl;

    cout << "Direction [" << SearchDirection.size() << "] : ";
    for(int j = 0; j < SearchDirection.size(); j++){
      cout << SearchDirection[j] << " ";
    }
    cout << endl;

    cout << "Range [" << SearchRange.size() << "] : ";
    for(int j = 0; j < SearchRange.size(); j++){
      cout << SearchRange[j] << " ";
    }
    cout << endl;

#endif

  Pavg_sum.resize(SearchHeight.size());
  for (int ih = 0; ih < SearchHeight.size(); ih++) {
    Pavg_sum[ih].resize(SearchDirection.size());
    for (int id = 0; id < SearchDirection.size(); id++) {
      Pavg_sum[ih][id].resize(SearchRange.size());
    }
  }

  Pavg.resize(SearchHeight.size());
  for (int ih = 0; ih < SearchHeight.size(); ih++) {
    Pavg[ih].resize(SearchDirection.size());
    for (int id = 0; id < SearchDirection.size(); id++) {
      Pavg[ih][id].resize(SearchRange.size());
    }
  }

  Pavg_bin.resize(A[0][0][0].size1());
  for (int bins = 0; bins < A[0][0][0].size1(); bins++) {
    Pavg_bin[bins].resize(SearchHeight.size());
    for (int ih = 0; ih < SearchHeight.size(); ih++) {
      Pavg_bin[bins][ih].resize(SearchDirection.size());
      for (int id = 0; id < SearchDirection.size(); id++) {
	Pavg_bin[bins][ih][id].resize(SearchRange.size());
      }
    }
  }

  // Interpolated Transfer Function

  if(enable_interpolation){

    // Make fine grids

    int azimuth_grid_local;
    float Direction_lower, Direction_upper;
    int Direction_index_lower, Direction_index_upper;
    SearchDirection_Itpl.resize(0);
    vector<float> WxxV(0);
    vector<int> AxxL(0);
    vector<int> AxxH(0);
    for(int id = 0; id < SearchDirection.size() - 1; id++){
      azimuth_grid_local = 0;
      Direction_lower = (float)SearchDirection[id];
      Direction_upper = (float)SearchDirection[id+1];
      bool swapFlag = (Direction_lower > Direction_upper) ?  true : false;
      if(swapFlag) Direction_upper += 360.0;
      Direction_index_lower = id;
      Direction_index_upper = id+1; 
      while( (Direction_upper - (Direction_lower + azimuth_grid_local * resolution[1])) > resolution[1] / 2.0 ){
	float DirectionTmp = Direction_lower + (float)azimuth_grid_local * resolution[1];
	SearchDirection_Itpl.push_back(DirectionTmp);
	WxxV.push_back((DirectionTmp - Direction_lower)/(Direction_upper - Direction_lower));
	AxxL.push_back(Direction_index_lower);
	AxxH.push_back(Direction_index_upper);
	azimuth_grid_local++;
      }
    }
    azimuth_grid_local = 0;
    Direction_lower = (float)SearchDirection[SearchDirection.size()-1];
    Direction_upper = (float)SearchDirection[0];
    bool swapFlag = (Direction_lower > Direction_upper) ?  true : false;
    if(swapFlag) Direction_upper += 360.0;
    Direction_index_lower = SearchDirection.size() - 1;
    Direction_index_upper = 0;
    while( (Direction_upper - (Direction_lower + azimuth_grid_local * resolution[1])) > resolution[1] / 2.0 ){
      float DirectionTmp = Direction_lower + (float)azimuth_grid_local * resolution[1];
      SearchDirection_Itpl.push_back(DirectionTmp);
      WxxV.push_back((DirectionTmp - Direction_lower)/(Direction_upper - Direction_lower));
      AxxL.push_back(Direction_index_lower);
      AxxH.push_back(Direction_index_upper);
      azimuth_grid_local++;
    }
    for(int id = 0; id < SearchDirection_Itpl.size() - 1; id++){
      while( SearchDirection_Itpl[id] < -180 )SearchDirection_Itpl[id] += 360;
      while( SearchDirection_Itpl[id] > 180 ) SearchDirection_Itpl[id] -= 360;
    }

    int range_grid_local;
    float Range_lower, Range_upper;
    int Range_index_lower, Range_index_upper;
    SearchRange_Itpl.resize(0);
    vector<float> WyyV(0);
    vector<int> AyyL(0);
    vector<int> AyyH(0);
    for(int ir = 0; ir < SearchRange.size() - 1; ir++){
      range_grid_local = 0;
      Range_lower = (float)SearchRange[ir];
      Range_upper = (float)SearchRange[ir+1];
      Range_index_lower = ir;
      Range_index_upper = ir+1; 
      while( (Range_upper - (Range_lower + range_grid_local * resolution[2])) > resolution[2] / 2.0 ){
	float RangeTmp = Range_lower + (float)range_grid_local * resolution[2];
	SearchRange_Itpl.push_back(RangeTmp);
	WyyV.push_back((RangeTmp - Range_lower)/(Range_upper - Range_lower));
	AyyL.push_back(Range_index_lower);
	AyyH.push_back(Range_index_upper);
	range_grid_local++;
      }
    }
    SearchRange_Itpl.push_back(SearchRange[SearchRange.size()-1]);
    WyyV.push_back(1.0);
    AyyL.push_back(SearchRange.size()-1);
    AyyH.push_back(SearchRange.size()-1);
    
    int height_grid_local;
    float Height_lower, Height_upper;
    int Height_index_lower, Height_index_upper;
    SearchHeight_Itpl.resize(0);
    vector<float> WzzV(0);
    vector<int> AzzL(0);
    vector<int> AzzH(0);
    for(int ir = 0; ir < SearchHeight.size() - 1; ir++){
      height_grid_local = 0;
      Height_lower = (float)SearchHeight[ir];
      Height_upper = (float)SearchHeight[ir+1];
      Height_index_lower = ir;
      Height_index_upper = ir+1; 
      while( (Height_upper - (Height_lower + height_grid_local * resolution[0])) > resolution[0] / 2.0 ){
	float HeightTmp = Height_lower + (float)height_grid_local * resolution[0];
	SearchHeight_Itpl.push_back(HeightTmp);
	WzzV.push_back((HeightTmp - Height_lower)/(Height_upper - Height_lower));
	AzzL.push_back(Height_index_lower);
	AzzH.push_back(Height_index_upper);
	height_grid_local++;
      }
    }
    SearchHeight_Itpl.push_back(SearchHeight[SearchHeight.size()-1]);
    WzzV.push_back(1.0);
    AzzL.push_back(SearchHeight.size()-1);
    AzzH.push_back(SearchHeight.size()-1);

    // Make fine grids

    Pavg_sum_Itpl.resize(SearchHeight_Itpl.size());
    for (int ih = 0; ih < SearchHeight_Itpl.size(); ih++) {
      Pavg_sum_Itpl[ih].resize(SearchDirection_Itpl.size());
      for (int id = 0; id < SearchDirection_Itpl.size(); id++) {
	Pavg_sum_Itpl[ih][id].resize(SearchRange_Itpl.size());
      }
    }
      
    Pavg_Itpl.resize(SearchHeight_Itpl.size());
    for (int ih = 0; ih < SearchHeight_Itpl.size(); ih++) {
      Pavg_Itpl[ih].resize(SearchDirection_Itpl.size());
      for (int id = 0; id < SearchDirection_Itpl.size(); id++) {
	Pavg_Itpl[ih][id].resize(SearchRange_Itpl.size());
      }
    }
      
    Pavg_bin_Itpl.resize(A[0][0][0].size1());
    for (int bins = 0; bins < A[0][0][0].size1(); bins++) {
      Pavg_bin_Itpl[bins].resize(SearchHeight_Itpl.size());
      for (int ih = 0; ih < SearchHeight_Itpl.size(); ih++) {
	Pavg_bin_Itpl[bins][ih].resize(SearchDirection_Itpl.size());
	for (int id = 0; id < SearchDirection_Itpl.size(); id++) {
	  Pavg_bin_Itpl[bins][ih][id].resize(SearchRange_Itpl.size());
	}
      }
    }

    // A matrix generation

    A_Itpl.resize(extents[SearchHeight_Itpl.size()][SearchDirection_Itpl.size()][SearchRange_Itpl.size()]);
    for (int ih = 0; ih < SearchHeight_Itpl.size(); ih++) {
      for (int id = 0; id < SearchDirection_Itpl.size(); id++) {
	for (int ir = 0; ir < SearchRange_Itpl.size(); ir++) {
	  A_Itpl[ih][id][ir].resize(A[0][0][0].size1(), A[0][0][0].size2());
	}
      }
    }

    // SearchFlag matrix generation

    SearchHeightFlag_Itpl.resize(SearchHeight_Itpl.size());    
    SearchDirectionFlag_Itpl.resize(SearchDirection_Itpl.size());
    SearchRangeFlag_Itpl.resize(SearchRange_Itpl.size());    
      
#if 0

    cout << "Height [" << SearchHeight_Itpl.size() << "] : ";
    for(int j = 0; j < SearchHeight_Itpl.size(); j++){
      cout << SearchHeight_Itpl[j] << " ";
    }
    cout << endl;

    cout << "Direction [" << SearchDirection_Itpl.size() << "] : ";
    for(int j = 0; j < SearchDirection_Itpl.size(); j++){
      cout << SearchDirection_Itpl[j] << " ";
    }
    cout << endl;

    cout << "Range [" << SearchRange_Itpl.size() << "] : ";
    for(int j = 0; j < SearchRange_Itpl.size(); j++){
      cout << SearchRange_Itpl[j] << " ";
    }
    cout << endl;

#endif
      
    ublas::vector<ublas::matrix<complex<float> > > AVbase;
    AVbase.resize(2);
    for(int j = 0; j < AVbase.size(); j++) AVbase[j].resize(2,2);

    vector<vector<vector<ublas::matrix<complex<float> > > > > AMbase;
    AMbase.resize(2);
    for(int j = 0; j < AMbase.size(); j++){
      AMbase[j].resize(2);
      for(int k = 0; k < AMbase[j].size(); k++){
	AMbase[j][k].resize(2);
	for(int m = 0; m < AMbase[j][k].size(); m++){
	  AMbase[j][k][m].resize(2,2);
	}
      }
    }

    vector<vector<vector<ublas::vector<float> > > > AMEig;
    AMEig.resize(2);
    for(int j = 0; j < AMEig.size(); j++){
      AMEig[j].resize(2);
      for(int k = 0; k < AMEig[j].size(); k++){
	AMEig[j][k].resize(2);
	for(int m = 0; m < AMEig[j][k].size(); m++){
	  AMEig[j][k][m].resize(A[0][0][0].size2());
	}
      }
    }

    vector<vector<vector<ublas::matrix<complex<float> > > > > AMEigV;
    AMEigV.resize(2);
    for(int j = 0; j < AMEigV.size(); j++){
      AMEigV[j].resize(2);
      for(int k = 0; k < AMEigV[j].size(); k++){
	AMEigV[j][k].resize(2);
	for(int m = 0; m < AMEigV[j][k].size(); m++){
	  AMEigV[j][k][m].resize(A[0][0][0].size2(),A[0][0][0].size2());
	}
      }
    }

    ublas::vector<float> Axx;
    Axx.resize(2);

    ublas::vector<float> Ayy;
    Ayy.resize(2);

    ublas::vector<float> Azz;
    Azz.resize(2);

#if 0
    cout << "( H : " << height_id_l << "," << height_id_c << "," << height_id_h << ") " << endl;
    cout << "( A : " << azimuth_id_l << "," << azimuth_id_c << "," << azimuth_id_h << ") " << endl;
    cout << "( R : " << range_id_l << "," << range_id_c << "," << range_id_h << ") " << endl;
#endif	

    bool enable_EVSI = ( ((interpolation_type=="MEVSI_UNSTABLE")) ? true : false);
      
    int height_id_lower, height_id_upper;      
    int azimuth_id_lower, azimuth_id_upper;
    int range_id_lower, range_id_upper;      

    for (int ih = 0; ih < SearchHeight_Itpl.size(); ih++) {
      for (int id = 0; id < SearchDirection_Itpl.size(); id++) {
	for (int ir = 0; ir < SearchRange_Itpl.size(); ir++) {

	  azimuth_id_lower = AxxL[id];
	  azimuth_id_upper = AxxH[id];
	  range_id_lower = AyyL[ir];
	  range_id_upper = AyyH[ir];
	  height_id_lower = AzzL[ih];
	  height_id_upper = AzzH[ih];
	  Axx[0] = 1.0 - WxxV[id];
	  Axx[1] = WxxV[id];
	  Ayy[0] = 1.0 - WyyV[ir];
	  Ayy[1] = WyyV[ir];	      
	  Azz[0] = 1.0 - WzzV[ih];
	  Azz[1] = WzzV[ih];	      
	    
	  for (int k = 0; k < A[0][0][0].size1(); k++) {

	    if(enable_EVSI){
		
	      noalias(AMbase[0][0][0]) += outer_prod(ublas::column(A[height_id_lower][azimuth_id_lower][range_id_lower], k), 
						     ublas::herm(ublas::column(A[height_id_lower][azimuth_id_lower][range_id_lower], k)));	      
	      noalias(AMbase[0][0][1]) += outer_prod(ublas::column(A[height_id_lower][azimuth_id_upper][range_id_lower], k), 
						     ublas::herm(ublas::column(A[height_id_lower][azimuth_id_upper][range_id_lower], k)));	      
	      noalias(AMbase[0][1][0]) += outer_prod(ublas::column(A[height_id_lower][azimuth_id_lower][range_id_upper], k), 
						     ublas::herm(ublas::column(A[height_id_lower][azimuth_id_lower][range_id_upper], k)));	      
	      noalias(AMbase[0][1][1]) += outer_prod(ublas::column(A[height_id_lower][azimuth_id_upper][range_id_upper], k), 
						     ublas::herm(ublas::column(A[height_id_lower][azimuth_id_upper][range_id_upper], k)));	      
	      noalias(AMbase[1][0][0]) += outer_prod(ublas::column(A[height_id_upper][azimuth_id_lower][range_id_lower], k), 
						     ublas::herm(ublas::column(A[height_id_upper][azimuth_id_lower][range_id_lower], k)));	      
	      noalias(AMbase[1][0][1]) += outer_prod(ublas::column(A[height_id_upper][azimuth_id_upper][range_id_lower], k), 
						     ublas::herm(ublas::column(A[height_id_upper][azimuth_id_upper][range_id_lower], k)));	      
	      noalias(AMbase[1][1][0]) += outer_prod(ublas::column(A[height_id_upper][azimuth_id_lower][range_id_upper], k), 
						     ublas::herm(ublas::column(A[height_id_upper][azimuth_id_lower][range_id_upper], k)));	      
	      noalias(AMbase[1][1][1]) += outer_prod(ublas::column(A[height_id_upper][azimuth_id_upper][range_id_upper], k), 
						     ublas::herm(ublas::column(A[height_id_upper][azimuth_id_upper][range_id_upper], k)));	  

	      evd(AMbase[0][0][0], AMEig[0][0][0], AMEigV[0][0][0]);
	      evd(AMbase[0][0][1], AMEig[0][0][1], AMEigV[0][0][1]);
	      evd(AMbase[0][1][0], AMEig[0][1][0], AMEigV[0][1][0]);
	      evd(AMbase[0][1][1], AMEig[0][1][1], AMEigV[0][1][1]);
	      evd(AMbase[1][0][0], AMEig[1][0][0], AMEigV[1][0][0]);
	      evd(AMbase[1][0][1], AMEig[1][0][1], AMEigV[1][0][1]);
	      evd(AMbase[1][1][0], AMEig[1][1][0], AMEigV[1][1][0]);
	      evd(AMbase[1][1][1], AMEig[1][1][1], AMEigV[1][1][1]);
		
	      if(interpolation_type == "MEVSI_UNSTABLE") {
		  
		for (int m = 0; m < A[0][0][0].size2(); m++) {
		    
		  AVbase[0](0,0) = A[height_id_lower][azimuth_id_lower][range_id_lower](k,m);
		  AVbase[0](0,1) = A[height_id_lower][azimuth_id_upper][range_id_lower](k,m);
		  AVbase[0](1,0) = A[height_id_lower][azimuth_id_lower][range_id_upper](k,m);
		  AVbase[0](1,1) = A[height_id_lower][azimuth_id_upper][range_id_upper](k,m);
		  AVbase[1](0,0) = A[height_id_upper][azimuth_id_lower][range_id_lower](k,m);
		  AVbase[1](0,1) = A[height_id_upper][azimuth_id_upper][range_id_lower](k,m);
		  AVbase[1](1,0) = A[height_id_upper][azimuth_id_lower][range_id_upper](k,m);
		  AVbase[1](1,1) = A[height_id_upper][azimuth_id_upper][range_id_upper](k,m);
		    
		  complex<float> A_FDLI = Azz[0] * ublas::inner_prod(Ayy,ublas::prod(AVbase[0],Axx)) + Azz[1] * ublas::inner_prod(Ayy,ublas::prod(AVbase[1],Axx));
		    
#if 0
		  float A_MEVSI
		    = pow(AMEig[0][0][0][m], (float)(1.0 / 2.0 * Axx[0] * Ayy[0] * Azz[0]) )
		    * pow(AMEig[0][0][1][m], (float)(1.0 / 2.0 * Axx[1] * Ayy[0] * Azz[0]) )
		    * pow(AMEig[0][1][0][m], (float)(1.0 / 2.0 * Axx[0] * Ayy[1] * Azz[0]) )
		    * pow(AMEig[0][1][1][m], (float)(1.0 / 2.0 * Axx[1] * Ayy[1] * Azz[0]) )
		    * pow(AMEig[1][0][0][m], (float)(1.0 / 2.0 * Axx[0] * Ayy[0] * Azz[1]) )
		    * pow(AMEig[1][0][1][m], (float)(1.0 / 2.0 * Axx[1] * Ayy[0] * Azz[1]) )
		    * pow(AMEig[1][1][0][m], (float)(1.0 / 2.0 * Axx[0] * Ayy[1] * Azz[1]) )
		    * pow(AMEig[1][1][1][m], (float)(1.0 / 2.0 * Axx[1] * Ayy[1] * Azz[1]) );
#else
		  float A_MEVSI
		    = pow(AMEig[0][0][0][0], (float)(1.0 / 2.0 * Axx[0] * Ayy[0] * Azz[0]) )
		    * pow(AMEig[0][0][1][0], (float)(1.0 / 2.0 * Axx[1] * Ayy[0] * Azz[0]) )
		    * pow(AMEig[0][1][0][0], (float)(1.0 / 2.0 * Axx[0] * Ayy[1] * Azz[0]) )
		    * pow(AMEig[0][1][1][0], (float)(1.0 / 2.0 * Axx[1] * Ayy[1] * Azz[0]) )
		    * pow(AMEig[1][0][0][0], (float)(1.0 / 2.0 * Axx[0] * Ayy[0] * Azz[1]) )
		    * pow(AMEig[1][0][1][0], (float)(1.0 / 2.0 * Axx[1] * Ayy[0] * Azz[1]) )
		    * pow(AMEig[1][1][0][0], (float)(1.0 / 2.0 * Axx[0] * Ayy[1] * Azz[1]) )
		    * pow(AMEig[1][1][1][0], (float)(1.0 / 2.0 * Axx[1] * Ayy[1] * Azz[1]) );
#endif
		  A_Itpl[ih][id][ir](k, m) = A_MEVSI * A_FDLI / abs(A_FDLI);
		    
		}
		  
	      }
		
	    }else{
		
	      for (int m = 0; m < A[0][0][0].size2(); m++) {
		  
		AVbase[0](0,0) = A[height_id_lower][azimuth_id_lower][range_id_lower](k,m);
		AVbase[0](0,1) = A[height_id_lower][azimuth_id_upper][range_id_lower](k,m);
		AVbase[0](1,0) = A[height_id_lower][azimuth_id_lower][range_id_upper](k,m);
		AVbase[0](1,1) = A[height_id_lower][azimuth_id_upper][range_id_upper](k,m);
		AVbase[1](0,0) = A[height_id_upper][azimuth_id_lower][range_id_lower](k,m);
		AVbase[1](0,1) = A[height_id_upper][azimuth_id_upper][range_id_lower](k,m);
		AVbase[1](1,0) = A[height_id_upper][azimuth_id_lower][range_id_upper](k,m);
		AVbase[1](1,1) = A[height_id_upper][azimuth_id_upper][range_id_upper](k,m);

		if(interpolation_type == "FDLI") {
		  A_Itpl[ih][id][ir](k, m) = Azz[0] * ublas::inner_prod(Ayy,ublas::prod(AVbase[0],Axx)) + Azz[1] * ublas::inner_prod(Ayy,ublas::prod(AVbase[1],Axx));
		}
		  
		if(interpolation_type == "TDLI") {
		  A_Itpl[ih][id][ir](k, m) 
		    =   pow(AVbase[0](0,0), Axx[0] * Ayy[0] * Azz[0] )
		    * pow(AVbase[0](0,1), Axx[1] * Ayy[0] * Azz[0] )
		    * pow(AVbase[0](1,0), Axx[0] * Ayy[1] * Azz[0] )
		    * pow(AVbase[0](1,1), Axx[1] * Ayy[1] * Azz[0] )
		    * pow(AVbase[1](0,0), Axx[0] * Ayy[0] * Azz[1] )
		    * pow(AVbase[1](0,1), Axx[1] * Ayy[0] * Azz[1] )
		    * pow(AVbase[1](1,0), Axx[0] * Ayy[1] * Azz[1] )
		    * pow(AVbase[1](1,1), Axx[1] * Ayy[1] * Azz[1] );
		}
		  
		if(interpolation_type == "FTDLI") {
		  complex<float> A_FDLI = Azz[0] * ublas::inner_prod(Ayy,ublas::prod(AVbase[0],Axx)) + Azz[1] * ublas::inner_prod(Ayy,ublas::prod(AVbase[1],Axx));
		  complex<float> A_TDLI
		    =   pow(AVbase[0](0,0), Axx[0] * Ayy[0] * Azz[0] )
		    * pow(AVbase[0](0,1), Axx[1] * Ayy[0] * Azz[0] )
		    * pow(AVbase[0](1,0), Axx[0] * Ayy[1] * Azz[0] )
		    * pow(AVbase[0](1,1), Axx[1] * Ayy[1] * Azz[0] )
		    * pow(AVbase[1](0,0), Axx[0] * Ayy[0] * Azz[1] )
		    * pow(AVbase[1](0,1), Axx[1] * Ayy[0] * Azz[1] )
		    * pow(AVbase[1](1,0), Axx[0] * Ayy[1] * Azz[1] )
		    * pow(AVbase[1](1,1), Axx[1] * Ayy[1] * Azz[1] );
		  A_Itpl[ih][id][ir](k, m) = abs(A_TDLI) * A_FDLI / abs(A_FDLI);
		}
	      }

	    }
	      
	  }
	}
      }
    }

#if 0

    for (int ih = height_grid_l; ih < SearchHeight_Itpl.size(); ih++) {
      for (int id = azimuth_grid_l; id < SearchDirection_Itpl.size(); id++) {
	for (int ir = range_grid_l; ir < SearchRange_Itpl.size(); ir++) {
	  for (int k = 0; k < A[0][0].size1(); k++) {
	    for (int m = 0; m < A[0][0].size2(); m++) {
	      cout << A_Itpl[ih][id][ir](k, m) << " ";
	    }
	  }
	  cout << endl;
	}
      } 
    }

#endif

    if(!enable_hierarchical){

      A.resize(extents[A_Itpl.size()][A_Itpl[0].size()][A_Itpl[0][0].size()]);
      for (int ih = 0; ih < A_Itpl.size(); ih++) {
	for (int id = 0; id < A_Itpl[0].size(); id++) {
	  for (int ir = 0; ir < A_Itpl[0][0].size(); ir++) {
	    A[ih][id][ir].resize(A_Itpl[0][0][0].size1(), A_Itpl[0][0][0].size2());
	  }
	}
      }
      for (int ih = 0; ih < A_Itpl.size(); ih++) {
	for (int id = 0; id < A_Itpl[0].size(); id++) {
	  for (int ir = 0; ir < A_Itpl[0][0].size(); ir++) {
	    for (int k = 0;  k < A_Itpl[0][0][0].size1();  k++) {
	      for (int m = 0;  m < A_Itpl[0][0][0].size2();  m++) {
		A[ih][id][ir](k,m) = A_Itpl[ih][id][ir](k,m);
	      }
	    }
	  }
	}
      }

      SearchHeight.resize(SearchHeight_Itpl.size());
      for (int ih = 0; ih < SearchHeight_Itpl.size(); ih++) {
	SearchHeight[ih] = (float)SearchHeight_Itpl[ih];
      }
      
      SearchDirection.resize(SearchDirection_Itpl.size());
      for (int id = 0; id < SearchDirection_Itpl.size(); id++) {
	SearchDirection[id] = (float)SearchDirection_Itpl[id];
      }
	
      SearchRange.resize(SearchRange_Itpl.size());
      for (int ir = 0; ir < SearchRange_Itpl.size(); ir++) {
	SearchRange[ir] = (float)SearchRange_Itpl[ir];
      }

      Pavg_sum.resize(Pavg_sum_Itpl.size());
      for (int ih = 0; ih < Pavg_sum_Itpl.size(); ih++) {
	Pavg_sum[ih].resize(Pavg_sum_Itpl[ih].size());
	for (int id = 0; id < Pavg_sum_Itpl[ih].size(); id++) {
	  Pavg_sum[ih][id].resize(Pavg_sum_Itpl[ih][id].size());
	}
      }
      
      Pavg.resize(Pavg_Itpl.size());
      for (int ih = 0; ih < Pavg_Itpl.size(); ih++) {
	Pavg[ih].resize(Pavg_Itpl[ih].size());
	for (int id = 0; id < Pavg_Itpl[ih].size(); id++) {
	  Pavg[ih][id].resize(Pavg_Itpl[ih][id].size());
	}
      }
      
      Pavg_bin.resize(Pavg_bin_Itpl.size());
      for (int bins = 0; bins < Pavg_bin_Itpl.size(); bins++) {
	Pavg_bin[bins].resize(Pavg_bin_Itpl[bins].size());
	for (int ih = 0; ih < Pavg_bin_Itpl[bins].size(); ih++) {
	  Pavg_bin[bins][ih].resize(Pavg_bin_Itpl[bins][ih].size());
	  for (int id = 0; id < Pavg_bin_Itpl[bins][ih].size(); id++) {
	    Pavg_bin[bins][ih][id].resize(Pavg_bin_Itpl[bins][ih][id].size());
	  }
	}
      }

    }
           
  } // enable interpolation

  cerr << SearchHeight.size() << " heights, "
       << SearchDirection.size() << " directions, "
       << SearchRange.size() << " ranges, "
       << A[0][0][0].size2() << " microphones, "
       << (A[0][0][0].size1() - 1) * 2 << " points" << endl;

}

void LocalizationMUSIC::CalcAveragePower(vector<fvector>& w, vector<fcmatrix>& E,
					  AMatrixArray3D& A,
					  vector<bool>& UsesFreq,
					  int num_source, 
					  vector<vector<vector<float> > >& Pavg,
					  vector<vector<vector<vector<float> > > >& Pavg_bin,
					  ublas::vector<float>& Weight,
					  bool enable_eigenvalue_weight)
{
  vector<bool> UsesX(Pavg.size());
  for(int i = 0; i < Pavg.size(); i++) UsesX[i] = true;
  vector<bool> UsesY(Pavg[0].size());
  for(int i = 0; i < Pavg[0].size(); i++) UsesY[i] = true;
  vector<bool> UsesZ(Pavg[0][0].size());
  for(int i = 0; i < Pavg[0][0].size(); i++) UsesZ[i] = true;
  CalcAveragePower(w, E, A, UsesFreq, num_source, Pavg, Pavg_bin, Weight, enable_eigenvalue_weight, UsesX, UsesY, UsesZ);
}

void LocalizationMUSIC::CalcAveragePower(vector<fvector>& w, vector<fcmatrix>& E,
					  AMatrixArray3D& A,
					  vector<bool>& UsesFreq,
					  int num_source, 
					  vector<vector<vector<float> > >& Pavg,
					  vector<vector<vector<vector<float> > > >& Pavg_bin,
					  ublas::vector<float>& Weight,
					  bool enable_eigenvalue_weight,
					  vector<bool>& UsesNh,
					  vector<bool>& UsesNd,
					  vector<bool>& UsesNr)
{
  int nh = Pavg.size();
  int nd = Pavg[0].size();
  int nr = Pavg[0][0].size();
  int num_mic = w[0].size();
    
  for (int ih = 0; ih < nh; ih++) {
    for (int id = 0; id < nd; id++) {
      for (int ir = 0; ir < nr; ir++) {
	Pavg[ih][id][ir] = 0.0f;
      }
    }
  }

  for (int k = 0; k < Pavg_bin.size(); k++) {
    for (int ih = 0; ih < Pavg_bin[0].size(); ih++) {
      for (int id = 0; id < Pavg_bin[0][0].size(); id++) {
	for (int ir = 0; ir < Pavg_bin[0][0][0].size(); ir++) {
	  Pavg_bin[k][ih][id][ir] = 0.0f;
	}
      }
    }
  }
  
  float G = 0.0f;
  float norm_a = 0.0f;
  for (int k = 0; k < UsesFreq.size(); k++) {
    if (!UsesFreq[k]) {
      continue;
    }
        
    for (int ih = 0; ih < nh; ih++) {
      if (!UsesNh[ih]) continue; 
      for (int id = 0; id < nd; id++) {
	if (!UsesNd[id]) continue; 
	for (int ir = 0; ir < nr; ir++) {
	  if (!UsesNr[ir]) continue; 
	  G = 0.0f;
	  for (int m = num_mic - num_source - 1; m >= 0; m--) {
	    G += norm(ublas::inner_prod(ublas::herm(ublas::row(A[ih][id][ir], k)),
					ublas::column(E[k], m)));
	  }
	  norm_a = ublas::inner_prod(ublas::herm(ublas::row(A[ih][id][ir], k)),
				     ublas::row(A[ih][id][ir], k)).real();
	  float EV_Weight = (enable_eigenvalue_weight ? sqrt(w[k][num_mic - 1]) : 1.0 );
	  Pavg[ih][id][ir] += (norm_a / G) * Weight[k] * EV_Weight;
	  Pavg_bin[k][ih][id][ir] = 10 * log10((norm_a / G * Weight[k]) + 1.0f);
	  cerr << "";
	}
      }
    }
  }
}

int LocalizationMUSIC::SearchLocation(vector<vector<vector<float> > >& Pavg_sum, 
				       vector<vector<vector<float> > >& Pavg,
				       int n_source, 
				       vector<int>& height, 
				       vector<int>& direction, 
				       vector<int>& range, 
				       vector<int>& SDirection, int mindeg, int maxdeg)
{
  vector<float> fSDirection(SDirection.size());
  for(int i = 0; i < SDirection.size(); i++){
    fSDirection[i] = (float)SDirection[i];
  }
  return SearchLocation(Pavg_sum, Pavg, n_source,
			height, direction, range, 
			fSDirection, mindeg, maxdeg);
}

int LocalizationMUSIC::SearchLocation(vector<vector<vector<float> > >& Pavg_sum, 
				       vector<vector<vector<float> > >& Pavg,
				       int n_source, 
				       vector<int>& height, 
				       vector<int>& direction, 
				       vector<int>& range, 
				       vector<float>& SDirection, int mindeg, int maxdeg)
{
  int n_result;
  int nh = Pavg.size();
  int nd = Pavg[0].size();
  int nr = Pavg[0][0].size();
  vector<int>   candidate_id;
  vector<float> candidate_val;

  candidate_id.reserve(nh * nd * nr);
  candidate_id.resize(0);
  candidate_val.reserve(nh * nd * nr);
  candidate_val.resize(0);
  SearchPeak(Pavg_sum, candidate_id, candidate_val, SDirection, mindeg, maxdeg);

  vector<int> index;
  for(int i = 0; i < candidate_id.size(); i++) 
    index.push_back( i );
  
  sort( index.begin(), index.end(), bind<bool>(less_for_vec(), candidate_val, _2, _1) );

  vector<int> res_id;
  for(int i = 0; i < index.size(); i++) 
    res_id.push_back( candidate_id[index[i]] );

  vector<float> res_val;
  for(int i = 0; i < index.size(); i++) 
    res_val.push_back( candidate_val[index[i]] );

  if (n_source == 0) {
    n_result = res_id.size();    
  } else if (res_id.size() < n_source) {
    n_result = res_id.size();
  } else {
    n_result = n_source;
  }

  height.resize(0);
  direction.resize(0);
  range.resize(0);  
  for (int i = 0; i < n_result; i++){
    int indextmp = res_id[i];
    int ihtmp = floor(indextmp / (nd * nr));
    int ihrem = indextmp - ihtmp * (nd * nr);
    int idtmp = floor(ihrem / nr);
    int irtmp = ihrem - idtmp * nr;
    height.push_back(ihtmp);
    direction.push_back(idtmp);
    range.push_back(irtmp);
  }

  return n_result;
}

void LocalizationMUSIC::SearchPeak(vector<vector<vector<float> > >& power, 
				    vector<int>& peak_cand_id, 
				    vector<float>& peak_cand_val, 
				    vector<float>& SDirection, int mindeg, int maxdeg)
{

  while( maxdeg - mindeg > 360 ) maxdeg -= 360; 
  while( maxdeg - mindeg <= 0 ) maxdeg += 360; 

  int nh = power.size();
  int nd = power[0].size();
  int nr = power[0][0].size();

  bool h_flg = (nh < 2 ? false : true);
  bool d_flg = (nd < 2 ? false : true);
  bool r_flg = (nr < 2 ? false : true);
  
  if ( !h_flg && !d_flg && !r_flg ) {

    peak_cand_id.resize(0);
    peak_cand_val.resize(0);
    if(nh == 1 && nd == 1 && nr == 1){
      int deg = (int)(SDirection[0] + 0.5); 
      while( deg - mindeg > 360 ) deg -= 360; 
      while( deg - mindeg <= 0 )  deg += 360; 
      if ( (deg >= mindeg) && (deg <= maxdeg) ){
	peak_cand_id.push_back(0);  
	peak_cand_val.push_back( power[0][0][0] );  
      }
    }

  } else if(psearch_alg == "LOCAL_MAXIMUM") {

    vector<vector<vector<float> > > pt;
    pt.resize(3);
    for(int ptx = 0; ptx < pt.size(); ptx++){
      pt[ptx].resize(3);
      for(int pty = 0; pty < pt[ptx].size(); pty++){
	pt[ptx][pty].resize(3);
      }
    }

    for (int ih = 0; ih < nh; ih++) {

      for (int id = 0; id < nd; id++) {

	int deg = (int)(SDirection[id] + 0.5); 
	while( deg - mindeg > 360 ) deg -= 360; 
	while( deg - mindeg <= 0 )  deg += 360; 
	
	if ( (deg < mindeg) || (deg > maxdeg) ) continue;

	for (int ir = 0; ir < nr; ir++) {

	  for (int jh = -1; jh <= 1; jh++) {
	    for (int jd = -1; jd <= 1; jd++) {
	      for (int jr = -1; jr <= 1; jr++) {
		bool exp_flg = false;
		bool h_exp_flg = (((ih+jh < 0)||(ih+jh > nh-1)) ? true : false);
		bool r_exp_flg = (((ir+jr < 0)||(ir+jr > nr-1)) ? true : false);		
		if(id+jd < 0){pt[jh+1][jd+1][jr+1]    = (((h_exp_flg)||(r_exp_flg)) ? -1.0f : power[ih+jh][nd-1][ir+jr] ); exp_flg = true;}
		if(id+jd > nd-1){pt[jh+1][jd+1][jr+1] = (((h_exp_flg)||(r_exp_flg)) ? -1.0f : power[ih+jh][0][ir+jr] );    exp_flg = true;}
		if(h_exp_flg){pt[jh+1][jd+1][jr+1] = -1.0f; exp_flg = true;}
		if(r_exp_flg){pt[jh+1][jd+1][jr+1] = -1.0f; exp_flg = true;}
		if(exp_flg) continue;
		pt[jh+1][jd+1][jr+1] = power[ih+jh][id+jd][ir+jr];
	      }
	    }
	  }
	
	  bool pf = true;
	  for (int jh = 0; jh < 3; jh++) {
	    for (int jd = 0; jd < 3; jd++) {
	      for (int jr = 0; jr < 3; jr++) {
		if (jh == 1 && jd == 1 && jr == 1) continue;
		if(pt[jh][jd][jr] >= pt[1][1][1]) pf = false;
	      }
	    }
	  }
	  if (pf) {
	    peak_cand_id.push_back(ir + id * nr + ih * nd * nr);  
	    peak_cand_val.push_back( power[ih][id][ir] );  
	  }
	  
	}
      }
    }    
    
  } else if (psearch_alg == "HILL_CLIMBING") {

    vector<float> pt;
    pt.resize(3);
    vector<int> candd(0);

    for (int id = 0; id < nd; id++) {
      
      int deg = (int)(SDirection[id] + 0.5); 
      while( deg - mindeg > 360 ) deg -= 360; 
      while( deg - mindeg <= 0 )  deg += 360; 
      
      if ( (deg < mindeg) || (deg > maxdeg) ) continue;

      pt[0] = (id == 0 ? power[0][nd-1][0] : power[0][id-1][0]);
      pt[1] = power[0][id][0];
      pt[2] = (id == nd-1 ? power[0][0][0] : power[0][id+1][0]);
      
      if (pt[1] - pt[0] > 0 && pt[2] - pt[1] < 0) {
	candd.push_back(id);
      }

    }
      
    for (int id = 0; id < candd.size(); id++) {
      float maxh = -1.0f;
      int indh = -1;
      for (int ih = 0; ih < nh; ih++) {
	if(maxh < power[ih][candd[id]][0]){
	  maxh = power[ih][candd[id]][0];
	  indh = ih;
	}
      }
      float maxr = -1.0f;
      int indr = -1;
      for (int ir = 0; ir < nr; ir++) {
	if(maxr < power[indh][candd[id]][ir]){
	  maxr = power[indh][candd[id]][ir];
	  indr = ir;
	}
      }
      peak_cand_id.push_back(indr + candd[id] * nr + indh * nd * nr);  
      peak_cand_val.push_back( power[indh][candd[id]][indr] );  
    }

  }
  
}

#endif
#endif
