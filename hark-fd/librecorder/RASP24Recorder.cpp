#include <iostream>
#include <math.h>
#include <stdio.h>
#include "RASP24Recorder.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

scoped_ptr<RASP24Recorder> RASP24Recorder::instance;

//***************************************************************************************//
// This function is executed in another thread, and
// records acoustic signals into circular buffer.
//***************************************************************************************//
void RASP24Recorder::operator()()
{
	read_buf.resize(channel_count * (header_period - 1) * 20);
	vector<char> buf(sample_bytes * channel_count * header_period * 20);
	int read_size    = 0;
	int header_count = 0;

	ptime t_start = microsec_clock::local_time();
	ptime t_cur;

cout << "RASP24Recorder::operator START" << endl;
cout << "  channel_count" << channel_count << endl;
cout << "  header_period" << header_period << endl;
cout << "  sample_bytes" << sample_bytes << endl;



	for (;;) 
	{
		if (state == PAUSE) 
		{
		  // cout << "RECORDING PAUSE" << endl;
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
		  // cout << "RECORDING START" << endl;

			read_size = WsSampleDataRead(h_ws, (char*)&buf[0], buf.size());
			
//			cout << "read_size = " << read_size << " " << buf.size() << endl;

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

				//cout << "before cur_time increment. cur_time=" << cur_time << endl;

				cur_time++;

				//cout << "after cur_time increment. cur_time=" << cur_time << endl;

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

			// cout << cur_time << " " << (t_cur - t_start).total_microseconds() - 10500 - 51200 << endl;
			/*
			cout << sample_time_queue.back().sample << " "
			<< sample_time_queue.back().time << endl;
			*/

			vector<float>::iterator it = read_buf.begin();

			advance(it, i * channel_count);

			mutex::scoped_lock lk(mutex_buffer);

			buffer.insert(buffer.end(), read_buf.begin(), it);

			lk.unlock();

			usleep(sleep_time);
		}//if end
	}//for end

cout << "RASP24Recorder::operator END" << endl;

}



//***************************************************************************************//
// A device for recording is initialized.
//   device_name : Audio device name.  ex. "plughw:0,0"
//   device_type : Audio device type.  ex. "RASP24-32 or RASP24-16"
//   chan_count  : A number of channels. ex. 8 if eight microphones are used.
//   samp_rate   : A frequency of sampling.
//   buf_size    : A size of circular buffer.
//***************************************************************************************//
int RASP24Recorder::Initialize( const string& device_name,
                                int    chan_count,
                                int    samp_rate,
                                size_t buf_size )
{
	mutex::scoped_lock lk(mutex_buffer);

	recorderThread.reset();
	buffer.clear();
	sample_time_queue.clear();

	state         = PAUSE;
	buffer_size   = buf_size;
	channel_count = chan_count;
	sampling_rate = samp_rate;
	int ret;

	string  dev_name;
	string  dev_type;
	string  dev_gain;
	int     ad_gain=0; 
	int     dev_point=0;
	int  gain[RASP24_MAX_CH_NUM*RASP24_MAX_BOARD];
	int  count;



	buffer.set_capacity(buffer_size * channel_count);
	read_buf.resize(read_buf_size * channel_count);
	cur_time = 0;
	sleep_time = 160 * 1000000 / sampling_rate;

	header_period = 1024;




	cout << "buffer_size="   << buf_size << endl;
	cout << "channel_count=" << chan_count << endl;
	cout << "sampling_rate=" << samp_rate << endl;



	/*------------------------------------------------------------------*/
	/*  device_name から　device　と device type と gain を分割して取出す */
	/*------------------------------------------------------------------*/
	string tmp;
	dev_point = device_name.find(",");

	if( dev_point == string::npos )
	{
		cerr << "Cannot Get Device Name.\n";
		return -1;
	}
	
	dev_name = device_name.substr(0 , dev_point); //カンマ前の文字列
	tmp = device_name.substr(dev_point+1);        //カンマ後の文字列

	dev_type = tmp.substr(0,9);
	dev_gain = tmp.substr(10);
	ad_gain = atoi(dev_gain.c_str());
	

	cout << "device_name" << device_name << endl;	
	cout << "dev_name=" << dev_name << endl;
	cout << "dev_type=" << dev_type << endl;
	cout << "dev_gain=" << dev_gain << endl;
	cout << "ad_gain=" << ad_gain << endl;






	/*-----------------------------------------------------------*/
	/* Handle Cesck                                              */
	/*-----------------------------------------------------------*/
	if (h_ws) 
	{
		WsHandleClose(h_ws);
	}


	/*-----------------------------------------------------------*/
	/* Handle Open                                               */
	/*-----------------------------------------------------------*/
	h_ws = WsHandleOpen((char*)dev_name.c_str());

	if (!h_ws) 
	{
		cerr << "Cannot open RASP24 System.\n";
		return -1;
	}

	cout << "[OK] WsHandleOpen" << endl;

	/*-----------------------------------------------------------*/
	/* サンプリング周波数設定                                    */
	/*-----------------------------------------------------------*/
	ret = WsSetSamplingFreq(h_ws, sampling_rate);

	if( ret != 0 )
	{
		cerr << "Cannot Set Sampling Frequency.\n";
		WsHandleClose(h_ws);
		return -1;
	}

	cout << "[OK] WsSetSamplingFreq" << endl;


	/*-----------------------------------------------------------*/
	/* 接続相手の識別コード取得                                  */
	/*-----------------------------------------------------------*/
	int target_type;
	ret = WsGetTargetType(h_ws, &target_type);

	if( ret != 0 )
	{
		cerr << "Cannot Get Target Type.\n";
		WsHandleClose(h_ws);
		return -1;
	}

	cout << "[OK] WsSetSamplingFreq=" << target_type << endl;

	if( target_type != TARGET_TYPE_RASP24 )
	{
		cerr << "Target Type  Not RASP24.\n";
		WsHandleClose(h_ws);
		return -1;
	}



	/*-----------------------------------------------------------*/
	/* A/Dデータフォーマット設定                                 */
	/*-----------------------------------------------------------*/
	if( dev_type == "RASP24-16" )
	{
		cout << "dev_type == RASP24-16 "  << endl;
		ret = WsSetAdDataFormat(h_ws , DATA_FORMAT_16BIT_PCM);
		sample_bytes = 2;
	}
	else
	{
		cout << "dev_type == RASP24-32 "  << endl;
		ret = WsSetAdDataFormat(h_ws , DATA_FORMAT_32BIT_PCM);
		sample_bytes = 4;
	}

	if( ret != 0 )
	{
		cerr << "Cannot Set A/D Data Format: sample_bytes=" << sample_bytes << endl;
		WsHandleClose(h_ws);
		return -1;
	}

	cout << "[OK] WsSetAdDataFormat : sample_bytes=" << sample_bytes << endl;
	



	/*-----------------------------------------------------------*/
	/* chan_count から 同時サンプリング数を取得                  */
	/*-----------------------------------------------------------*/
	int board_num=0;
	int board_no =0;
	int max_bord_num=0;
	int mode9ch = 0;
	if(chan_count % 9 == 0){
	  board_num = abs((chan_count-1)/9); //使用ボード番号取得
	  mode9ch = 1;
	}else{
	  board_num = abs((chan_count-1)/8); //使用ボード番号取得
	}

	cout << "board_num=" << board_num << endl;

	ret=WsGetBoardNum(h_ws , &max_bord_num);
	if( ret != 0 )
	{
		cerr << "Cannot Get Board Num.\n";
		WsHandleClose(h_ws);
		return -1;
	}
        if( board_num < 0 )
        {
                cerr << "Number of channels was lower than zero.\n";
		WsHandleClose(h_ws);
		return -1;
        }
        else if( max_bord_num <= board_num )
        {
                cerr << "Number of channels has exceeded the upper limit of hardware.\n";
                WsHandleClose(h_ws);
	        return -1;
        }
	

		cout << "[OK] WsGetBoardNum max_bord_num=" << max_bord_num << endl;




	for(board_no=0 ; board_no < board_num+1 ; board_no++)
	{
	  if(mode9ch == 1)
	    ret = WsSetCaptureChannels(h_ws, board_no, AD_RASP24AD_CAPTURE_CHANNELS);
	  else
	    ret = WsSetCaptureChannels(h_ws, board_no, AD_8CH_MODE_CAPTURE_CHANNELS);	    

		if( ret != 0 )
		{
		        cerr << "Cannot Set Capture Channels"  << endl;
			WsHandleClose(h_ws);
			return -1;
		}

		cout << "[OK] WsSetCaptureChannels board_no=" << board_no << endl;

	}
	

	//未使用分初期化
	int loop = max_bord_num - (board_num+1);
	for( board_no=1 ; board_no <= loop ; board_no++)
	{
	    ret = WsSetCaptureChannels(h_ws, board_no, 0);

		if( ret != 0 )
		{
			cerr << "Cannot Set Capture Channels for Init"  << endl;
			WsHandleClose(h_ws);
			return -1;
		}
	}



	/*-----------------------------------------------------------*/
	/* Gainを設定する                                           */
	/*-----------------------------------------------------------*/
	cout << "RASP24_MAX_CH_NUM->" << RASP24_MAX_CH_NUM << endl;
	cout << "RASP24_MAX_BOARD->" <<  max_bord_num << endl;
	cout << "ad_gain-> " << ad_gain << endl;

	for(count=0; count<RASP24_MAX_CH_NUM*max_bord_num; count++)
	{
		gain[count] = 0;
		gain[count] = ad_gain;
	}


    for(count=0; count<max_bord_num; count++)
    {
        cout << count << ": before Gain List" << endl;
        
        cout << gain[count*RASP24_MAX_CH_NUM+0] << " " ;
        cout <<        gain[count*RASP24_MAX_CH_NUM+1]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+2]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+3]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+4]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+5]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+6]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+7]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+8] << endl;

	}


	ret = WsSetCaptureGainList(h_ws, gain);


	if( ret != 0 )
	{
		cerr << "Cannot Set Capture Gain.\n";
		WsHandleClose(h_ws);
		return -1;
	}


    for(count=0; count<RASP24_MAX_CH_NUM*RASP24_MAX_BOARD; count++)
    {
		gain[count] = 0;
	}

    ret  = WsGetCaptureGainList(h_ws, gain);

    printf("Get Status = %d\n", ret);
    
    for(count=0; count<max_bord_num; count++)
    {
        cout << count << ": after Gain List" << endl;
        
        cout << gain[count*RASP24_MAX_CH_NUM+0] << " " ;
        cout <<        gain[count*RASP24_MAX_CH_NUM+1]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+2]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+3]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+4]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+5]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+6]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+7]  << " ";
        cout <<        gain[count*RASP24_MAX_CH_NUM+8] << endl;

	}
	
	cout << "[OK] WsSetCaptureGainList" << endl;



	/*-----------------------------------------------------------*/
	/* A/Dデータのヘッダ情報を付加しない                         */
	/*-----------------------------------------------------------*/
	ret = WsSetHeaderFlag(h_ws, 0);

	if( ret != 0 )
	{
		cerr << "Cannot Set Header Flag.\n";
		WsHandleClose(h_ws);
		return -1;
	}

	cout << "[OK] WsSetHeaderFlag" << endl;


	/*-----------------------------------------------------------*/
	/* debug                                                     */
	/*-----------------------------------------------------------*/
	int get_val;
	WsGetSamplingFreq(h_ws, &get_val);
	cout << "[OK] WsGetSamplingFreq=" << get_val << endl;

    WsGetAdDataFormat(h_ws, &get_val);
	cout << "[OK] WsGetAdDataFormat format=" << get_val << endl;

	for( board_no=0 ; board_num < board_no ; board_no++)
	{
	    WsGetCaptureChannels(h_ws, board_no, &get_val);
		cout << "[OK] WsGetCaptureChannels  channels=" << get_val << endl;
	}


	int channel;
	int  capture_flag;

	for( board_no=0 ; board_no < 2 ; board_no++)
	{
		WsGetCaptureChannels(h_ws, board_no, &get_val);

		/* CHごとの状態を表示 */
		cout << "Capture Channels : "  <<   board_no << endl;
		for(channel=0; channel<RASP24_MAX_CH_NUM; channel++)
		{
			capture_flag = (get_val >> channel) & 0x1;

			cout << "CH[%02d] Capture : " <<  channel ;
			if(capture_flag)
			{
			    cout << "Enable\n" ;
			}
			else
			{
			    cout << "Disable\n" ;
			}
		}
	}



	int i=TARGET_TYPE_RASP24;
	WsGetSystemInfo(h_ws, &i);
	WsHandleShow(h_ws);


	WsADStop(h_ws);
	recorderThread.reset(new thread(ref(*this)));
	return 0;
}
