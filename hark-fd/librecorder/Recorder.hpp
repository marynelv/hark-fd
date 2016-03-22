#ifndef ___RECORDER_HPP___
#define ___RECORDER_HPP___

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <deque>

#include <boost/thread.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/progress.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <unistd.h>

class Recorder {
public:
  enum STATE {RECORDING, PAUSE, STOP};
  enum BUFFER_STATE {OK = 0, OVERRUN = 1, UNDERRUN = 2, STOPPING = 3};

  struct sample_time {
    int sample;
    boost::posix_time::ptime time;
  public:
    sample_time() : sample(0) {}
    sample_time(int s, boost::posix_time::ptime t) : sample(s), time(t) {}
  };
	
public:
  STATE state;
  boost::mutex mutex_buffer;
  boost::circular_buffer<float> buffer;
  size_t buffer_size;
  int channel_count;
  unsigned sampling_rate;
  int cur_time;
  int sleep_time;
  int frame_size;
  std::deque<sample_time> sample_time_queue;

public:
  Recorder() :
    state(PAUSE), buffer_size(1), buffer(1), cur_time(0)
  {
    //cout << "Recorder constructor\n";
  }

  virtual ~Recorder() {}
	
  virtual int Initialize(const std::string& device_name,
                         int channel_count, int sampling_rate,
                         size_t buf_size) = 0;

  virtual void Start() = 0;
  virtual void Pause() = 0;
  virtual void Stop() = 0;
  virtual void Close() = 0;
    
    
  int GetChannelCount() {
    return channel_count;
  }
    
  int GetSamplingRate() {
    return sampling_rate;
  }
    
  int GetCurrentTime() 
  {
    boost::mutex::scoped_lock lk(mutex_buffer);
        
    return cur_time;
  }

  /*
    const circular_buffer<short>& GetBuffer() {
    return buffer;
    }
  */

  template <typename OutputIterator>
  BUFFER_STATE ReadBuffer(int t, int duration,OutputIterator result) 
  {
    boost::mutex::scoped_lock lk(mutex_buffer);
    static int count = 0;
    count++;


    int diff = cur_time - buffer.size() / channel_count;



    if (state == STOP) 
    {
      return STOPPING;
    }
    
    if (t + duration > cur_time) 
    {
      return UNDERRUN;
    }
    //else if (t < cur_time - buffer.size() / channel_count) {
    else if (t < diff) 
    {
      std::cout << "buffer overrun!" << std::endl;
      return OVERRUN;
    }
    else 
    {
		boost::circular_buffer<float>::iterator first = buffer.end();

		advance(first, (t - cur_time) * channel_count);

		boost::circular_buffer<float>::iterator last = first;
		advance(last, duration * channel_count);
		std::copy(first, last, result);
	    return OK;
    }
  }

  template <typename OutputIterator>
  BUFFER_STATE ReadBuffer(int t, 
                          int duration,
			              OutputIterator result,
			              boost::posix_time::ptime& time) 
  {



    BUFFER_STATE state = ReadBuffer(t, duration, result);

    if (state != OK) 
    {
      return state;
    }

    boost::mutex::scoped_lock lk(mutex_buffer);
    int end_time = cur_time - buffer.size() / channel_count;
    std::deque<sample_time>::reverse_iterator it_st;

	int chkflg = 0; 
    for (it_st = sample_time_queue.rbegin();it_st != sample_time_queue.rend(); ++it_st) 
	 {
		if (it_st->sample <= t) 
		{
			chkflg = 1;
			break;
		}
    }


	// chkflg=0の場合は、it_stの値がプラス(++it_st)されるためエラーになる
	if( chkflg != 0 )
	{
		time = it_st->time + boost::posix_time::microseconds((t - it_st->sample) * 1e6f / sampling_rate);
	 }
	else
	{
		//std::cout << "not date-time t=" << t <<  std::endl; 
	}



    //std::cout << "TS " << t << " " << time.time_of_day() << " " << it_st->sample << "\n";
    return state;
  }
};

#endif
