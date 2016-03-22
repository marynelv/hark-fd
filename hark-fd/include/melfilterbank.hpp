/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#ifndef ___MELFILTERBANK_HPP___
#define ___MELFILTERBANK_HPP___

#include "Vector.h"

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

using namespace boost;
using namespace boost::numeric;

typedef ublas::vector<complex<float> > VecComplex;
typedef ublas::vector<float> VecReal;
typedef shared_ptr<VecComplex> VecComplexPtr;
typedef shared_ptr<VecReal> VecRealPtr;

inline
float Mel(int k, int fs, int n) {
    return 1127 * log(1 + (k - 1) * fs * (1 / 700.0f) / n);
}
float Mel(int f) {
    return 1127 * log(1 + f * (1 / 700.0f));
}

class MelFilterBank {
private:
    int fbank_count;
    vector<float> mel_center;
    vector<short> lo_index;
    vector<float> lo_weight;
    int lo;
    int hi;
    
public:
    MelFilterBank(int fb_count, int cutoff, int fs, int length) {
        int slength = length / 2 + 1;
        fbank_count = fb_count;
        lo = 2;
        hi = length / 2;
        
        mel_center.resize(fbank_count + 2);

        //float mel_hi = Mel(slength, fs, length);
        float mel_hi = Mel(cutoff);
        float mel_lo = 0;
        float mel_s = mel_hi - mel_lo;
        for (int c = 1; c < mel_center.size(); c++) {
            mel_center[c] = ((float)c / (fbank_count + 1)) * mel_s + mel_lo;
        }

        lo_index.resize(slength);
        for (int k = 1, c = 1; k < slength; k++) {
            float mel_k = Mel(k, fs, length);
            while (mel_center[c] < mel_k && c < mel_center.size()) {
                c++;
            }
            lo_index[k] = c - 1;
        }

        lo_weight.resize(slength);
        for (int k = 1; k <= hi; k++) {
            int c = lo_index[k];
            if (c > 0) {
                lo_weight[k] = (mel_center[c + 1] - Mel(k, fs, length)) /
                    (mel_center[c + 1] - mel_center[c]);
            }
            else {
                lo_weight[k] = (mel_center[1] - Mel(k, fs, length)) /
                    (mel_center[1] - mel_lo);
            }
        }
    }

    //void Calc(VecComplexPtr specptr, VecRealPtr fbankptr) {
    void Calc(Vector<float>& spec, Vector<float>& fbank) {
        VecComplex& spec = *specptr;
        VecReal& fbank = *fbankptr;

        //fbank.resize(fbank_count);
        for (VecReal::iterator fbank_it = fbank.begin(); fbank_it != fbank.end();
             ++fbank_it) {
            *fbank_it = 0.0f;
        }

        for (int k = lo; k <= hi; k++) {
            float a = abs(spec[k]);
            int bin = lo_index[k];
            float weighted_a = lo_weight[k] * a;

            //if (bin > 0) {
            if (bin > 0 && bin <= fbank_count) {
                fbank[bin - 1] += weighted_a;
            }
            if (bin < fbank_count) {
                fbank[bin] += a - weighted_a;
            }
        }

        for (int bin = 1; bin <= fbank_count; bin++) {
            fbank[bin - 1] = log(fbank[bin - 1] < 1.0 ? 1.0 : fbank[bin - 1]);
        }
    }
};

#endif
