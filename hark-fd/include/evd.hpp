/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#ifndef ___EVD_HPP___
#define ___EVD_HPP___

#include <cmath>
#include <complex>

#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>

template <typename T>
void rotate(boost::numeric::ublas::matrix<T>& a, int i, int j, int k, int l,
            T c, T s, T tau)
{
  T g = a(i, j);
  T h = a(k, l);
  a(i, j) = g - s * (h + g * tau);
  a(k, l) = h + s * (g - h * tau);
}

// Eigenvalue decomposition for real symmetric matrix
template <typename T>
void evd(boost::numeric::ublas::matrix<T>& a,
         boost::numeric::ublas::vector<T>& d,
         boost::numeric::ublas::matrix<T>& v)
{
  using namespace boost::numeric;
  typedef boost::numeric::ublas::matrix<T> matrix;
  typedef boost::numeric::ublas::vector<T> vector;
  static vector b;
  static vector z;
  const int n = a.size1();
  T sm;
  T thresh;
  T g;
  T h;
  T t;
  T c;
  T s;
  T tau;
  T theta;

  b.resize(n);
  z.resize(n);
    
  v = ublas::identity_matrix<T>(n);

  for (int ip = 0; ip < n; ip++) {
    b[ip] = d[ip] = a(ip, ip);
    z[ip] = 0;
  }

  for (int i = 0; i < 50; i++) {
    sm = 0.0;
    for (int ip = 0; ip < n - 1; ip++) {
      for (int iq = ip + 1; iq < n; iq++) {
        sm += std::abs(a(ip, iq));
      }
    }
    if (sm == 0.0) {
      return;
    }
    if (i < 4) {
      thresh = 0.2 * sm / (n * n);
    }
    else {
      thresh = 0.0;
    }

    for (int ip = 0; ip < n - 1; ip++) {
      for (int iq = ip + 1; iq < n; iq++) {
        g = 100.0 * abs(a(ip, iq));
        if (i > 4 && abs(d[ip] + g) == abs(d[ip]) &&
            (abs(d[iq]) + g) == abs(d[iq])) {
          a(ip, iq) = 0.0;
        }
        else if (abs(a(ip, iq)) > thresh) {
          h = d[iq] - d[ip];
          if ((abs(h) + g) == abs(h)) {
            t = a(ip, iq) / h;
          }
          else {
            theta = 0.5 * h / a(ip, iq);
            t = 1.0 / (abs(theta) + sqrt(1.0 + theta * theta));
            if (theta < 0.0) {
              t = -t;
            }
          }
          c = 1.0 / sqrt(1 + t * t);
          s = t * c;
          tau = s / (1.0 + c);
          h = t * a(ip, iq);
          z[ip] -= h;
          z[iq] += h;
          d[ip] -= h;
          d[iq] += h;
          a(ip, iq) = 0.0;
          for (int j = 0; j <= ip - 1; j++) {
            rotate(a, j, ip, j, iq, c, s, tau);
          }
          for (int j = ip + 1; j <= iq - 1; j++) {
            rotate(a, ip, j, j, iq, c, s, tau);
          }
          for (int j = iq + 1; j < n; j++) {
            rotate(a, ip, j, iq, j, c, s, tau);
          }
          for (int j = 0; j < n; j++) {
            rotate(v, j, ip, j, iq, c, s, tau);
          }
        }
      }
    }
    for (int ip = 0; ip < n; ip++) {
      b[ip] += z[ip];
      d[ip] = b[ip];
      z[ip] = 0.0;
    }
  }
}

// Eigenvalue decomposition for Hermitian matrix
template <typename T>
void evd(boost::numeric::ublas::matrix<std::complex<T> >& a,
         boost::numeric::ublas::vector<T>& d,
         boost::numeric::ublas::matrix<std::complex<T> >& v)
{
  using namespace boost::numeric;
  typedef boost::numeric::ublas::matrix<std::complex<T> > matrix;
  typedef boost::numeric::ublas::vector<T> vector;
  static vector b;
  static vector z;
  const int n = a.size1();
  T sm;
  T thresh;
  T g;
  std::complex<T> h;
  std::complex<T> t;
  T c;
  std::complex<T> s;
  std::complex<T> tau;
  std::complex<T> theta;

  b.resize(n);
  z.resize(n);
    
  v = ublas::identity_matrix<std::complex<T> >(n);

  for (int ip = 0; ip < n; ip++) {
    b[ip] = d[ip] = a(ip, ip).real();
    z[ip] = 0;
  }

  for (int i = 0; i < 50; i++) {
    sm = 0.0;
    for (int ip = 0; ip < n - 1; ip++) {
      for (int iq = ip + 1; iq < n; iq++) {
        sm += std::abs(a(ip, iq));
      }
    }
    if (sm == 0.0) {
      return;
    }
    if (i < 4) {
      thresh = 0.2 * sm / (n * n);
    }
    else {
      thresh = 0.0;
    }

    for (int ip = 0; ip < n - 1; ip++) {
      for (int iq = ip + 1; iq < n; iq++) {
        g = 100.0 * std::abs(a(ip, iq));
        if (i > 4 && std::abs(d[ip] + g) == std::abs(d[ip]) &&
            (std::abs(d[iq]) + g) == std::abs(d[iq])) {
          a(ip, iq) = 0.0;
        }
        else if (std::abs(a(ip, iq)) > thresh) {
          h = d[iq] - d[ip];
          if ((std::abs(h) + g) == std::abs(h)) {
            //t = a(ip, iq) / h;
            t = conj(a(ip, iq)) / h;
          }
          else {
            //theta = T(0.5) * h / a(ip, iq);
            //T abs_theta = abs(theta);
            //t = 1.0 / (abs(theta) + sqrt(1.0 + theta * theta));
            //t = abs(a(ip, iq)) / a(ip, iq) /
            //    (abs_theta + sqrt(T(1.0) + abs_theta * abs_theta));
            //T norm_a_pq = abs(a(ip, iq)) * abs(a(ip, iq));//norm(a(ip, iq));
            //T norm_a_pq = norm(a(ip, iq));
            //t = T(2.0) * conj(a(ip, iq)) / (h.real() - sqrt(h.real() * h.real() + T(4.0) * norm_a_pq));
            t = (-h.real() + std::sqrt(h.real() * h.real() +
                                       T(4.0) * std::norm(a(ip, iq))))
              / (T(2.0) * a(ip, iq));
            //if (theta < 0.0) {
            //    t = -t;
            //}
          }
          //T abs_t = std::abs(t);
          //c = 1.0 / sqrt(1 + t * t);
          c = 1.0 / sqrt(1 + norm(t));
          s = t * c;
          tau = s / (T)(1.0 + c);
          h = t * a(ip, iq);
          z[ip] -= h.real();
          z[iq] += h.real();
          d[ip] -= h.real();
          d[iq] += h.real();
          a(ip, iq) = 0.0;
          for (int j = 0; j <= ip - 1; j++) {
            //rotate(a, j, ip, j, iq, c, s, tau);
            std::complex<T> g = a(j, ip);
            std::complex<T> h = a(j, iq);
            a(j, ip) = g - s * (h + g * conj(tau));
            a(j, iq) = h + conj(s) * (g - h * tau);
          }
          for (int j = ip + 1; j <= iq - 1; j++) {
            //rotate(a, ip, j, j, iq, c, s, tau);
            std::complex<T> g = a(ip, j);
            std::complex<T> h = a(j, iq);
            a(ip, j) = g - conj(s) * (conj(h) + g * tau);
            a(j, iq) = h + conj(s) * (conj(g) - h * tau);
          }
          for (int j = iq + 1; j < n; j++) {
            //rotate(a, ip, j, iq, j, c, s, tau);
            std::complex<T> g = a(ip, j);
            std::complex<T> h = a(iq, j);
            a(ip, j) = g - conj(s) * (h + g * tau);
            a(iq, j) = h + s * (g - h * conj(tau));
          }
          for (int j = 0; j < n; j++) {
            //rotate(v, j, ip, j, iq, c, s, tau);
            std::complex<T> g = v(j, ip);
            std::complex<T> h = v(j, iq);
            v(j, ip) = g - s * (h + g * conj(tau));
            v(j, iq) = h + conj(s) * (g - h * tau);
          }
        }
      }
    }
    for (int ip = 0; ip < n; ip++) {
      b[ip] += z[ip];
      d[ip] = b[ip];
      z[ip] = 0.0;
    }
  }

  // sorting eigenvalues in ascending order
  for (int i = 0; i < n - 1; i++) {
    T min = d[i];
    int imin = i;
    for (int j = i + 1; j < n; j++) {
      if (min > d[j]) {
        min = d[j];
        imin = j;
      }
    }
    if (imin != i) {
      std::swap(d[i], d[imin]);
      ublas::column(v, i).swap(ublas::column(v, imin));
    }
  }
}

// power_method
template <typename T>
void evd_pm(boost::numeric::ublas::matrix<std::complex<T> >& A,
            boost::numeric::ublas::vector<T>& w,
            boost::numeric::ublas::matrix<std::complex<T> >& old_v,
            boost::numeric::ublas::matrix<std::complex<T> >& new_v,
            int num_eig)
{
  using namespace boost::numeric;
  const int N = w.size();
  const T eps = 1e-6;
  //int num_eig = 2;
  ublas::vector<std::complex<T> > x0(N);
  ublas::vector<std::complex<T> > x1(N);
  std::complex<T> l = 0.0;
  std::complex<T> last_l = 0.0;
  for (int n = 0; n < num_eig; n++) {
    //ublas::matrix_column<ublas::matrix<std::complex<T> > > x1(new_v, n);
    //x0 = one;
    x0 = column(old_v, n);
    while (1) {
      //count++;
      noalias(x1) = prod(A, x0);
      //l = inner_prod(conj(x1), x0) / inner_prod(conj(x1), x1);
      l = inner_prod(conj(x1), x0);
      if (std::abs(last_l.real() - l.real()) < eps) {
        // x1 / norm_2(x1) を固有ベクトルとして返す方が望ましいが，x0でもほとんど同じ
        //column(new_v, n) = x1 / norm_2(x1);
        column(new_v, n) = x0;
        break;
      }
      noalias(x0) = x1 / norm_2(x1);
      last_l = l;
    }
    w[n] = l.real();
    if (n != num_eig - 1) {
      noalias(A) -= l * outer_prod(x0, conj(x0));
    }
  }
}

template <typename T>
void evd_pm_acc(boost::numeric::ublas::matrix<std::complex<T> >& A,
                boost::numeric::ublas::vector<T>& w,
                boost::numeric::ublas::matrix<std::complex<T> >& old_v,
                boost::numeric::ublas::matrix<std::complex<T> >& new_v,
                int num_eig)
{
  using namespace boost::numeric;
  const int N = w.size();
  const T eps = 1e-6;
  //int num_eig = 2;
  //static ublas::vector<std::complex<T> > one;
  ublas::vector<std::complex<T> > x0(N);
  ublas::vector<std::complex<T> > x1(N);
  ublas::vector<std::complex<T> > x2(N);
  ublas::vector<std::complex<T> > dx0(N);
  std::complex<T> gamma = 0.0;
  std::complex<T> l = 0.0;
  std::complex<T> last_l = 0.0;
  //x0 = x1 = one;
  //x1 = one;
  for (int n = 0; n < num_eig; n++) {
    x1 = column(old_v, old_v.size2() - 1 - n);
    //x0 = x1 = one;
    //l = 0.0;
    while (1) {
      noalias(x2) = prod(A, x1);
      l = inner_prod(conj(x2), x1);
      if (std::abs(last_l.real() - l.real()) < eps) {
        // x2 / norm_2(x2) を固有ベクトルとして返す方が望ましいが，x1でもほとんど同じ
        //column(new_v, n) = x2 / norm_2(x2);
        column(new_v, new_v.size2() - 1 - n) = x1;
        break;
      }
      x0 = x1;
      //x2 /= norm_2(x2);
      //x1 = x2;
      noalias(x1) = x2 / norm_2(x2);
      last_l = l;

      noalias(x2) = prod(A, x1);
      l = inner_prod(conj(x2), x1);
      if (std::abs(last_l.real() - l.real()) < eps) {
        // x2 / norm_2(x2) を固有ベクトルとして返す方が望ましいが，x1でもほとんど同じ
        //column(new_v, n) = x2 / norm_2(x2);
        column(new_v, new_v.size2() - 1 - n) = x1;
        break;
      }
      x2 /= norm_2(x2);
      dx0 = x1 - x0;
      gamma = inner_prod(conj(x2 - x1), dx0) / inner_prod(conj(dx0), dx0);
      x0 = x1;
      x1 = (gamma * x1 - x2) / (gamma - (T)1.0);
      x1 /= norm_2(x1);
      last_l = l;
    }
    w[w.size() - 1 - n] = l.real();
    if (n != num_eig - 1) {
      noalias(A) -= l * outer_prod(x0, conj(x0));
    }
  }
}


#endif
