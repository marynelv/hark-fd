#ifndef _HARKPARAMSOBJECTS_H_
#define _HARKPARAMSOBJECTS_H_

#include "Object.h"
#include "ObjectParser.h"
#include "Source.h"
#include "misc.h"
#include <cmath>

using namespace FD;
////////////////////////////////////////////////////////////////////
//
// LocalizeMUSIC
//
////////////////////////////////////////////////////////////////////

class ParamsLocalizeMUSIC : public Object{

 public:
  int num_source;
  int min_deg;
  int max_deg;
  int lower_bound_frequency;
  int upper_bound_frequency;

  ParamsLocalizeMUSIC(){
  }

  ParamsLocalizeMUSIC(const ParamsLocalizeMUSIC& input) {
    num_source = input.num_source;
    min_deg = input.min_deg;
    max_deg = input.max_deg;
    lower_bound_frequency = input.lower_bound_frequency;
    upper_bound_frequency = input.upper_bound_frequency;
  }

  ~ParamsLocalizeMUSIC(){
  }

  /**Serialize (binary) the object to a stream*/
  virtual void serialize(std::ostream &out) const {
  }

  /**Unserialize (binary) the object from a stream*/
  virtual void unserialize(std::istream &in){
  }

  /**Generic print function*/
  virtual void printOn(std::ostream &out=std::cout) const {
  }

  /**Generic read function*/
  virtual void readFrom(std::istream &in=std::cin){
  }

};


////////////////////////////////////////////////////////////////////
//
// SourceTracker
//
////////////////////////////////////////////////////////////////////

class ParamsSourceTracker : public Object{

 public:
  float thresh;
  float pause_length;
  float min_src_interval;
  float min_tfindex_interval;
  Source::CompareMode compare_mode;

  ParamsSourceTracker(){
  }

  ParamsSourceTracker(const ParamsSourceTracker& input) {
    thresh = input.thresh;
    pause_length = input.pause_length;
    min_src_interval = input.min_src_interval;
    min_tfindex_interval = input.min_tfindex_interval;
    compare_mode = input.compare_mode;
  }

  ~ParamsSourceTracker(){
  }

  /**Serialize (binary) the object to a stream*/
  virtual void serialize(std::ostream &out) const {
  }

  /**Unserialize (binary) the object from a stream*/
  virtual void unserialize(std::istream &in){
  }

  /**Generic print function*/
  virtual void printOn(std::ostream &out=std::cout) const {
  }

  /**Generic read function*/
  virtual void readFrom(std::istream &in=std::cin){
  }

};

////////////////////////////////////////////////////////////////////
//
// HRLE
//
////////////////////////////////////////////////////////////////////

class ParamsHRLE : public Object{

 public:
  float lx;
  float time_constant;

  ParamsHRLE(){
  }

  ParamsHRLE(const ParamsHRLE& input) {
    lx = input.lx;
    time_constant = input.time_constant;
  }

  ~ParamsHRLE(){
  }

  /**Serialize (binary) the object to a stream*/
  virtual void serialize(std::ostream &out) const {
  }

  /**Unserialize (binary) the object from a stream*/
  virtual void unserialize(std::istream &in){
  }

  /**Generic print function*/
  virtual void printOn(std::ostream &out=std::cout) const {
  }

  /**Generic read function*/
  virtual void readFrom(std::istream &in=std::cin){
  }

};



#endif
