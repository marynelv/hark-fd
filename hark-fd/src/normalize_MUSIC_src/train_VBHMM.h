/*
 * parameter estimation for Bayesian sound tracking
 */
#ifndef _TRAIN_VBHMM_H_
#define _TRAIN_VBHMM_H_

#include <iostream>
#include <vector>
#include <stdio.h>

#include "Configuration.hpp"
#include "Array2D.hpp"
#include "utilities.hpp"
#include "PreciseNumber.hpp"

class TrainVBHMM{
public:
	explicit TrainVBHMM(const HyperparameterVBHMM& hyper_param);
	virtual ~TrainVBHMM(){}
	
	// body
	int train(const Array2D<Real>& music_spec_td, const HyperparameterVBHMM& hyper_param);
	
	// get
	void get_posterior_parameters(Array2D<Real>& hat_alpha, std::vector<Real>& hat_beta, 
		std::vector<Real>& hat_m, std::vector<Real>& hat_a, std::vector<Real>& hat_b) const;
	const Array2D<PN>& p_on_s_td() const {return p_on_s_td_; }
	const Array2D<Real>& hat_alpha_kj() const {return hat_alpha_kj_; }
	const std::vector<Real>& hat_beta_j() const {return hat_beta_j_; }
	const std::vector<Real>& hat_m_j() const {return hat_m_j_; }
	const std::vector<Real>& hat_a_j() const {return hat_a_j_; }
	const std::vector<Real>& hat_b_j() const {return hat_b_j_; }
	
protected:
	// initializers
	Real init_1st_time(const Array2D<Real>& music_spec_td, const Real thres, const Real init_probability, Array2D<PN>& p_on_s_td, Array2D<Array2D<PN> >& prev_on_s_td_kj) const;
	Real init(const Array2D<Real>& music_spec_td, Array2D<PN>& p_on_s_td, Array2D<Array2D<PN> >& prev_on_s_td_kj) const;
	
	// iterated procedures
	void update_suff_stat(const Array2D<Real>& music_spec_td, const Array2D<PN>& p_on_s_td_, std::vector<Real>& suff_1_j, std::vector<Real>& suff_x_j, std::vector<Real>& suff_xx_j) const;
	void update_alpha(const HyperparameterVBHMM& hyper_param, const Array2D<Array2D<PN> >& prev_on_s_td_kj);
	void update_normal_gamma(const HyperparameterVBHMM& hyper_param, const std::vector<Real>& suff_1_j, const std::vector<Real>& suff_x_j, const std::vector<Real>& suff_xx_j);
	void update_normal_gamma(const Array2D<Real>& music_spec_td, const HyperparameterVBHMM& hyper_param, const std::vector<Real>& suff_1_j, const std::vector<Real>& suff_x_j, const std::vector<Real>& suff_xx_j);
	void forward_backward(const Array2D<Real>& music_spec_td, Array2D<PN>& p_on_s_td, Array2D<Array2D<PN> >& prev_on_s_td_kj) const;
	
	// forward and backward
	void forward_path(const Array2D<Real>& music_spec_td, const Array2D<PN>& trans_kj, const Array2D<Array2D<Real> >& log_p_obs_td_j, Array2D<PN>& alpha_on_td, Array2D<PN>& alpha_off_td) const;
	void backward_path(const Array2D<Real>& music_spec_td, const Array2D<PN>& trans_kj, const Array2D<Array2D<Real> >& log_p_obs_td_j, Array2D<PN>& beta_on_td, Array2D<PN>& beta_off_td) const;

private:
	Array2D<PN> p_on_s_td_;
	
	// posterior parameters
	Array2D<Real> hat_alpha_kj_; // j: state at time t, k: state at time t-1
	std::vector<Real> hat_beta_j_;
	std::vector<Real> hat_m_j_;
	std::vector<Real> hat_a_j_;
	std::vector<Real> hat_b_j_;
	
	bool is_silent_;
};
 
#endif //_TRAIN_VBHMM_H_
