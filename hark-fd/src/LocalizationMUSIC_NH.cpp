/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#define ENABLE_TIMER 0

#include "../config.h"

#ifdef ENABLE_HARKIO_NH

#include "LocalizationMUSIC_NH.hpp"

#include <iostream>
#include <fstream>
#include <math.h>

#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "evd.hpp"
#include "svd.hpp"
// #include "svdold.hpp" 

#ifdef ENABLE_HARKIO3
extern "C" {
#include <libharkio3.h>
}
#endif

using namespace std;
using namespace boost;
using namespace boost::numeric;
using namespace boost::posix_time; 

struct less_for_vec {
  bool operator()(vector<float>& v, int i, int j) { return v[i] < v[j]; }
};

LocalizationMUSIC::LocalizationMUSIC(int num_mic, int sampling_rate,
                                     int tlength, int shift, int maxn_out_peaks) :
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

  FoundId.resize(NumSource);

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
				  bool enable_eigenvalue_weight)
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

  for (int ih = 0; ih < SearchId.size(); ih++) {
    Pavg_sum[ih] = 10 * log10(Pavg[ih] + 1.0f);
  }
  
  if(maxnum_output_peaks < 0) maxnum_output_peaks = NumSource;
  FoundSourceCount = SearchLocation(Pavg_sum, Pavg, maxnum_output_peaks,
                                    FoundId, mindeg, maxdeg);

#if ENABLE_TIMER
  end_time = microsec_clock::local_time();
  cout << end_time - start_time << endl;
#endif

}

void LocalizationMUSIC::ReadTransferFunction(char* filename,
					     vector<int>& ch)
{

#ifdef ENABLE_HARKIO3

  harkio_TransferFunction *tf = NULL;
  tf = harkio_TransferFunction_fromFile(filename, "transfer function");
  // tf = harkio_TransferFunction_new();
  // libharkio3_TransferFunction_readlibhakio2(&tf, filename, NULL);
  // ck_assert(tf != NULL);  

  int slength = tf->cfg->nfft / 2 + 1;
  if ( tf->loctfs[0]->cols != slength) {
    cerr << "LocalizationMUSIC. The number of bins does not match to the size of tf file." << endl;
    exit(1);
  }

  int nsrc = tf->poses->size;
  // if ( tf->loctfs.size() != nsrc) {
  //   cerr << "LocalizationMUSIC. The number of loctfs does not match to the number of srcs." << endl;
  //   exit(1);
  // }
  if ( tf->nbrs->size != nsrc) {
    cerr << "LocalizationMUSIC. The number of loctfs does not match to the size of nbrs." << endl;
    exit(1);
  }

  int nmic = tf->mics->size;
  if ( tf->loctfs[0]->rows != nmic) {
    cerr << "LocalizationMUSIC. The number of mics does not match to the size of tf file." << endl;
    exit(1);
  }

  int nmic2 = ch.size();
  for (int c = 0; c < ch.size(); c++) {
    if (!(ch[c] >= 0 && ch[c] < nmic)) {
      cerr << "LocalizationMUSIC. Nonexistent channel is selected." << endl;
      exit(1);
    }
  }
  
  A.resize(extents[nsrc]);
  for (int iid = 0; iid < nsrc; iid++) {
    A[iid].resize(slength, nmic2);
  }
  COMPLEX_TYPE complexValue;
  for (int iid = 0; iid < nsrc; iid++) {
    for (int m = 0; m < nmic2; m++) {
      for (int k = 0; k < slength; k++) {
	harkio_Matrix_getValueComplex(tf->loctfs[iid], ch[m], k, &complexValue);
	A[iid](k, m) = complexValue;
      }
    }
  }

  AId.resize(nsrc);
  for (int iid = 0; iid < nsrc; iid++) {
    AId[iid] = tf->poses->pos[iid]->id;
  }  
  
  SearchId.resize(nsrc);
  for (int iid = 0; iid < nsrc; iid++) {
    SearchId[iid].resize(3);
    if(tf->poses->pos[iid]->sys == Cartesian){
      SearchId[iid][0] = tf->poses->pos[iid]->coord[0];
      SearchId[iid][1] = tf->poses->pos[iid]->coord[1];
      SearchId[iid][2] = tf->poses->pos[iid]->coord[2];
    }
    if(tf->poses->pos[iid]->sys == Polar){
      float azimuth_radian   = harkio_Position_convToRadian( tf->poses->pos[iid]->coord[0] );
      float elevation_radian = harkio_Position_convToRadian( tf->poses->pos[iid]->coord[1] );
      float coef = tf->poses->pos[iid]->coord[2] * cosf(elevation_radian);
      SearchId[iid][0] = coef * cosf(azimuth_radian);
      SearchId[iid][1] = coef * sinf(azimuth_radian);
      SearchId[iid][2] = tf->poses->pos[iid]->coord[2] * sinf(elevation_radian);
    }    
  }
  
  nbrs_ids.resize(tf->nbrs->size);
  for (int i = 0; i < tf->nbrs->size; i++) {
    nbrs_ids[i] = tf->nbrs->ids[i];
  }  
  nbrs_numNeighbors.resize(tf->nbrs->size);
  for (int i = 0; i < tf->nbrs->size; i++) {
    nbrs_numNeighbors[i] = tf->nbrs->numNeighbors[i];
  }
  nbrs_neighbors.resize(tf->nbrs->size);
  for (int i = 0; i < tf->nbrs->size; i++) {
    nbrs_neighbors[i].resize(nbrs_numNeighbors[i]);
    for (int j = 0; j < nbrs_numNeighbors[i]; j++) {
      nbrs_neighbors[i][j] = tf->nbrs->neighbors[i][j];
    }
  }
  
  harkio_TransferFunction_delete(&tf);

#endif  

  Pavg_sum.resize(SearchId.size());
  Pavg.resize(SearchId.size());
  Pavg_bin.resize(A[0].size1());
  for (int bins = 0; bins < A[0].size1(); bins++) {
    Pavg_bin[bins].resize(SearchId.size());
  }

  cerr << SearchId.size() << " srcs, "
       << A[0].size2() << " microphones, "
       << (A[0].size1() - 1) * 2 << " points" << endl;

}

void LocalizationMUSIC::CalcAveragePower(vector<fvector>& w, vector<fcmatrix>& E,
					  AMatrixArray1D& A,
					  vector<bool>& UsesFreq,
					  int num_source, 
					  vector<float>& Pavg,
					  vector<vector<float> >& Pavg_bin,
					  ublas::vector<float>& Weight,
					  bool enable_eigenvalue_weight)
{
  int nh = Pavg.size();
  int num_mic = w[0].size();
    
  for (int ih = 0; ih < nh; ih++) {
	Pavg[ih] = 0.0f;
  }

  for (int k = 0; k < Pavg_bin.size(); k++) {
    for (int ih = 0; ih < Pavg_bin[0].size(); ih++) {
      Pavg_bin[k][ih] = 0.0f;
    }
  }
  
  float G = 0.0f;
  float norm_a = 0.0f;
  for (int k = 0; k < UsesFreq.size(); k++) {
    if (!UsesFreq[k]) {
      continue;
    }
    for (int ih = 0; ih < nh; ih++) {
      G = 0.0f;
      for (int m = num_mic - num_source - 1; m >= 0; m--) {
	G += norm(ublas::inner_prod(ublas::herm(ublas::row(A[ih], k)),
				    ublas::column(E[k], m)));
      }
      norm_a = ublas::inner_prod(ublas::herm(ublas::row(A[ih], k)),
				 ublas::row(A[ih], k)).real();
      float EV_Weight = (enable_eigenvalue_weight ? sqrt(w[k][num_mic - 1]) : 1.0 );
      Pavg[ih] += (norm_a / G) * Weight[k] * EV_Weight;
      Pavg_bin[k][ih] = 10 * log10((norm_a / G * Weight[k]) + 1.0f);
      cerr << "";
    }
  }
}

int LocalizationMUSIC::SearchLocation(vector<float>& Pavg_sum, 
				      vector<float>& Pavg,
				      int n_source, 
				      vector<int>& direction, 
				      int mindeg, int maxdeg)
{
  int n_result;
  int nh = Pavg.size();
  vector<int>   candidate_id;
  vector<float> candidate_val;

  candidate_id.reserve(nh);
  candidate_id.resize(0);
  candidate_val.reserve(nh);
  candidate_val.resize(0);
  SearchPeak(Pavg_sum, candidate_id, candidate_val, mindeg, maxdeg);

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

  direction.resize(0);
  for (int i = 0; i < n_result; i++){
    direction.push_back(res_id[i]);
  }

  return n_result;
}

void LocalizationMUSIC::SearchPeak(vector<float>& power, 
				   vector<int>& peak_cand_id, 
				   vector<float>& peak_cand_val, 
				   int mindeg, int maxdeg)
{

  while( maxdeg - mindeg > 360 ) maxdeg -= 360; 
  while( maxdeg - mindeg <= 0 ) maxdeg += 360; 

  int nh = power.size();

  for (int ih = 0; ih < nh; ih++) {

    int deg = (int)(180.0 / M_PI * atan2(SearchId[ih][1], SearchId[ih][0]));
    while( deg - mindeg > 360 ) deg -= 360; 
    while( deg - mindeg <= 0 )  deg += 360;     
    if ( (deg < mindeg) || (deg > maxdeg) ) continue;
    float maxc = power[ih];
    bool pf = true;

    int i;
    for(i=0; i < nbrs_ids.size(); i++){
      if(nbrs_ids[i] == AId[ih]) break;
    }
    for (int j = 0; j < nbrs_numNeighbors[i]; j++) {
      int jh;
      for(jh=0; jh < AId.size(); jh++){
	if(nbrs_neighbors[i][j] == AId[jh]) break;
      }
      if(maxc < power[jh]) pf = false;
    }
    if (pf) {
      peak_cand_id.push_back(ih);  
      peak_cand_val.push_back( power[ih] );  
    }

  }
  
}

#endif
