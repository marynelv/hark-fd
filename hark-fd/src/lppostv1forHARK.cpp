#include <iostream>
#include <fstream>
#include <string>
#include <complex>
#include <string.h>

const float PI = 3.14159265358979323846264338;

using namespace std;



// --------------------------------------------
// CLASS: Compmat
// --------------------------------------------
// Required : complex, string.h
class Compmat{
	int            nrow, ncol, nrowcol;
	complex<float> *zval;
public:
	Compmat();
	Compmat(const int& Nrow);
	Compmat(const int& Nrow, const int& Ncol);
	Compmat(const Compmat&);
	Compmat(const float&);
	~Compmat(){delete [] zval;}
	Compmat& operator=(const Compmat&);
	Compmat& operator=(const float&);
	Compmat operator+(const float&) const; // Scalar(real) Addition
	Compmat operator-(const float&) const; // Scalar(real) Subtruction
	Compmat operator+(const Compmat&) const; // Addition with truncation
	Compmat operator-(const Compmat&) const; // Subtruction with truncation
	Compmat& operator+=(const float&); // Addition
	Compmat& operator-=(const float&); // Subtruction
	Compmat& operator+=(const Compmat&); // Addition
	Compmat& operator-=(const Compmat&); // Subtruction
	Compmat operator*(const float&); // Scalar(real) multiplication
	Compmat& operator*=(const float&); // Scalar(real) multiplication
	Compmat operator&(const Compmat&); // MATLAB (*this).*X with repetition of X
	Compmat& operator&=(const Compmat&);
	Compmat operator|(const Compmat&); // MATLAB (*this)./X with repetition of X
	complex<float>& operator()(const int& row, const int& col) const {return zval[row+col*nrow];}
	complex<float>& operator()(const int& rowcol)               const {return zval[rowcol];}
	Compmat operator()(const Compmat&) const; // Read only (A(B) = C is not OK)
	void zeros(const int& Nrow, const int& Ncol);
	void zerossize(const Compmat& x){zeros(x.size1(), x.size2());}
	int size1() const {return nrow;}
	int size2() const {return ncol;}
	int size()  const {return nrowcol;}
	int end1() const {return nrow-1;}
	int end2() const {return ncol-1;}
	int end()  const {return nrowcol-1;}
	float& r(const int& row, const int& col)          const {return *((float*)&zval[row+col*nrow]);}
	float& r(const int& rowcol)                        const {return *((float*)&zval[rowcol]);}
	float& i(const int& row, const int& col)          const {return *((float*)&zval[row+col*nrow] + 1);}
	float& i(const int& rowcol)                        const {return *((float*)&zval[rowcol] + 1);}
	Compmat mid1(int, int) const; // Y = (*this)(start:end, :)
	Compmat mid1(const int& row) const {return mid1(row, row);} // Y = (*this)(row, :)
	Compmat mid2(int, int) const; // Y = (*this)(start:end, :)
	Compmat mid2(const int& col) const {return mid2(col, col);} // Y = (*this)(:, col)
	Compmat norm2() const;
	void hanning(const int&);
	void disp() const;
	void colon(const float&, const float&, const float&); // MATLAB : operator (start:step:end)
	void colon(const float& st, const float& en){colon(st, 1., en);}; // : operator (start:end)
	Compmat dB2Pow() const; // for real value
	Compmat reshape(const int&, const int&) const; // reshape with automatic truncation or 0 padding
	Compmat reshape(const int& Nrow) const {return reshape(Nrow, nrow*ncol/Nrow);}; // Nlow ommit version
	Compmat reshape() const {return reshape(nrow*ncol, 1);}; // column vector case
	Compmat signGT_BLK(const Compmat&) const; // ~= (1+sign((*this)-X))/2 with Block extension
	void    signGT_BLK_addset_tr(const Compmat&, Compmat&, const float&) const;
	void    TBL_addset_tr(const Compmat&, const short*, const int&, const float&);
	Compmat repmat0(const int&, const int&) const; // repmat with allow non-integer repeat
	Compmat repmat(const int& NrowR, const int& NcolR) const{return repmat0(nrow*NrowR, ncol*NcolR);}; // MATLAB repmat
	Compmat repmatsize(const Compmat& x) const{return repmat0(x.size1(), x.size2());}
	Compmat absri() const; // Y = abs(Re{X}) + i*abs(Im{X})
	Compmat minind() const; // [(void), Y] = min( (*this) )
	Compmat ind_firstplus() const; //
	Compmat nearest_inc(const Compmat&) const; // [(void), Y] = min( (*this) - X.*ones(1, size((*this), 2) )
	void    nearest_inc(const Compmat&, Compmat&) const;
	Compmat flooring(const float&) const; // flooring real only
	Compmat sqrt() const; // for real value
	void dump(const char*) const;
};

// Constructers
Compmat::Compmat()                                   {nrowcol = 0; zeros(1, 1);}
Compmat::Compmat(const int& Nrow)                   {nrowcol = 0; zeros(Nrow, 1);}
Compmat::Compmat(const int& Nrow, const int& Ncol) {nrowcol = 0; zeros(Nrow, Ncol);}
Compmat::Compmat(const Compmat& x)                   {nrowcol = 0; *this = x;}
Compmat::Compmat(const float& x)                    {nrowcol = 0; *this = x;}

// Operators
Compmat& Compmat::operator=(const Compmat& x){
	if(this == &x) return *this;
	zeros(x.size1(), x.size2());
	memcpy( (void*)zval, &(x(0)), size()*sizeof(complex<float>) );
	return *this;
}

Compmat& Compmat::operator=(const float& val){
	zeros(1, 1);
	r(0) = val;
	i(0) = 0;
	return *this;
}

Compmat Compmat::operator+(const float& x) const{
	Compmat y = *this;
	for(int n = 0; n < y.size(); n++){
		y(n) = y(n) + x;}
	return y;
}

Compmat Compmat::operator-(const float& x) const{
	Compmat y = *this;
	for(int n = 0; n < y.size(); n++){
		y(n) = y(n) - x;}
	return y;
}

Compmat Compmat::operator+(const Compmat& x) const{
	int s1, s2, n1, n2;
	Compmat y = *this;
	s1 = size1();
	s2 = size2();
	if(s1 > x.size1()){s1 = x.size1();}
	if(s2 > x.size2()){s2 = x.size2();}
	for(n1 = 0; n1 < s1; n1++){
		for(n2 = 0; n2 < s2; n2++){
			y(n1, n2) = (*this)(n1, n2) + x(n1, n2);
		}
	}
	return y;
}

Compmat Compmat::operator-(const Compmat& x) const{
	int s1, s2, n1, n2;
	Compmat y = *this;
	s1 = size1();
	s2 = size2();
	if(s1 > x.size1()){s1 = x.size1();}
	if(s2 > x.size2()){s2 = x.size2();}
	for(n1 = 0; n1 < s1; n1++){
		for(n2 = 0; n2 < s2; n2++){
			y(n1, n2) = (*this)(n1, n2) - x(n1, n2);
		}
	}
	return y;
}

Compmat& Compmat::operator+=(const float& val){
	for (int n = 0; n < size(); n++){
		(*this)(n) += val;
	}
	return *this;
}

Compmat& Compmat::operator-=(const float& val){
	for (int n = 0; n < size(); n++){
		(*this)(n) -= val;
	}
	return *this;
}

Compmat& Compmat::operator+=(const Compmat& x){
	int s1, s2, n1, n2;
	s1 = size1();
	s2 = size2();
	if(s1 > x.size1()){s1 = x.size1();}
	if(s2 > x.size2()){s2 = x.size2();}
	for(n1 = 0; n1 < s1; n1++){
		for(n2 = 0; n2 < s2; n2++){
			(*this)(n1, n2) += x(n1, n2);
		}
	}
	return *this;
}

Compmat& Compmat::operator-=(const Compmat& x){
	int s1, s2, n1, n2;
	s1 = size1();
	s2 = size2();
	if(s1 > x.size1()){s1 = x.size1();}
	if(s2 > x.size2()){s2 = x.size2();}
	for(n1 = 0; n1 < s1; n1++){
		for(n2 = 0; n2 < s2; n2++){
			(*this)(n1, n2) -= x(n1, n2);
		}
	}
	return *this;
}

Compmat Compmat::operator*(const float& val){
	Compmat y = *this;
	for (int n = 0; n < size(); n++){
		y(n) = (*this)(n) * val;
	}
	return y;
}

Compmat& Compmat::operator*=(const float& val){
	for (int n = 0; n < size(); n++){
		(*this)(n) *= val;
	}
	return *this;
}

Compmat Compmat::operator&(const Compmat& x){
	int n1, n2, n1mod, n2mod;
	if(this == &x) return *this;
	Compmat y = *this;
	for (n1 = 0, n1mod = 0; n1 < size1() ; n1++, n1mod++){
		if(n1mod == x.size1()){n1mod = 0;}
		for (n2 = 0, n2mod = 0; n2 < size2() ; n2++, n2mod++){
			if(n2mod == x.size2()){n2mod = 0;}
			y(n1, n2) = (*this)(n1, n2) * x(n1mod, n2mod);
		}
	}
	return y;
}

Compmat& Compmat::operator&=(const Compmat& x){
	int n1, n2, n1mod, n2mod;
	for (n1 = 0, n1mod = 0; n1 < size1() ; n1++, n1mod++){
		if(n1mod == x.size1()){n1mod = 0;}
		for (n2 = 0, n2mod = 0; n2 < size2() ; n2++, n2mod++){
			if(n2mod == x.size2()){n2mod = 0;}
			(*this)(n1, n2) *= x(n1mod, n2mod);
		}
	}
	return *this;
}

Compmat Compmat::operator|(const Compmat& x){
	int n1, n2, n1mod, n2mod;
	if(this == &x) return *this;
	Compmat y = *this;
	for (n1 = 0, n1mod = 0; n1 < size1() ; n1++, n1mod++){
		if(n1mod == x.size1()){n1mod = 0;}
		for (n2 = 0, n2mod = 0; n2 < size2() ; n2++, n2mod++){
			if(n2mod == x.size2()){n2mod = 0;}
			y(n1, n2) = (*this)(n1, n2) / x(n1mod, n2mod);
		}
	}
	return y;
}

Compmat Compmat::operator()(const Compmat& x) const{
	Compmat retval;
	int m;
	retval.zerossize(x);
	for(int n = 0; n < x.size(); n++){
		m = (int)(x.r(n));
		if(m >= 0 && m < size()){retval(n) = (*this)(m);}
	}
	return retval;
}

void Compmat::zeros(const int& Nrow, const int& Ncol){
	int Nrowcol;
	nrow = Nrow;
	ncol = Ncol;
	if(nrow < 1){nrow = 1;}
	if(ncol < 1){ncol = 1;}
	Nrowcol = nrow*ncol;
	if(nrowcol != Nrowcol){
		if(nrowcol > 0){delete [] zval;}
		nrowcol = Nrowcol;
		zval = new complex<float>[nrowcol];
	}
	memset( (void*)zval, 0, nrowcol*sizeof(complex<float>) );
}

Compmat Compmat::mid1(int start, int end) const{
	Compmat y;
	if(start==0 && end == size1()-1){return *this;}
	if(start > end){start = end;}
	if(start >= size1()){start = size1()-1;}
	if(end >= size1()){end = size1()-1;}
	y.zeros(end-start+1, size2());
	for(int n1 = 0; n1 < y.size1(); n1++){
		for(int n2 = 0; n2 < size2() ; n2++){
			y(n1, n2) = (*this)(n1 + start, n2);
		}
	}
	return y;
}

Compmat Compmat::mid2(int start, int end) const{
	Compmat y;
	if(start==0 && end == end2()){return *this;}
	if(start > end){start = end;}
	if(start >= size2()){start = end2();}
	if(end >= size2()){end = end2();}
	y.zeros(size1(), end-start+1);
	for(int n1 = 0; n1 < size1(); n1++){
		for(int n2 = 0; n2 < y.size2() ; n2++){
			y(n1, n2) = (*this)(n1, n2+start);
		}
	}
	return y;
}

Compmat Compmat::norm2() const {
	Compmat y;
	y.zeros(nrow, ncol);
	for (int n = 0; n < nrow*ncol; n++){
		y.r(n) = norm((*this)(n));
	}
	return y;
	
}

void Compmat::hanning(const int& Nrow){
	float pnrow = 1.0/Nrow;
	zeros(Nrow, 1);
	for (int n = 0; n < nrow; n++){
		zval[n] = 0.5 - 0.5 * cos( 2.0 * PI* n * pnrow );
	}
}

void Compmat::disp() const{
	cout << endl;
	cout << "---------- Compmat.disp() ----------" << endl;
	cout << "[";
	for(int n2 = 0; n2 < size2(); n2++){
		for(int n1 = 0; n1 < size1(); n1++){
			cout << (*this)(n1, n2);
			if(n1 < size1()-1){cout << ", ";}
		}
		if(n2 < size2()-1){cout << ";" << endl;}
	}
	cout << "]^T" << endl;
	cout << "Matrix size = " << size1() << " x " << size2() << endl;
}

void Compmat::colon(const float& st, const float& step, const float& en){
	int N, n;
	N = 1 + floor((en - st)/step);
	if(N < 1){return;};
	zeros(N, 1);
	for(n = 0; n < N; n++){
		r(n) = st + step * n;
		i(n) = 0;
	}
}

Compmat Compmat::dB2Pow() const{
	Compmat retval;
	retval.zerossize(*this);
	for(int n = 0; n < size(); n++){
		retval.r(n) = pow(10., r(n)*0.1);
	}
	return retval;
}

Compmat Compmat::reshape(const int& Nrow, const int& Ncol) const{
	Compmat retval;
	int N;
	retval.zeros(Nrow, Ncol);
	N = retval.size();
	if(N > size()){N = size();}
	memcpy( &(retval(0)), &((*this)(0)), N*sizeof(complex<float>) );
	return retval;
}

Compmat Compmat::signGT_BLK(const Compmat& x) const{
	Compmat retval;
	float dtemp;
	int n1, n2, m1, m2, N1, N2;
	retval.zeros( nrow * x.size1(), ncol * x.size2() );
	for(n1 = 0, N1 = 0; n1 < nrow; n1++, N1 += x.size1()){
		for(n2 = 0, N2 = 0; n2 < ncol; n2++, N2 += x.size2()){
			dtemp = r(n1, n2);
			for(m1 = 0; m1 < x.size1(); m1++){
				for(m2 = 0; m2 < x.size2(); m2++){
					if( dtemp > x.r(m1, m2) ){retval.r(N1+m1, N2+m2) = 1;}
				}
			}
		}
	}
	return retval;
}

void Compmat::signGT_BLK_addset_tr(const Compmat& x, Compmat& y, const float& tr) const{
	float dtemp;
	int n1, n2, m1, m2, N1, N2;
	for(n1 = 0, N1 = 0; n1 < nrow; n1++, N1 += x.size1()){
		for(n2 = 0, N2 = 0; n2 < ncol; n2++, N2 += x.size2()){
			dtemp = r(n1, n2);
			for(m1 = 0; m1 < x.size1(); m1++){
				for(m2 = 0; m2 < x.size2(); m2++){
					if( dtemp > x.r(m1, m2) ){y.r(N1+m1, N2+m2) += tr;}
				}
			}
		}
	}
}


//	High speed spec.
void Compmat::TBL_addset_tr(const Compmat& x, const short* TABLE, const int& SBIT, const float& tr){
	int  *ptemp;
	int  ltemp;
	int n1, n2, n12, N2, N12;
	float *rn12;
	for(n2 = 0, N2 = 0; n2 < size2(); n2++, N2 += size1()){
		if (isinf(x.r(n2)) || isnan(x.r(n2))){
			//cout << "HRLE : Input Power is Inf or NaN !!" << endl;
		}
		else{
			ptemp = (int*)(&(x.r(n2)));
			ltemp = *ptemp;
			ltemp >>= SBIT;
			n1 = (int)TABLE[ltemp];
			n12 = N2 + n1;
			N12 = N2 + size1();
			for(rn12 = &(r(n12)); n12 < N12; n12++, rn12+=2){
				*rn12 += tr;
			}
		}
	}
}

Compmat Compmat::repmat0(const int& Nrow, const int& Ncol) const{
	Compmat retval;
	int n1, n2, n1mod, n2mod;
	retval.zeros(Nrow, Ncol);
	for(n1 = 0, n1mod = 0; n1 < Nrow; n1++, n1mod++){
		if(n1mod == size1()){n1mod = 0;}
		for(n2 = 0, n2mod = 0; n2 < Ncol; n2++, n2mod++){
			if(n2mod == size2()){n2mod = 0;}
				retval(n1, n2) = (*this)(n1mod, n2mod);
		}
	}
	return retval;
}

Compmat Compmat::absri() const{
	Compmat retval;
	retval.zerossize(*this);
	for(int n = 0; n < size(); n++){
		retval.r(n) = fabs(r(n));
		retval.i(n) = fabs(i(n));
	}
	return retval;
}

Compmat Compmat::minind() const{
	Compmat retval;
	int n1, n2;
	float minval;
	retval.zeros(1, size2());
	for(n2 = 0; n2 < size2(); n2++){
		minval = r(0, n2);
		retval.r(n2) = 0;
		for(n1 = 1; n1 < size1(); n1++){
			if(minval > r(n1, n2)){
				minval = r(n1, n2);
				retval.r(n2) = n1;
			}
		}
	}
	return retval;
}

Compmat Compmat::ind_firstplus() const{
	Compmat retval;
	int n1, n2;
	retval.zeros(1, size2());
	for(n2 = 0; n2 < size2(); n2++){
		for(n1 = 0; n1 < size1(); n1++){
			if(r(n1, n2) > 0){
				break;
			}
		}
		retval.r(n2) = n1;
	}
	return retval;
}

//	High speed spec.
Compmat Compmat::nearest_inc(const Compmat& x) const{
	Compmat retval;
	int n1, n2;
	float ftmp;
	float xr;
	float *rn2, *rn12;
	retval.zeros(1, size2());
	for(n2 = 0; n2 < size2(); n2++){
		xr = x.r(0, n2);
		rn2 = &(retval.r(n2));
		rn12 = &(r(0, n2));
		if(xr < *rn12 ){
			*rn2 = 0;
			continue;}
		for(n1 = 1, rn12+=2; n1 < size1(); n1++, rn12+=2){
			if(xr < *rn12){break;}
		}
		retval.r(n2) = n1 - 1;
		if(n1 == size1()){continue;}
		ftmp = *rn12;
		rn12 -= 2;
		ftmp += *rn12;
		ftmp -= xr;
		ftmp -= xr;
		if(ftmp < 0){retval.r(n2) += 1;}
	}
	return retval;
}

//	Suprt high speed spec.
void Compmat::nearest_inc(const Compmat& x, Compmat& retval) const{
	int n1, n2;
	float ftmp;
	float xr;
	float *rn2, *rn12;
	for(n2 = 0; n2 < size2(); n2++){
		xr = x.r(0, n2); //reference val
		rn2 = &(retval.r(n2));
		n1 = *rn2; // previous frame index
//n1 = size1()-1;
		rn12 = &(r(n1, n2));
		if(xr < *rn12 ){ // backward search
			for(n1--, rn12-=2; n1 >= 0; n1--, rn12-=2){
				if(xr >= *rn12){break;}
			}
			retval.r(n2) = n1 + 1;
			if(n1 < 0){continue;}
			ftmp = *rn12;
			rn12 += 2;
			ftmp += *rn12;
			ftmp -= xr;
			ftmp -= xr;
			if(ftmp > 0){*rn2 -= 1;}
		}
		else{  // forward search
			for(n1++, rn12+=2; n1 < size1(); n1++, rn12+=2){
				if(xr < *rn12){break;}
			}
			retval.r(n2) = n1 - 1;
			if(n1 == size1()){continue;}
			ftmp = *rn12;
			rn12 -= 2;
			ftmp += *rn12;
			ftmp -= xr;
			ftmp -= xr;
			if(ftmp < 0){*rn2 += 1;}
		}
	}
}


Compmat Compmat::flooring(const float& x) const{
	Compmat retval;
	retval = *this;
	for(int n = 0; n < size(); n++){
		if( retval.r(n) < x){ retval.r(n) = x;}
	}
	return retval;
}

Compmat Compmat::sqrt() const{
	Compmat retval;
	retval.zerossize(*this);
	for(int n = 0; n < size(); n++){
		retval.r(n) = std::sqrt(r(n));
	}
	return retval;
}

void Compmat::dump(const char* ofname) const{
	ofstream  ofs;

	ofs.open(ofname, ios::binary | ios::in);
	ofs.seekp(0, ios_base::end);
	ofs.write((const char*)(&((*this).r(0))), sizeof(complex<float>)*size());
	ofs.close();

}


//	Compmat retval, Ctemp;
//	Ctemp = *this;
//	Ctemp -= x.repmat0(size1(), size2());
//	Ctemp = Ctemp.absri();


// --------------------------------------------
// CLASS: Cfft
// --------------------------------------------
// Required : Compmat
// FFT functions' original: "fft4g.c" by Ooura
// WEB: http://www.kurims.kyoto-u.ac.jp/~ooura/fft.html
class Cfft{
	int     nfft;
	int     nfftsqrt;
	float   *w;
	int     *ip;
public:
	Cfft() : nfft(0), w(0), ip(0) {};
	~Cfft(){delete [] w; delete [] ip;}
	Compmat fft0(const Compmat&, int, int);
	Compmat fft(const Compmat& x, int NFFT){return fft0(x, NFFT, -1);}
	Compmat ifft(const Compmat& x, int NFFT){return fft0(x, NFFT, 1);}
	void winfft(Compmat&, const Compmat&);
	void fft0ov(Compmat&, int); // size1() should be 2^N
};

Compmat Cfft:: fft0(const Compmat& x, int NFFT, int sign){
	void cdft(int, int, float*, int*, float*);
	Compmat X;
	if(nfft != NFFT){
		delete [] w;
		delete [] ip;
		nfft = NFFT;
		nfftsqrt = (int)sqrt((float)nfft);
		w = new float[nfft * 5 / 4];
		ip = new int[nfftsqrt + 2];
		ip[0] = 0;
	}
	X.zeros(nfft, x.size2());
	X += x;
	if(sign > 0){X *= 1/(float)nfft;}
	for(int n2 = 0; n2 < x.size2(); n2++){
		cdft((int)nfft*2, (int)sign, &(X.r(0, n2)), (int*)ip, w);
	}
	return X;
}

void Cfft:: fft0ov(Compmat& x, int sign){
	void cdft(int, int, float*, int*, float*);
	int NFFT = x.size1();
	if(nfft != NFFT){
		delete [] w;
		delete [] ip;
		nfft = NFFT;
		nfftsqrt = (int)sqrt((float)nfft);
		w = new float[nfft * 5 / 4];
		ip = new int[nfftsqrt + 2];
		ip[0] = 0;
	}
	if(sign > 0){x *= 1/(float)nfft;}
	for(int n2 = 0; n2 < x.size2(); n2++){
		cdft((int)nfft*2, (int)sign, &(x.r(0, n2)), (int*)ip, w);
	}
}

void Cfft::winfft(Compmat&x, const Compmat& win){
	x &= win;
	fft0ov(x, -1);
}


void cdft(int n, int isgn, float *a, int *ip, float *w)
{
    void makewt(int nw, int *ip, float *w);
    void bitrv2(int n, int *ip, float *a);
    void bitrv2conj(int n, int *ip, float *a);
    void cftfsub(int n, float *a, float *w);
    void cftbsub(int n, float *a, float *w);
    
    if (n > (ip[0] << 2)) {
        makewt(n >> 2, ip, w);
    }
    if (n > 4) {
        if (isgn >= 0) {
            bitrv2(n, ip + 2, a);
            cftfsub(n, a, w);
        } else {
            bitrv2conj(n, ip + 2, a);
            cftbsub(n, a, w);
        }
    } else if (n == 4) {
        cftfsub(n, a, w);
    }
}

void makewt(int nw, int *ip, float *w)
{
    void bitrv2(int n, int *ip, float *a);
    int j, nwh;
    float delta, x, y;
    
    ip[0] = nw;
    ip[1] = 1;
    if (nw > 2) {
        nwh = nw >> 1;
        delta = atan(1.0) / nwh;
        w[0] = 1;
        w[1] = 0;
        w[nwh] = cos(delta * nwh);
        w[nwh + 1] = w[nwh];
        if (nwh > 2) {
            for (j = 2; j < nwh; j += 2) {
                x = cos(delta * j);
                y = sin(delta * j);
                w[j] = x;
                w[j + 1] = y;
                w[nw - j] = y;
                w[nw - j + 1] = x;
            }
            bitrv2(nw, ip + 2, w);
        }
    }
}

void bitrv2(int n, int *ip, float *a)
{
    int j, j1, k, k1, l, m, m2;
    float xr, xi, yr, yi;
    
    ip[0] = 0;
    l = n;
    m = 1;
    while ((m << 3) < l) {
        l >>= 1;
        for (j = 0; j < m; j++) {
            ip[m + j] = ip[j] + l;
        }
        m <<= 1;
    }
    m2 = 2 * m;
    if ((m << 3) == l) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 -= m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            j1 = 2 * k + m2 + ip[k];
            k1 = j1 + m2;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
        }
    } else {
        for (k = 1; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += m2;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
        }
    }
}

void bitrv2conj(int n, int *ip, float *a)
{
    int j, j1, k, k1, l, m, m2;
    float xr, xi, yr, yi;
    
    ip[0] = 0;
    l = n;
    m = 1;
    while ((m << 3) < l) {
        l >>= 1;
        for (j = 0; j < m; j++) {
            ip[m + j] = ip[j] + l;
        }
        m <<= 1;
    }
    m2 = 2 * m;
    if ((m << 3) == l) {
        for (k = 0; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 -= m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += 2 * m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 2 * k + ip[k];
            a[k1 + 1] = -a[k1 + 1];
            j1 = k1 + m2;
            k1 = j1 + m2;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            k1 += m2;
            a[k1 + 1] = -a[k1 + 1];
        }
    } else {
        a[1] = -a[1];
        a[m2 + 1] = -a[m2 + 1];
        for (k = 1; k < m; k++) {
            for (j = 0; j < k; j++) {
                j1 = 2 * j + ip[k];
                k1 = 2 * k + ip[j];
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m2;
                k1 += m2;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
            }
            k1 = 2 * k + ip[k];
            a[k1 + 1] = -a[k1 + 1];
            a[k1 + m2 + 1] = -a[k1 + m2 + 1];
        }
    }
}

void cftfsub(int n, float *a, float *w)
{
    void cft1st(int n, float *a, float *w);
    void cftmdl(int n, int l, float *a, float *w);
    int j, j1, j2, j3, l;
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    l = 2;
    if (n > 8) {
        cft1st(n, a, w);
        l = 8;
        while ((l << 2) < n) {
            cftmdl(n, l, a, w);
            l <<= 2;
        }
    }
    if ((l << 2) == n) {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = a[j + 1] + a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = a[j + 1] - a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            a[j2] = x0r - x2r;
            a[j2 + 1] = x0i - x2i;
            a[j1] = x1r - x3i;
            a[j1 + 1] = x1i + x3r;
            a[j3] = x1r + x3i;
            a[j3 + 1] = x1i - x3r;
        }
    } else {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            x0r = a[j] - a[j1];
            x0i = a[j + 1] - a[j1 + 1];
            a[j] += a[j1];
            a[j + 1] += a[j1 + 1];
            a[j1] = x0r;
            a[j1 + 1] = x0i;
        }
    }
}


void cftbsub(int n, float *a, float *w)
{
    void cft1st(int n, float *a, float *w);
    void cftmdl(int n, int l, float *a, float *w);
    int j, j1, j2, j3, l;
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    l = 2;
    if (n > 8) {
        cft1st(n, a, w);
        l = 8;
        while ((l << 2) < n) {
            cftmdl(n, l, a, w);
            l <<= 2;
        }
    }
    if ((l << 2) == n) {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = -a[j + 1] - a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = -a[j + 1] + a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i - x2i;
            a[j2] = x0r - x2r;
            a[j2 + 1] = x0i + x2i;
            a[j1] = x1r - x3i;
            a[j1 + 1] = x1i - x3r;
            a[j3] = x1r + x3i;
            a[j3 + 1] = x1i + x3r;
        }
    } else {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            x0r = a[j] - a[j1];
            x0i = -a[j + 1] + a[j1 + 1];
            a[j] += a[j1];
            a[j + 1] = -a[j + 1] - a[j1 + 1];
            a[j1] = x0r;
            a[j1 + 1] = x0i;
        }
    }
}

void cft1st(int n, float *a, float *w)
{
    int j, k1, k2;
    float wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    x0r = a[0] + a[2];
    x0i = a[1] + a[3];
    x1r = a[0] - a[2];
    x1i = a[1] - a[3];
    x2r = a[4] + a[6];
    x2i = a[5] + a[7];
    x3r = a[4] - a[6];
    x3i = a[5] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[2] = x1r - x3i;
    a[3] = x1i + x3r;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
    wk1r = w[2];
    x0r = a[8] + a[10];
    x0i = a[9] + a[11];
    x1r = a[8] - a[10];
    x1i = a[9] - a[11];
    x2r = a[12] + a[14];
    x2i = a[13] + a[15];
    x3r = a[12] - a[14];
    x3i = a[13] - a[15];
    a[8] = x0r + x2r;
    a[9] = x0i + x2i;
    a[12] = x2i - x0i;
    a[13] = x0r - x2r;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[10] = wk1r * (x0r - x0i);
    a[11] = wk1r * (x0r + x0i);
    x0r = x3i + x1r;
    x0i = x3r - x1i;
    a[14] = wk1r * (x0i - x0r);
    a[15] = wk1r * (x0i + x0r);
    k1 = 0;
    for (j = 16; j < n; j += 16) {
        k1 += 2;
        k2 = 2 * k1;
        wk2r = w[k1];
        wk2i = w[k1 + 1];
        wk1r = w[k2];
        wk1i = w[k2 + 1];
        wk3r = wk1r - 2 * wk2i * wk1i;
        wk3i = 2 * wk2i * wk1r - wk1i;
        x0r = a[j] + a[j + 2];
        x0i = a[j + 1] + a[j + 3];
        x1r = a[j] - a[j + 2];
        x1i = a[j + 1] - a[j + 3];
        x2r = a[j + 4] + a[j + 6];
        x2i = a[j + 5] + a[j + 7];
        x3r = a[j + 4] - a[j + 6];
        x3i = a[j + 5] - a[j + 7];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        x0r -= x2r;
        x0i -= x2i;
        a[j + 4] = wk2r * x0r - wk2i * x0i;
        a[j + 5] = wk2r * x0i + wk2i * x0r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j + 2] = wk1r * x0r - wk1i * x0i;
        a[j + 3] = wk1r * x0i + wk1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j + 6] = wk3r * x0r - wk3i * x0i;
        a[j + 7] = wk3r * x0i + wk3i * x0r;
        wk1r = w[k2 + 2];
        wk1i = w[k2 + 3];
        wk3r = wk1r - 2 * wk2r * wk1i;
        wk3i = 2 * wk2r * wk1r - wk1i;
        x0r = a[j + 8] + a[j + 10];
        x0i = a[j + 9] + a[j + 11];
        x1r = a[j + 8] - a[j + 10];
        x1i = a[j + 9] - a[j + 11];
        x2r = a[j + 12] + a[j + 14];
        x2i = a[j + 13] + a[j + 15];
        x3r = a[j + 12] - a[j + 14];
        x3i = a[j + 13] - a[j + 15];
        a[j + 8] = x0r + x2r;
        a[j + 9] = x0i + x2i;
        x0r -= x2r;
        x0i -= x2i;
        a[j + 12] = -wk2i * x0r - wk2r * x0i;
        a[j + 13] = -wk2i * x0i + wk2r * x0r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j + 10] = wk1r * x0r - wk1i * x0i;
        a[j + 11] = wk1r * x0i + wk1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j + 14] = wk3r * x0r - wk3i * x0i;
        a[j + 15] = wk3r * x0i + wk3i * x0r;
    }
}


void cftmdl(int n, int l, float *a, float *w)
{
    int j, j1, j2, j3, k, k1, k2, m, m2;
    float wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    float x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    m = l << 2;
    for (j = 0; j < l; j += 2) {
        j1 = j + l;
        j2 = j1 + l;
        j3 = j2 + l;
        x0r = a[j] + a[j1];
        x0i = a[j + 1] + a[j1 + 1];
        x1r = a[j] - a[j1];
        x1i = a[j + 1] - a[j1 + 1];
        x2r = a[j2] + a[j3];
        x2i = a[j2 + 1] + a[j3 + 1];
        x3r = a[j2] - a[j3];
        x3i = a[j2 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j2] = x0r - x2r;
        a[j2 + 1] = x0i - x2i;
        a[j1] = x1r - x3i;
        a[j1 + 1] = x1i + x3r;
        a[j3] = x1r + x3i;
        a[j3 + 1] = x1i - x3r;
    }
    wk1r = w[2];
    for (j = m; j < l + m; j += 2) {
        j1 = j + l;
        j2 = j1 + l;
        j3 = j2 + l;
        x0r = a[j] + a[j1];
        x0i = a[j + 1] + a[j1 + 1];
        x1r = a[j] - a[j1];
        x1i = a[j + 1] - a[j1 + 1];
        x2r = a[j2] + a[j3];
        x2i = a[j2 + 1] + a[j3 + 1];
        x3r = a[j2] - a[j3];
        x3i = a[j2 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j2] = x2i - x0i;
        a[j2 + 1] = x0r - x2r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j1] = wk1r * (x0r - x0i);
        a[j1 + 1] = wk1r * (x0r + x0i);
        x0r = x3i + x1r;
        x0i = x3r - x1i;
        a[j3] = wk1r * (x0i - x0r);
        a[j3 + 1] = wk1r * (x0i + x0r);
    }
    k1 = 0;
    m2 = 2 * m;
    for (k = m2; k < n; k += m2) {
        k1 += 2;
        k2 = 2 * k1;
        wk2r = w[k1];
        wk2i = w[k1 + 1];
        wk1r = w[k2];
        wk1i = w[k2 + 1];
        wk3r = wk1r - 2 * wk2i * wk1i;
        wk3i = 2 * wk2i * wk1r - wk1i;
        for (j = k; j < l + k; j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = a[j + 1] + a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = a[j + 1] - a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            x0r -= x2r;
            x0i -= x2i;
            a[j2] = wk2r * x0r - wk2i * x0i;
            a[j2 + 1] = wk2r * x0i + wk2i * x0r;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j1] = wk1r * x0r - wk1i * x0i;
            a[j1 + 1] = wk1r * x0i + wk1i * x0r;
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3] = wk3r * x0r - wk3i * x0i;
            a[j3 + 1] = wk3r * x0i + wk3i * x0r;
        }
        wk1r = w[k2 + 2];
        wk1i = w[k2 + 3];
        wk3r = wk1r - 2 * wk2r * wk1i;
        wk3i = 2 * wk2r * wk1r - wk1i;
        for (j = k + m; j < l + (k + m); j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = a[j + 1] + a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = a[j + 1] - a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            x0r -= x2r;
            x0i -= x2i;
            a[j2] = -wk2i * x0r - wk2r * x0i;
            a[j2 + 1] = -wk2i * x0i + wk2r * x0r;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j1] = wk1r * x0r - wk1i * x0i;
            a[j1 + 1] = wk1r * x0i + wk1i * x0r;
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3] = wk3r * x0r - wk3i * x0i;
            a[j3 + 1] = wk3r * x0i + wk3i * x0r;
        }
    }
}


// --------------------------------------------
// CLASS: Wavdata
// --------------------------------------------
// Required : iostream, fstream, string, string.h
typedef struct {
	short          wFormatTag;
	unsigned short wChannels;
	unsigned int  dwSamplesPerSec;
	unsigned int  dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBitsPerSample;
} Wavfmt;

class Wavdata {
	ifstream  ifs;
	Wavfmt    fmt;
	short     *data;
	int      dataptr; // pointer to DATA chank
	int      datasize; // size of DATA chank
	int      buffersize; // buffersize in bytes
	int	  cptr; // for BCC32's bug tellg() return wrongly	
	bool      err;
	Wavdata(const Wavdata&);
	Wavdata& operator=(const Wavdata&);
public:
	Wavdata() : data(0) {};
	~Wavdata();
	Wavdata(const char *fname){open(fname);};
	void open(const char*);
	void setframe(int);
	void read0(char* ptr, int size);
	int read(int);
	short& x(int n){return x(n, 0);}
	short& x(int n, int ch){return data[n+ch*(int)fmt.wChannels];}
	int nch() const {return (int)fmt.wChannels;}
	int nsmp() const {return datasize/sizeof(short)/(int)fmt.wChannels;}
	int nfs() const {return (int)fmt.dwSamplesPerSec;}
	bool fail() const {return err;}
	int bufsize() const {return buffersize;}
};

Wavdata::~Wavdata(){
	ifs.close();
	delete [] data;
}

void Wavdata::open(const char* fname){
	char strRIFF[5];
	char strWAVE[5];
	char strFMT[5];
	char strDATA[5];
	int riffsize, chunksize;
	string stemp;
	
	data = 0;
	err = true; // for return before end
	
	ifs.close(); // for already exist
	ifs.open(fname, ios::binary);
	if(ifs.fail()){
		cout << "Error: Can not open file:" << fname << endl;
		return;}
	else{
		cout << "File opened:" << fname << endl;	
	}
	cptr = 0;
	
	strRIFF[4] = 0;
	read0(strRIFF, 4);
	stemp = strRIFF;
	cout << "file header[" << stemp << "]" << endl;
	if(stemp.compare("RIFF") != 0){
		cout << fname << " is not RIFF format (wav) file" << endl;
		return;
	}
	read0((char*)&riffsize, 4); 
	cout << "RIFF datasize = " << datasize << "Bytes" << endl;	
	read0(strWAVE, 4);
	strWAVE[4] = 0;
	stemp = strWAVE;
	cout << "wave header[" << stemp << "]" << endl;
	if(stemp.compare("WAVE") != 0){
		cout << "Error: " << fname << " is not WAVE format file" << endl;
		return;
	}

	while(1){
		strFMT[4] = 0;
		read0(strFMT, 4);
		if(ifs.fail()){
			cout << "Error: failed to read fmt chunk" << endl;
			return;}
		stemp = strFMT;
		cout << "chunk header[" << stemp << "]" << endl;
		if(stemp.compare("fmt ") == 0){
			read0((char*)&chunksize, 4);
			read0((char*)&fmt, sizeof(Wavfmt));
			break;
		}
		else{
			read0((char*)&chunksize, 4);
			cout << "chunk read skipped " << chunksize << "Bytes" << endl;
			cptr += chunksize;
			ifs.seekg(cptr, ios::beg);
		}
	}
	cout << "chunkSize       :" << chunksize << endl;
	cout << "wFormatTag      :" << fmt.wFormatTag << endl;
	cout << "wChannels       :" << fmt.wChannels << endl;
	cout << "dwSamplesPerSec :" << fmt.dwSamplesPerSec << endl;
	cout << "dwAvgBytesPerSec:" << fmt.dwAvgBytesPerSec << endl;
	cout << "wBlockAlign     :" << fmt.wBlockAlign << endl;
	cout << "wBitsPerSample  :" << fmt.wBitsPerSample << endl;

	if(fmt.wBitsPerSample != 16){
		cout << "Error: not 16bits format" << endl;
		return;}

	while(1){
		strDATA[4] = 0;
		read0(strDATA, 4);
		if(ifs.fail()){
			cout << "Error: failed to read data chunk" << endl;
			return;}
		stemp = strDATA;
		cout << "data header[" << stemp << "]" << endl;
		if(stemp.compare("data") == 0){
			read0((char*)&datasize, 4);
			dataptr = cptr;
			break;
		}
		else{
			read0((char*)&chunksize, 4);
			cout << "chunk read skipped " << chunksize << " Bytes" << endl;
			cptr += chunksize;
			ifs.seekg(cptr, ios::beg);
		}
	}
	cout << "Find data chunk at = " << dataptr << endl;
	cout << "File opened successfully " << endl;

	err = false;
	return;
}

void Wavdata::setframe(int nframe){
	delete data;
	data = new short[nframe * fmt.wChannels];
	buffersize = sizeof(short) * nframe * fmt.wChannels;
	cout << "Read buffer created size = " << buffersize << " Bytes" << endl;
	return;
}
int Wavdata::read(int startsample){
	if(data == 0){ setframe( nsmp() );}
	ifs.seekg(dataptr + startsample * sizeof(short) * fmt.wChannels);
	ifs.read((char*)data, buffersize);
//	cout << "gcount : " << ifs.gcount() << endl; // BCC32 has bug: gcount = 47848 is maximum
	if (ifs.gcount() < buffersize){
		memset((char*)data + ifs.gcount(), 0, buffersize - ifs.gcount());
		cout << "wavdata reached tail: zero filled " << buffersize - ifs.gcount() << " Bytes" << endl;
	}
	return ifs.gcount();
}
	
void Wavdata::read0(char* ptr, int size){
	ifs.read(ptr, size);
	cptr += ifs.gcount();
}

// --------------------------------------------
// CLASS: CWavdata
// --------------------------------------------
// Required : Wavdata, Compmat
class CWavdata : public Wavdata{
	Compmat   cdata;
	int      framesize; // frame in time samples
public:
	CWavdata() : framesize(0) {};
	void setframe(int);
	Compmat& read(int);
	Compmat& read(int, int);
	Compmat& cx(){return cdata;}
};

void CWavdata::setframe(int nframe){
	framesize = nframe;
	Wavdata::setframe(nframe);
	cdata.zeros(framesize, nch());
}

Compmat& CWavdata::read(int startsample){
    Wavdata::read(startsample);
	for(int ch = 0; ch < nch(); ch++){
		for(int t = 0; t < framesize; t++){
			cdata.r(t, ch) = x(t, ch);
		}
	}
	return cdata;
}

Compmat& CWavdata::read(int startsample, int nframe){
	if(nframe != framesize){setframe(nframe);}
	return read(startsample);
}

// --------------------------------------------
// CLASS: Wavdata2
// --------------------------------------------
// Required : fstream, string
class Wavdata2 {
	string    ofname;
	ofstream  ofs;
	Wavfmt    ofmt;
	Wavdata2(const Wavdata2&);
	Wavdata2& operator=(const Wavdata2&);
public:
	Wavdata2();
	Wavdata2(const char *fname, int nfs){openw(fname, nfs);}
	void openw(const char* fname){openw(fname, 16000);}
	void openw(const char* fname, int nfs);
	int nch() const {return (int)ofmt.wChannels;}
	int nfs() const {return (int)ofmt.dwSamplesPerSec;}
	int nsmp(int);
	int nsmp();
	void fmtwrite(int, int);
	void fmtwritep(int);
	void write0(short*, int, int, int);
	void write0(short*, int, int);
};

Wavdata2::Wavdata2(){
	ofname = "";
	ofmt.wChannels = 0; // not set flag
}

void Wavdata2::openw(const char* fname, int nfs){
	ofname = fname;
	ofmt.dwSamplesPerSec = nfs;
}

void Wavdata2::fmtwrite(int Nch, int Nsmp){
	int ltmp;
	
	if(ofname == ""){return;}

	// set format tag
	ofmt.wFormatTag = 1; // PCM
	ofmt.wChannels = (unsigned short)Nch; // #channels
	ofmt.dwSamplesPerSec = (unsigned int)nfs(); // #fs
	ofmt.dwAvgBytesPerSec = (unsigned int)(nfs()*Nch*sizeof(short)); //#byte/sec
	ofmt.wBlockAlign = (unsigned short)(Nch*sizeof(short)); //block size
	ofmt.wBitsPerSample = 16; // bit/sample

	// open and write
	ofs.open(ofname.c_str(), ios::binary | ios::in | ios::app);
	ofs.seekp(0, ios_base::beg);
	ofs.write("RIFF", 4);
	ltmp = sizeof(short)* Nch * Nsmp + 36;
	ofs.write((const char*)&ltmp , 4);
	ofs.write("WAVEfmt ", 8);
	ltmp = sizeof(ofmt);
	ofs.write((const char*)&ltmp , 4);
	ofs.write((const char*)&ofmt , sizeof(ofmt));
	ofs.write("data", 4);
	ltmp = sizeof(short)* Nch * Nsmp;
	ofs.write((const char*)&ltmp , 4);	
	ofs.close();

}

void Wavdata2::write0(short* wdata, int startsample, int length, int Nch){
	if(ofname == ""){return;}
	ofs.open(ofname.c_str(), ios::binary | ios::in | ios::out);
	ofs.seekp(44+sizeof(short)*Nch*startsample, ios_base::beg);
	ofs.write((const char*)wdata, sizeof(short)*Nch*length);
	ofs.close();
}

void Wavdata2::write0(short* wdata, int startsample, int length){
	int Nch;
	Nch = ofmt.wChannels;
	if(Nch == 0){Nch = 1;}
	write0(wdata, startsample, length, Nch);
}

int Wavdata2::nsmp(int Nch) {
	int len;
	if(ofname == ""){return 0;}
	ofs.open(ofname.c_str(), ios::binary | ios::in);
	ofs.seekp(0, ios_base::end);
	len = ofs.tellp();
	ofs.close();
	len -= 44;
	len /= sizeof(short)*Nch;	
	return len;
}

int Wavdata2::nsmp() {
	int Nch;
	Nch = ofmt.wChannels;
	if(Nch == 0){Nch = 1;}
	return nsmp(Nch);
}

void Wavdata2::fmtwritep(int Nch){
	fmtwrite(Nch, nsmp(Nch));
}

// --------------------------------------------
// CLASS: CWavdata2
// --------------------------------------------
// Required : Wavdata2, Compmat
class CWavdata2 : public Wavdata2{
	short*    data;
	int      sbufsize;
public:
	CWavdata2() : data(0), sbufsize(0) {};
	~CWavdata2(){  delete [] data;}
	void setframe(int);
	void write(Compmat, int);
	short& x(int n){return data[n];}
};

void CWavdata2::setframe(int sbsize){
	if( sbufsize == sbsize){ return; }
	delete [] data;
	sbufsize = sbsize;
	data = new short[sbsize];
}

void CWavdata2::write(Compmat cmat, int startsample){
	int sbsize, s1, s2, s1t, s2t,lcnt;
	
	s1t = cmat.size1();
	s2t = cmat.size2();
	sbsize =  s1t* s2t;
	setframe(sbsize);
	lcnt = 0;
	for(s1 = 0; s1 < s1t; s1++){
		for(s2 = 0; s2 < s2t; s2++){
			data[lcnt] = (short)cmat.r(s1, s2);
			lcnt++;
		}
	}
	write0(data, startsample, s1t, s2t);
	fmtwritep(s2t);
}




// --------------------------------------------
// classses, functions for main
// --------------------------------------------

// -----------------
// CLASS: Prms
// -----------------
class Prms {
public:
	int nfft;
	int shft;
	Prms() : nfft(1024), shft(512) {}
};

// -----------------
// CLASS: HPost4
// -----------------
const int UPDATE_TABLE_SIZE = 32768;
const int UPDATE_TABLE_SBIT = 16;
const int UPDATE_TABLE_STEP = (int)1 << UPDATE_TABLE_SBIT;

class HPostv4 {
	Compmat YHistA;
	Compmat PowV;
	HPostv4(const HPostv4&);
	HPostv4& operator=(const HPostv4&);
public:
	float Lx;
	float tr;
	float N;
	float Min;
	float Step;
	float tr_inv;
	float tr_pow;
	short *UPDATE_TABLE;
	Compmat LxI;
	HPostv4() : YHistA((float)0.), UPDATE_TABLE(0) {setdefault();}
	~HPostv4(){ delete [] UPDATE_TABLE;}
	void setdefault();
	void initialize(const int&);
        void disp();
        Compmat getHist();
	Compmat execute(Compmat&);
	void resetHist() ;
};

void HPostv4::setdefault(){

//	Lx = 0.85;
//	tr = 1. - 1./(16000./512.*60);
//	tr = 1. - 1./(16000.*60); // for debugｌ setting
//	N  = 1000;
//	Min = -100;
//	Step = 0.2;
	tr_pow = 1.0e-20;
//	tr_inv = 1/tr;
//	tr_pow = FLT_MIN; // slow

}

void HPostv4::disp(){
  YHistA.disp();
}

Compmat HPostv4::getHist(){
  return YHistA;
}
  
void HPostv4::initialize(const int& NfreqNch){
	int n, m;
    void *vp;
	Compmat ctmp, mind;
	float*tpflt;
	YHistA.zeros(N, NfreqNch);
	PowV.colon(Min, Step, Min+(N-1)*Step);
	PowV = PowV.dB2Pow();
	// Make Table for update
	UPDATE_TABLE = new short[UPDATE_TABLE_SIZE+1024];
	m = 0;
	for(n = 0; n < UPDATE_TABLE_SIZE; n++){
        vp = reinterpret_cast<void*> (&m); // for erasing warning
		tpflt = reinterpret_cast<float*> (vp); // tpflt = (float*)&m;
		//if( (~m & 0x78000000) == 0){break;} // avoid Inf or Non
		if(isnan(m) | isinf(m)){cout << "error!! Inf or Non. HRLE:initialize UPDATE_TABLE" <<endl;  break;} // avoid Inf or Non
		ctmp = PowV;
		ctmp -= *tpflt;
		mind = ctmp.ind_firstplus();
		UPDATE_TABLE[n] = mind.r(0);
		m += UPDATE_TABLE_STEP;
	}
	// Initialize LxI
	LxI.zeros(1, YHistA.size2());
}

Compmat HPostv4::execute(Compmat& YP){
	Compmat LxP;
	clock_t init_time;
	LxP.zerossize(YP);
	if(YHistA.size1() != N || YHistA.size2() != YP.size() ){
		cout << "HPostv4 Initialized..." << endl;
		init_time = clock();
		initialize(YP.size());
		cout << "Initialize finished" << endl;
		init_time = clock() - init_time;
		cout << "Elapsed (s) = " << init_time/CLOCKS_PER_SEC << endl;
		}
	YHistA.TBL_addset_tr(YP.reshape(1), UPDATE_TABLE, UPDATE_TABLE_SBIT, tr_pow);
//cout<<"tr_inv="<<tr_inv<<endl;
//cout<<"LX="<<Lx<<endl;
//cout<<"N="<<N<<endl;
//cout<<"min="<<Min<<endl;
//cout<<"STEP="<<Step<<endl;
	tr_pow *= tr_inv;
	if(tr_pow > 1.0e+20){
		tr_pow *= 1.0e-40;
		YHistA *= 1.0e-40;}
	YHistA.nearest_inc( (YHistA.mid1(YHistA.end1()) * Lx), LxI); // nearest index
	LxP = PowV(LxI.reshape());
	return LxP;
}

void HPostv4::resetHist(){
	for (int n1 = 0; n1 < YHistA.size1(); n1++){
		for (int n2 = 0; n2 < YHistA.size2(); n2++){
			YHistA(n1,n2) = 0.0;
		}
	}
}



#ifdef original_code
// --------------------------------------------
// main
// --------------------------------------------
// Required : Wavdata (class)
int main(int argc, char **argv)
{
	CWavdata wdata;
	CWavdata2 wdata2;
	Compmat x, win, X, PX, LxP, G;
	Compmat Y, y, yout;
	Cfft FT;
	HPostv4 PF;
	Prms prm; // set default parameters
	int nsrc, nsmp, nfs;
	int nframe;
	int n;
	int tst, ten, tvalid;
	clock_t starttime, endtime;
	
	
	starttime = clock();
	
	cout << "argc = " << argc << endl;
	for(n = 0; n < argc; n++){
		cout << "argv[" << n << "]=" << argv[n] << endl;
	}
	if(argc < 2){
		cout << "set_target filtname" << endl;
		return 0;}

	const char	*sfile = argv[1];
	
	wdata.open(sfile); // open file for read
	if(wdata.fail()){return 1;}
	
	nsrc = wdata.nch();
	nsmp = wdata.nsmp();
	nfs  = wdata.nfs();
	cout << "[nsrc, nsmp, nfs]=[" << nsrc << ", " << nsmp << ", " << nfs << "]" << endl;
	
	nframe = 1 + (nsmp-prm.nfft)/prm.shft;
	cout << "[NFFT, SHFT, NFrame]=[" << prm.nfft << ", " << prm.shft << ", " << nframe << "]" << endl;

	win.hanning(prm.nfft);
	cout << "hanning window initialized Len = " << win.size1() << endl;

//	rename("testout.wav", "testout_bk.wav");
//  remove("testout.wav");
	wdata2.openw("testout_vf0.wav", nfs); // open file for write
	tst = 0;
	
	while(1){
		cout << "time: " << tst << endl;
		if(tst > nsmp-1){break;}
		ten = tst + prm.nfft -1;
		if(ten > nsmp-1){ten = nsmp-1;}
		tvalid = ten-tst+1;
		X = wdata.read(tst, prm.nfft);
		FT.winfft(X, win);
		
		X = X.mid1(0, prm.nfft/2-1);
		PX = X.norm2();
		LxP = PF.execute(PX);
		PX += (float)1e-20; // avoid /0 error (1e-30 is not OK)
		G = (LxP | PX)*(-1.) + 1.;
		G = G.flooring(0.);
		Y = (X & G.sqrt());

		Y *= 2; Y(0) *= 0.5;
		y = FT.ifft(Y, prm.nfft);
		yout = y + yout;
		if(tvalid < yout.size1()){yout = yout.mid1(0, tvalid-1);}
		wdata2.write(yout, tst);
		tst += prm.shft;
		yout = yout.mid1(prm.shft, prm.nfft-1);
	}

	cout << "finished" << endl;
	endtime = clock();
	cout << "Elapsed (s) = " << (float)(endtime-starttime)/CLOCKS_PER_SEC << endl;
}


//string dum;
//(Ctemp.absri()).disp();
//cin >> dum;
#endif

