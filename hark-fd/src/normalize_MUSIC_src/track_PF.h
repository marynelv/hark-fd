/*
 * online sound tracking using a particle filter
 */
#ifndef _TRACK_PF_H_
#define _TRACK_PF_H_

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <stdio.h>
#include <sys/time.h>
#include <limits>

#ifdef ENABLE_BOOST_RANDOM
#include <boost/random.hpp>
#else
#include "mt.h"
#endif // ENABLE_BOOST_RANDOM

#include "Configuration.hpp"
#include "Array2D.hpp"
#include "utilities.hpp"
#include "PreciseNumber.hpp"
#include "train_VBHMM.h"

class TrackParticleFilter{
private:
	struct Particle{
		//int is_enabled_;
		Real log_weight_;
		std::vector<int> active_dir_;
		
		Particle():log_weight_(0),active_dir_(){}
		virtual ~Particle(){}
		
		inline bool is_active_dir(const int direction) const {
			return active_dir_.end() != std::find(active_dir_.begin(), active_dir_.end(), direction);
		}
		/*
		template <typename T> inline T weight() const{
			if(is_enabled_>0){
				return T(Bayes::exp(log_weight_));
			}else
				return T(0);
		}
		*/
	};
public:
	explicit TrackParticleFilter(const HyperparameterPF& hyper_param);
	explicit TrackParticleFilter(const TrainVBHMM& vbhmm, const HyperparameterPF& hyper_param);
	virtual ~TrackParticleFilter(){}
	
	void set_parameters(const TrainVBHMM& vbhmm);
	void init(const HyperparameterPF& hyper_param);
	
	void update_online(std::vector<Real>& p_on_d, const std::vector<Real>& music_frame_d, const HyperparameterPF& hyper_param);
	void update_all(Array2D<Real>& p_on_td, const Array2D<Real>& music_spec_td, const HyperparameterPF& hyper_param);
	
	const std::vector<Particle>& particles() const {return particle_vec_;}
	
protected:
	void particle_proposal(std::vector<Particle>& prop_particle, const std::vector<Real>& music_frame_d, const int N, const Real local_peak_margin);
	void state_transition(std::vector<Particle>& next_particle, const std::vector<Particle>& prev_particle, const int D) const;
	void update_weight(std::vector<Particle>& particle, const std::vector<Real>& music_frame_d, const HyperparameterPF& hyper_param) const;
	void normalize_weight(std::vector<Particle>& particle) const;
	
	Real resample(std::vector<Particle>& particle);


private:
	// estimated posterior parameters
	// state transition
	Array2D<Real> hat_alpha_kj_; // j: state at time t, k: state at time t-1
	// observation
	std::vector<Real> hat_beta_j_;
	std::vector<Real> hat_m_j_;
	std::vector<Real> hat_a_j_;
	std::vector<Real> hat_b_j_;
	
	// particles
	std::vector<Particle> particle_vec_;

#ifdef ENABLE_BOOST_RANDOM
	boost::mt19937 mt_eng_;
#else
	MersenneTwister mt_eng_;
#endif // ENABLE_BOOST_RANDOM
};

#endif //_TRACK_PF_H_
