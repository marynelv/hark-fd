/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#ifndef ___LOCALIZATIONMUSIC_HPP___
#define ___LOCALIZATIONMUSIC_HPP___

#include <vector>
#include <algorithm>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/multi_array.hpp>
#include <string>

typedef std::complex<float> fcomplex;
typedef boost::numeric::ublas::vector<float> fvector;
typedef boost::numeric::ublas::vector<fcomplex> fcvector;
typedef boost::numeric::ublas::matrix<float> fmatrix;
typedef boost::numeric::ublas::matrix<fcomplex> fcmatrix;
// typedef boost::numeric::ublas::matrix<std::complex<double>,
//                                       boost::numeric::ublas::column_major> dcmatrix;
typedef boost::multi_array<fcmatrix, 1> AMatrixArray1D;

void AddToCorrelation(std::vector<fcmatrix>& Rxx, const std::vector<bool>& UsesFreq,
                      fcmatrix& X);
void NormalizeCorrelation(std::vector<fcmatrix>& Rxx, const std::vector<bool>& UsesFreq,
                          int sum_count);
void ClearCorrelation(std::vector<fcmatrix>& Rxx, const std::vector<bool>& UsesFreq);
float MaxOfAbsValue(fcmatrix& A);

float MaxOfAbsValueFvector(fvector& A);
void mulfcomplex(fcomplex& a, fcomplex& b, fcomplex& c);
void addfcomplex(fcomplex& a, fcomplex& b, fcomplex& c);
void subfcomplex(fcomplex& a, fcomplex& b, fcomplex& c);
void devfcomplex(fcomplex& a, fcomplex& b, fcomplex& c);
void mulMAT(fcmatrix& a,fcmatrix& b,fcmatrix& c);
void invMAT(fcmatrix& a,fcmatrix& c);
void display_fcmat(fcmatrix& a);
void read_fcmat(char *name1,char *name2, fcmatrix& a);
int save_mat_fcmat(char *name1, char *name2, fcmatrix& a);
int save_mat_fcmat_real(char *name1, fcmatrix& a);
void norm_display_fcmat(fcmatrix& a);
void sort_eig_fvec_ascending(fvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf);
void sort_eig_fcvec_ascending(fcvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf);
void sort_eig_fvec_decending(fvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf);
void sort_eig_fcvec_decending(fcvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf);


class LocalizationMUSIC {
  int NumMicrophones;
  int TimeLength;
  int Shift;
  int FreqLength;
  int SamplingRate;
  std::vector<bool> UsesFreq;
  
  int maxnum_output_peaks;
  
  AMatrixArray1D A;
  std::vector<int> AId;

  std::vector<std::vector<float> > SearchId;
    
  std::vector<int> nbrs_ids;
  std::vector<int> nbrs_numNeighbors;
  std::vector<std::vector<int> > nbrs_neighbors;

  std::vector<fcmatrix> Rxx;
  std::vector<float> MaxRxx;
  std::vector<fcmatrix> RxxN;
  std::vector<float> MaxRxxN;
  std::vector<fcmatrix> E;
  std::vector<fcmatrix> EN;
    
  int SumCount;
  fcmatrix R;
  fcmatrix RN;
  fcmatrix V;
  fcmatrix VN;

  fcmatrix TempCalc1;  
  fcmatrix TempCalc2;  
  fcmatrix TempCalc3;  
  fcmatrix TempCalc4;  
  fcmatrix TempCalc5;  
  fcmatrix TempCalc6;  
  fcmatrix TempCalc7;  
  fcmatrix TempCalc8;  
  fcmatrix TempCalc9;  

  std::vector<fvector> EigenValue;
  fvector  REigenValue;
  fcvector CEigenValue;

  std::vector<float> Pavg;
  std::vector<float> Pavg_sum;
  std::vector<std::vector<float> > Pavg_bin;

  int NumSource;
  int FoundSourceCount;

  std::vector<int> FoundId;

public:
  LocalizationMUSIC(int num_mic, int sampling_rate, int tlength, int shift, int maxn_out_peaks);

  void SetNumSource(int num_source) { NumSource = num_source; }
  void SetUsedFrequency(int lo, int up);
  const std::vector<bool>& GetUsedFrequency() { return UsesFreq; }
  void ClearUsedFrequency() { std::fill(UsesFreq.begin(), UsesFreq.end(), false); }

  void Localize(std::vector<fcmatrix>& Rxx, std::vector<fcmatrix>& RxxN, int mindeg, int maxdeg, 
		std::string music_alg, boost::numeric::ublas::vector<float>& MSSW,
		bool enable_eigenvalue_weight);

  void CalcAveragePower(std::vector<fvector>& w, std::vector<fcmatrix>& E,
                        AMatrixArray1D& A,
                        std::vector<bool>& UsesFreq,
                        int num_source, 
			std::vector<float>& Pavg, 
			std::vector<std::vector<float> >& Pavg_bin, 
			boost::numeric::ublas::vector<float>& Weight,
			bool enable_eigenvalue_weight);

  int SearchLocation(std::vector<float>& Pavg_sum, std::vector<float>& Pavg,
		     int n_source, std::vector<int>& direction, 
		     int mindeg, int maxdeg);

  void SearchPeak(std::vector<float>& power, 
		  std::vector<int>& peak_cand_id, 
		  std::vector<float>& peak_cand_val, 
		  int mindeg, int maxdeg);

  void ReadTransferFunction(char* filename, 
			    std::vector<int>& ch);
  

  int GetFoundSourceCount() { return FoundSourceCount; }

  const std::vector<int>& GetFoundId() { return FoundId; }

  const std::vector<std::vector<float> >& GetSearchId() { return SearchId; }

  float GetP(int iid) { return Pavg_sum[iid]; }
  float GetP_bin(int bin, int iid) { return Pavg_bin[bin][iid]; }

  float GetEigenValue(int bin, int nm) { return EigenValue[bin][nm] * MaxRxx[bin] / MaxRxxN[bin]; }
};

#endif //___LOCALIZATIONMUSIC_HPP___
