#include <iostream>
#include <string>
#include <stdio.h>

#include "train_VBHMM.h"
#include "track_PF.h"
#include "CsvIO.hpp"

int main(int argc, char** argv){
	if(argc < 2){
		std::cerr << "too few arguments" << std::endl;
		std::cerr << "this input_music.csv (output_probability.csv)" << std::endl;
		
		return 1;
	}
	std::string out_name;
	if(argc > 2){
		out_name = std::string(argv[2]);
	}else{
		out_name = std::string("pf_posterior.csv");
	}
	
	CsvIO<Real> csv_io(argv[1]);
	const Array2D<Real> music_td_original = csv_io.get_data();
	const int T  = music_td_original.rows(), D = music_td_original.cols();
	Array2D<Real> music_td = music_td_original;
	
	HyperparameterVBHMM hp_vbhmm;
	hp_vbhmm.max_iteration_ = 20;
	hp_vbhmm.beta0_ = music_td.rows()*music_td.cols()/50;
	hp_vbhmm.m0_ = 30;
	//hp_vbhmm.a0_ = music_td.rows()*music_td.cols()/100;
	hp_vbhmm.b0_ = 100;
	
	TrainVBHMM vbhmm_MUSIC(hp_vbhmm);
	vbhmm_MUSIC.train(music_td, hp_vbhmm);
	
	Array2D<Real> posterior = vbhmm_MUSIC.p_on_s_td().cast_type<Real>();
	csv_io.write_file(std::string("vbhmm.csv"), posterior);
	
	
	std::cout << "posterior parameters" << std::endl;
	for(int k = 0; k < 2; ++k){
		for(int j = 0; j < 2; ++j){
			printf("alpha(%d,%d) = %f\t",k,j,vbhmm_MUSIC.hat_alpha_kj()(k,j));
		}
		std::cout << std::endl;
	}
	for(int j = 0; j < 2; ++j){
		printf("beta(%d) = %f\t", j, vbhmm_MUSIC.hat_beta_j()[j]);
	}
	std::cout << std::endl;
	for(int j = 0; j < 2; ++j){
		printf("m(%d) = %f\t", j, vbhmm_MUSIC.hat_m_j()[j]);
	}
	std::cout << std::endl;
	for(int j = 0; j < 2; ++j){
		printf("var(%d) = %f\t", j, vbhmm_MUSIC.hat_b_j()[j] / vbhmm_MUSIC.hat_a_j()[j]);
	}
	std::cout << std::endl;
	
	HyperparameterPF hp_pf;
	hp_pf.particle_num_ = 100;
	hp_pf.max_source_num_ = 3;
	hp_pf.local_peak_margin_ = 0.2;
	
	TrackParticleFilter pf_MUSIC(vbhmm_MUSIC, hp_pf);
	pf_MUSIC.update_all(posterior, music_td, hp_pf);
	
	csv_io.write_file(out_name, posterior);
}
