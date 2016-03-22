#include <iostream>
#include <string>
#include <stdio.h>

#include "train_VBHMM.h"
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
		out_name = std::string("music_post.csv");
	}
	
	CsvIO<Real> csv_io(argv[1]);
	const Array2D<Real> music_td = csv_io.get_data();
	
	HyperparameterVBHMM hp_vbhmm;
	hp_vbhmm.m0_ = 25;
	hp_vbhmm.b0_ = 1;
	
	TrainVBHMM vbhmm_MUSIC(hp_vbhmm);
	vbhmm_MUSIC.train(music_td, hp_vbhmm);
	
	Array2D<Real> posterior = vbhmm_MUSIC.p_on_s_td().cast_type<Real>();
	csv_io.write_file(out_name, posterior);
	
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
		printf("var(%d) = %f\t", j, vbhmm_MUSIC.hat_a_j()[j] / vbhmm_MUSIC.hat_b_j()[j]);
	}
	std::cout << std::endl;
	
	// 2nd training
	//*
	vbhmm_MUSIC.train(music_td, hp_vbhmm);
	posterior = vbhmm_MUSIC.p_on_s_td().cast_type<Real>();
	csv_io.write_file(std::string("2nd_training.csv"), posterior);
	
	std::cout << "2nd posterior parameters" << std::endl;
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
		printf("var(%d) = %f\t", j, vbhmm_MUSIC.hat_a_j()[j] / vbhmm_MUSIC.hat_b_j()[j]);
	}
	std::cout << std::endl;
	//*/
	
	return 0;
}
