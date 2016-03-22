/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#ifndef ___SPLINE_HPP___
#define ___SPLINE_HPP___

#include <math.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

template <typename T>
T cubic_spline(const boost::numeric::ublas::vector<T>& x, 
	       const boost::numeric::ublas::vector<T>& y, 
	       const T x1) 
{
  int n = x.size()-1;
  if(x.size()!=y.size()) { std::cerr << "cubic_spline : Vector size not matched" << std::cout; exit(0); }
  if((x[0] > x1)||(x[n] < x1)) { std::cerr << "cubic_spline : Interpolated point is out of range" << std::cout; exit(0); }
  
  boost::numeric::ublas::vector<T> h(n);
  boost::numeric::ublas::vector<T> b(n);
  boost::numeric::ublas::vector<T> d(n);
  boost::numeric::ublas::vector<T> g(n);
  boost::numeric::ublas::vector<T> u(n);
  boost::numeric::ublas::vector<T> r(n+1);
 
  int i = -1, i1, k;
  T y1, qi, si, xx;

  for (i1 = 1; i1 < n && i < 0; i1++) {
    if (x1 < x[i1])
      i = i1 - 1;
  }
  if (i < 0) i = n - 1;

  for (i1 = 0; i1 < n; i1++) 
    h[i1] = x[i1+1] - x[i1];
  for (i1 = 1; i1 < n; i1++) {
    b[i1] = 2.0 * (h[i1] + h[i1-1]);
    d[i1] = 3.0 * ((y[i1+1] - y[i1]) / h[i1] - (y[i1] - y[i1-1]) / h[i1-1]);
  }

  g[1] = h[1] / b[1];
  for (i1 = 2; i1 < n-1; i1++)
    g[i1] = h[i1] / (b[i1] - h[i1-1] * g[i1-1]);
  u[1] = d[1] / b[1];
  for (i1 = 2; i1 < n; i1++)
    u[i1] = (d[i1] - h[i1-1] * u[i1-1]) / (b[i1] - h[i1-1] * g[i1-1]);

  k      = (i > 1) ? i : 1;
  r[0]   = 0.0;
  r[n]   = 0.0;
  r[n-1] = u[n-1];
  for (i1 = n-2; i1 >= k; i1--)
    r[i1] = u[i1] - g[i1] * r[i1+1];

  xx = x1 - x[i];
  qi = (y[i+1] - y[i]) / h[i] - h[i] * (r[i+1] + 2.0 * r[i]) / 3.0;
  si = (r[i+1] - r[i]) / (3.0 * h[i]);
  y1 = y[i] + xx * (qi + xx * (r[i]  + si * xx));
  
  return y1;
	
}

template <typename T>
boost::numeric::ublas::vector<T> cubic_spline(const boost::numeric::ublas::vector<T>& x, 
					      const boost::numeric::ublas::vector<T>& y, 
					      const boost::numeric::ublas::vector<T>& x1) 
{
  boost::numeric::ublas::vector<T> y1(x1.size());
  for(unsigned int cnt = 0; cnt < x1.size(); cnt++)
    y1[cnt] = cubic_spline(x,y,x1[cnt]);
  return y1;
}

#endif

