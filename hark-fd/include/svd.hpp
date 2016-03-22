/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#define NDEBUG

#ifndef ___SVD_HPP___
#define ___SVD_HPP___

#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <complex>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>

template <typename T>
T ABS(T a)
{
  return ((a) < (T)0 ? (T)-(a) : (a) > 0 ? (T)(a) : (T)0);
}

template <typename T>
T SQR(T a)
{
  return ( a == (T)0.0 ? (T)0.0 : (T)(a * a) );
}

template <typename T>
T SIGN(T a, T b)
{
  return ((b) >= (T)0.0 ? (T)fabs(a) : (T)-fabs(a));
}

template <typename T>
T PYTHAG(T a, T b)
{
  T absa,absb;
  absa=(T)fabs(a);
  absb=(T)fabs(b);
  if (absa > absb) return absa * (T)sqrt( 1.0 + (T)SQR(absb/absa) );
  else return (absb == 0.0 ? (T)0.0 : absb * (T)sqrt( 1.0 + (T)SQR(absa/absb) ));
}

template <typename T>
void svd(boost::numeric::ublas::matrix<T>& amat, 
	 boost::numeric::ublas::vector<T>& wmat, 
	 boost::numeric::ublas::matrix<T>& umat, 
	 boost::numeric::ublas::matrix<T>& vmat)
{    

  int flag,i,its,j,jj,k,l,nm;
  T anorm,c,f,g,h,s,scale,x,y,z;

  int ma = amat.size1();
  int na = amat.size2();
  int mnmax = ( na > ma ? na : ma);
  int mnmin = ( na > ma ? ma : na);
  bool n_g = ( na > ma ? true : false);
  boost::numeric::ublas::matrix<T> a(mnmax,mnmax);
  noalias(a) = boost::numeric::ublas::zero_matrix<T>(mnmax,mnmax);
  if(n_g) {
    boost::numeric::ublas::matrix<T> t_amat(na,ma);
    noalias(t_amat) = boost::numeric::ublas::herm(amat);
    for(i=0;i<t_amat.size1();i++)
      for(j=0;j<t_amat.size2();j++)
	a(i,j) = t_amat(i,j);
  }else{
    for(i=0;i<amat.size1();i++)
      for(j=0;j<amat.size2();j++)
	a(i,j) = amat(i,j);    
  }
  boost::numeric::ublas::vector<T> w(mnmax);
  noalias(w) = boost::numeric::ublas::zero_vector<T>(mnmax);
  boost::numeric::ublas::matrix<T> u(mnmax,mnmax);
  noalias(u) = boost::numeric::ublas::zero_matrix<T>(mnmax,mnmax);
  boost::numeric::ublas::matrix<T> v(mnmax,mnmax);
  noalias(v) = boost::numeric::ublas::zero_matrix<T>(mnmax,mnmax);

  int m = a.size1();
  int n = a.size2();

  boost::numeric::ublas::vector<T> rv1(n);
	
  g=scale=anorm=0.0;

  for (i=1;i<=n;i++) {
    l=i+1;
    rv1[i-1]=scale*g;
    g=s=scale=0.0;
    if (i <= m) {
      for (k=i;k<=m;k++) scale += fabs(a(k-1,i-1));
      if (scale) {
	for (k=i;k<=m;k++) {
	  a(k-1,i-1) /= scale;
	  s += a(k-1,i-1)*a(k-1,i-1);
	}
	f=a(i-1,i-1);
	g = -SIGN((T)sqrt(s),(T)f);
	h=f*g-s;
	a(i-1,i-1)=f-g;
	for (j=l;j<=n;j++) {
	  for (s=0.0,k=i;k<=m;k++) s += a(k-1,i-1)*a(k-1,j-1);
	  f=s/h;
	  for (k=i;k<=m;k++) a(k-1,j-1) += f*a(k-1,i-1);
	}
	for (k=i;k<=m;k++) a(k-1,i-1) *= scale;
      }
    }
    w[i-1]=scale *g;
    g=s=scale=0.0;
    if (i <= m && i != n) {
      for (k=l;k<=n;k++) scale += fabs(a(i-1,k-1));
      if (scale) {
	for (k=l;k<=n;k++) {
	  a(i-1,k-1) /= scale;
	  s += a(i-1,k-1)*a(i-1,k-1);
	}
	f=a(i-1,l-1);
	g = -SIGN((T)sqrt(s),(T)f);
	h=f*g-s;
	a(i-1,l-1)=f-g;
	for (k=l;k<=n;k++) rv1[k-1]=a(i-1,k-1)/h;
	for (j=l;j<=m;j++) {
	  for (s=0.0,k=l;k<=n;k++) s += a(j-1,k-1)*a(i-1,k-1);
	  for (k=l;k<=n;k++) a(j-1,k-1) += s*rv1[k-1];
	}
	for (k=l;k<=n;k++) a(i-1,k-1) *= scale;
      }
    }
    //anorm=FMAX(anorm,(fabs(w[i-1])+fabs(rv1[i-1])));
    anorm=std::max((T)anorm,(T)(fabs(w[i-1])+fabs(rv1[i-1])));
  }

  for (i=n;i>=1;i--) {
    if (i < n) {
      if (g) {
	for (j=l;j<=n;j++)
	  v(j-1,i-1)=(a(i-1,j-1)/a(i-1,l-1))/g;
	for (j=l;j<=n;j++) {
	  for (s=0.0,k=l;k<=n;k++) s += a(i-1,k-1)*v(k-1,j-1);
	  for (k=l;k<=n;k++) v(k-1,j-1) += s*v(k-1,i-1);
	}
      }
      for (j=l;j<=n;j++) v(i-1,j-1)=v(j-1,i-1)=0.0;
    }
    v(i-1,i-1)=1.0;
    g=rv1[i-1];
    l=i;
  }

  //for (i=IMIN(m,n);i>=1;i--) {
  for (i=std::min(m,n);i>=1;i--) {
    l=i+1;
    g=w[i-1];
    for (j=l;j<=n;j++) a(i-1,j-1)=0.0;
    if (g) {
      g=1.0/g;
      for (j=l;j<=n;j++) {
	for (s=0.0,k=l;k<=m;k++) s += a(k-1,i-1)*a(k-1,j-1);
	f=(s/a(i-1,i-1))*g;
	for (k=i;k<=m;k++) a(k-1,j-1) += f*a(k-1,i-1);
      }
      for (j=i;j<=m;j++) a(j-1,i-1) *= g;
    } else for (j=i;j<=m;j++) a(j-1,i-1)=0.0;
    ++a(i-1,i-1);
  }

  for (k=n;k>=1;k--) {
    for (its=1;its<=30;its++) {
      flag=1;
      for (l=k;l>=1;l--) {
	nm=l-1;
	if ((T)(fabs(rv1[l-1])+anorm) == anorm) {
	  flag=0;
	  break;
	}
	if ((T)(fabs(w[nm-1])+anorm) == anorm) break;
      }
      if (flag) {
	c=0.0;
	s=1.0;
	for (i=l;i<=k;i++) {
	  f=s*rv1[i-1];
	  rv1[i-1]=c*rv1[i-1];
	  if ((T)(fabs(f)+anorm) == anorm) break;
	  g=w[i-1];
	  h=PYTHAG((T)f,(T)g);
	  w[i-1]=h;
	  h=1.0/h;
	  c=g*h;
	  s = -f*h;
	  for (j=1;j<=m;j++) {
	    y=a(j-1,nm-1);
	    z=a(j-1,i-1);
	    a(j-1,nm-1)=y*c+z*s;
	    a(j-1,i-1)=z*c-y*s;
	  }
	}
      }
      z=w[k-1];
      if (l == k) {
	if (z < 0.0) {
	  w[k-1] = -z;
	  for (j=1;j<=n;j++) v(j-1,k-1) = -v(j-1,k-1);
	}
	break;
      }
      if (its == 30) printf("no convergence in 30 svd iterations");
      x=w[l-1];
      nm=k-1;
      y=w[nm-1];
      g=rv1[nm-1];
      h=rv1[k-1];
      f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
      g=PYTHAG((T)f,(T)1.0);
      f=((x-z)*(x+z)+h*((y/(f+SIGN((T)g,(T)f)))-h))/x;
      c=s=1.0;
      for (j=l;j<=nm;j++) {
	i=j+1;
	g=rv1[i-1];
	y=w[i-1];
	h=s*g;
	g=c*g;
	z=PYTHAG((T)f,(T)h);
	rv1[j-1]=z;
	c=f/z;
	s=h/z;
	f=x*c+g*s;
	g = g*c-x*s;
	h=y*s;
	y *= c;
	for (jj=1;jj<=n;jj++) {
	  x=v(jj-1,j-1);
	  z=v(jj-1,i-1);
	  v(jj-1,j-1)=x*c+z*s;
	  v(jj-1,i-1)=z*c-x*s;
	}
	z=PYTHAG((T)f,(T)h);
	w[j-1]=z;
	if (z) {
	  z=1.0/z;
	  c=f*z;
	  s=h*z;
	}
	f=c*g+s*y;
	x=c*y-s*g;
	for (jj=1;jj<=m;jj++) {
	  y=a(jj-1,j-1);
	  z=a(jj-1,i-1);
	  a(jj-1,j-1)=y*c+z*s;
	  a(jj-1,i-1)=z*c-y*s;
	}
      }
      rv1[l-1]=0.0;
      rv1[k-1]=f;
      w[k-1]=x;
    }
  }

  noalias(u) = a;

  if(n_g){
    wmat.resize(mnmin);
    for(i=0;i<wmat.size();i++) 
      wmat[i] = w[i];
    vmat.resize(mnmax,mnmax);
    noalias(vmat) = u;
    umat.resize(mnmin,mnmin);
    for(i=0;i<umat.size1();i++)
      for(j=0;j<umat.size2();j++)
	umat(i,j) = v(i,j);        
  }else{
    wmat.resize(mnmin);
    for(i=0;i<wmat.size();i++) 
      wmat[i] = w[i];
    umat.resize(mnmax,mnmax);
    noalias(umat) = u;
    vmat.resize(mnmin,mnmin);
    for(i=0;i<vmat.size1();i++)
      for(j=0;j<vmat.size2();j++)
	vmat(i,j) = v(i,j);        
  }

}

template <typename T>
void svd(boost::numeric::ublas::matrix<std::complex<T> >& amat, 
	 boost::numeric::ublas::vector<T>& smat, 
	 boost::numeric::ublas::matrix<std::complex<T> >& umat, 
	 boost::numeric::ublas::matrix<std::complex<T> >& vmat)
{

  int i, j, k, k1, L, L1, nM1, np;

  int ma = amat.size1();
  int na = amat.size2();
  int mnmax = ( na > ma ? na : ma);
  int mnmin = ( na > ma ? ma : na);
  bool n_g = ( na > ma ? true : false);
  boost::numeric::ublas::matrix<std::complex<T> > a(mnmax,mnmax);
  noalias(a) = boost::numeric::ublas::zero_matrix<std::complex<T> >(mnmax,mnmax);
  if(n_g) {
    boost::numeric::ublas::matrix<std::complex<T> > t_amat(na,ma);
    noalias(t_amat) = boost::numeric::ublas::herm(amat);
    for(i=0;i<t_amat.size1();i++)
      for(j=0;j<t_amat.size2();j++)
	a(i,j) = t_amat(i,j);
  }else{
    for(i=0;i<amat.size1();i++)
      for(j=0;j<amat.size2();j++)
	a(i,j) = amat(i,j);    
  }
  boost::numeric::ublas::vector<T> s(mnmax);
  noalias(s) = boost::numeric::ublas::zero_vector<T>(mnmax);
  boost::numeric::ublas::matrix<std::complex<T> > u(mnmax,mnmax);
  noalias(u) = boost::numeric::ublas::zero_matrix<std::complex<T> >(mnmax,mnmax);
  boost::numeric::ublas::matrix<std::complex<T> > v(mnmax,mnmax);
  noalias(v) = boost::numeric::ublas::zero_matrix<std::complex<T> >(mnmax,mnmax);

  int m = a.size1();
  int n = a.size2();
  int p = 0; // transformation Conj(Tran(u)) is applied to the p vectors given in columns n, n+1, ..., n+p-1 of matrix a
  int nu = m; // considered number of cols of u
  int nv = n; // considered number of cols of v
  boost::numeric::ublas::vector<T> b(n);
  boost::numeric::ublas::vector<T> c(n);
  boost::numeric::ublas::vector<T> t(n);
  std::complex<T> q;
  std::complex<T> r;
  T cs, eps, eta, f, g, h;
  T sn;
  T tol, w, x, y, z;
  eta = 1.5E-07F;
  tol = 1.5E-31F;
  np = n + p;
  nM1 = n - 1;
  L = 0;
  /*
    HOUSEHOLDER REDUCTION
  */
  c[0] = 0.0F;
  k = 0;
  while (1)
    {
      k1 = k + 1;
      /*
	ELIMINATION OF a(i,k), i = k, ..., m-1
      */
      z = 0.0F;
      for (i = k; i < m; i++)
	z += norm(a(i,k));
      b[k] = 0.0F;
      if (z > tol)
	{
	  z = (T)sqrt(z);
	  b[k] = z;
	  w = abs(a(k,k));
	  q = std::complex<T>(1.0F, 0.0F);
	  if (w != 0.0F) q = a(k,k) / w;
	  a(k,k) = q * (z + w);
	  if (k != np - 1)
	    {
	      for (j = k1; j < np; j++)
		{
		  q = std::complex<T>(0.0F, 0.0F);
		  for (i = k; i < m; i++)
		    q += conj(a(i,k)) * a(i,j);
		  q /= z * (z + w);
		  for (i = k; i < m; i++)
		    a(i,j) -= q * a(i,k);
		}
	    }
	  /*
	    PHASE TRANSFORMATION
	  */
	  q = -conj(a(k,k)) / abs(a(k,k));
	  for (j = k1; j < np; j++)
	    a(k,j) *= q;
	}
      /*
	ELIMINATION OF a(k][j], j = k+2, ..., n-1
      */
      if (k == nM1) break;
      z = 0.0F;
      for (j = k1; j < n; j++)
	z += norm(a(k,j));
      c[k1] = 0.0F;
      if (z > tol)
	{
	  z = (T)sqrt(z);
	  c[k1] = z;
	  w = abs(a(k,k1));
	  q = std::complex<T>(1.0F, 0.0F);
	  if (w != 0.0F) q = a(k,k1) / w;
	  a(k,k1) = q * (z + w);
	  for (i = k1; i < m; i++)
	    {
	      q = std::complex<T>(0.0F, 0.0F);
	      for (j = k1; j < n; j++)
		q += conj(a(k,j)) * a(i,j);
	      q /= z * (z + w);
	      for (j = k1; j < n; j++) 
		a(i,j) -= q * a(k,j);
	    }
	  /*
	    PHASE TRANSFORMATION
	  */
	  q = -conj(a(k,k1)) / abs(a(k,k1));
	  for (i = k1; i < m; i++)
	    a(i,k1) *= q;
	}
      k = k1;
    }
  /*
    TOLERANCE FOR NEGLIGIBLE ELEMENTS
  */
  eps = 0.0F;
  for (k = 0; k < n; k++)
    {
      s[k] = b[k];
      t[k] = c[k];
      if (s[k] + t[k] > eps)
	eps = s[k] + t[k];
    }
  eps *= eta;
  /*
    INITIALIZATION OF u AND v
  */
  if (nu > 0)
    {
      for (j = 0; j < nu; j++)
	{
	  for (i = 0; i < m; i++)
	    u(i,j) = std::complex<T>(0.0F, 0.0F);
	  u(j,j) = std::complex<T>(1.0F, 0.0F);
	}
    }
  if (nv > 0)
    {
      for (j = 0; j < nv; j++)
	{
	  for (i = 0; i < n; i++)
	    v(i,j) = std::complex<T>(0.0F, 0.0F);
	  v(j,j) = std::complex<T>(1.0F, 0.0F);
	}
    }
  /*
    QR DIAGONALIZATION
  */
  for (k = nM1; k >= 0; k--)
    {
      /*
	TEST FOR SPLIT
      */
      while (1)
	{
	  for (L = k; L >= 0; L--)
	    {
	      if (ABS(t[L]) <= eps) goto Test;
	      if (ABS(s[L - 1]) <= eps) break;
	    }
	  /*
	    CANCELLATION OF E(L)
	  */
	  cs = 0.0F;
	  sn = 1.0F;
	  L1 = L - 1;
	  for (i = L; i <= k; i++)
	    {
	      f = sn * t[i];
	      t[i] *= cs;
	      if (ABS(f) <= eps) goto Test;
	      h = s[i];
	      w = (T)sqrt(f * f + h * h);
	      s[i] = w;
	      cs = h / w;
	      sn = -f / w;
	      if (nu > 0)
		{
		  for (j = 0; j < n; j++)
		    {
		      x = real(u(j,L1));
		      y = real(u(j,i));
		      u(j,L1) = std::complex<T>(x * cs + y * sn, 0.0F);
		      u(j,i) = std::complex<T>(y * cs - x * sn, 0.0F);
		    }
		}
	      if (np == n) continue;
	      for (j = n; j < np; j++)
		{
		  q = a(L1,j);
		  r = a(i,j);
		  a(L1,j) = q * cs + r * sn;
		  a(i,j) = r * cs - q * sn;
		}
	    }
	  /*
	    TEST FOR CONVERGENCE
	  */
	Test:	w = s[k];
	  if (L == k) break;
	  /*
	    ORIGIN SHIFT
	  */
	  x = s[L];
	  y = s[k - 1];
	  g = t[k - 1];
	  h = t[k];
	  f = ((y - w) * (y + w) + (g - h) * (g + h)) / (2.0F * h * y);
	  g = (T)sqrt(f * f + 1.0F);
	  if (f < 0.0F) g = -g;
	  f = ((x - w) * (x + w) + (y / (f + g) - h) * h) / x;
	  /*
	    QR STEP
	  */
	  cs = 1.0F;
	  sn = 1.0F;
	  L1 = L + 1;
	  for (i = L1; i <= k; i++)
	    {
	      g = t[i];
	      y = s[i];
	      h = sn * g;
	      g = cs * g;
	      w = (T)sqrt(h * h + f * f);
	      t[i - 1] = w;
	      cs = f / w;
	      sn = h / w;
	      f = x * cs + g * sn;
	      g = g * cs - x * sn;
	      h = y * sn;
	      y = y * cs;
	      if (nv > 0)
		{
		  for (j = 0; j < n; j++)
		    {
		      x = real(v(j,i - 1));
		      w = real(v(j,i));
		      v(j,i - 1) = std::complex<T>(x * cs + w * sn, 0.0F);
		      v(j,i) = std::complex<T>(w * cs - x * sn, 0.0F);
		    }
		}
	      w = (T)sqrt(h * h + f * f);
	      s[i - 1] = w;
	      cs = f / w;
	      sn = h / w;
	      f = cs * g + sn * y;
	      x = cs * y - sn * g;
	      if (nu > 0)
		{
		  for (j = 0; j < n; j++)
		    {
		      y = real(u(j,i - 1));
		      w = real(u(j,i));
		      u(j,i - 1) = std::complex<T>(y * cs + w * sn, 0.0F);
		      u(j,i) = std::complex<T>(w * cs - y * sn, 0.0F);
		    }
		}
	      if (n == np) continue;
	      for (j = n; j < np; j++)
		{
		  q = a(i - 1,j);
		  r = a(i,j);
		  a(i - 1,j) = q * cs + r * sn;
		  a(i,j) = r * cs - q * sn;
		}
	    }
	  t[L] = 0.0F;
	  t[k] = f;
	  s[k] = x;
	}
      /*
	CONVERGENCE
      */
      if (w >= 0.0F) continue;
      s[k] = -w;
      if (nv == 0) continue;
      for (j = 0; j < n; j++)
	v(j,k) = -v(j,k);
    }
  /*
    SORT SINGULAR VALUES
  */
  for (k = 0; k < n; k++)	/* sort descending */
    {
      g = -1.0F;
      j = k;
      for (i = k; i < n; i++)	/* sort descending */
	{
	  if (s[i] <= g) continue;
	  g = s[i];
	  j = i;
	}
      if (j == k) continue;
      s[j] = s[k];
      s[k] = g;
      if (nv > 0)
	{
	  for (i = 0; i < n; i++)
	    {
	      q = v(i,j);
	      v(i,j) = v(i,k);
	      v(i,k) = q;
	    }
	}
      if (nu > 0)
	{
	  for (i = 0; i < n; i++)
	    {
	      q = u(i,j);
	      u(i,j) = u(i,k);
	      u(i,k) = q;
	    }
	}
      if (n == np) continue;
      for (i = n; i < np; i++)
	{
	  q = a(j,i);
	  a(j,i) = a(k,i);
	  a(k,i) = q;
	}
    }
  /*
    BACK TRANSFORMATION
  */
  if (nu > 0)
    {
      for (k = nM1; k >= 0; k--)
	{
	  if (b[k] == 0.0F) continue;
	  q = -a(k,k) / abs(a(k,k));
	  for (j = 0; j < nu; j++)
	    u(k,j) *= q;
	  for (j = 0; j < nu; j++)
	    {
	      q = std::complex<T>(0.0F, 0.0F);
	      for (i = k; i < m; i++)
		q += conj(a(i,k)) * u(i,j);
	      q /= abs(a(k,k)) * b[k];
	      for (i = k; i < m; i++)
		u(i,j) -= q * a(i,k);
	    }
	}
    }
  if (nv > 0 && n > 1)
    {
      for (k = n - 2; k >= 0; k--)
	{
	  k1 = k + 1;
	  if (c[k1] == 0.0F) continue;
	  q = -conj(a(k,k1)) / abs(a(k,k1));
	  for (j = 0; j < nv; j++)
	    v(k1,j) *= q;
	  for (j = 0; j < nv; j++)
	    {
	      q = std::complex<T>(0.0F, 0.0F);
	      for (i = k1; i < n; i++)
		q += a(k,i) * v(i,j);
	      q = q / (abs(a(k,k1)) * c[k1]);
	      for (i = k1; i < n; i++)
		v(i,j) -= q * conj(a(k,i));
	    }
	}
    }

  if(n_g){
    smat.resize(mnmin);
    for(i=0;i<smat.size();i++) 
      smat[i] = s[i];
    vmat.resize(mnmax,mnmax);
    noalias(vmat) = u;
    umat.resize(mnmin,mnmin);
    for(i=0;i<umat.size1();i++)
      for(j=0;j<umat.size2();j++)
	umat(i,j) = v(i,j);        
  }else{
    smat.resize(mnmin);
    for(i=0;i<smat.size();i++) 
      smat[i] = s[i];
    umat.resize(mnmax,mnmax);
    noalias(umat) = u;
    vmat.resize(mnmin,mnmin);
    for(i=0;i<vmat.size1();i++)
      for(j=0;j<vmat.size2();j++)
	vmat(i,j) = v(i,j);        
  }

}

#endif

