/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#ifndef ___LOCALIZATIONFUNCTION_HPP___
#define ___LOCALIZATIONFUNCTION_HPP___

#include <vector>
#include <algorithm>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/multi_array.hpp>


typedef std::complex<float> fcomplex;
typedef boost::numeric::ublas::vector<float> fvector;
typedef boost::numeric::ublas::vector<fcomplex> fcvector;
typedef boost::numeric::ublas::matrix<float> fmatrix;
typedef boost::numeric::ublas::matrix<fcomplex> fcmatrix;
typedef boost::multi_array<fcmatrix, 2> AMatrixArray;
typedef boost::multi_array<fcmatrix, 3> AMatrixArray3D;
//typedef boost::numeric::ublas::matrix<std::complex<double>,
//                                      boost::numeric::ublas::column_major> dcmatrix;


float MaxOfAbsValueFvector(fvector& A);
void mulfcomplex(fcomplex& a, fcomplex& b, fcomplex& c);
void addfcomplex(fcomplex& a, fcomplex& b, fcomplex& c);
void subfcomplex(fcomplex& a, fcomplex& b, fcomplex& c);
void devfcomplex(fcomplex& a, fcomplex& b, fcomplex& c);
void mulMAT(fcmatrix& a,fcmatrix& b,fcmatrix& c);
void invMAT(fcmatrix& a,fcmatrix& c);
void mulMATdot(fcmatrix& a,fcmatrix& b,fcmatrix& c);
void devMATdot(fcmatrix& a,fcmatrix& b,fcmatrix& c);

void zeroMAT(fcmatrix& a,fcmatrix& c);
void identityMAT(fcmatrix& a,fcmatrix& c);

void display_fcmat(fcmatrix& a);
void read_fcmat(char *name1,char *name2, fcmatrix& a);
int save_mat_fcmat(char *name1, char *name2, fcmatrix& a);
int save_mat_fcmat_real(char *name1, fcmatrix& a);
void save_mat_fmat_real(char *name1, fmatrix& a);

void AddToCorrelationAllFreq(std::vector<fcmatrix>& Rxx, int FreqLength, fcmatrix& X);
void NormalizeCorrelationAllFreq(std::vector<fcmatrix>& Rxx, int FreqLength, int sum_count);
void ClearCorrelationAllFreq(std::vector<fcmatrix>& Rxx, int FreqLength);
void LoadCorrelation(char *tmp_name1, char *tmp_name2, std::vector<fcmatrix>& Rxx);
void SaveCorrelation(char *tmp_name1, char *tmp_name2, std::vector<fcmatrix>& Rxx);

float powfloat(float a, float b);
void makeLambda(fvector& a,fcmatrix& b,float decay);

void norm_display_fcmat(fcmatrix& a);

void sort_eig_fvec_ascending(fvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf);
void sort_eig_fcvec_ascending(fcvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf);
void sort_eig_fvec_decending(fvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf);
void sort_eig_fcvec_decending(fcvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf);

void ReadTransferFunction(const char* filename,
                          AMatrixArray& A,
                          std::vector<int>& search_dir,
                          std::vector<int>& search_range);

void ReadTransferFunction(const char* filename,
                          AMatrixArray& A,
                          std::vector<int>& search_dir,
                          std::vector<int>& search_range,
			  std::vector<int>& ch);

void ReadTransferFunction(const char* filename,
                          AMatrixArray3D& A,
                          std::vector<float>& search_height,
                          std::vector<float>& search_dir,
                          std::vector<float>& search_range);

void ReadTransferFunction(const char* filename,
                          AMatrixArray3D& A,
                          std::vector<float>& search_height,
                          std::vector<float>& search_dir,
                          std::vector<float>& search_range,
			  std::vector<int>& ch);

#endif //___LOCALIZATIONFUNCTION_HPP___
