#ifndef ___ALSARECORDER_HPP___
#define ___ALSARECORDER_HPP___

#include "Recorder.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#include <alsa/asoundlib.h>

class ALSARecorder : public Recorder {
private:
  static boost::scoped_ptr<ALSARecorder> instance;
  snd_pcm_t* p_pcm;
  boost::shared_ptr<boost::thread> recorderThread;
  std::vector<short> read_buf;
  int read_buf_size;

public:
  ALSARecorder() : p_pcm(0), read_buf_size(128) {
    //cerr << "ALSARecorder\n";
  }

  static
  Recorder* GetInstance() {
    if (!instance) {
      instance.reset(new ALSARecorder);
    }
    return instance.get();
  }
    
  void operator()();
  virtual int Initialize(const std::string& device_name,
			 int channel_count, int sampling_rate,
			 size_t buf_size);

  // start recording
  virtual void Start() {
    boost::mutex::scoped_lock lk(mutex_buffer);
    state = RECORDING;
  }
    
  // pause in recording
  virtual void Pause() {
    boost::mutex::scoped_lock lk(mutex_buffer);
    state = PAUSE;
  }
    
  // stop and finish recording
  virtual void Stop() {
    boost::mutex::scoped_lock lk(mutex_buffer);
    state = STOP;
  }

  virtual void Close() {
  }
};


#endif
