#ifndef __BAYES_UTILITIES__
#define __BAYES_UTILITIES__

#include <iostream>
#include <cmath>

#include <boost/cstdint.hpp>

static const double log_pi = 1.144729885849;
static const double log_2 = 0.693147180599;
static const double inv_sqrt_2 = 0.7071067811865;
static const double log_2pi = 1.837877066409;

#define FAST_MATH // for approximate log, exp, digamma, lgamma

namespace Bayes{
    // std::vector, boost::ublas::vector versions
    template <typename Vec, typename D> D log_sum(const Vec& vec){
        size_t N = vec.size();
        D sum = vec[0];
        for(size_t n = 1; n < N; ++n){
            D vec_n = vec[n];
            if(sum < vec_n){
                D tmp = vec[n];
                vec_n = sum;
                sum = tmp;
            }
            // add vec_n to sum, where sum > vec_n
            if(sum - vec_n > 50.0)
                continue;
            sum += std::log(std::exp(vec_n - sum) + 1.0);
        }

        return sum;
    } 

    /*
     * fast log
     */
    inline float log2(float x){
    	union {float f; uint32_t i;} vx = {x};
    	union {uint32_t i; float f;} mx = { (vx.i & 0x007FFFFF) | (0x7e << 23) };
		float y = vx.i;
		y *= 1.0 / (1<<23);

		return y - 124.22544637f - 1.498030302f * mx.f - 1.72587999f / (0.3520887068f + mx.f);
    }

    inline double log2(double x){
    	union {double d; uint64_t i;} vx = {x};
    	union {uint64_t i; double d;} mx = { (vx.i & uint64_t(0x000FFFFFFFFFFFFF)) | uint64_t(0x3Fe0000000000000) };
    	//union {uint64_t i; double d;} mx = { (vx.i & uint64_t(4503599627370495)) | (0x3Fe << 52) };
    	double y = vx.i;
    	y *= 1.0 / uint64_t(0x10000000000000);

    	return y - 1020.22544637 - 1.498030302 * mx.d - 1.72587999 / (0.3520887068 + mx.d);
    }

    template <typename T> inline T log(T x){
        return T(0.69314718) * log2(x);
    }

    /*
     * fast exp
     */
	inline float pow2(float p){
		union { float f; uint32_t i; } vp = { p };
		int sign = (vp.i >> 31);
		int w = p; // floor(p)
		float z = p - w + sign;
		union { uint32_t i; float f; } v = { (1 << 23) * (p + 121.2740838f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) };
		return v.f;
    }

	inline double pow2(double p){
		union { double f; uint64_t i; } vp = { p };
		int sign = (vp.i >> 63);
		int w = p; // floor(p)
		double z = p - w + sign;
		union { uint64_t i; double f; } v = { uint64_t(0x10000000000000) * (p + 1017.2740838 + 27.7280233 / (4.84252568 - z) - 1.49012907 * z) };
		return v.f;
	}
	template <typename T> inline T exp(T p){
		return pow2(1.442695040 * p);
	}

	/*
	 * fast lgamma
	 */
	template <typename T> inline T lgamma(T x){
		T logterm = log(x * (1.0+x) * (2.0 +x));
		T xp3 = 3.0 + x;

		return -2.081061466 - x + 0.0833333 / xp3 - logterm + (2.5 + x) * log(xp3);
	}

	/*
	 * fast digamma
	 */
	template <typename T> inline T digamma(T x){
		T twopx = 2.0 + x;
		T logterm = log(twopx);

		return - (1.0 + 2.0 * x) / (x * (1.0 + x)) - (13.0 + 6.0 * x) / (12.0 * twopx * twopx)+ logterm;
	}
};


#endif // __BAYES_UTILITIES__
