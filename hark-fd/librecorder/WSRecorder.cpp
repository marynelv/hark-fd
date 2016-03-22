#include <iostream>

#include "WSRecorder.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

scoped_ptr<WSRecorder> WSRecorder::instance;

// This function is executed in another thread, and
// records acoustic signals into circular buffer.
void WSRecorder::operator()()
{
  read_buf.resize(channel_count * (header_period - 1) * 20);
  vector<char> buf(sample_bytes * channel_count * header_period * 20);
  //vector<char> buf(sample_bytes * channel_count * 320);
  int read_size = 0;
  int header_count = 0;

  ptime t_start = microsec_clock::local_time();
  ptime t_cur;

  for (;;) {
    if (state == PAUSE) {
      usleep(sleep_time);
      continue;
    }
    else if (state == STOP) {
      break;
    }
    else if (state == RECORDING) {
      read_size = WsSampleDataRead(h_ws, (char*)&buf[0], buf.size());
      //cout << "read_size = " << read_size << " " << buf.size() << endl;
      if (read_size <= 0) {
        break;
      }
      //cout << "before mutexlock at WSRead" << endl;
      //mutex::scoped_lock lk(mutex_buffer);
      //cout << "after mutexlock at WSRead" << endl;
      int frame_count = read_size / channel_count / sample_bytes;
      int i = 0;
      int j = 0;
      while (j < frame_count) {
	if (header_count != 0) {
	  int start_rb = i * channel_count;
	  int start_buf = sample_bytes * j * channel_count;
	  int end_buf = start_buf + sample_bytes * channel_count;
	  
	  switch (sample_bytes) {
	  case 2: {
	    int16_t* first = (int16_t*)&buf[start_buf];
	    int16_t* last = (int16_t*)&buf[end_buf];
	    copy(first, last, read_buf.begin() + start_rb);
	    break;
	  }
	  case 4: {
	    int32_t* first = (int32_t*)&buf[start_buf];
	    int32_t* last = (int32_t*)&buf[end_buf];
	    copy(first, last, read_buf.begin() + start_rb);
	    break;
	  }
	  }
	  //cout << "before cur_time increment. cur_time=" << cur_time << endl;
	  cur_time++;
	  //cout << "after cur_time increment. cur_time=" << cur_time << endl;
	  i++;
	  j++;
	}
	else {
	  j++;
	}

	header_count++;
	if (header_count == header_period) {
	  header_count = 0;
	}
      }
      t_cur = microsec_clock::local_time();
      sample_time_queue.push_back(sample_time(cur_time, t_cur - microseconds(51200)));
      int end_time = cur_time - buffer.size() / channel_count;
      std::deque<sample_time>::iterator it_st;
      for (it_st = sample_time_queue.begin();
	   it_st != sample_time_queue.end(); ++it_st) {
	if (it_st->sample >= end_time) {
	  break;
	}
      }
      sample_time_queue.erase(sample_time_queue.begin(), it_st);
      
      //cout << cur_time << " " << (t_cur - t_start).total_microseconds() - 10500 - 51200 << endl;
      /*
      cout << sample_time_queue.back().sample << " "
	   << sample_time_queue.back().time << endl;
      */
      vector<float>::iterator it = read_buf.begin();
      advance(it, i * channel_count);
      mutex::scoped_lock lk(mutex_buffer);
      buffer.insert(buffer.end(), read_buf.begin(), it);
      lk.unlock(); //2007/01/08
      usleep(sleep_time);
    }
  }
}

// A device for recording is initialized.
//   device_name : Audio device name.  ex. "plughw:0,0"
//   chan_count : A number of channels. ex. 8 if eight microphones are used.
//   samp_rate : A frequency of sampling.
//   buf_size : A size of circular buffer.
int WSRecorder::Initialize(const string& device_name,
                           int chan_count, int samp_rate,
                           size_t buf_size)
{
  mutex::scoped_lock lk(mutex_buffer);

  recorderThread.reset();
  buffer.clear();
  sample_time_queue.clear();

  state = PAUSE;
  buffer_size = buf_size;
  channel_count = chan_count;
  sampling_rate = samp_rate;
  buffer.set_capacity(buffer_size * channel_count);
  read_buf.resize(read_buf_size * channel_count);
  cur_time = 0;
  sleep_time = 160 * 1000000 / sampling_rate;
  
  int result;
  //snd_pcm_t *chandle;

  if (h_ws) {
    WsHandleClose(h_ws);
  }
    
  h_ws = WsHandleOpen((char*)device_name.c_str());
  if (!h_ws) {
    cerr << "Cannot open Wireless System.\n";
    return -1;
  }

  WsSetSamplingFreq(h_ws, sampling_rate);

#ifdef TARGET_TYPE_AS_RASP
  int target_type;
  WsGetTargetType(h_ws, &target_type);
  if (!((target_type == TARGET_TYPE_AS_RASP && channel_count == 8) ||
        (target_type == TARGET_TYPE_AS_RASP && channel_count == 10) ||
        (target_type == TARGET_TYPE_RASP_WL && channel_count == 16))) {
    WsHandleClose(h_ws);
    return -1;
  }
#endif
  
  int ret;
  switch (channel_count) {
#ifdef AD_10CH_MODE  // 無線RASPでは16chモードのみサポート
  case 8:
    header_period = 1024;
    sample_bytes = 4;
    ret = WsSetChannelMode(h_ws, AD_8CH_MODE);
    if (ret != 0) {
      WsHandleClose(h_ws);
      return -1;
    }
    break;
  case 10:
    header_period = 1024;
    sample_bytes = 4;
    ret = WsSetChannelMode(h_ws, AD_10CH_MODE);
    if (ret != 0) {
      WsHandleClose(h_ws);
      return -1;
    }
    break;
#endif
  case 16:
    header_period = 512;
    sample_bytes = 2;
    ret = WsSetChannelMode(h_ws, AD_16CH_MODE);
    if (ret != 0) {
      //cerr << channel_count << " ch input is not supported.\n";
      WsHandleClose(h_ws);
      return -1;
    }
    break;
  default:
    //cerr << channel_count << " ch input is not supported.\n";
    WsHandleClose(h_ws);
    return -1;
  }

  WsADStop(h_ws);
  //WsADStart(h_ws);
  recorderThread.reset(new thread(ref(*this)));

  return 0;
}
