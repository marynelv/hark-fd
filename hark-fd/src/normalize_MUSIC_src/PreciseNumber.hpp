#ifndef __PRECISE_NUMBER_HPP__
#define __PRECISE_NUMBER_HPP__

#include <cmath>
#include <iostream>
#include <cstdlib>

#include <boost/cstdint.hpp>

#define INT_THRESHOLD 2147483600
#define ZERO_EXP_THRES 10000


inline double pow2(double p){
		union { double f; uint64_t i; } vp = { p };
		int sign = (vp.i >> 63);
		int w = p; // floor(p)
		double z = p - w + sign;
		union { uint64_t i; double f; } v = { uint64_t(0x10000000000000) * (p + 1017.2740838 + 27.7280233 / (4.84252568 - z) - 1.49012907 * z) };
		return v.f;
};
inline float pow2(float p){
	union { float f; uint32_t i; } vp = { p };
	int sign = (vp.i >> 31);
	int w = p; // floor(p)
	float z = p - w + sign;
	union { uint32_t i; float f; } v = { (1 << 23) * (p + 121.2740838f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) };
	return v.f;
}



template <typename T> struct PreciseNumber{
public:
	inline PreciseNumber(){
		*this = T(0);
		return;
	}
    inline PreciseNumber(const T d){
        *this = d;
        return;
    }
    /*
    PreciseNumber(){
        frac_ = 0.0;
        expo_ = 0;
        return;
    }
    */
    inline PreciseNumber(const PreciseNumber<T> &src):frac_(src.frac_),expo_(src.expo_){}

    /*
    virtual ~PreciseNumber(){
    }
    */

    /*
    T frac() const{ return frac_; }
    int expo() const { return expo_; }
	*/

    /*
     * normalize the number to avoid underflow such that 0.5 < abs(frac_) < 1.0
     */
    inline void normalize(){
        int expo_diff;
        T norm_frac = frexp(frac_, &expo_diff);
        frac_ = norm_frac;
        expo_ += expo_diff;

        return;
    }

    // misc
    bool isNegative() const {return frac_ < 0; }

    inline PreciseNumber<T> calcLn() const {
        // sign check
        if(frac_ <= 0){ // negative!
            std::cerr << "PreciseNumber::calcLn(): log(x) is required for a negative number: x = " << *this << ", f = " << frac_ << ", e = " << expo_ << std::endl;
            return PreciseNumber(std::log(-frac_) + expo_ * PreciseNumber::ln_2);
        }else{
            return PreciseNumber(std::log(frac_) + expo_ * PreciseNumber::ln_2);
        }
    }

    inline void toLn(){
    	// sign check
    	if(frac_ <= 0){
    		std::cerr << "PreciseNumber::toLn(): log(x) must be negative, frac_ = " << frac_ << std::endl;
    		T tmp_frac = std::log(-frac_) + expo_*PreciseNumber::ln_2;
    		frac_ = frexp(tmp_frac, &expo_);
    	}else{
    		T tmp_frac = std::log(frac_) + expo_*PreciseNumber::ln_2;
    		frac_ = frexp(tmp_frac, &expo_);
    	}
    }

    inline PreciseNumber<T> calcExp() const {
        PreciseNumber<T> pn(0);
        
        T temp_expo = ldexp(PreciseNumber::log2_e * frac_, expo_);
        T temp_expo_int, temp_expo_frac;
        temp_expo_frac = std::modf(temp_expo, &temp_expo_int);
        int i_temp_expo_int = static_cast<int>(temp_expo_int);

        //pn.frac_ = std::pow(2.0, temp_expo_frac);
        pn.frac_ = pow2(temp_expo_frac);
        pn.expo_ = i_temp_expo_int;
        pn.normalize();

        return pn;
    }
    inline void toExp(){
        T temp_expo = ldexp(PreciseNumber::log2_e * frac_, expo_);
        T temp_expo_int, temp_expo_frac;
        temp_expo_frac = std::modf(temp_expo, &temp_expo_int);
        int i_temp_expo_int = static_cast<int>(temp_expo_int);

        //frac_ = std::pow(2.0, temp_expo_frac);
        frac_ = pow2(temp_expo_frac);
        expo_ = i_temp_expo_int;
        normalize();

        return;
    }

    inline T getLn() const {
        // sign check
        if(frac_ <= 0){ // negative!
            std::cerr << "PreciseNumber::getLn(): log(x) is required for a negative number : x = " << *this << ", f = " << frac_ << ", e = " << expo_ << std::endl;
            return std::log(-frac_) + expo_ * PreciseNumber::ln_2;
        }else{
            return std::log(frac_) + expo_ * PreciseNumber::ln_2;
        }
    }
    inline T getExp() const {
        return ldexp(PreciseNumber::log2_e * frac_, expo_);
    }

    // operator overloads
    inline PreciseNumber<T>& operator=(const T d){
        frac_ = frexp(d, &expo_);
        return *this;
    }

    // cast to type double
    /*
    inline operator double() const {
        return ldexp(frac_, expo_);
    }
    */
    // cast to type float
    inline operator float() const {
    	return ldexp(static_cast<float>(frac_), expo_);
    }

    // negative
    inline PreciseNumber<T>& operator-(){
        frac_ = -frac_;
        return *this;
    }
    inline PreciseNumber<T> operator-() const{
        PreciseNumber<T> pn = *this;
        pn.frac_ = -pn.frac_;
        return pn;
    }

    // multiplication
    inline PreciseNumber<T> operator*(const T d) const {
        int temp_expo;
        PreciseNumber<T> pn = *this;
        pn.frac_ = frexp(d * pn.frac_, &temp_expo);

        const long l_expo = static_cast<long>(pn.expo_) + static_cast<long>(temp_expo);
        if(l_expo < INT_THRESHOLD && l_expo >= -INT_THRESHOLD)
        	pn.expo_ = static_cast<int>(l_expo);
        else if(l_expo < -INT_THRESHOLD)
        	pn.expo_ = -INT_THRESHOLD;
        else
        	pn.expo_ = INT_THRESHOLD;

        return pn;
    }

    inline PreciseNumber<T> operator*(const PreciseNumber<T> &num) const {
        /* old try using frexp       
        PreciseNumber pn = *this;
        pn.frac_ = this->frac_ * num.getFrac();
        pn.expo_ = this->expo_ + num.getExpo();

        pn.normalize();
        */
        int temp_expo;
        PreciseNumber<T> pn = *this;
        pn.frac_ = frexp(num.frac_ * pn.frac_, &temp_expo);

        const long l_expo = static_cast<long>(pn.expo_) + static_cast<long>(temp_expo) + static_cast<long>(num.expo_);
        if(l_expo < INT_THRESHOLD && l_expo >= -INT_THRESHOLD)
        	pn.expo_ = static_cast<int>(l_expo);
        else if(l_expo < -INT_THRESHOLD)
        	pn.expo_ = -INT_THRESHOLD;
        else
        	pn.expo_ = INT_THRESHOLD;

        return pn;
    }

    inline PreciseNumber<T>& operator*=(const T d){
        int temp_expo;
        frac_ = frexp(d * frac_, &temp_expo);

        const long l_expo = static_cast<long>(expo_) + static_cast<long>(temp_expo);
        if(l_expo < INT_THRESHOLD && l_expo >= -INT_THRESHOLD)
        	expo_ = static_cast<int>(l_expo);
        else if(l_expo < -INT_THRESHOLD)
        	expo_ = -INT_THRESHOLD;
        else
        	expo_ = INT_THRESHOLD;

        return *this;
    }

    inline PreciseNumber<T>& operator*=(const PreciseNumber<T> &num){
        int temp_expo;
        frac_ = frexp(num.frac_ * frac_, &temp_expo);

        const long l_expo = static_cast<long>(expo_) + static_cast<long>(temp_expo) + static_cast<long>(num.expo_);
        if(l_expo < INT_THRESHOLD && l_expo >= -INT_THRESHOLD)
			expo_ = static_cast<int>(l_expo);
		else if(l_expo < -INT_THRESHOLD)
			expo_ = -INT_THRESHOLD;
		else
			expo_ = INT_THRESHOLD;

        //expo_ += temp_expo + num.expo_;

        return *this;
    }

    // division
    inline PreciseNumber<T> operator/(const T d) const {
        int temp_expo;
        PreciseNumber<T> pn = *this;
        pn.frac_ = frexp(pn.frac_ / d, &temp_expo);

        const long l_expo = static_cast<long>(pn.expo_) + static_cast<long>(temp_expo);
        if(l_expo < INT_THRESHOLD && l_expo >= -INT_THRESHOLD)
        	pn.expo_ = static_cast<int>(l_expo);
        else if(l_expo < -INT_THRESHOLD)
        	pn.expo_ = -INT_THRESHOLD;
        else
        	pn.expo_ = INT_THRESHOLD;

        //pn.expo_ += temp_expo;

        return pn;
    }

    inline PreciseNumber<T> operator/(const PreciseNumber<T> &num) const {
        int temp_expo;
        PreciseNumber<T> pn = *this;
        pn.frac_ = frexp(pn.frac_ / num.frac_, &temp_expo);

        const long l_expo = static_cast<long>(pn.expo_) + static_cast<long>(temp_expo) - static_cast<long>(num.expo_);
        if(l_expo < INT_THRESHOLD && l_expo >= -INT_THRESHOLD)
        	pn.expo_ = l_expo;
        else if(l_expo < -INT_THRESHOLD)
        	pn.expo_ = -INT_THRESHOLD;
        else
        	pn.expo_ = INT_THRESHOLD;

        //pn.expo_ += temp_expo - num.expo_;

        return pn;
    }

    inline PreciseNumber<T>& operator/=(const T d){
        int temp_expo;
        frac_ = frexp(frac_ / d, &temp_expo);

        const long l_expo = static_cast<long>(expo_) + static_cast<long>(temp_expo);
        if(l_expo < INT_THRESHOLD && l_expo >= -INT_THRESHOLD)
			expo_ = l_expo;
		else if(l_expo < -INT_THRESHOLD)
			expo_ = -INT_THRESHOLD;
		else
			expo_ = INT_THRESHOLD;

        //expo_ += temp_expo;
        return *this;
    }

    inline PreciseNumber<T>& operator/=(const PreciseNumber<T> &num){
        int temp_expo;
        frac_ = frexp(frac_ / num.frac_, &temp_expo);

        const long l_expo = static_cast<long>(expo_) + static_cast<long>(temp_expo) - static_cast<long>(num.expo_);
        if(l_expo < INT_THRESHOLD && l_expo >= -INT_THRESHOLD)
			expo_ = l_expo;
		else if(l_expo < -INT_THRESHOLD)
			expo_ = -INT_THRESHOLD;
		else
			expo_ = INT_THRESHOLD;

        //expo_ += temp_expo - num.expo_;

        return *this;
    }

    // addition
    inline PreciseNumber<T>& operator+=(const PreciseNumber<T> &num){
        bool this_is_bigger = (frac_ != 0 && expo_ > num.expo_);
        const PreciseNumber<T>& smaller = this_is_bigger ? num : *this;
        const PreciseNumber<T>& bigger = this_is_bigger ? *this : num;

        //int neg_expo_diff = smaller.expo_ - bigger.expo_;
        const long l_expo = static_cast<long>(smaller.expo_) - static_cast<long>(bigger.expo_);
        if(l_expo < -ZERO_EXP_THRES){
        	*this = bigger;
        	return *this;
        }
        //frac_ = bigger.frac_ + ldexp(smaller.frac_, neg_expo_diff);
        frac_ = bigger.frac_ + ldexp(smaller.frac_, static_cast<int>(l_expo));
        expo_ = bigger.expo_;

        // normalize the frac_ part s.t. 0.5 <= frac_ <= 1.0
        normalize();
        return *this;
    }

    inline PreciseNumber<T>& operator+=(const T d){
        return (*this += PreciseNumber(d));
    }

    inline PreciseNumber<T> operator+(const PreciseNumber<T> &num) const {
        PreciseNumber<T> pn = *this;
        bool pn_is_bigger = (pn.frac_ != 0 && pn.expo_ > num.expo_);
        const PreciseNumber<T>& smaller = pn_is_bigger ? num : pn;
        const PreciseNumber<T>& bigger = pn_is_bigger ? pn : num;

        //int neg_expo_diff = smaller.expo_ - bigger.expo_;
        const long l_expo = static_cast<long>(smaller.expo_) - static_cast<long>(bigger.expo_);
        if(l_expo < -ZERO_EXP_THRES){
        	return bigger;
        }
        //pn.frac_ = bigger.frac_ + ldexp(smaller.frac_, neg_expo_diff);
        pn.frac_ = bigger.frac_ + ldexp(smaller.frac_, static_cast<int>(l_expo));
        pn.expo_ = bigger.expo_;

        pn.normalize();
        return pn;
    }

    inline PreciseNumber<T> operator+(const T d) const {
        return (*this + PreciseNumber(d));
    }

    // subtraction
    inline PreciseNumber<T>& operator-=(const PreciseNumber<T> &num){
        return (*this += -num);
    }
    inline PreciseNumber<T>& operator-=(const T d){
        return (*this -= PreciseNumber(d));
    }
    inline PreciseNumber<T> operator-(const PreciseNumber<T> &num) const{
        return (*this + (-num));
    }
    inline PreciseNumber<T> operator-(const T d) const {
        return (*this - PreciseNumber<T>(d));
    }

    // comparison
    inline bool operator>(const PreciseNumber<T> &num) const {
        // sign check!
        if(frac_ >= 0 && num.isNegative())
            return true;
        else if(frac_ < 0 && !num.isNegative())
            return false;
        else if(frac_ < 0 && num.isNegative())
            return ((-(*this)) < (-num));

        if(expo_ > num.expo_){
            return true;
        }else if(expo_ == num.expo_ && frac_ > num.frac_){
            return true;
        }else{
            return false;
        }
    }
    inline bool operator>=(const PreciseNumber<T> &num) const {
        // sign check!
        if(frac_ >= 0 && num.isNegative())
            return true;
        else if(frac_ < 0 && !num.isNegative())
            return false;
        else if(frac_ < 0 && num.isNegative())
            return ((-(*this)) < (-num));

        if(expo_ > num.expo_){
            return true;
        }else if(expo_ == num.expo_ && frac_ >= num.frac_){
            return true;
        }else{
            return false;
        }
    }


    inline bool operator<(const PreciseNumber<T> &num) const {
        // sign check!
        if(frac_ < 0 && !num.isNegative())
            return true;
        else if(frac_ >= 0 && num.isNegative())
            return false;
        else if(frac_ < 0 && num.isNegative())
            return ((-(*this)) > (-num));

        if(expo_ < num.expo_){
            return true;
        }else if(expo_ == num.expo_ && frac_ < num.frac_){
            return true;
        }else{
            return false;
        }
    }
    inline bool operator<=(const PreciseNumber<T> &num) const {
        // sign check!
        if(frac_ < 0 && !num.isNegative())
            return true;
        else if(frac_ >= 0 && num.isNegative())
            return false;
        else if(frac_ < 0 && num.isNegative())
            return ((-(*this)) > (-num));

        if(expo_ < num.expo_){
            return true;
        }else if(expo_ == num.expo_ && frac_ <= num.frac_){
            return true;
        }else{
            return false;
        }
    }

    inline bool operator==(const PreciseNumber<T>& num) const {
        return expo_ == num.expo_ && frac_ == num.frac_;
    }

    // for debug
    inline T frac() const {return frac_; }
    inline int expo() const {return expo_; }

    bool operator>(const T d) const { return *this > PreciseNumber<T>(d); }
    bool operator>=(const T d) const {return *this >= PreciseNumber<T>(d); }
    bool operator<(const T d) const { return *this < PreciseNumber<T>(d); }
    bool operator<=(const T d) const {return *this <= PreciseNumber<T>(d); }
    bool operator==(const T d) const {return *this == PreciseNumber<T>(d); }

    static const T ln_2 = 0.693147181;
    static const T log2_e = 1.44269504;

private:
    T frac_;
    int expo_;
};


template <typename T> PreciseNumber<T> log(const PreciseNumber<T>& pn){
	return pn.calcLn();
}
template <typename T> PreciseNumber<T> exp(const PreciseNumber<T>& pn){
	return pn.calcExp();
}

template <typename T> std::ostream& operator<<(std::ostream& os, const PreciseNumber<T>& pn){
	return (os << static_cast<double>(pn));
}


#endif // __PRECISE_NUMBER_HPP__
