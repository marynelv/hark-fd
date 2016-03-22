/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#ifndef ___LOCALIZATIONFUNCTIONMUSIC_HPP___
#define ___LOCALIZATIONFUNCTIONMUSIC_HPP___

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
//typedef boost::numeric::ublas::matrix<std::complex<double>,
//                                      boost::numeric::ublas::column_major> dcmatrix;
typedef boost::multi_array<fcmatrix, 2> AMatrixArray;

void ReadTransferFunction(const char* filename,
                          AMatrixArray& A,
                          std::vector<int>& search_dir,
                          std::vector<int>& search_range);
void AddToCorrelation(std::vector<fcmatrix>& Rxx, const std::vector<bool>& UsesFreq,
                      fcmatrix& X);
void NormalizeCorrelation(std::vector<fcmatrix>& Rxx, const std::vector<bool>& UsesFreq,
                          int sum_count);
void ClearCorrelation(std::vector<fcmatrix>& Rxx, const std::vector<bool>& UsesFreq);
float MaxOfAbsValue(fcmatrix& A);

#endif //___LOCALIZATIONFUNCTIONMUSIC_HPP___
