#include "train_VBHMM.h"

TrainVBHMM::TrainVBHMM(const HyperparameterVBHMM& hyper_param){
	is_silent_ = true;
	
	/*
	hat_alpha_kj_ = Array2D<Real>::Constant(2,2,1.0); // {off,on} at time t-1 (k) x {off,on} at time t (j)
	hat_beta_j_.resize(2,1.0); // {off,on}
	
	// this configuration aims to pad zero for sound detection probability
	hat_m_j_.resize(2);
	hat_m_j_[0] = hyper_param.m0_ - 10; // 'off'
	hat_m_j_[1] = hyper_param.m0_ + 5; // 'on'
	
	hat_a_j_.resize(2,hyper_param.a0_); // {off,on}
	hat_b_j_.resize(2,hyper_param.b0_); // {off,on}
	*/
	
	// pretend after training
	//*
	hat_alpha_kj_ = Array2D<Real>::Constant(2,2,10);
	hat_alpha_kj_(0,0) = 50; hat_alpha_kj_(1,1) = 20;
	
	hat_beta_j_.resize(2,100);
	hat_beta_j_[0] = 1000;
	
	hat_m_j_.resize(2);
	hat_m_j_[0] = 23;
	hat_m_j_[1] = 35;
	//hat_m_j_[0] = hyper_param.m0_ - 20; // 'off'
	//hat_m_j_[1] = hyper_param.m0_ + 5; // 'on'
	//hat_m_j_[0] = 25;
	//hat_m_j_[1] = 10000;
	
	hat_a_j_.resize(2,4); hat_a_j_[1] = 9;
	hat_b_j_.resize(2,1);
	//*/
}

void TrainVBHMM::get_posterior_parameters(Array2D<Real>& hat_alpha, std::vector<Real>& hat_beta, 
		std::vector<Real>& hat_m, std::vector<Real>& hat_a, std::vector<Real>& hat_b) const {
	hat_alpha = hat_alpha_kj_;
	hat_beta = hat_beta_j_;
	hat_m = hat_m_j_;
	hat_a = hat_a_j_;
	hat_b = hat_b_j_;
	
	return;
}


/*
 * inferece of HMM parameters based on variational Bayes method
 */
int TrainVBHMM::train(const Array2D<Real>& music_spec_td, const HyperparameterVBHMM& hyper_param){
	const int T = music_spec_td.rows(), D = music_spec_td.cols();
	
	Array2D<Array2D<PN> > prev_on_s_td_kj; // 4D-array for transition probability
	// prev_on_s_td_kj(k,j)(t,d): probability of s_{t,d} being j where s_{t-1,d} = k and k = {0,1}
	
	// p_on_s_td_(t,d): probability of s_{t,d} being 1. 1-p_on_s_td_(t,d) is the probability of s_{t,d}=0.
	
	// 1. initialize
	Real active_proportion;
	if(is_silent_)
		active_proportion = init_1st_time(music_spec_td, hyper_param.m0_, hyper_param.init_active_probability_, p_on_s_td_, prev_on_s_td_kj);
	else
		active_proportion = init(music_spec_td, p_on_s_td_, prev_on_s_td_kj);
	
	if(active_proportion < hyper_param.active_proportion_){ // no inference for now
		// the observation is too biased to silence
		return 1;
	}
	is_silent_ = false;
	
	// initialize the suffcient statistics
	std::vector<Real> suff_1_j(2,0), suff_x_j(2,0), suff_xx_j(2,0);
	update_suff_stat(music_spec_td, p_on_s_td_, suff_1_j, suff_x_j, suff_xx_j);
	update_normal_gamma(music_spec_td, hyper_param, suff_1_j, suff_x_j, suff_xx_j); // hat_*** variables are updated
	
	// 2. iteration
	const Real convergence_threshold = 0.001;
	Array2D<PN> before_p_on_s_td = p_on_s_td_;
	for(int iter = 0; iter < hyper_param.max_iteration_; ++iter){
		forward_backward(music_spec_td, p_on_s_td_, prev_on_s_td_kj);
		update_alpha(hyper_param, prev_on_s_td_kj);
		update_suff_stat(music_spec_td, p_on_s_td_, suff_1_j, suff_x_j, suff_xx_j);
		update_normal_gamma(music_spec_td, hyper_param, suff_1_j, suff_x_j, suff_xx_j);
		
		// check convergence
		if(iter == 0)
			continue;
		
		Real diff_p=0;
		for(int t = 0; t < T; ++t){
			for(int d = 0; d < D; ++d){
				const Real diff_td = static_cast<Real>(p_on_s_td_(t,d) - before_p_on_s_td(t,d));
				diff_p += diff_td > 0 ? diff_td : -diff_td;
			}
		}
		diff_p /= (T*D);
		//printf("iter (%02d): diff_p = %f\n",iter,diff_p);
		
		if(diff_p < convergence_threshold){
			break; // end the iteration after converged
		}
		
		before_p_on_s_td = p_on_s_td_;
	}
	
	return 0;
}

Real TrainVBHMM::init_1st_time(const Array2D<Real>& music_spec_td, const Real thres, const Real init_probability, Array2D<PN>& p_on_s_td, Array2D<Array2D<PN> >& prev_on_s_td_kj) const {
	// resize
	const int T = music_spec_td.rows(), D = music_spec_td.cols();
	p_on_s_td = Array2D<PN>::Constant(T,D, 1-init_probability);
	prev_on_s_td_kj = Array2D<Array2D<PN> >::Constant(T,D, Array2D<PN>::Constant(2,2, (1-init_probability)/3));
	// constraints
	// 1. 0 <= p_on_s_td(t,d) <= 1
	// 2. summation of prev(t,d)(k,j) over k={0,1} and j={0,1} counts to 1 for any t and d
	
	int active_count = 0;
	for(int t = 0; t < T; ++t){
		for(int d = 0; d < D; ++d){
			if(music_spec_td(t,d) > thres){
				++active_count;
				p_on_s_td(t,d) = init_probability;
				if(t > 0 && music_spec_td(t-1,d) > thres){
					// previous state is 'on'
					prev_on_s_td_kj(t,d)(1,1) = init_probability;
				}else{
					// previous state is 'off'
					prev_on_s_td_kj(t,d)(0,1) = init_probability;
				}
			}else{
				// s_{t,d} is 'off'
				p_on_s_td(t,d) = 1-init_probability; // initialized this way above
				if(t > 0 && music_spec_td(t-1,d) > thres){
					prev_on_s_td_kj(t,d)(1,0) = init_probability; // previous 'on'
				}else{
					prev_on_s_td_kj(t,d)(0,0) = init_probability; 
				}
			}
		}
	}
	
	return static_cast<Real>(active_count) / (T*D); // return the proportion of active time-direction points
}

Real TrainVBHMM::init(const Array2D<Real>& music_spec_td, Array2D<PN>& p_on_s_td, Array2D<Array2D<PN> >& prev_on_s_td_kj) const {
	// resize
	const int T = music_spec_td.rows(), D = music_spec_td.cols();
	p_on_s_td = Array2D<PN>::Zero(T,D);
	prev_on_s_td_kj = Array2D<Array2D<PN> >::Constant(T,D, Array2D<PN>::Zero(2,2));
	
	// calculate the weight based on the student's t
	const Real nu_off = 2*hat_a_j_[0], nu_on = 2*hat_a_j_[1];
	const Real lmd_off = (hat_beta_j_[0] * hat_a_j_[0])/((1+hat_beta_j_[0])*hat_b_j_[0]);
	const Real lmd_on = (hat_beta_j_[1] * hat_a_j_[1])/((1+hat_beta_j_[1])*hat_b_j_[1]);
	
	const Real log_const_on = Bayes::lgamma((nu_on+1)/2) - Bayes::lgamma(nu_on/2) + 0.5*(Bayes::log(lmd_on)-Bayes::log(nu_on));
	const Real log_const_off = Bayes::lgamma((nu_off+1)/2) - Bayes::lgamma(nu_off/2) + 0.5*(Bayes::log(lmd_off)-Bayes::log(nu_off));
	const Real diff_scale_on = lmd_on / nu_on, diff_scale_off = lmd_off / nu_off;
	/*
	 * log p(x_{t,d} | s_{t,d}=on) = log_const_on - (nu_on+1)/2 * log(1 + diff_scale_on * (x - m_on)^2)
	 */
	int active_count = 0;
	for(int t = 0; t < T; ++t){
		for(int d = 0; d < D; ++d){
			const Real diff2_x_m_off = (music_spec_td(t,d) - hat_m_j_[0])*(music_spec_td(t,d) - hat_m_j_[0]);
			const Real diff2_x_m_on = (music_spec_td(t,d) - hat_m_j_[1])*(music_spec_td(t,d) - hat_m_j_[1]);
			
			const Real log_p_off = log_const_off - (nu_off+1)/2 * Bayes::log(1 + diff_scale_off * diff2_x_m_off);
			const Real log_p_on = log_const_on - (nu_on+1)/2 * Bayes::log(1 + diff_scale_on * diff2_x_m_on);
			
			PN p_off = exp(PN(log_p_off)), p_on = exp(PN(log_p_on));
			
			p_on_s_td(t,d) = p_on / (p_on+p_off); // normalize
			
			if(p_on_s_td(t,d) > static_cast<Real>(0.95))
				++active_count;
		}
	}
	
	// prev_on_s_td_kj
	for(int t = 0; t < T; ++t){
		for(int d = 0; d < D; ++d){
			if(t > 0){
				const PN p00 = (1-p_on_s_td(t-1,d))*(1-p_on_s_td(t,d));
				const PN p01 = (1-p_on_s_td(t-1,d))*p_on_s_td(t,d);
				const PN p10 = p_on_s_td(t-1,d)*(1-p_on_s_td(t,d));
				const PN p11 = p_on_s_td(t-1,d)*p_on_s_td(t,d);
				const PN sum_w = p00+p01+p10+p11; // should be always 1
				prev_on_s_td_kj(t,d)(0,0) = p00/sum_w;
				prev_on_s_td_kj(t,d)(0,1) = p01/sum_w;
				prev_on_s_td_kj(t,d)(1,0) = p10/sum_w;
				prev_on_s_td_kj(t,d)(1,1) = p11/sum_w;
			}else{
				// t < 0: always off!
				const Real eps = 0.05;
				const PN p00 = (1-eps)*(1-p_on_s_td(t,d));
				const PN p01 = (1-eps)*p_on_s_td(t,d);
				const PN p10 = eps*(1-p_on_s_td(t,d));
				const PN p11 = eps*p_on_s_td(t,d);
				const PN sum_w = p00+p01+p10+p11; // should be always 1
				prev_on_s_td_kj(t,d)(0,0) = p00/sum_w;
				prev_on_s_td_kj(t,d)(0,1) = p01/sum_w;
				prev_on_s_td_kj(t,d)(1,0) = p10/sum_w;
				prev_on_s_td_kj(t,d)(1,1) = p11/sum_w;
			}
		}
	}
	
	return static_cast<Real>(active_count) / (T*D); // return the proportion of active time-direction points
}

/*
 * calculate suff_*** from music_spec and p_on
 */
void TrainVBHMM::update_suff_stat(const Array2D<Real>& music_spec_td, const Array2D<PN>& p_on_s_td_, std::vector<Real>& suff_1_j, std::vector<Real>& suff_x_j, std::vector<Real>& suff_xx_j) const {
	const int T = music_spec_td.rows(), D = music_spec_td.cols();
	// resize containers
	suff_1_j.assign(2,0);
	suff_x_j.assign(2,0);
	suff_xx_j.assign(2,0);
	
	for(int t = 0; t < T; ++t){
		for(int d = 0; d < D; ++d){
			const Real p_off = 1-static_cast<Real>(p_on_s_td_(t,d)), p_on = static_cast<Real>(p_on_s_td_(t,d));
			const Real xx = music_spec_td(t,d)*music_spec_td(t,d);
			suff_1_j[0] += p_off;
			suff_1_j[1] += p_on;
			
			suff_x_j[0] += p_off * music_spec_td(t,d);
			suff_x_j[1] += p_on * music_spec_td(t,d);
			
			suff_xx_j[0] += p_off * xx;
			suff_xx_j[1] += p_on * xx;
		}
	}
	return;
}

// update hat_alpha_kj_
void TrainVBHMM::update_alpha(const HyperparameterVBHMM& hyper_param, const Array2D<Array2D<PN> >& prev_on_s_td_kj) {
	// size
	const int T = prev_on_s_td_kj.rows(), D = prev_on_s_td_kj.cols();
	// resize
	hat_alpha_kj_.resize(2,2);
	// initialize with hyperparameter
	hat_alpha_kj_(0,0) = hyper_param.alpha0_[0];
	hat_alpha_kj_(0,1) = hyper_param.alpha0_[1];
	hat_alpha_kj_(1,0) = hyper_param.alpha0_[0];
	hat_alpha_kj_(1,1) = hyper_param.alpha0_[1];
	
	for(int t = 0; t < T; ++t){
		for(int d = 0; d < D; ++d){
			for(int k = 0; k < 2; ++k){
				for(int j = 0; j < 2; ++j){
					hat_alpha_kj_(k,j) += static_cast<Real>(prev_on_s_td_kj(t,d)(k,j));
				}
			}
		}
	}
	
	return;
}

// update hat_{beta,m,a,b}
void TrainVBHMM::update_normal_gamma(const HyperparameterVBHMM& hyper_param, const std::vector<Real>& suff_1_j, const std::vector<Real>& suff_x_j, const std::vector<Real>& suff_xx_j){
	// initialize the parameters
	hat_beta_j_.assign(2,hyper_param.beta0_);
	hat_m_j_.assign(2,hyper_param.m0_);
	hat_a_j_.assign(2,hyper_param.a0_);
	hat_b_j_.assign(2,hyper_param.b0_);
	
	for(int j=0; j < 2; ++j){ // j={0,1}
		// normal
		hat_beta_j_[j] += suff_1_j[j];
		const Real m_bias = j==0 ? -hyper_param.m0_+5 : 20;
		hat_m_j_[j] = (hyper_param.beta0_*(hyper_param.m0_+m_bias) + suff_x_j[j]) / (hyper_param.beta0_ + suff_1_j[j]);
		
		// gamma
		hat_a_j_[j] += suff_1_j[j] / 2;
		hat_b_j_[j] += (suff_xx_j[j] - suff_x_j[j]*suff_x_j[j] / suff_1_j[j]) / 2;
		const Real diff2_bx_m = (suff_x_j[j]/suff_1_j[j] - hyper_param.m0_)*(suff_x_j[j]/suff_1_j[j] - hyper_param.m0_);
		hat_b_j_[j] += hyper_param.beta0_*suff_1_j[j] * diff2_bx_m / (hyper_param.beta0_ + suff_1_j[j]) / 2;
	}
	
	return;
}

// update hat_{beta,m,a,b}
// empirically configure the hyperparameter m0
void TrainVBHMM::update_normal_gamma(const Array2D<Real>& music_spec_td, const HyperparameterVBHMM& hyper_param, const std::vector<Real>& suff_1_j, const std::vector<Real>& suff_x_j, const std::vector<Real>& suff_xx_j){
	const int T = music_spec_td.rows(), D = music_spec_td.cols();
	std::vector<Real> spec_array(T*D);
	for(int t = 0; t < T; ++t){
		for(int d = 0; d < D; ++d){
			spec_array[t*D+d] = music_spec_td(t,d);
		}
	}
	// sort the music spec
	std::sort(spec_array.begin(), spec_array.end());
	const Real min_spec = spec_array[0];
	Real up_spec = 0; // aggregate the larger value
	int up_num = 0;
	for(int n = spec_array.size()*(1.0-hyper_param.active_proportion_), N = spec_array.size(); n < N; ++n){
		up_spec += spec_array[n];
		++up_num;
	}
	if(up_num == 0){
		up_spec = spec_array[spec_array.size()-1];
	}else{
		up_spec /= up_num;
	}
	const Real m0[2] = {min_spec, up_spec};
	
	// initialize the parameters
	hat_beta_j_.assign(2,hyper_param.beta0_);
	hat_m_j_.assign(2,hyper_param.m0_);
	hat_a_j_.assign(2,hyper_param.a0_);
	hat_b_j_.assign(2,hyper_param.b0_);
	
	for(int j=0; j < 2; ++j){ // j={0,1}
		// normal
		hat_beta_j_[j] += suff_1_j[j];
		hat_m_j_[j] = (hyper_param.beta0_*(m0[j]) + suff_x_j[j]) / (hyper_param.beta0_ + suff_1_j[j]);
		
		// gamma
		//hat_a_j_[j] += suff_1_j[j] / 2;
		//hat_b_j_[j] += (suff_xx_j[j] - suff_x_j[j]*suff_x_j[j] / suff_1_j[j]) / 2;
		const Real diff2_bx_m = (suff_x_j[j]/suff_1_j[j] - m0[j])*(suff_x_j[j]/suff_1_j[j] - m0[j]);
		hat_b_j_[j] += hyper_param.beta0_*suff_1_j[j] * diff2_bx_m / (hyper_param.beta0_ + suff_1_j[j]) / 2;
		hat_a_j_[j] += (suff_1_j[0] + suff_1_j[1]) / 2;
		hat_b_j_[j] += (suff_xx_j[0] - suff_x_j[0]*suff_x_j[0] / suff_1_j[0] + suff_xx_j[1] - suff_x_j[1]*suff_x_j[1] / suff_1_j[1]) / 2;
		
	}
	return;
}

// forward-backward algorithm to calculate p_on, p_kj
void TrainVBHMM::forward_backward(const Array2D<Real>& music_spec_td, Array2D<PN>& p_on_s_td, Array2D<Array2D<PN> >& prev_on_s_td_kj) const{
	const int T = music_spec_td.rows(), D = music_spec_td.cols();
	p_on_s_td = Array2D<PN>::Zero(T,D);
	prev_on_s_td_kj = Array2D<Array2D<PN> >::Constant(T,D, Array2D<PN>::Zero(2,2));
	
	// preparation
	Array2D<PN> trans_kj(2,2);
	Array2D<Array2D<Real> > log_p_obs_td_j = Array2D<Array2D<Real> >::Constant(T,D,Array2D<Real>::Zero(1,2)); // TxDx{0,1}
	
	for(int k = 0; k < 2; ++k){
		const Real sum_alpha_k = hat_alpha_kj_(k,0)+hat_alpha_kj_(k,1);
		for(int j = 0; j < 2; ++j){
			trans_kj(k,j) = exp(PN(Bayes::digamma(hat_alpha_kj_(k,j)) - Bayes::digamma(sum_alpha_k)));
		}
	}
	// log obs
	{
		Real log_const_j[2];
		Real diff2_scale[2];
		for(int j = 0; j < 2; ++j){
			log_const_j[j] = Bayes::digamma(hat_a_j_[j])-Bayes::log(hat_b_j_[j])-1/hat_beta_j_[j];
			diff2_scale[j] = hat_a_j_[j] / hat_b_j_[j];
		}
		
		for(int t = 0; t < T; ++t){
			for(int d = 0; d < D; ++d){
				for(int j = 0; j < 2; ++j){
					const Real diff2_td = (music_spec_td(t,d)-hat_m_j_[j])*(music_spec_td(t,d)-hat_m_j_[j]);
					log_p_obs_td_j(t,d)(0,j) = (log_const_j[j] - diff2_scale[j] * diff2_td)/2;
				}
			}
		}
	}
	
	Array2D<PN> fwd_on_td(T,D), fwd_off_td(T,D), bwd_on_td(T,D), bwd_off_td(T,D);
	// forward path
	forward_path(music_spec_td, trans_kj, log_p_obs_td_j, fwd_on_td, fwd_off_td);
	// backward path
	backward_path(music_spec_td, trans_kj, log_p_obs_td_j, bwd_on_td, bwd_off_td);
	
	// aggregate
	for(int t = 0; t < T; ++t){
		for(int d = 0; d < D; ++d){
			const PN p_on = fwd_on_td(t,d)*bwd_on_td(t,d);
			const PN p_off = fwd_off_td(t,d)*bwd_off_td(t,d);
			
			p_on_s_td(t,d) = p_on/(p_on+p_off);
			
			const Real eps = 0.05;
			const PN p00 = (t>0 ? fwd_off_td(t-1,d) : PN(1-eps)) * bwd_off_td(t,d) * trans_kj(0,0) * exp(PN(log_p_obs_td_j(t,d)(0,0)));
			const PN p01 = (t>0 ? fwd_off_td(t-1,d) : PN(1-eps)) * bwd_on_td(t,d) * trans_kj(0,1) * exp(PN(log_p_obs_td_j(t,d)(0,1)));
			const PN p10 = (t>0 ? fwd_on_td(t-1,d) : PN(eps)) * bwd_off_td(t,d) * trans_kj(1,0) * exp(PN(log_p_obs_td_j(t,d)(0,0)));
			const PN p11 = (t>0 ? fwd_on_td(t-1,d) : PN(eps)) * bwd_on_td(t,d) * trans_kj(1,1) * exp(PN(log_p_obs_td_j(t,d)(0,1)));
			
			// normalize!
			const PN sum_p = p00+p01+p10+p11; // four probabilities
			prev_on_s_td_kj(t,d)(0,0) = p00 / sum_p;
			prev_on_s_td_kj(t,d)(0,1) = p01 / sum_p;
			prev_on_s_td_kj(t,d)(1,0) = p10 / sum_p;
			prev_on_s_td_kj(t,d)(1,1) = p11 / sum_p;
		}
	}
	
	return;
}

void TrainVBHMM::forward_path(const Array2D<Real>& music_spec_td, const Array2D<PN>& trans_kj, const Array2D<Array2D<Real> >& log_p_obs_td_j, Array2D<PN>& alpha_on_td, Array2D<PN>& alpha_off_td) const {
	const int T = music_spec_td.rows(), D = music_spec_td.cols();
	
	for(int t = 0; t < T; ++t){
		for(int d = 0; d < D; ++d){
			// off
			alpha_off_td(t,d) = ((t>0 ? alpha_off_td(t-1,d) : PN(1)) * trans_kj(0,0) + (t>0 ? alpha_on_td(t-1,d) : PN(0)) * trans_kj(1,0)) * exp(PN(log_p_obs_td_j(t,d)(0,0)));
			
			// on
			alpha_on_td(t,d) = ((t>0 ? alpha_off_td(t-1,d) : PN(1)) * trans_kj(0,1) + (t>0 ? alpha_on_td(t-1,d) : PN(0)) * trans_kj(1,1)) * exp(PN(log_p_obs_td_j(t,d)(0,1)));
		}
	}
	
	return;
}

void TrainVBHMM::backward_path(const Array2D<Real>& music_spec_td, const Array2D<PN>& trans_kj, const Array2D<Array2D<Real> >& log_p_obs_td_j, Array2D<PN>& beta_on_td, Array2D<PN>& beta_off_td) const {
	const int T = music_spec_td.rows(), D = music_spec_td.cols();
	
	for(int t = T-1; t >= 0; --t){
		for(int d = 0; d < D; ++d){
			// off
			beta_off_td(t,d) = ((t>=T-1 ? PN(1) : beta_off_td(t+1,d)) * trans_kj(0,0) + (t>=T-1 ? PN(1) : beta_on_td(t+1,d)) * trans_kj(0,1)) * exp(PN(log_p_obs_td_j(t,d)(0,0)));
			
			// on
			beta_on_td(t,d) = ((t>=T-1 ? PN(1) : beta_off_td(t+1,d)) * trans_kj(1,0) + (t>=T-1 ? PN(1) : beta_on_td(t+1,d)) * trans_kj(1,1)) * exp(PN(log_p_obs_td_j(t,d)(0,1)));
		}
	}
	
	return;
}
