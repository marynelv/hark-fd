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
typedef boost::multi_array<fcmatrix, 2> AMatrixArray;
typedef boost::multi_array<fcmatrix, 3> AMatrixArray3D;

void ReadTransferFunction(const char* filename,
                          AMatrixArray3D& A,
                          std::vector<float>& search_height,
                          std::vector<float>& search_dir,
                          std::vector<float>& search_range,
			  std::vector<int>& ch);
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
  
  std::string psearch_alg;
  int maxnum_output_peaks;
  
  AMatrixArray3D A;
  AMatrixArray3D A_Itpl;

  std::vector<float> SearchHeight;
  std::vector<float> SearchDirection;
  std::vector<float> SearchRange;
  
  std::vector<float> SearchHeight_Itpl;
  std::vector<float> SearchDirection_Itpl;
  std::vector<float> SearchRange_Itpl;

  std::vector<bool> SearchHeightFlag_Itpl;
  std::vector<bool> SearchDirectionFlag_Itpl;
  std::vector<bool> SearchRangeFlag_Itpl;
  
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

  std::vector<std::vector<std::vector<float> > > Pavg;
  std::vector<std::vector<std::vector<float> > > Pavg_sum;
  std::vector<std::vector<std::vector<std::vector<float> > > > Pavg_bin;

  std::vector<std::vector<std::vector<float> > > Pavg_Itpl;
  std::vector<std::vector<std::vector<float> > > Pavg_sum_Itpl;
  std::vector<std::vector<std::vector<std::vector<float> > > > Pavg_bin_Itpl;

  int NumSource;
  int FoundSourceCount;

  std::vector<int> FoundHeight;
  std::vector<int> FoundDirection;
  std::vector<int> FoundRange;

  std::vector<int> FoundHeight_Itpl;
  std::vector<int> FoundDirection_Itpl;
  std::vector<int> FoundRange_Itpl;
  std::vector<float> FoundPower_Itpl;

public:
  LocalizationMUSIC(int num_mic, int sampling_rate, int tlength, int shift, std::string peak_search_alg, int maxn_out_peaks);

  void SetNumSource(int num_source) { NumSource = num_source; }
  void SetUsedFrequency(int lo, int up);
  const std::vector<bool>& GetUsedFrequency() { return UsesFreq; }
  void ClearUsedFrequency() { std::fill(UsesFreq.begin(), UsesFreq.end(), false); }

  void Localize(std::vector<fcmatrix>& Rxx, std::vector<fcmatrix>& RxxN, int mindeg, int maxdeg, 
		std::string music_alg, boost::numeric::ublas::vector<float>& MSSW,
		bool enable_eigenvalue_weight, bool enable_hierarchical);

  void CalcAveragePower(std::vector<fvector>& w, std::vector<fcmatrix>& E,
                        AMatrixArray3D& A,
                        std::vector<bool>& UsesFreq,
                        int num_source, 
			std::vector<std::vector<std::vector<float> > >& Pavg, 
			std::vector<std::vector<std::vector<std::vector<float> > > >& Pavg_bin, 
			boost::numeric::ublas::vector<float>& Weight,
			bool enable_eigenvalue_weight);

  void CalcAveragePower(std::vector<fvector>& w, std::vector<fcmatrix>& E,
                        AMatrixArray3D& A,
                        std::vector<bool>& UsesFreq,
                        int num_source, 
			std::vector<std::vector<std::vector<float> > >& Pavg, 
			std::vector<std::vector<std::vector<std::vector<float> > > >& Pavg_bin, 
			boost::numeric::ublas::vector<float>& Weight,
			bool enable_eigenvalue_weight,
                        std::vector<bool>& UsesNh,
                        std::vector<bool>& UsesNd,
                        std::vector<bool>& UsesNr);

  int SearchLocation(std::vector<std::vector<std::vector<float> > >& Pavg_sum, std::vector<std::vector<std::vector<float> > >& Pavg,
		     int n_source, std::vector<int>& height, std::vector<int>& direction, std::vector<int>& range, 
		     std::vector<int>& SDirection, int mindeg, int maxdeg);

  int SearchLocation(std::vector<std::vector<std::vector<float> > >& Pavg_sum, std::vector<std::vector<std::vector<float> > >& Pavg,
		     int n_source, std::vector<int>& height, std::vector<int>& direction, std::vector<int>& range, 
		     std::vector<float>& SDirection, int mindeg, int maxdeg);

  void SearchPeak(std::vector<std::vector<std::vector<float> > >& power, 
		  std::vector<int>& peak_cand_id, 
		  std::vector<float>& peak_cand_val, 
		  std::vector<float>& SDirection, int mindeg, int maxdeg);

  void ReadTransferFunction(const char* filename, 
			    std::vector<int>& ch, 
			    bool enable_interpolation, 
			    bool enable_hierarchical, 
			    std::string interpolation_type, 
			    std::vector<float>& resolution);

  int GetFoundSourceCount() { return FoundSourceCount; }

  const std::vector<int>& GetFoundHeight() { return FoundHeight; }
  const std::vector<int>& GetFoundDirection() { return FoundDirection; }
  const std::vector<int>& GetFoundRange() { return FoundRange; }

  const std::vector<int>& GetFoundHeight_Itpl() { return FoundHeight_Itpl; }
  const std::vector<int>& GetFoundDirection_Itpl() { return FoundDirection_Itpl; }
  const std::vector<int>& GetFoundRange_Itpl() { return FoundRange_Itpl; }
  const std::vector<float>& GetFoundPower_Itpl() { return FoundPower_Itpl; }

  const std::vector<float>& GetSearchHeight() { return SearchHeight; }
  const std::vector<float>& GetSearchDirection() { return SearchDirection; }
  const std::vector<float>& GetSearchRange() { return SearchRange; }

  const std::vector<float>& GetSearchHeight_Itpl() { return SearchHeight_Itpl; }
  const std::vector<float>& GetSearchDirection_Itpl() { return SearchDirection_Itpl; }
  const std::vector<float>& GetSearchRange_Itpl() { return SearchRange_Itpl; }

  float GetP(int ih,int id,int ir) { return Pavg_sum[ih][id][ir]; }
  float GetP_bin(int bin, int ih, int id, int ir) { return Pavg_bin[bin][ih][id][ir]; }

  float GetP_Itpl(int ih,int id,int ir) { return Pavg_sum_Itpl[ih][id][ir]; }
  float GetP_bin_Itpl(int bin, int ih, int id, int ir) { return Pavg_bin_Itpl[bin][ih][id][ir]; }

  float GetEigenValue(int bin, int nm) { return EigenValue[bin][nm] * MaxRxx[bin] / MaxRxxN[bin]; }
};

#endif //___LOCALIZATIONMUSIC_HPP___
