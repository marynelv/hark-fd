#ifndef _CONFIGURATION_HPP_
#define _CONFIGURATION_HPP_

#include <vector>

#include "PreciseNumber.hpp"

//typedef double Real;
typedef float Real;
typedef PreciseNumber<Real> PN;

struct HyperparameterVBHMM{
	Real beta0_, m0_, a0_, b0_;
	Real active_proportion_, init_active_probability_;
	
	int max_iteration_;
	
	std::vector<Real> alpha0_;
	
	HyperparameterVBHMM():alpha0_(2,1){
		beta0_ = 1;
		m0_ = 25;
		a0_ = 1;
		b0_ = 100;
		
		active_proportion_ = 0.05;
		init_active_probability_ = 0.99;
		max_iteration_ = 20;
	}
};

struct HyperparameterPF{
	int particle_num_;
	int max_source_num_;
	Real local_peak_margin_;
	
	HyperparameterPF(){
		particle_num_ = 300;
		max_source_num_ = 3;
		local_peak_margin_ = 1;
	}
};

#endif
