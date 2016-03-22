#ifndef ___RASPRECORDER_HPP___
#define ___RASPRECORDER_HPP___

#include "Recorder.hpp"

#include <boost/thread.hpp>

#include <fcntl.h>
#include <sys/ioctl.h>
#include "adc16ch.h"

using namespace boost;

class RASPRecorder : public Recorder {
private:
  static RASPRecorder* instance;
  int fd_adc;
  shared_ptr<thread> recorderThread;
  std::vector<short> read_buf;
  int read_buf_size;

public:
  RASPRecorder() : fd_adc(0), read_buf_size(128) {
    //cerr << "RASPRecorder\n";
  }

  static
  Recorder* GetInstance() {
    if (!instance) {
      instance = new RASPRecorder;
    }
    return instance;
  }
    
  void operator()();
  virtual int Initialize(const std::string& device_name,
			 int channel_count, int sampling_rate,
			 size_t buf_size);

  // start recording
  virtual void Start() {
    mutex::scoped_lock lk(mutex_buffer);
    state = RECORDING;
    ioctl(fd_adc, FIO_ADC_CONTROL, ADC_START);
  }
    
  // pause in recording
  virtual void Pause() {
    mutex::scoped_lock lk(mutex_buffer);
    state = PAUSE;
  }
    
  // stop and finish recording
  virtual void Stop() {
    mutex::scoped_lock lk(mutex_buffer);
    state = STOP;
    ioctl(fd_adc, FIO_ADC_CONTROL, ADC_STOP);
  }

  virtual void Close() {
    close(fd_adc);
  }
};


#endif
