#include <iostream>
#include <math.h>
#include <csignal>
#include <fstream>
#include "RASPLCRecorder.hpp"


#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;


scoped_ptr<RASPLCRecorder> RASPLCRecorder::instance;
//***************************************************************************************//
// This function is executed in another thread, and
// records acoustic signals into circular buffer.
//***************************************************************************************//
void RASPLCRecorder::operator()()
{
	int read_size;                         // a read frame size
	vector<char> buf(928);

	ptime t_start = microsec_clock::local_time();
	ptime t_cur;
	
//	int loop=0;

	for (;;) 
	{
//		cout << "RASPLCRecorder START" << endl;

		usleep(sleep_time);
		mutex::scoped_lock lk(mutex_buffer);

		if (state == PAUSE) 
		{
		  continue;
		}
		else if (state == STOP) 
		{
		  break;
		}
		else if (state == RECORDING) 
		{
			//928byte固定でrecv
			read_size = WsLcDataRead(h_ws, (char*)&buf[0], LC_LOCAL_BUF_SIZE); 


			if (read_size > 0 ) 
			{
				cur_time += (read_size/channel_count/4);

				int j=0;

				while(j < LC_SAMPLE_NUM )
				{
					int start_rb  = j * channel_count;
					int start_buf = sample_bytes * j * channel_count;
					int end_buf   = start_buf + sample_bytes * channel_count;

					int32_t* first = (int32_t*)&buf[start_buf];
					int32_t* last  = (int32_t*)&buf[end_buf];
					copy(first, last, read_buf.begin() + start_rb);

					j++;
				}

				t_cur = microsec_clock::local_time();
				sample_time_queue.push_back(sample_time(cur_time, t_cur - microseconds(51200)));
				int end_time = cur_time - buffer.size() / channel_count;
//				cout << "!! cur_time="  << cur_time << " : t=" << sample_time_queue[loop++].time << endl;
				std::deque<sample_time>::iterator it_st;

				for (it_st = sample_time_queue.begin(); it_st != sample_time_queue.end(); ++it_st) 
				{
					if (it_st->sample >= end_time) 
					{
						break;
					}
				}

				sample_time_queue.erase(sample_time_queue.begin(), it_st);
//				mutex::scoped_lock lk(mutex_buffer);
				vector<float>::iterator it = read_buf.begin();
				advance(it, 232 );
				buffer.insert(buffer.end(), read_buf.begin(), it);
//				lk.unlock(); //2007/01/08
			}
			else 
			{
				cerr << "read error!\n";
			}
		}

		lk.unlock();
	}
	
	cout << "RASPLCRecorder::operator END" << endl;
	
}





//***************************************************************************************//
// A device for recording is initialized.
//   device_name : Audio device name.  ex. "plughw:0,0"
//   device_type : Audio device type.  ex. "RASP-LC"
//   chan_count  : A number of channels. ex. 4 if eight microphones are used.
//   samp_rate   : A frequency of sampling.
//   buf_size    : A size of circular buffer.
//***************************************************************************************//
int RASPLCRecorder::Initialize( const string& device_name,
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
	channel_count = chan_count; // 4ch fixity
	sampling_rate = samp_rate;  // 16000Hz fixity
	buffer.set_capacity(buffer_size * channel_count);
	read_buf.resize(read_buf_size * channel_count);
	cur_time = 0;
//	sleep_time = 160 * 1000000 / sampling_rate;
	sleep_time = 1000000 / sampling_rate;

	header_period = 2560;
	sample_bytes = 4;


	cout << "RASPLCRecorder::Initialize START"  << endl;
	cout << "buffer_size="   << buf_size << endl;
	cout << "channel_count=" << chan_count << endl;
	cout << "sampling_rate=" << samp_rate << endl;
	cout << "sample_bytes="  << sample_bytes << endl;
	cout << "read_buf="      << read_buf.size() << endl;


	/*-----------------------------------------------------------*/
	/* Handle Close                                              */
	/*-----------------------------------------------------------*/

	if (h_ws) 
	{
//		cout << "@ WsLcHandleClose"  << endl;
		WsLcHandleClose(h_ws);
	}


	/*-----------------------------------------------------------*/
	/* Handle Open                                               */
	/* in: ip addr
	/*-----------------------------------------------------------*/
//	cout << "@ WsLcHandleOpen"  << endl;

	h_ws = WsLcHandleOpen((char*)device_name.c_str());

	if (!h_ws) 
	{
		cout << "Cannot open Rasp-LC : ip = " << device_name.c_str() << endl; 
		return -1;
	}



	/*-----------------------------------------------------------*/
	/* Get Frame size                                            */
	/*    out: frame size                                        */              
	/*-----------------------------------------------------------*/
//	cout << "@ WsLcGetFrameSize"  << endl;

	int ret;
	ret = WsLcGetFrameSize(h_ws , &local_buf_size );

	if (ret == -1) 
	{
		cerr << "Cannot get frame size.\n";
		return -1;
	}




	/*-----------------------------------------------------------*/
	/* 終了処理                                                  */
	/*-----------------------------------------------------------*/
//	cout << "@ WsLcADStop"  << endl;
	//WsLcADStop(h_ws);
	recorderThread.reset(new thread(ref(*this)));

	return 0;


}



