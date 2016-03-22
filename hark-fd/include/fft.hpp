/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#ifndef ___FFT_HPP___
#define ___FFT_HPP___

#include <complex>
#include <vector>
#include <cmath>

#include <boost/shared_array.hpp>
#include <boost/type_traits.hpp>
#include <boost/numeric/ublas/vector.hpp>

static const double pi = 3.14159265358979323846;
static const double two_pi = 2.0 * pi;

template <typename T>
class fft {
private:
    int m_tlength;
    int m_flength;
    std::vector<std::complex<T> > w;

public:
    std::vector<std::complex<T> > a;
    std::vector<std::complex<T> > A;

public:
    fft() : m_tlength(0), m_flength(0) {}
    fft(int n) {
        initialize(n);
    }

    int tlength() { return m_tlength; }
    int flength() { return m_flength; }

    void initialize(int n) {
        m_tlength = n;
        m_flength = n;
        w.resize(m_tlength);
        a.resize(m_tlength);
        A.resize(m_flength);
        for (int i = 0; i < n; i++) {
            w[i] = std::complex<T>(std::cos(two_pi * i / n),
                                   -std::sin(two_pi * i / n));
        }
    }

    //void analyze(std::complex<T>* a) {
    void analyze() {
        int m, mh, i, j, k, irev, prev_irev;
        std::complex<T> y;
        //FloatType theta = 2.0 * M_PI / n;
        const int n = m_tlength;

        std::copy(a.begin(), a.end(), A.begin());
        i = 0;
        for (j = 1; j < n - 1; j++) {
            for (k = n >> 1; k > (i ^= k); k >>= 1);
            if (j < i) {
                std::swap(A[i], A[j]);
            }
        }
        for (mh = 1; (m = mh << 1) <= n; mh = m) {
            irev = 0;
            for (i = 0; i < n; i += m) {
                prev_irev = irev;
                for (k = n >> 2; k > (irev ^= k); k >>= 1);
                for (j = i; j < mh + i; j++) {
                    k = j + mh;
                    y = A[j] - A[k];
                    A[j] += A[k];
                    A[k] = w[prev_irev] * y;
                }
            }
        }
    }

    void synthesize() {
        int m, mh, i, j, k, irev, prev_irev;
        std::complex<T> y;
        //FloatType theta = 2.0 * M_PI / n;
        const int n = m_tlength;

        std::copy(A.begin(), A.end(), a.begin());
        i = 0;
        for (j = 1; j < n - 1; j++) {
            for (k = n >> 1; k > (i ^= k); k >>= 1);
            if (j < i) {
                std::swap(a[i], a[j]);
            }
        }
        for (mh = 1; (m = mh << 1) <= n; mh = m) {
            irev = 0;
            for (i = 0; i < n; i += m) {
                prev_irev = irev;
                for (k = n >> 2; k > (irev ^= k); k >>= 1);
                for (j = i; j < mh + i; j++) {
                    k = j + mh;
                    y = a[j] - a[k];
                    a[j] += a[k];
                    a[k] = std::conj(w[prev_irev]) * y;
                }
            }
        }
    }
};

template <typename T>
class rfft {
private:
    fft<T> m_fft;
    int m_tlength;
    int m_flength;
    std::vector<std::complex<T> > w;
    
public:
    std::vector<T> a;
    std::vector<std::complex<T> > A;

public:
    rfft() : m_tlength(0), m_flength(0) {}
    rfft(int n) {
        initialize(n);
    }

    int tlength() { return m_tlength; }
    int flength() { return m_flength; }

    void initialize(int n) {
        m_tlength = n;
        m_flength = n / 2 + 1;
        m_fft.initialize(n / 2);
        w.resize(m_tlength);
        a.resize(m_tlength);
        A.resize(m_flength);        

        for (int i = 0; i < n; i++) {
            w[i] = std::complex<T>(std::cos(two_pi * i / n),
                                   -std::sin(two_pi * i / n));
        }
    }

    void analyze() {
        const int n = m_tlength;
        const int n_2 = n / 2;
        //memcpy(&fft.x[0], &x[0], fft.tlength());
        for (int i = 0; i < n_2; i++) {
            m_fft.a[i] = std::complex<T>(a[i * 2], a[i * 2 + 1]);
        }
        m_fft.analyze();
        for (int k = 1; k < n / 4; k++) {
            std::complex<T> y = (T)0.5 *
                std::complex<T>(1.0 - w[k].imag(), w[k].real()) *
                (m_fft.A[k] - conj(m_fft.A[n_2 - k]));
            A[k] = m_fft.A[k] - y;
            A[n_2 - k] = conj(conj(m_fft.A[n_2 - k]) + y);
        }
        A[0] = m_fft.A[0].real() + m_fft.A[0].imag();
        A[n_2] = m_fft.A[0].real() - m_fft.A[0].imag();
        A[n / 4] = conj(m_fft.A[n / 4]);
    }

    void synthesize() {
        const int n = m_tlength;
        const int n_2 = n / 2;

        m_fft.A[0] = (T)0.5 * std::complex<T>((A[0].real() + A[n_2].real()),
                                            (A[0].real() - A[n_2].real()));
        m_fft.A[n / 4] = conj(A[n / 4]);
        for (int k = 1; k < n / 4; k++) {
            std::complex<T> y = (T)0.5 *
                std::complex<T>(1.0 + w[k].imag(), w[k].real()) *
                (A[k] - conj(A[n_2 - k]));
            m_fft.A[k] = conj(A[n_2 - k]) + y;
            m_fft.A[n_2 - k] = conj(A[k] - y);
        }
        m_fft.synthesize();
        for (int i = 0; i < n_2; i++) {
            a[i * 2] = m_fft.a[i].real() * 2;
            a[i * 2 + 1] = m_fft.a[i].imag() * 2;
        }
    }
};

template <typename T>
void Hanning(boost::numeric::ublas::vector<T>& w)
{
    float c = 2.0 * M_PI / (w.size() - 1);
    for (int n = 0; n < w.size(); n++) {
        w[n] = 0.5 - 0.5 * cos(c * n);
    }
}

template <typename T>
void Hamming(boost::numeric::ublas::vector<T>& w)
{
    float c = 2.0 * M_PI / (w.size() - 1);
    for (int n = 0; n < w.size(); n++) {
        w[n] = 0.54 - 0.46 * cos(c * n);
    }
}

template <typename T>
void ConjWindow(boost::numeric::ublas::vector<T>& w)
{
    for (int i = 0; i < w.size(); i++) {
        T x = 4 * i / (T)w.size();
        int inv = 0;

        if (x < 1) {
        }
        else if (x < 2) {
            x = 2 - x;
            inv = 1;
        }
        else if (x < 3) {
            x = x - 2;
            inv = 1;
        }
        else {
            x = 4 - x;
        }
        x *= 1.9979;
        w[i] = (0.5 - 0.5 * cos(x)) * (0.5 - 0.5 * cos(x));
        if (inv) {
            w[i] = 1 - w[i];
        }
        w[i] = sqrt(w[i]);
    }
}

template <class FloatType>
int calc_trigonometric_func_table(int n, std::complex<FloatType>* w)
{
    FloatType theta = 2.0 * M_PI;

    for (int i = 0; i < n; i++) {
        w[i] = std::complex<FloatType>(std::cos(theta * i / n),
                                       -std::sin(theta * i / n));
    }
}

#if 0
template <class FloatType>
int fft(int n, std::complex<FloatType>* a, const std::complex<FloatType>* w)
{
    int m, mh, i, j, k, irev, prev_irev;
    std::complex<FloatType> y;
    //FloatType theta = 2.0 * M_PI / n;

    i = 0;
    for (j = 1; j < n - 1; j++) {
        for (k = n >> 1; k > (i ^= k); k >>= 1);
        if (j < i) {
            std::swap(a[i], a[j]);
        }
    }
    for (mh = 1; (m = mh << 1) <= n; mh = m) {
        irev = 0;
        for (i = 0; i < n; i += m) {
            prev_irev = irev;
            for (k = n >> 2; k > (irev ^= k); k >>= 1);
            for (j = i; j < mh + i; j++) {
                k = j + mh;
                y = a[j] - a[k];
                a[j] += a[k];
                a[k] = w[prev_irev] * y;
            }
        }
    }
}
#endif

#endif
