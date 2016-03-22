#include "track_PF.h"

TrackParticleFilter::TrackParticleFilter(const HyperparameterPF& hyper_param){
	particle_vec_.resize(hyper_param.particle_num_);
	//mt_eng_.seed(0);
	struct timeval t;
	gettimeofday(&t, NULL);

#ifdef ENABLE_BOOST_RANDOM
	mt_eng_.seed(static_cast<unsigned long>(t.tv_sec+t.tv_usec));
#else
	unsigned long init[4] = {t.tv_sec, t.tv_usec, t.tv_sec+t.tv_usec, t.tv_sec*t.tv_usec};
	unsigned long length = 4;
	mt_eng_.init_by_array(init,length);
#endif
}

TrackParticleFilter::TrackParticleFilter(const TrainVBHMM& vbhmm, const HyperparameterPF& hyper_param){
	particle_vec_.resize(hyper_param.particle_num_);
	//mt_eng_.seed(0);
	struct timeval t;
	gettimeofday(&t, NULL);
	
	
#ifdef ENABLE_BOOST_RANDOM
	mt_eng_.seed(static_cast<unsigned long>(t.tv_sec+t.tv_usec));
#else
	unsigned long init[4] = {t.tv_sec, t.tv_usec, t.tv_sec+t.tv_usec, t.tv_sec*t.tv_usec};
	unsigned long length = 4;
	mt_eng_.init_by_array(init,length);
#endif
	
	set_parameters(vbhmm);
}

void TrackParticleFilter::set_parameters(const TrainVBHMM& vbhmm){
	hat_alpha_kj_ = vbhmm.hat_alpha_kj();
	hat_beta_j_ = vbhmm.hat_beta_j();
	hat_m_j_ = vbhmm.hat_m_j();
	hat_a_j_ = vbhmm.hat_a_j();
	hat_b_j_ = vbhmm.hat_b_j();
	
	return;
}

void TrackParticleFilter::init(const HyperparameterPF& hyper_param){
	// erase all particles
	particle_vec_.assign(hyper_param.particle_num_, Particle());
}

void TrackParticleFilter::update_online(std::vector<Real>& p_on_d, const std::vector<Real>& music_frame_d, const HyperparameterPF& hyper_param){
	// importance-resampling particle filter
	std::vector<Particle> prop_particle = particle_vec_; // copy the weight
	
	// proposal: inverse weight is applied
	particle_proposal(prop_particle, music_frame_d, hyper_param.particle_num_, hyper_param.local_peak_margin_);
	
	// state transtion
	state_transition(prop_particle, particle_vec_, music_frame_d.size()); // compare with the previous particles
	
	// obsrevation
	update_weight(prop_particle, music_frame_d, hyper_param);
	
	particle_vec_ = prop_particle;
	resample(particle_vec_); // resample if necessary
	
	// aggregate the result
	const int N = particle_vec_.size(), D = music_frame_d.size();
	p_on_d.assign(D,0);
	for(int d = 0; d < D; ++d){
		for(int n = 0; n < N; ++n){
			if(particle_vec_[n].is_active_dir(d)){
				p_on_d[d] += std::exp(particle_vec_[n].log_weight_);
			}
		}
	}
	
	
	return;
}

void TrackParticleFilter::update_all(Array2D<Real>& p_on_td, const Array2D<Real>& music_spec_td, const HyperparameterPF& hyper_param){
	const int T = music_spec_td.rows(), D = music_spec_td.cols();
	p_on_td = Array2D<Real>::Zero(T,D);
	init(hyper_param);
	
	std::vector<Real> music_frame_d;
	std::vector<Real> p_on_frame_d;
	for(int t = 0; t < T; ++t){
		music_frame_d = music_spec_td.row(t); // extract one frame
		update_online(p_on_frame_d, music_frame_d, hyper_param);
		
		// put the results into the target container
		for(int d = 0; d < D; ++d){
			p_on_td(t,d) = p_on_frame_d[d];
		}
	}
	
	return;
}

void TrackParticleFilter::particle_proposal(std::vector<Particle>& prop_particle, const std::vector<Real>& music_frame_d, const int N, const Real local_peak_margin) {
	const int D = music_frame_d.size();
	
	if(N != static_cast<int>(prop_particle.size())){
		std::cerr << "invalid particle number: " << prop_particle.size() << " / " << N << std::endl;
		prop_particle.resize(N);
	}
	
	std::vector<Real> p_on_d(D);
	for(int d = 0; d < D; ++d){
		int is_local_peak = 1;
		if(d > 0 && d < D-1){
			if(music_frame_d[d]+local_peak_margin < music_frame_d[d-1] || music_frame_d[d]+local_peak_margin < music_frame_d[d+1])
				is_local_peak = 0;
		}else if(d==0){
			if(music_frame_d[0]+local_peak_margin < music_frame_d[D-1] || music_frame_d[0]+local_peak_margin < music_frame_d[1])
				is_local_peak = 0;
		}else if(d==D-1){
			if(music_frame_d[D-1]+local_peak_margin < music_frame_d[D-2] || music_frame_d[D-1]+local_peak_margin < music_frame_d[0])
				is_local_peak = 0;
		}
		
		const PN p_off = exp(PN(-0.5 * hat_a_j_[0]/hat_b_j_[0] * (music_frame_d[d]-hat_m_j_[0])*(music_frame_d[d]-hat_m_j_[0])));
		const PN p_on = exp(PN(-0.5 * hat_a_j_[1]/hat_b_j_[1] * (music_frame_d[d]-hat_m_j_[1])*(music_frame_d[d]-hat_m_j_[1]))) * static_cast<Real>(is_local_peak);
		
		p_on_d[d] = static_cast<Real>(p_on / (p_on + p_off));
	}
	
	//boost::random::uniform_real_distribution<Real> rnd_u01(0,1);
#ifdef ENABLE_BOOST_RANDOM
	boost::uniform_real<Real> rnd_u01(0,1);
#endif

	const Real eps = std::numeric_limits<Real>::epsilon();
	const Real log_eps = std::log(eps);
	for(int n = 0; n < N; ++n){
		prop_particle[n].active_dir_.clear();
		
		for(int d = 0; d < D; ++d){
#ifdef ENABLE_BOOST_RANDOM
			const Real u = rnd_u01(mt_eng_);
#else
			const Real u = mt_eng_.genrand_res53();
#endif
			if(u < p_on_d[d]){
				prop_particle[n].active_dir_.push_back(d);
				// apply the proposal weight
				const Real log_w = p_on_d[d] < eps ? log_eps : Bayes::log(p_on_d[d]);
				prop_particle[n].log_weight_ -= log_w;
			}else{
				// apply the proposal weight
				const Real log_w = (1-p_on_d[d]) < eps ? log_eps : Bayes::log(1-p_on_d[d]);
				prop_particle[n].log_weight_ -= log_w;
			}
		}
	}
	
	return;
}

void TrackParticleFilter::state_transition(std::vector<Particle>& next_particle, const std::vector<Particle>& prev_particle, const int D) const {
	const int N = prev_particle.size();
	
	Array2D<Real> log_p_kj(2,2);
	for(int k = 0; k < 2; ++k){
		const Real log_sum_k = Bayes::log(hat_alpha_kj_(k,0) + hat_alpha_kj_(k,1));
		for(int j = 0; j < 2; ++j){
			log_p_kj(k,j) = Bayes::log(hat_alpha_kj_(k,j)) - log_sum_k;
		}
	}
	
	for(int n = 0; n < N; ++n){
		for(int d = 0; d < D; ++d){
			const int k_n = prev_particle[n].is_active_dir(d) ? 1 : 0;
			const int j_n = next_particle[n].is_active_dir(d) ? 1 : 0;
			next_particle[n].log_weight_ += log_p_kj(k_n, j_n);
		}
	}
	
	return;
}

void TrackParticleFilter::update_weight(std::vector<Particle>& particle, const std::vector<Real>& music_frame_d, const HyperparameterPF& hyper_param) const {
	const int N = particle.size(), D = music_frame_d.size();
	
	// preparation for student's t-distribution
	const Real nu_j[2] = {2*hat_a_j_[0], 2*hat_a_j_[1]};
	const Real lmd_j[2] = {(hat_beta_j_[0]*hat_a_j_[0])/((1+hat_beta_j_[0])*hat_b_j_[0]) , (hat_beta_j_[1]*hat_a_j_[1])/((1+hat_beta_j_[1])*hat_b_j_[1])};
	const Real nu_over_lmd_j[2] = {nu_j[0]/lmd_j[0], nu_j[1]/lmd_j[1]};
	
	const Real log_const[2] = {Bayes::lgamma((nu_j[0]+1)/2)-Bayes::lgamma(nu_j[0]/2)+0.5*Bayes::log(nu_over_lmd_j[0]), Bayes::lgamma((nu_j[0]+1)/2)-Bayes::lgamma(nu_j[0]/2)+0.5*Bayes::log(nu_over_lmd_j[0])};
	
	Array2D<Real> student_t_dj(D,2);
	for(int d = 0; d < D; ++d){
		for(int j = 0; j < 2; ++j){
			const Real diff = music_frame_d[d] - hat_m_j_[j];
			
			// student's t
			student_t_dj(d,j) = -(nu_j[j]+1)*0.5 * Bayes::log(1 + nu_over_lmd_j[j] * diff*diff);
		}
	}
	
	const Real log_eps = std::log(std::numeric_limits<Real>::epsilon())*N*D*1000;
	for(int n = 0; n < N; ++n){
		for(int d = 0; d < D; ++d){
			const int on_nd = particle[n].is_active_dir(d) ? 1 : 0;
			
			particle[n].log_weight_ += log_const[on_nd] + student_t_dj(d,on_nd);
		}
		// too many active sources?
		if(static_cast<int>(particle[n].active_dir_.size()) > hyper_param.max_source_num_){
			particle[n].log_weight_ += log_eps; // penalize the weight
		}
	}
	
	// normalize the weight of particles
	normalize_weight(particle);
	
	return;
}


void TrackParticleFilter::normalize_weight(std::vector<Particle>& particle) const {
	const int N = particle.size();
	PN sum_weight(0);
	
	// take the maximum weight to prevent overflow
	Real max_weight = 0;
	for(int n = 0; n < N; ++n){
		if(n == 0 || max_weight < particle[n].log_weight_){
			max_weight = particle[n].log_weight_;
		}
	}
	
	// sum of the particle weights
	for(int n = 0; n < N; ++n){
		sum_weight += exp(PN(particle[n].log_weight_ - max_weight));
	}
	
	const Real log_w_normalizer = -max_weight - log(sum_weight);
	// normalize the weights
	for(int n = 0; n < N; ++n){
		particle[n].log_weight_ += log_w_normalizer;
	}
	
	return;
}

Real TrackParticleFilter::resample(std::vector<Particle>& particle) {
	const int N = particle.size();
	
	std::vector<PN> cumsum_w(N);
	
	Real max_weight = 0;
	for(int n = 0; n < N; ++n){
		if(n==0 || max_weight < particle[n].log_weight_){
			max_weight = particle[n].log_weight_;
		}
	}
	
	for(int n = 0; n < N; ++n){
		const PN w_n = exp(PN(particle[n].log_weight_ - max_weight));
		if(n==0)
			cumsum_w[0] = w_n;
		else
			cumsum_w[n] = cumsum_w[n-1]+w_n;
	}
	
	//boost::random::uniform_real_distribution<Real> rnd_u(0,static_cast<Real>(cumsum_w[N-1]));
#ifdef ENABLE_BOOST_RANDOM
	boost::uniform_real<Real> rnd_u(0,static_cast<Real>(cumsum_w[N-1]));
#else
	const Real cum_weight = static_cast<Real>(cumsum_w[N-1]);
#endif
	std::vector<Particle> new_particle;
	for(int n = 0; n < N; ++n){
#ifdef ENABLE_BOOST_RANDOM
		const Real u = rnd_u(mt_eng_);
#else
		const Real u = mt_eng_.genrand_res53() * cum_weight;
#endif
		const int index = std::lower_bound(cumsum_w.begin(), cumsum_w.end(), PN(u)) - cumsum_w.begin();
		
		new_particle.push_back(particle[index]); // copy the particle
	}
	
	// equal weights
	const Real log_equal_w = -Bayes::log(static_cast<Real>(N));
	for(int n = 0; n < N; ++n){
		particle[n].log_weight_ = log_equal_w;
	}
	
	return N;
}
