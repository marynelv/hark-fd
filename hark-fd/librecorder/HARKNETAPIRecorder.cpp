#include <iostream>
#include <math.h>
#include <csignal>
#include <fstream>
#include "HARKNETAPIRecorder.hpp"


#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


scoped_ptr<HARKNETAPIRecorder> HARKNETAPIRecorder::instance;
//***************************************************************************************//
// This function is executed in another thread, and
// records acoustic signals into circular buffer.
//***************************************************************************************//
void HARKNETAPIRecorder::operator()()
{
//	read_buf.resize(channel_count * (header_period - 1) * 20);
	read_buf.resize(channel_count * (header_period) * 20);
	vector<char> buf(sample_bytes * channel_count * header_period * 20);
	int read_size    = 0;
	int header_count = 0;

	ptime t_start = microsec_clock::local_time();
	ptime t_cur;

	int ret;

	for (;;) 
	{
		if (state == PAUSE) 
		{
			cout << "RECORDING PAUSE" << endl;
			usleep(sleep_time);
			continue;
		}
		else if (state == STOP) 
		{
			cout << "RECORDING STOP" << endl;
			break;
		}
		else if (state == RECORDING) 
		{
			memset( &buf[0] ,0, buf.size());
			read_size = KcSampleDataRead(h_ws, (char*)&buf[0], buf.size());

			if (read_size <= 0) 
			{
				break;
			}

			int frame_count = read_size / channel_count / sample_bytes;

			int i = 0;
			int j = 0;

			while ( j < frame_count ) 
			{
				int start_rb  = i * channel_count;
				int start_buf = sample_bytes * j * channel_count;
				int end_buf   = start_buf + sample_bytes * channel_count;

				switch (sample_bytes) 
				{
					case 2: //16bit mode
					{
						cout << "16bit mode" << endl;
						int16_t* first = (int16_t*)&buf[start_buf];
						int16_t* last  = (int16_t*)&buf[end_buf];
						copy(first, last, read_buf.begin() + start_rb);
						break;
					}
					case 4: //32bit mode
					{
						int32_t* first = (int32_t*)&buf[start_buf];
						int32_t* last  = (int32_t*)&buf[end_buf];
						copy(first, last, read_buf.begin() + start_rb);
						break;
					}
				}
			
				cur_time++;

				i++;
				j++;

			}//while end


			t_cur = microsec_clock::local_time();
			sample_time_queue.push_back(sample_time(cur_time, t_cur - microseconds(51200)));
			int end_time = cur_time - buffer.size() / channel_count;
			std::deque<sample_time>::iterator it_st;

			for (it_st = sample_time_queue.begin() ; it_st != sample_time_queue.end(); ++it_st) 
			{
				if (it_st->sample >= end_time) 
				{
					break;
				}
			}

			sample_time_queue.erase(sample_time_queue.begin(), it_st);

			vector<float>::iterator it = read_buf.begin();
			advance(it, i * channel_count);
			mutex::scoped_lock lk(mutex_buffer);
			buffer.insert(buffer.end(), read_buf.begin(), it);
			ret= usleep(sleep_time);
			lk.unlock();
		}//if end
	}
	
}



//***************************************************************************************//
// A device for recording is initialized.
//   device_name : Audio device name.  ex. "plughw:0,0"
//   device_type : Audio device type.  ex. "RASP24-32 or RASP24-16"
//   chan_count  : A number of channels. ex. 8 if eight microphones are used.
//   samp_rate   : A frequency of sampling.
//   buf_size    : A size of circular buffer.
//***************************************************************************************//
int HARKNETAPIRecorder::Initialize( const string& device_name,
                                int    chan_count,
                                int    samp_rate,
                                size_t buf_size )
{
	
	mutex::scoped_lock lk(mutex_buffer);

	recorderThread.reset();
	buffer.clear();
	sample_time_queue.clear();

	//read_buf_size=buf_size;
	state         = PAUSE;
	buffer_size   = buf_size;
	channel_count = chan_count;
	sampling_rate = samp_rate;
	buffer.set_capacity(buffer_size * channel_count);
	read_buf.resize(read_buf_size * channel_count);
	cur_time = 0;
	sleep_time = 160 * 1000000 / sampling_rate;

	header_period = 2560;
	sample_bytes = 4;


	/*-----------------------------------------------------------*/
	/* Handle Close                                              */
	/*-----------------------------------------------------------*/

	if (h_ws) 
	{
		KcHandleClose(h_ws);
	}


	/*-----------------------------------------------------------*/
	/* Handle Open                                               */
	/*-----------------------------------------------------------*/
	h_ws = KcHandleOpen((char*)device_name.c_str());

	if (!h_ws) 
	{
		cerr << "Can not open Device.\n";
		return -1;
	}


	/*-----------------------------------------------------------*/
	/* 終了処理                                                  */
	/*-----------------------------------------------------------*/
//	KcADStop(h_ws);
	recorderThread.reset(new thread(ref(*this)));
	return 0;
}



