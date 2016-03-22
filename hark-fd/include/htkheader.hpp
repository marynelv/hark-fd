#ifndef ___HTKHEADER_HPP___
#define ___HTKHEADER_HPP___

#include <iostream>
#include <fstream>

using namespace std;

#include "endian.hpp"

const unsigned HTK_USER = 9;
const unsigned HTK_MFCC_E_D = 326;
const unsigned HTK_MFCC_E_D_Z = 326 + 2048;

struct HTKHeader {
    unsigned nSamples;              // number of samples
    unsigned Period;                // period [100ns]
    unsigned short BytesPerSample;
    unsigned short FeatureType;     // if you use a user-defined type, set to HTK_USER.

public:
    void read(istream& in)
    {
     	in.read((char*)&nSamples, sizeof(nSamples));
        in.read((char*)&Period, sizeof(Period));
        in.read((char*)&BytesPerSample, sizeof(BytesPerSample));
        in.read((char*)&FeatureType, sizeof(FeatureType));
        
        nSamples = swap_endian(nSamples);
        Period = swap_endian(Period);
        BytesPerSample = swap_endian(BytesPerSample);
        FeatureType = swap_endian(FeatureType);   
    }

    void write(ostream& out) const
    {
        HTKHeader h;
        h.nSamples = swap_endian(nSamples);
        h.Period = swap_endian(Period);
        h.BytesPerSample = swap_endian(BytesPerSample);
        h.FeatureType = swap_endian(FeatureType);

        out.write((char*)&h.nSamples, sizeof(h.nSamples));
        out.write((char*)&h.Period, sizeof(h.Period));
        out.write((char*)&h.BytesPerSample, sizeof(h.BytesPerSample));
        out.write((char*)&h.FeatureType, sizeof(h.FeatureType));
    }
};

#endif
