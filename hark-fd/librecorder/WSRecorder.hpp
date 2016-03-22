#ifndef ___WSRECORDER_HPP___
#define ___WSRECORDER_HPP___

#include "Recorder.hpp"
#include "librasp-netapi.h"
#include "as-rasp.h"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

class WSRecorder : public Recorder {
private:
  static boost::scoped_ptr<WSRecorder> instance;
  WS_HANDLE_t* h_ws;
  boost::shared_ptr<boost::thread> recorderThread;
  int header_period;
  std::vector<float> read_buf;
  int read_buf_size;
  int sample_bytes;
  boost::posix_time::time_duration latency;

public:
  WSRecorder() : h_ws(0), header_period(0), sample_bytes(2), read_buf_size(0) {
    //cerr << "WSRecorder\n";
  }
  virtual ~WSRecorder() {
    if (h_ws) {
      WsADStop(h_ws);
      WsHandleClose(h_ws);
    }
  }

  static
  Recorder* GetInstance() {
    if (!instance) {
      instance.reset(new WSRecorder);
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
