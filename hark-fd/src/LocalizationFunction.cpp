/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#include "LocalizationFunction.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <boost/bind.hpp>

#include <cstdlib>
#include <string.h>

#include <boost/tuple/tuple.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::numeric;

/********************************************************************************/

// 3D
void ReadTransferFunction(const char* filename,
                          AMatrixArray3D& A,
                          vector<float>& search_height_f, 
			  vector<float>& search_dir_f, 
			  vector<float>& search_range_f)
{
  vector<int> search_height;
  vector<int> search_dir;
  vector<int> search_range;  

  fstream fin;
  fin.open(filename, ios::in);
  if(fin.fail())
    {
      cout << string("Can't open file'")+filename+"'." << endl;
      exit(1);
    }

  int nh;
  int nd;
  int nr;
  int nmic;
  int nfft;

  fin.read((char*)&nh, sizeof(int));
  fin.read((char*)&nd, sizeof(int));
  fin.read((char*)&nr, sizeof(int));
  fin.read((char*)&nmic, sizeof(int));
  fin.read((char*)&nfft, sizeof(int));

  int slength = nfft / 2 + 1;

  A.resize(extents[nh][nd][nr]);
  for (int ih = 0; ih < nh; ih++) {
    for (int id = 0; id < nd; id++) {
      for (int ir = 0; ir < nr; ir++) {
	A[ih][id][ir].resize(slength, nmic);
      }
    }
  }
  
  search_height.resize(nh);
  search_dir.resize(nd);
  search_range.resize(nr);
  fin.read((char*)&search_height[0], nh * sizeof(int));
  fin.read((char*)&search_dir[0], nd * sizeof(int));
  fin.read((char*)&search_range[0], nr * sizeof(int));

  search_height_f.resize(nh);
  search_dir_f.resize(nd);
  search_range_f.resize(nr);
  for (int ih = 0; ih < nh; ih++) {
    search_height_f[ih] = (float)search_height[ih];
  }
  for (int id = 0; id < nd; id++) {
    search_dir_f[id] = (float)search_dir[id];
  }
  for (int ir = 0; ir < nr; ir++) {
    search_range_f[ir] = (float)search_range[ir];
  }
  
  vector<complex<float> > buf(slength);
  for (int ih = 0; ih < nh; ih++) {
    for (int id = 0; id < nd; id++) {
      for (int ir = 0; ir < nr; ir++) {
	for (int m = 0; m < nmic; m++) {
	  fin.read((char*)&buf[0], slength * sizeof(complex<float>));
	  
	  for (int k = 0; k < slength; k++) {
	    A[ih][id][ir](k, m) = buf[k];
	  }
	}
      }
    }
  }
}


// 3D with channel selection
void ReadTransferFunction(const char* filename,
                          AMatrixArray3D& A,
                          vector<float>& search_height_f, 
			  vector<float>& search_dir_f, 
			  vector<float>& search_range_f,
			  vector<int>& ch)
{
  vector<int> search_height;
  vector<int> search_dir;
  vector<int> search_range;

  fstream fin;
  fin.open(filename, ios::in);
  if(fin.fail())
    {
      cout << string("Can't open file'")+filename+"'." << endl;
      exit(1);
    }

  int nh;
  int nd;
  int nr;
  int nmic;
  int nfft;

  fin.read((char*)&nh, sizeof(int));
  fin.read((char*)&nd, sizeof(int));
  fin.read((char*)&nr, sizeof(int));
  fin.read((char*)&nmic, sizeof(int));
  fin.read((char*)&nfft, sizeof(int));

  int slength = nfft / 2 + 1;

  AMatrixArray3D A0;

  A0.resize(extents[nh][nd][nr]);
  for (int ih = 0; ih < nh; ih++) {
    for (int id = 0; id < nd; id++) {
      for (int ir = 0; ir < nr; ir++) {
	A0[ih][id][ir].resize(slength, nmic);
      }
    }
  }
  
  search_height.resize(nh);
  search_dir.resize(nd);
  search_range.resize(nr);
  fin.read((char*)&search_height[0], nh * sizeof(int));
  fin.read((char*)&search_dir[0], nd * sizeof(int));
  fin.read((char*)&search_range[0], nr * sizeof(int));

  search_height_f.resize(nh);
  search_dir_f.resize(nd);
  search_range_f.resize(nr);
  for (int ih = 0; ih < nh; ih++) {
    search_height_f[ih] = (float)search_height[ih];
  }
  for (int id = 0; id < nd; id++) {
    search_dir_f[id] = (float)search_dir[id];
  }
  for (int ir = 0; ir < nr; ir++) {
    search_range_f[ir] = (float)search_range[ir];
  }
  
  vector<complex<float> > buf(slength);
  for (int ih = 0; ih < nh; ih++) {
    for (int id = 0; id < nd; id++) {
      for (int ir = 0; ir < nr; ir++) {
	for (int m = 0; m < nmic; m++) {
	  fin.read((char*)&buf[0], slength * sizeof(complex<float>));
	  
	  for (int k = 0; k < slength; k++) {
	    A0[ih][id][ir](k, m) = buf[k];
	  }
	}
      }
    }
  }

  // channel selection

  int nmic2 = ch.size();
  for (int c = 0; c < ch.size(); c++) {
    if (!(ch[c] >= 0 && ch[c] < nmic)) {
      cerr << "LocalizationGEVD. Nonexistent channel is selected." << endl;
      exit(1);
    }
  }

  A.resize(extents[nh][nd][nr]);
  for (int ih = 0; ih < nh; ih++) {
    for (int id = 0; id < nd; id++) {
      for (int ir = 0; ir < nr; ir++) {
	A[ih][id][ir].resize(slength, nmic2);
      }
    }
  }
  
  for (int ih = 0; ih < nh; ih++) {
    for (int id = 0; id < nd; id++) {
      for (int ir = 0; ir < nr; ir++) {
	for (int m = 0; m < nmic2; m++) {
	  for (int k = 0; k < slength; k++) {
	    A[ih][id][ir](k, m) = A0[ih][id][ir](k, ch[m]);
	  }
	}
      }
    }
  }

}


// 2D with channel selection
void ReadTransferFunction(const char* filename,
                          AMatrixArray& A,
                          vector<int>& search_dir, 
			  vector<int>& search_range,
                          vector<int>& ch)
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

  AMatrixArray A0;

  cerr << nd << " directions, " << nr << " ranges, " << nmic << " microphones, ";
  cerr << nfft << " points" << endl;
  A0.resize(extents[nd][nr]);
  for (int id = 0; id < nd; id++) {
    for (int ir = 0; ir < nr; ir++) {
      A0[id][ir].resize(slength, nmic);
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
          A0[id][ir](k, m) = buf[k];
        }
      }
    }
  }

  // channel selection

  int nmic2 = ch.size();
  for (int c = 0; c < ch.size(); c++) {
    if (!(ch[c] >= 0 && ch[c] < nmic)) {
      cerr << "LocalizationGEVD. Nonexistent channel is selected." << endl;
      exit(1);
    }
  }

  A.resize(extents[nd][nr]);
  for (int id = 0; id < nd; id++) {
    for (int ir = 0; ir < nr; ir++) {
      A[id][ir].resize(slength, nmic2);
    }
  }

  for (int id = 0; id < nd; id++) {
    for (int ir = 0; ir < nr; ir++) {
      for (int m = 0; m < nmic2; m++) {
	for (int k = 0; k < slength; k++) {
	  A[id][ir](k, m) = A0[id][ir](k, ch[m]);
	}
      }
    }
  }

}

/********************************************************************************/

float MaxOfAbsValueFvector(fvector& A)
{
  float max = 0.0f;
  for (int i = 0; i < A.size(); i++) {
    if (fabs(A(i)) > max) {
      max = fabs(A(i));
    }
  }
  return max;
}

void mulfcomplex(fcomplex& a, fcomplex& b, fcomplex& c)
{
  float tmp1,tmp2;
  tmp1 = a.real()*b.real() - a.imag()*b.imag();
  tmp2 = a.real()*b.imag() + a.imag()*b.real();
  c = complex<float>(tmp1,tmp2);

}

void addfcomplex(fcomplex& a, fcomplex& b, fcomplex& c)
{
  float tmp1,tmp2;
  tmp1 = a.real() + b.real();
  tmp2 = b.imag() + a.imag();
  c = complex<float>(tmp1,tmp2);
}

void subfcomplex(fcomplex& a, fcomplex& b, fcomplex& c)
{
  float tmp1,tmp2;
  tmp1 = a.real() - b.real();
  tmp2 = b.imag() - a.imag();
  c = complex<float>(tmp1,tmp2);
}

void devfcomplex(fcomplex& a, fcomplex& b, fcomplex& c)
{
  float tmp1,tmp2;
  tmp1 = (a.real()*b.real()+a.imag()*b.imag())/(b.real()*b.real()+b.imag()*b.imag());
  tmp2 = (a.imag()*b.real()-a.real()*b.imag())/(b.real()*b.real()+b.imag()*b.imag());
  c = complex<float>(tmp1,tmp2);
}

/*Multiplying 2 matrices :  a[d,e] * b[e,f] = c[d,f] */
 void mulMAT(fcmatrix& a,fcmatrix& b,fcmatrix& c)
{
  int i, j, k;  
  int d, e, f;

  fcomplex z=0.0f;
  fcomplex tmpfc = 0.0f;

  d = a.size1();
  e = a.size2();
  f = b.size2();
  
  if(e == b.size1()){
    for(i=0;i<d;i++){
      for(j=0;j<f;j++){
	for(k=0;k<e;k++){
	  mulfcomplex(a(i,k),b(k,j),tmpfc);
	  addfcomplex(z,tmpfc,z);
	}
	c(i,j)=z;
	z=0.0f;
      }
    }  
  }else{
    printf("Error : mulMAT : Not an appropriate size.\n");
  }

}

/*derive the inverse matrix :  inv(a) = c */
void invMAT(fcmatrix& a,fcmatrix& c)
{
  int i,j,k=0;
  int p;

  fcmatrix tmpa(a.size1(), a.size2());
  
  fcomplex x,y=0.0f;
  fcomplex tmpfc = 0.0f;
  fcomplex tmpfc2 = 0.0f;

  noalias(tmpa) = a;

  p = tmpa.size1();

  if(p == tmpa.size2()){

    // Make a Unit Matrix
    for(i=0;i<p;i++){
      for(j=0;j<p;j++){
	if(i==j){
	  c(i,j) = complex<float>(1.0f,0.0f);
	}else{
	  c(i,j) = complex<float>(0.0f,0.0f);
	}
      }
    }
    for(i=0;i<p;i++){
      if((tmpa(i,i).real()==0.0f)&&(tmpa(i,i).imag()==0.0f)){
	for(j=i;j<p;j++){
	  if((tmpa(j,i).real()!=0.0f)||(tmpa(j,i).imag()!=0.0f)){
	    for(k=0;k<p;k++){
	      y=tmpa(i,k);
	      tmpa(i,k)=tmpa(j,k);
	      tmpa(j,k)=y;
	      y=c(i,k);
	      c(i,k)=c(j,k);
	      c(j,k)=y;
	    }
	    k=1;
	    break;
	  };
	}
	if(k!=1)
	  printf("ERROR!!No inverse matrix available.\n");
      }
      for(j=0;j<p;j++){
	devfcomplex(tmpa(j,i),tmpa(i,i),tmpfc);
	x = -tmpfc;
	if(i==j)
	  {
	    x = complex<float>(0.0f,0.0f);
	  }
	for(k=0;k<p;k++){
	  mulfcomplex(x,c(i,k),tmpfc);
	  addfcomplex(c(j,k),tmpfc,tmpfc2);
	  c(j,k) = tmpfc2;
	  mulfcomplex(x,tmpa(i,k),tmpfc);
	  addfcomplex(tmpa(j,k),tmpfc,tmpfc2);
	  tmpa(j,k) = tmpfc2;
	}
      }
      x=tmpa(i,i);
      for(k=0;k<p;k++){
	devfcomplex(tmpa(i,k),x,tmpfc);
	tmpa(i,k) = tmpfc;
	devfcomplex(c(i,k),x,tmpfc);
	c(i,k) = tmpfc;
      }
    } 

  }else{
    printf("Error : invMAT : Not a square matrix.\n");
  }
  
}

/*Multiplying 2 matrices by each element :  a[m,n] .* b[m,n] = c[m,n] */
 void mulMATdot(fcmatrix& a,fcmatrix& b,fcmatrix& c)
{

  int i, j;
  int d, e;

  d = a.size1();
  e = a.size2();

  for(i=0;i<d;i++){
    for(j=0;j<e;j++){
	mulfcomplex(a(i,j),b(i,j),c(i,j));
    }
  }

}

/*Deviding 2 matrices by each element :  a[m,n] ./ b[m,n] = c[m,n] */
 void devMATdot(fcmatrix& a,fcmatrix& b,fcmatrix& c)
{

  int i,j;
  int d, e;

  d = a.size1();
  e = a.size2();

  for(i=0;i<d;i++){
    for(j=0;j<e;j++){
	devfcomplex(a(i,j),b(i,j),c(i,j));
    }
  }

}


/********************************************************************************/

/* make a zero matrix which is the same size of "a" and substitute it to "c" */
void zeroMAT(fcmatrix& a,fcmatrix& c)
{
  
  int row;
  int col;

  row = a.size1();
  col = a.size2();
  
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      if(i==j){
	c(i, j) = complex<float>(0.0f,0.0f);
      }else{
	c(i, j) = complex<float>(0.0f,0.0f);
      }
    }
  }  

}

/* make an identity matrix which is the same size of "a" and substitute it to "c" */
void identityMAT(fcmatrix& a,fcmatrix& c)
{
  int row;
  int col;

  row = a.size1();
  col = a.size2();
  
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      if(i==j){
	c(i, j) = complex<float>(1.0f,0.0f);
      }else{
	c(i, j) = complex<float>(0.0f,0.0f);
      }
    }
  }

}


/********************************************************************************/

// Display a matrix
void display_fcmat(fcmatrix& a)
{
  int i,j;
  int row, col;
  
  row = a.size1();
  col = a.size2();

  printf("*************************************************************\n");
  for(i=0;i<row;i++){
    for(j=0;j<col;j++){
      printf("%f + %f i\t",a(i,j).real(),a(i,j).imag());
    }
    printf("\n\n");
  }
}

/********************************************************************************/

/*Load matrix data*/
//char name1[80] = "data1.mat";
//char name2[80] = "data2.mat";
//read_fcmat(name1,name2,A);
void read_fcmat(char *name1,char *name2, fcmatrix& a)
{  
  FILE *fp1;
  FILE *fp2;
  int i, j;
  int row = a.size1();
  int col = a.size2();
  float tmp1 = 1.0000010e-002;
  float tmp2 = 1.1234567e-001;
  //printf("%f\n",tmp1);
  //printf("%f\n",tmp2);

  if(NULL == (fp1=fopen(name1, "r"))) {
    printf("no such file '%s'.\n", name1);
  }
  if(NULL == (fp2=fopen(name2, "r"))) {
    printf("no such file '%s'.\n", name2);
  }

  for(i=0; i<row; i++){
    for(j=0; j<col; j++){
      fscanf(fp1, "%f", &tmp1);
      fscanf(fp2, "%f", &tmp2);
      //printf("%f\n",tmp2);
      a(i,j) = complex<float>(tmp1,tmp2);
    }
    fscanf(fp1, "\n");
    fscanf(fp2, "\n");
  }
  //printf("%f + %f i\n",a(4,4).real(),a(4,4).imag());
  fclose(fp1);
  fclose(fp2);
}

/* Save matrix data */
int save_mat_fcmat(char *name1, char *name2, fcmatrix& a)
{
  FILE *fp1;
  FILE *fp2;
  int i, j;
  int row, col;
  
  row = a.size1();
  col = a.size2();
      
  fp1=fopen(name1, "w");
  fp2=fopen(name2, "w");
  for(i=0; i<row; i++){
    for(j=0; j<col; j++){
      fprintf(fp1, "%8.6e ", a(i,j).real());
      fprintf(fp2, "%8.6e ", a(i,j).imag());
    }
    fprintf(fp1, "\n");
    fprintf(fp2, "\n");
  }
  
  fclose(fp1);
  fclose(fp2);
  
  return 0;
}

int save_mat_fcmat_real(char *name1, fcmatrix& a)
{
  FILE *fp1;
  int i, j;
  int row, col;
 
  row = a.size1();
  col = a.size2();

  fp1=fopen(name1, "w");
  for(i=0; i<row; i++){
    for(j=0; j<col; j++){
      fprintf(fp1, "%8.6e ", a(i,j).real());
    }
    fprintf(fp1, "\n");
  }
  
  fclose(fp1);
  
  return 0;
}

void save_mat_fmat_real(char *name1, fmatrix& a)
{
  FILE *fp1;
  int i, j;
  int row, col;
 
  row = a.size1();
  col = a.size2();

  fp1=fopen(name1, "w");
  for(i=0; i<row; i++){
    for(j=0; j<col; j++){
      fprintf(fp1, "%8.6e ", a(i,j));
    }
    fprintf(fp1, "\n");
  }
  
  fclose(fp1);
  
}

/********************************************************************************/

void AddToCorrelationAllFreq(vector<fcmatrix>& Rxx, int FreqLength, fcmatrix& X)
{
  for (int k = 0; k < FreqLength; k++) {
    Rxx[k] += outer_prod(ublas::column(X, k), ublas::herm(ublas::column(X, k)));
  }
}

void NormalizeCorrelationAllFreq(vector<fcmatrix>& Rxx, int FreqLength, int sum_count)
{
  float sum_count_1 = 1.0f / sum_count;
  for (int k = 0; k < FreqLength; k++) {
    Rxx[k] *= sum_count_1;
  }
}

void ClearCorrelationAllFreq(vector<fcmatrix>& Rxx, int FreqLength)
{
  for (int k = 0; k < FreqLength; k++) {
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

/********************************************************************************/

/*Load Correlation Matrix*/
//char name1[80] = "data1.mat";
//char name2[80] = "data2.mat";
//read_fcmat(name1,name2,A);
void LoadCorrelation(char *name1, char *name2, vector<fcmatrix>& Rxx)
{  

  ifstream fin1;
  ifstream fin2;
  ifstream::pos_type cp1;
  ifstream::pos_type cp2;
  string buffer1;
  string buffer2;
  vector<string> tokens1;
  vector<string> tokens2;

  typedef boost::tokenizer<boost::char_separator<char> > tokenizer_space;
  boost::char_separator<char> sep(" ");

  Rxx.resize(0);

  fin1.open(name1);
  if(fin1.fail()) {
    printf("no such file '%s'.\n", name1);
    exit(1);
  }
  
  fin2.open(name2);
  if(fin2.fail()) {
    printf("no such file '%s'.\n", name2);
    exit(1);
  }
  
  while(1){
  
    cp1=fin1.tellg();
    getline(fin1, buffer1);
    
    cp2=fin2.tellg();
    getline(fin2, buffer2);

    if(fin1.eof() || fin2.eof()) return;
    
    tokens1.resize(0);
    tokenizer_space tokenizer1(buffer1, sep);
    for (tokenizer_space::iterator it1 = tokenizer1.begin(); it1 != tokenizer1.end(); ++it1) {
      tokens1.push_back(*it1);
    }

    tokens2.resize(0);
    tokenizer_space tokenizer2(buffer2, sep);
    for (tokenizer_space::iterator it2 = tokenizer2.begin(); it2 != tokenizer2.end(); ++it2) {
      tokens2.push_back(*it2);
    }

    int nb_channels1 = (int)(floor(sqrtf((float)tokens1.size())));
    int nb_channels2 = (int)(floor(sqrtf((float)tokens2.size())));
    if(nb_channels1 != nb_channels2) {
      printf("CMLoad. Size mismatched. (%d, %d)\n", nb_channels1, nb_channels2);
      exit(1);
    }    
    if(tokens1.size() != nb_channels1 * nb_channels1){
      printf("CMLoad. Size mismatched. (%d, %d)\n", tokens1.size(), nb_channels1 * nb_channels1);
      exit(1);
    }    
    fcmatrix cormat;
    cormat.resize(nb_channels1, nb_channels1);
    for(int i=0; i < nb_channels1; i++){
      for(int j=0; j < nb_channels1; j++){
	float tmp1 = lexical_cast<float>(tokens1[j + nb_channels1 * i]);
	float tmp2 = lexical_cast<float>(tokens2[j + nb_channels1 * i]);
	cormat(i,j) = complex<float>(tmp1,tmp2);
      }
    }
    
    Rxx.push_back(cormat);
    
  }

  fin1.close();
  fin2.close();

}

/*Save Correlation Matrix*/
//char name1[80] = "data1.mat";
//char name2[80] = "data2.mat";
void SaveCorrelation(char *name1, char *name2, vector<fcmatrix>& Rxx)
{  
  FILE *fp1;
  FILE *fp2;

  int k, i, j;
  int veccol = Rxx.size();
  int row = Rxx[0].size1();
  int col = Rxx[0].size2();

  fp1=fopen(name1, "w");
  fp2=fopen(name2, "w");

  for (k=0; k < veccol; k++){
    for(i=0; i<row; i++){
      for(j=0; j<col; j++){
	fprintf(fp1, "%8.6e ", Rxx[k](i,j).real());
	fprintf(fp2, "%8.6e ", Rxx[k](i,j).imag());
      }
    }
    fprintf(fp1, "\n");
    fprintf(fp2, "\n");
  }
  fclose(fp1);
  fclose(fp2);
}


/********************************************************************************/

/* pow(a,b)=a^b : float version */
float powfloat(float a, float b)
{
  return (float)powf(a,b);
}

/* make diag(a) */
void makeLambda(fvector& a,fcmatrix& b,float decay)
{
  int i,j;
  fcomplex z=0.0f;
  for(i=0;i<b.size1();i++){
    for(j=0;j<b.size2();j++){
      b(i,j)=z;
      if(i==j)
	{
	  if(a[i]>=0.0)
	    {
	      b(i,i)=complex<float>(powfloat(a[i],decay),0.0f);
	    }
	  if(a[i]<0.0)
	    {
	      b(i,i)=complex<float>(-1.0f*powfloat(-1.0f*a[i],decay),0.0f);
	    }
	}
    }
  }
}

/********************************************************************************/

// Matrix norm display
void norm_display_fcmat(fcmatrix& a)
{
  int m;
  int row, col;
  
  row = a.size1();
  col = a.size2();

  for(m=0; m<col; m++){
    float eigvec_norm = norm(ublas::inner_prod(ublas::herm(ublas::column(a, m)),ublas::column(a, m)));
    printf("%f\t", eigvec_norm);
  }
  printf("\n");

}

/********************************************************************************/

struct eigenvalue_ascending_sort : public binary_function<tuple<float, fcvector>, tuple<float, fcvector>, bool> {
  bool operator()(const tuple<float, fcvector>& x, const tuple<float, fcvector>& y) const {
    return x.get<0>() < y.get<0>();
  }
};

struct eigenvalue_decending_sort : public binary_function<tuple<float, fcvector>, tuple<float, fcvector>, bool> {
  bool operator()(const tuple<float, fcvector>& x, const tuple<float, fcvector>& y) const {
    return x.get<0>() > y.get<0>();
  }
};

// Sort eigenvectors depending on their eigenvalue (ascending order)
void sort_eig_fvec_ascending(fvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf)
{

  vector<tuple<float, fcvector> > result;

  // make tuple
  for(int cnt = 0; cnt < ai.size(); cnt++){
    result.push_back(make_tuple(ai[cnt], ublas::column(bi, cnt)));
  }
  
  // sort
  sort(result.begin(), result.end(), eigenvalue_ascending_sort());
  
  // make finalized eigenvalues and eigenvectors
  for(int cnt = 0; cnt < ai.size(); cnt++){
    af[cnt] = result[cnt].get<0>();
    noalias(ublas::column(bf, cnt)) = result[cnt].get<1>();
  }
  
}

// Sort eigenvectors depending on their eigenvalue (ascending order)
void sort_eig_fcvec_ascending(fcvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf)
{

  vector<tuple<float, fcvector> > result;

  // make tuple
  for(int cnt = 0; cnt < ai.size(); cnt++){
    float eigval = sqrt(ai[cnt].real() * ai[cnt].real() + ai[cnt].imag() * ai[cnt].imag());
    result.push_back(make_tuple(eigval, ublas::column(bi, cnt)));
  }
  
  // sort
  sort(result.begin(), result.end(), eigenvalue_ascending_sort());
  
  // make finalized eigenvalues and eigenvectors
  for(int cnt = 0; cnt < ai.size(); cnt++){
    af[cnt] = result[cnt].get<0>();
    noalias(ublas::column(bf, cnt)) = result[cnt].get<1>();
  }
  
}

// Sort eigenvectors depending on their eigenvalue (decending order)
void sort_eig_fvec_decending(fvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf)
{

  vector<tuple<float, fcvector> > result;

  // make tuple
  for(int cnt = 0; cnt < ai.size(); cnt++){
    result.push_back(make_tuple(ai[cnt], ublas::column(bi, cnt)));
  }
  
  // sort
  sort(result.begin(), result.end(), eigenvalue_decending_sort());
  
  // make finalized eigenvalues and eigenvectors
  for(int cnt = 0; cnt < ai.size(); cnt++){
    af[cnt] = result[cnt].get<0>();
    noalias(ublas::column(bf, cnt)) = result[cnt].get<1>();
  }
  
}

// Sort eigenvectors depending on their eigenvalue (decending order)
void sort_eig_fcvec_decending(fcvector& ai,fcmatrix& bi, fvector& af,fcmatrix& bf)
{

  vector<tuple<float, fcvector> > result;

  // make tuple
  for(int cnt = 0; cnt < ai.size(); cnt++){
    float eigval = sqrt(ai[cnt].real() * ai[cnt].real() + ai[cnt].imag() * ai[cnt].imag());
    result.push_back(make_tuple(eigval, ublas::column(bi, cnt)));
  }
  
  // sort
  sort(result.begin(), result.end(), eigenvalue_decending_sort());
  
  // make finalized eigenvalues and eigenvectors
  for(int cnt = 0; cnt < ai.size(); cnt++){
    af[cnt] = result[cnt].get<0>();
    noalias(ublas::column(bf, cnt)) = result[cnt].get<1>();
  }
  
}
