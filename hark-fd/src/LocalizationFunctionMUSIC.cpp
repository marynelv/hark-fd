/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#include "LocalizationFunctionMUSIC.hpp"

#include <iostream>
#include <fstream>

#include <boost/bind.hpp>

using namespace std;
using namespace boost;
using namespace boost::numeric;

struct less_for_vec {
  bool operator()(vector<float>& v, int i, int j) { return v[i] < v[j]; }
};

void ReadTransferFunction(const char* filename,
                          AMatrixArray& A,
                          vector<int>& search_dir, vector<int>& search_range)
{
  fstream fin;
  fin.open(filename, ios::in);
  if(fin.fail())
    {
      cout << string("Can't open file'")+filename+"'." << endl;
      exit(1);
    }

  int nd;
  int nr;
  int nmic;
  int nfft;

  fin.read((char*)&nd, sizeof(int));
  fin.read((char*)&nr, sizeof(int));
  fin.read((char*)&nmic, sizeof(int));
  fin.read((char*)&nfft, sizeof(int));

  int slength = nfft / 2 + 1;

  cerr << nd << " directions, " << nr << " ranges, " << nmic << " microphones, ";
  cerr << nfft << " points" << endl;
  A.resize(extents[nd][nr]);
  for (int id = 0; id < nd; id++) {
    for (int ir = 0; ir < nr; ir++) {
      A[id][ir].resize(slength, nmic);
    }
  }

  search_dir.resize(nd);
  search_range.resize(nr);
  fin.read((char*)&search_dir[0], nd * sizeof(int));
  fin.read((char*)&search_range[0], nr * sizeof(int));

  vector<complex<float> > buf(slength);
  for (int id = 0; id < nd; id++) {
    for (int ir = 0; ir < nr; ir++) {
      for (int m = 0; m < nmic; m++) {
        fin.read((char*)&buf[0], slength * sizeof(complex<float>));

        for (int k = 0; k < slength; k++) {
          A[id][ir](k, m) = buf[k];
        }
      }
    }
  }
}

void AddToCorrelation(vector<fcmatrix>& Rxx, const vector<bool>& UsesFreq, fcmatrix& X)
{
  for (int k = 0; k < UsesFreq.size(); k++) {
    if (UsesFreq[k]) {
      Rxx[k] += outer_prod(ublas::column(X, k), ublas::herm(ublas::column(X, k)));
    }
  }
}

void NormalizeCorrelation(vector<fcmatrix>& Rxx, const vector<bool>& UsesFreq,
                          int sum_count)
{
  float sum_count_1 = 1.0f / sum_count;
  for (int k = 0; k < UsesFreq.size(); k++) {
    if (UsesFreq[k]) {
      Rxx[k] *= sum_count_1;
    }
  }
}

void ClearCorrelation(vector<fcmatrix>& Rxx, const vector<bool>& UsesFreq)
{
  for (int k = 0; k < UsesFreq.size(); k++) {
    /*
    for (int i = 0; i < Rxx[k].size1(); i++) {
      for (int j = 0; j < Rxx[k].size2(); j++) {
        Rxx[k](i, j) = 0.0f;
      }
    }
    */
    Rxx[k] = ublas::zero_matrix<complex<float> >(Rxx[k].size1(), Rxx[k].size2());
  }
}

float MaxOfAbsValue(fcmatrix& A)
{
  float max = 0.0f;
  for (int i = 0; i < A.size1(); i++) {
    for (int j = 0; j < A.size2(); j++) {
      if (fabs(A(i, j).real()) > max) {
        max = fabs(A(i, j).real());
      }
    }
  }
  return max;
}

void CalcAveragePower(vector<fvector>& w, vector<fcmatrix>& E,
                      AMatrixArray& A,
                      vector<bool>& UsesFreq,
                      int num_source, vector<vector<float> >& Pavg)
{
  int nd = Pavg.size();
  int nr = Pavg[0].size();
  int num_mic = w[0].size();
    
  for (int id = 0; id < nd; id++) {
    for (int ir = 0; ir < nr; ir++) {
      Pavg[id][ir] = 0.0f;
    }
  }

  float G = 0.0f;
  float norm_a = 0.0f;
  static fcmatrix I(ublas::identity_matrix<complex<float> >(num_mic, num_mic));
  for (int k = 0; k < UsesFreq.size(); k++) {
    if (!UsesFreq[k]) {
      continue;
    }
        
    ublas::matrix_range<ublas::matrix<complex<float> > >
      E_ss(E[k], ublas::range(0, num_mic), ublas::range(0, num_source));
    fcmatrix E_nn = I - prod(E_ss, herm(E_ss));
    for (int id = 0; id < nd; id++) {
      for (int ir = 0; ir < nr; ir++) {
        G = 0.0f;

	// OLD SLOW ALGORITHM
        for (int m = num_mic - num_source - 1; m >= 0; m--) {
          G += norm(ublas::inner_prod(ublas::herm(ublas::row(A[id][ir], k)),
                                      ublas::column(E[k], m)));
        }
        norm_a = ublas::inner_prod(ublas::herm(ublas::row(A[id][ir], k)),
                                   ublas::row(A[id][ir], k)).real();
        Pavg[id][ir] += (norm_a / G) * sqrt(w[k][num_mic - 1]);


	/*
	// NEW FAST ALGORITHM but Different result ...
        G = (inner_prod(herm(ublas::row(A[id][ir], k)),
                        prod(E_nn, ublas::row(A[id][ir], k)))).real();
                            
        norm_a = ublas::inner_prod(ublas::herm(ublas::row(A[id][ir], k)),
                                   ublas::row(A[id][ir], k)).real();

        Pavg[id][ir] += (norm_a / G) * sqrt(w[k][0]);
	*/

      }
    }
  }
}

void SearchPeak(vector<float>& power, vector<int>& peak_cand)
{
  float p1;
  float p2;
  float p3;

  if (power.size() < 2) {
    peak_cand.push_back(0);
  }
  else {
    for (int i = 0; i < power.size(); i++) {
      p1 = (i == 0 ? power.back() : power[i - 1]);
      p2 = power[i];
      p3 = (i == power.size() - 1 ? power.front() : power[i + 1]);

      if (p2 - p1 > 0 && p3 - p2 < 0) {
        peak_cand.push_back(i);
      }
    }
  }
}


int SearchLocation(vector<float>& Pavg_sum, vector<vector<float> >& Pavg,
                   int n_source, vector<int>& direction, vector<int>& range)
{
  int n_candidate;
  int n_result;
  int nd = Pavg.size();
  int nr = Pavg[0].size();
  vector<int> candidate;
  vector<int> index;
  vector<float> tmp(nd + nr);
  float tmp_min;
  float tmp_max;

  candidate.reserve(nd + nr);

  candidate.resize(0);
  SearchPeak(Pavg_sum, candidate);

  if (candidate.size() < n_source) {
    //n_result = n_candidate;
    n_result = candidate.size(); //modified by shohei 08/02/13
  }
  else {
    n_result = n_source;
  }

  partial_sort(candidate.begin(),
               candidate.begin() + n_result,
               candidate.end(),
               bind<bool>(less_for_vec(), Pavg_sum, _2, _1));
  copy(candidate.begin(), candidate.begin() + n_result, direction.begin());

  candidate.resize(0);
  for (int source = 0; source < n_result; source++) {
    int id = direction[source];
    SearchPeak(Pavg[id], candidate);
    vector<int>::iterator it =
      max_element(candidate.begin(), candidate.end(),
                  bind<bool>(less_for_vec(), Pavg_sum, _1, _2));
    range[source] = *it;
  }

  return n_result;
}
