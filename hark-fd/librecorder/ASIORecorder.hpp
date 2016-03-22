#ifndef ___ASIORECORDER_HPP___
#define ___ASIORECORDER_HPP___

#include "Recorder.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

extern "C" {
#include "asio.h"
}


class ASIORecorder : public Recorder {
private:
  static boost::scoped_ptr<ASIORecorder> instance;
  ASIOCallbacks callbacks;
  std::vector<ASIOBufferInfo> bufferInfo;
  std::vector<ASIOChannelInfo> channelInfo;
  long bufferSize;
  long inputChannelCount;
  long outputChannelCount;
  std::vector<short> read_buf;
  int read_buf_size;

public:
  ASIORecorder() : read_buf_size(128) {}
  virtual ~ASIORecorder();

  static Recorder* GetInstance();
    
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
    ASIOStop();
  }

  virtual void Close() {
    boost::mutex::scoped_lock lk(mutex_buffer);
    ASIOStop();
    ASIODisposeBuffers();
    ASIOExit();
  }

private:
  static void BufferSwitch(long doubleBufferIndex, ASIOBool directProcess);
  static long ASIOMessages(long selector, long value, void* message, double* opt);
  static void SampleRateDidChanged(ASIOSampleRate sampleRate);
  static ASIOTime* BufferSwitchTimeInfo(ASIOTime* params, long doubleBufferIndex,
					ASIOBool directProcess);
};


#endif
