#include <iostream>

#include "ASRASPRecorder.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

scoped_ptr<ASRASPRecorder> ASRASPRecorder::instance;

// This function is executed in another thread, and
// records acoustic signals into circular buffer.
void ASRASPRecorder::operator()()
{
  read_buf.resize((frame_samples * channel_count * sample_bytes) * 20);
  vector<char> buf((header_bytes +
                    frame_samples * channel_count * sample_bytes) * 20);
  int buf_read_bytes = 0;
  int read_bytes = 0;
  int sample_count = 0;

  ptime t_start = microsec_clock::local_time();
  ptime t_cur;

  vector<sample_time> sample_time_buf(0);

  for (;;) {
    if (state == PAUSE) {
      usleep(sleep_time);
      continue;
    }
    else if (state == STOP) {
      break;
    }
    else if (state == RECORDING) {
      read_bytes = WsDataRead(h_ws, (char*)&buf[buf_read_bytes],
                              buf.size() - buf_read_bytes);
      if (read_bytes < 0) {
        break;
      }
      buf_read_bytes += read_bytes;
      int sample;
      char* ptr_sample = (char*)&sample;
      int converted_bytes = 0;
      int read_samples = 0;
      int sec = 0;
      int usec = 0;
      int shift = 0;
      for (;;) {
        if (sample_count != 0 &&
            (buf_read_bytes - converted_bytes >=
             sample_bytes * channel_count)) {
          for (int i = 0; i < channel_count; i++) {
            copy(&buf[converted_bytes], &buf[converted_bytes + sample_bytes],
                 ptr_sample + 1);
            sample >>= 8;
            read_buf[read_samples++] = sample;
            converted_bytes += sample_bytes;
          }
          sample_count++;
          if (sample_count == frame_samples) {
            sample_count = 0;
          }
        }
        else if (sample_count == 0 &&
                 (buf_read_bytes - converted_bytes >= header_bytes)) {
          int* header = (int*)&buf[converted_bytes];
          sec = header[1];
          usec = header[2];
          shift = header[3] & 0xff;
          //cout << header[0] << " " << shift << " " << from_time_t(sec) + microseconds(usec) << endl;
          sample_time_buf.
            push_back(sample_time(cur_time + read_samples / channel_count,
                                  from_time_t(sec) + microseconds(usec)));
          converted_bytes += header_bytes;
          sample_count++;
        }
        else {
          break;
        }
      }
      copy(&buf[converted_bytes], &buf[buf_read_bytes], &buf[0]);
      buf_read_bytes -= converted_bytes;

      int end_time = cur_time - buffer.size() / channel_count;
      std::deque<sample_time>::iterator it_st;
      for (it_st = sample_time_queue.begin();
       it_st != sample_time_queue.end(); ++it_st) {
        if (it_st->sample >= end_time - frame_samples) {
          break;
        }
      }

      vector<float>::iterator it = read_buf.begin();
      advance(it, read_samples);
      
      mutex::scoped_lock lk(mutex_buffer);
      sample_time_queue.erase(sample_time_queue.begin(), it_st);
      cur_time += read_samples / channel_count;
      sample_time_queue.insert(sample_time_queue.end(),
                               sample_time_buf.begin(), sample_time_buf.end());
      buffer.insert(buffer.end(), read_buf.begin(), it);
      lk.unlock();
      
      sample_time_buf.clear();
      usleep(sleep_time);
    }
  }
}

// A device for recording is initialized.
//   device_name : Audio device name.  ex. "plughw:0,0"
//   chan_count : A number of channels. ex. 8 if eight microphones are used.
//   samp_rate : A frequency of sampling.
//   buf_size : A size of circular buffer.
int ASRASPRecorder::Initialize(const string& device_name,
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
  int ret = -1;
  
  if (h_ws) {
    WsHandleClose(h_ws);
  }
  h_ws = WsHandleOpen((char*)device_name.c_str());
  if (h_ws) {
    WsHandleClose(h_ws);
  }
    
  h_ws = WsHandleOpen((char*)device_name.c_str());
  if (!h_ws) {
    cerr << "Cannot open Wireless System.\n";
    return -1;
  }

  int target_type;
  WsGetTargetType(h_ws, &target_type);
  if (!(target_type == TARGET_TYPE_AS_RASP)) {
    WsHandleClose(h_ws);
    return -1;
  }
  if (!(channel_count >= 8 && channel_count <= 10)) {
    WsHandleClose(h_ws);
    return -1;
  }
  
  ret = WsSetSamplingFreq(h_ws, sampling_rate);
  if (ret != 0) {
    WsHandleClose(h_ws);
    return -1;
  }
  
  switch (channel_count) {
  case 8:
    ret = WsSetChannelMode(h_ws, AD_8CH_MODE);
    break;
  case 9:
    ret = WsSetChannelMode(h_ws, AD_9CH_MODE);
    break;
  case 10:
    ret = WsSetChannelMode(h_ws, AD_10CH_MODE);
    break;
  default:
    ret = -1;
  }
  if (ret != 0) {
    WsHandleClose(h_ws);
    return -1;
  }

//  ret = WsSetAdDataFormat(h_ws, DATA_FORMAT_24BIT_PCM, 1.0f);
  ret = WsSetAdDataFormat(h_ws, DATA_FORMAT_24BIT_PCM);
  if (ret != 0) {
    WsHandleClose(h_ws);
    return -1;
  }

  WsADStop(h_ws);
  recorderThread.reset(new thread(ref(*this)));

  return 0;
}
