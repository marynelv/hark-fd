/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#define NDEBUG

#include <iostream>
#include <BufferedNode.h>
#include <Buffer.h>
#include <Vector.h>
#include <Matrix.h>
#include <math.h>
#include <Source.h>
#include <Tensor.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "spline.hpp"
#include <deque> 

#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../config.h"

#include "normalize_MUSIC_src/Configuration.hpp"
#include "normalize_MUSIC_src/train_VBHMM.h"
#include "normalize_MUSIC_src/track_PF.h"

#ifdef ENABLE_HARKIO

using namespace std;
using namespace boost;
using namespace FD;
using namespace boost::numeric;
using namespace boost::posix_time;

class NormalizeMUSIC;

DECLARE_NODE(NormalizeMUSIC);
/*Node
 *
 * @name NormalizeMUSIC
 * @category HARK:Localization
 * @description normalize the MUSIC spectrum produced by LocalizeMUSIC module
 * 
 * @input_name SOURCES_IN
 * @input_type Vector<ObjectRef>
 * @input_description 'OUTPUT' of LocalizeMUSIC
 *
 * @input_name MUSIC_SPEC
 * @input_type Vector<float>
 * @input_description 'SPECTRUM' of LocalizeMUSIC
 *
 * @output_name SOURCES_OUT
 * @output_type Vector<ObjectRef>
 * @output_description Source location with normalized MUSIC spectrum. The type of an element of the vector is Source.
 *
 * @output_name 01_SPEC
 * @output_type Vector<float>
 * @output_description Normalized MUSIC spectrum between [0 1].
 *
 * @parameter_name SHOW_ALL_PARAMETERS
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list false:true
 * @parameter_description Enable to access all parameters.
 * 
 * @parameter_name INITIAL_THRESHOLD
 * @parameter_type float
 * @parameter_value 30
 * @parameter_description Threshold on MUSIC spectrum to trigger the first parameter estimation.
 *
 * @parameter_name ACTIVE_PROP
 * @parameter_type float
 * @parameter_value 0.05
 * @parameter_valid SHOW_ALL_PARAMETERS=true
 * @parameter_description Threshold on proportion of active time-direction cells to trigger parameter estimation.
 * 
 * @parameter_name UPDATE_INTERVAL
 * @parameter_type int
 * @parameter_value 10
 * @parameter_valid SHOW_ALL_PARAMETERS=true
 * @parameter_description Number of MUSIC spectra to update the parameter used for the normalization.
 *  
 * @parameter_name PRIOR_WEIGHT
 * @parameter_type float
 * @parameter_value 0.02
 * @parameter_valid SHOW_ALL_PARAMETERS=true
 * @parameter_description Weight of prior on the initial threshold.
 *
 * @parameter_name MAX_SOURCE_NUM
 * @parameter_type int
 * @parameter_value 2
 * @parameter_valid SHOW_ALL_PARAMETERS=true
 * @parameter_description Maximum number of sources that one particle can have.
 * 
 * @parameter_name PARTICLE_NUM
 * @parameter_type int
 * @parameter_value 100
 * @parameter_valid SHOW_ALL_PARAMETERS=true
 * @parameter_description Number of particles to calculate the normalized MUSIC spectrum.
 * 
 * @parameter_name LOCAL_PEAK_MARGIN
 * @parameter_type float
 * @parameter_value 0
 * @parameter_valid SHOW_ALL_PARAMETERS=true
 * @parameter_description Difference allowed to obtain local peaks in the original MUSIC spectrum.
 *
END*/

class NormalizeMUSIC : public BufferedNode {
  // ID's for Input/Output
  int source_in_id, music_spec_id, source_out_id, spec_01_id;
  
  // module parameters
  float initial_threshold, active_prop, prior_weight, local_peak_margin;
  int update_interval, max_source_num, particle_num;
  
  // configurations for parameter estimation and source detection
  HyperparameterVBHMM hp_vbhmm_;
  HyperparameterPF hp_pf_;

  // variables for sequential update
  Array2D<Real> music_spec_buffer_td_;
  int update_counter_;
  
  TrainVBHMM vbhmm_MUSIC_;
  TrackParticleFilter pf_MUSIC_;
  
public:
  NormalizeMUSIC(string nodeName, ParameterSet params) 
   : BufferedNode(nodeName, params), vbhmm_MUSIC_(hp_vbhmm_), pf_MUSIC_(hp_pf_){
    // get ID
    source_in_id = addInput("SOURCES_IN");
    music_spec_id = addInput("MUSIC_SPEC");
    source_out_id = addOutput("SOURCES_OUT");
    spec_01_id = addOutput("01_SPEC");
    
    // get parameters
    initial_threshold = dereference_cast<float>(parameters.get("INITIAL_THRESHOLD"));
    active_prop = dereference_cast<float>(parameters.get("ACTIVE_PROP"));
    update_interval = dereference_cast<int>(parameters.get("UPDATE_INTERVAL"));
    prior_weight = dereference_cast<float>(parameters.get("PRIOR_WEIGHT"));
    max_source_num = dereference_cast<int>(parameters.get("MAX_SOURCE_NUM"));
    particle_num = dereference_cast<int>(parameters.get("PARTICLE_NUM"));
    local_peak_margin = dereference_cast<float>(parameters.get("LOCAL_PEAK_MARGIN"));
    
    // configuration parameters
    hp_vbhmm_.m0_ = initial_threshold;
    hp_vbhmm_.active_proportion_ = active_prop;
    if(hp_vbhmm_.active_proportion_ < 0) hp_vbhmm_.active_proportion_ = 0;
    if(hp_vbhmm_.active_proportion_ > 1) hp_vbhmm_.active_proportion_ = 1;
    
    if(update_interval < 10) update_interval = 10;
    
    hp_pf_.max_source_num_ = max_source_num;
    if(hp_pf_.max_source_num_ < 1) hp_pf_.max_source_num_ = 1;
    hp_pf_.particle_num_ = particle_num;
    if(hp_pf_.max_source_num_ < 10) hp_pf_.particle_num_ = 10;
    
    hp_pf_.local_peak_margin_ = local_peak_margin;
    if(hp_pf_.local_peak_margin_ < 0) hp_pf_.local_peak_margin_ = 0;
    
    update_counter_ = 0;
  }

  virtual void initialize()
  {
    outputs[source_out_id].lookAhead
      = outputs[spec_01_id].lookAhead
      = 1 + max(outputs[source_out_id].lookAhead, outputs[spec_01_id].lookAhead);

    outputs[source_out_id].lookBack
      = outputs[spec_01_id].lookBack
      = 1 + max(outputs[source_out_id].lookBack, outputs[spec_01_id].lookBack);
    
    this->BufferedNode::initialize();
  }
  
  void calculate(int output_id, int count, Buffer &out){
    // fetch the input
    RCPtr<Vector<float> > in_music_spec_ptr = getInput(music_spec_id, count);
    RCPtr<Vector<ObjectRef> > in_src_ptr = getInput(source_in_id, count);
    
    const int T = update_interval, D = (*in_music_spec_ptr).size();
    if(count == 0){ // first frame (initialization with dynamic parameters)
      // modify the 
      hp_vbhmm_.beta0_ = prior_weight * static_cast<float>(T*D);
      if(hp_vbhmm_.beta0_ < 0) hp_vbhmm_.beta0_ = 1;
      
      vbhmm_MUSIC_ = TrainVBHMM(hp_vbhmm_); // overwrite with the new parameters
      pf_MUSIC_.set_parameters(vbhmm_MUSIC_);
      pf_MUSIC_.init(hp_pf_);
      
      music_spec_buffer_td_ = Array2D<Real>::Zero(T,D); // resize(T,D) with zero values
    }
    
    // set the output
    RCPtr<Vector<ObjectRef> > out_src_ptr(new Vector<ObjectRef>(*in_src_ptr)); // copy the input
    RCPtr<Vector<float> > out_01_spec_ptr(new Vector<float>(*in_music_spec_ptr));
    (*(outputs[source_out_id].buffer))[count] = out_src_ptr;
    (*(outputs[spec_01_id].buffer))[count] = out_01_spec_ptr;
    
    
    // check if MUSIC spectrum is calculated in this frame
    bool music_spec_available = false;
    for(int d = 0; d < D; ++d){
      if((*in_music_spec_ptr)[d] > 0){ // see if the value is non-zero (implemented as positive here)
        music_spec_available = true;
        break;
      }
    }
    
    if(music_spec_available){
      // write into the container
      std::vector<Real> music_frame_d(D);
      for(int d = 0; d < D; ++d){
        music_frame_d[d] = (*in_music_spec_ptr)[d]; // copy the input frame into a compatible container
        music_spec_buffer_td_(update_counter_, d) = music_frame_d[d];
      }
      
      ++update_counter_;
      
      // check if the parameter should be updated
      if(update_counter_ >= T){
        const int skipped_init = vbhmm_MUSIC_.train(music_spec_buffer_td_, hp_vbhmm_);
        pf_MUSIC_.set_parameters(vbhmm_MUSIC_);
        
        if(skipped_init != 1)
          std::cerr << "NormalizeMUSIC: parameter updated\n" << std::endl;
        update_counter_=0; // reset the counter
      }
      
      // source detection by a particle filter
      std::vector<Real> spec_01_frame(D,0);
      pf_MUSIC_.update_online(spec_01_frame, music_frame_d, hp_pf_);
      
      // write to the output
      for(int d = 0; d < D; ++d){
        (*out_01_spec_ptr)[d] = spec_01_frame[d];
      }
      
      // overwite the Source information
      for(int n = 0, N = (*out_src_ptr).size(); n < N; ++n){
        Source& src_n = object_cast<Source>((*out_src_ptr)[n]);
        
        const int R = (src_n.tfindex_max[Source::TFRange]+1);
        const int DR = (src_n.tfindex_max[Source::TFDirection]+1)*R;
        const int index = src_n.tfindex[Source::TFHeight]*DR + src_n.tfindex[Source::TFDirection]*R + src_n.tfindex[Source::TFRange];
        
        // overwrite the power with normalized vale
        src_n.power = spec_01_frame[index];
      }
    }else{
      // nothing to do
      // output spectrum is identical to the input: all zero
    }
  }
};

#endif
