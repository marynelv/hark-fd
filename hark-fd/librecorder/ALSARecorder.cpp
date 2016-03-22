#include <iostream>

#include "ALSARecorder.hpp"

#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;

scoped_ptr<ALSARecorder> ALSARecorder::instance;

// This function is executed in another thread, and
// records acoustic signals into circular buffer.
void ALSARecorder::operator()()
{
  int read_size;                         // a read frame size

  for (;;) {
    // should wait during less than read_buf_size / sampling_rate ms
    usleep(sleep_time);

    mutex::scoped_lock lk(mutex_buffer);

    if (state == PAUSE) {
      continue;
    }
    else if (state == STOP) {
      break;
    }
    else if (state == RECORDING) {
      read_size = snd_pcm_readi(p_pcm, &read_buf[0], frame_size);

      if (read_size > 0 && read_size <= frame_size) {
	cur_time += read_size;
	//cout << cur_time << endl;

	buffer.insert(buffer.end(),
		      read_buf.begin(),
		      read_buf.begin()
		      + read_size * channel_count);
                
	if (read_size != frame_size) {
	  snd_pcm_wait(p_pcm, 500);
	}
      }
      else if (read_size == -EAGAIN) {
	snd_pcm_wait(p_pcm, 500);
      }
      else {
	//cerr << "error!\n";
      }
    }
    lk.unlock();
  }
}

// A device for recording is initialized.
//   device_name : Audio device name.  ex. "plughw:0,0"
//   chan_count : A number of channels. ex. 8 if eight microphones are used.
//   samp_rate : A frequency of sampling.
//   buf_size : A size of circular buffer.
int ALSARecorder::Initialize(const string& device_name,
                             int chan_count, int samp_rate,
                             size_t buf_size)
{
  mutex::scoped_lock lk(mutex_buffer);

  buffer_size = buf_size;
  channel_count = chan_count;
  sampling_rate = samp_rate;
  buffer.set_capacity(buffer_size * channel_count);
  buffer.clear();
  cur_time = 0;
  read_buf.resize(read_buf_size * channel_count);
  sleep_time = read_buf_size * 1000000 / sampling_rate / 3;
  frame_size = read_buf.size() / channel_count;
	
  int result;
  //snd_pcm_t *chandle;

  if (p_pcm != 0) {
    snd_pcm_close(p_pcm);
  }

  result = snd_pcm_open(&p_pcm, device_name.c_str(),
			SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK);
  if (result < 0) {
    cerr << "cannot open audio device "
	 << device_name << " (" << snd_strerror(result) << ")\n";
    exit(1);
  }

  snd_pcm_hw_params_t* hw_params;

  //領域確保
  result = snd_pcm_hw_params_malloc(&hw_params);
  if (result < 0) {
    cerr << "cannot allocate hardware parameter structure ("
	 << snd_strerror(result) << ")\n";
    exit(1);
  }
  
  //デフォルト値を取得？
  result = snd_pcm_hw_params_any(p_pcm, hw_params);
  if (result < 0) {
    cerr << "cannot initialize hardware parameter structure ("
	 << snd_strerror(result) << ")\n";
    exit(1);
  }
	
  //interleavedモードでアクセス
  result = snd_pcm_hw_params_set_access(p_pcm, hw_params,
					SND_PCM_ACCESS_RW_INTERLEAVED);
  if (result < 0) {
    cerr << "cannot set access type (" << snd_strerror(result) << ")\n";
    exit(1);
  }
	
  //符号あり16ビットのリトルエンディアン形式で保存
  result = snd_pcm_hw_params_set_format(p_pcm, hw_params,
					SND_PCM_FORMAT_S16_LE);
  if (result < 0) {
    cerr << "cannot set sample format (" << snd_strerror(result) << ")\n";
    exit(1);
  }
	
  //サンプリング周波数を設定
  //dirの値が何かは不明
  int dir = 0;
  result = snd_pcm_hw_params_set_rate_near(p_pcm, hw_params,
					   &sampling_rate, &dir);
  if (result < 0) {
    cerr << "cannot set sample rate (" << snd_strerror(result) << ")\n";
    exit(1);
  }
  
  //チャンネル数を設定
  result = snd_pcm_hw_params_set_channels(p_pcm, hw_params, channel_count);
  if (result < 0) {
    cerr << "cannot set channel count (" << snd_strerror(result) << ")\n";
    exit(1);
  }
  
  //period timeを設定
  unsigned buffer_time = 500000;
  unsigned period_time = buffer_time / 4;
  result = snd_pcm_hw_params_set_period_time_near(p_pcm, hw_params,
						  &period_time, 0);
  if (result < 0){
    cerr << "cannot set period time near\n";
    exit(1);
  }

  //buffer timeを設定
  result = snd_pcm_hw_params_set_buffer_time_near(p_pcm, hw_params,
						  &buffer_time, 0);
  if (result < 0){
    cerr << "cannot set buffer time near\n";
    exit(1);
  }

  //以上のパラメータを設定
  result = snd_pcm_hw_params(p_pcm, hw_params);
  if (result < 0) {
    cerr << "cannot set parameters (" << snd_strerror(result) << ")\n";
    exit(1);
  }
  
  //領域解放
  snd_pcm_hw_params_free(hw_params);

  result = snd_pcm_prepare(p_pcm);
  if (result < 0) {
    cerr << "cannot prepare audio interface for use ("
	 << snd_strerror(result) << ")\n";
    exit(1);
  }

  recorderThread.reset(new thread(ref(*this)));

  return result;
}
