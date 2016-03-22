#ifndef ___ASRASPRECORDER_HPP___
#define ___ASRASPRECORDER_HPP___

#include "Recorder.hpp"
#include "librasp-netapi.h"
#include "as-rasp.h"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

class ASRASPRecorder : public Recorder {
private:
  static boost::scoped_ptr<ASRASPRecorder> instance;
  WS_HANDLE_t* h_ws;
  boost::shared_ptr<boost::thread> recorderThread;
  int frame_samples;
  std::vector<float> read_buf;
  int read_buf_size;
  int sample_bytes;
  int header_bytes;
  boost::posix_time::time_duration latency;

public:
  ASRASPRecorder() : h_ws(0), frame_samples(1024), sample_bytes(3),
                     header_bytes(16), read_buf_size(0) {
    //cerr << "ASRASPRecorder\n";
  }
  virtual ~ASRASPRecorder() {
    if (h_ws) {
      WsADStop(h_ws);
      WsHandleClose(h_ws);
    }
  }

  static
  Recorder* GetInstance() {
    if (!instance) {
      instance.reset(new ASRASPRecorder);
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
    int ret = WsADStart(h_ws);
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
    WsADStop(h_ws);
  }
  
  virtual void Close() {
    if (h_ws) {
      WsADStop(h_ws);
      WsHandleClose(h_ws);
      h_ws = 0;
    }
  }
};


#endif
