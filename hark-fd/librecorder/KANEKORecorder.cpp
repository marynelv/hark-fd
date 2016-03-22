#include <iostream>
#include <math.h>
#include <stdio.h>
#include "KANEKORecorder.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

scoped_ptr<KANEKORecorder> KANEKORecorder::instance;

//***************************************************************************************//
// This function is executed in another thread, and
// records acoustic signals into circular buffer.
//***************************************************************************************//
void KANEKORecorder::operator()()
{
    read_buf.resize(channel_count * (header_period - 1) * 20);
    vector<char> buf(sample_bytes * channel_count * header_period * 20);
    int read_size    = 0;
    int header_count = 0;

    ptime t_start = microsec_clock::local_time();
    ptime t_cur;

    cout << "KANEKORecorder::operator START" << endl;
    cout << "  channel_count" << channel_count << endl;
    cout << "  header_period" << header_period << endl;
    cout << "  sample_bytes" << sample_bytes << endl;



    for (;;) 
    {
        if (state == PAUSE) 
        {
            //cout << "RECORDING PAUSE" << endl;
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
            //cout << "RECORDING START" << endl;
            //read_size = WsKmSampleDataRead(h_ws, (char*)&buf[0], buf.size());
            read_size = WsKmFrameRead(h_ws, (char*)&buf[0], buf.size());
            //cout << "read_size = " << read_size << " " << buf.size() << endl;
            if(read_size <= 0) 
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

                //switch (sample_bytes) 
                //{
                //    case 2: //16bit mode
                //    {
                        int16_t* first = (int16_t*)&buf[start_buf];
                        int16_t* last  = (int16_t*)&buf[end_buf];
                        copy(first, last, read_buf.begin() + start_rb);
                //        break;
                //    }
                //    case 4: //32bit mode
                //    {
                //        int32_t* first = (int32_t*)&buf[start_buf];
                //        int32_t* last  = (int32_t*)&buf[end_buf];
                //        copy(first, last, read_buf.begin() + start_rb);
                //        break;
                //    }
                //}

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

            for(it_st = sample_time_queue.begin() ; it_st != sample_time_queue.end(); ++it_st) 
            {
                if(it_st->sample >= end_time) 
                {
                    break;
                }
            }

            sample_time_queue.erase(sample_time_queue.begin(), it_st);

            //cout << cur_time << " " << (t_cur - t_start).total_microseconds() - 10500 - 51200 << endl;
            ////cout << sample_time_queue.back().sample << " " << sample_time_queue.back().time << endl;

            vector<float>::iterator it = read_buf.begin();

            advance(it, i * channel_count);

            mutex::scoped_lock lk(mutex_buffer);

            buffer.insert(buffer.end(), read_buf.begin(), it);

            lk.unlock();

            usleep(sleep_time);
        }//if end
	}//for end

    cout << "KANEKORecorder::operator END" << endl;

}



//***************************************************************************************//
// A device for recording is initialized.
//   device_name : Audio device name.  ex. "plughw:0,0"
//   device_type : Audio device type.  ex. "KANEKO"
//   chan_count  : A number of channels. ex. 8 if eight microphones are used.
//   samp_rate   : A frequency of sampling.
//   buf_size    : A size of circular buffer.
//***************************************************************************************//
int KANEKORecorder::Initialize( const string& device_name,
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
    int     dev_point=0;
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
    /*  device_name から　device　と device type を分割して取出す       */
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
    dev_type = tmp.substr(0,6);

    cout << "device_name" << device_name << endl;	
    cout << "dev_name=" << dev_name << endl;
    cout << "dev_type=" << dev_type << endl;

    /*-----------------------------------------------------------*/
    /* Handle Cesck                                              */
    /*-----------------------------------------------------------*/
    if(h_ws) 
    {
        WsKmHandleClose(h_ws);
    }


    /*-----------------------------------------------------------*/
    /* Handle Open                                               */
    /*-----------------------------------------------------------*/
    h_ws = WsKmHandleOpen((char*)dev_name.c_str());
    if(!h_ws) 
    {
        cerr << "Cannot open KANEKO System.\n";
        return -1;
    }
    cout << "[OK] WsKmHandleOpen" << endl;

    /*-----------------------------------------------------------*/
    /* サンプリング周波数設定                                    */
    /*-----------------------------------------------------------*/
    ret = WsKmSetSamplingFreq(h_ws, sampling_rate);
    if( ret != 0 )
    {
        cerr << "Cannot Set Sampling Frequency.\n";
        WsKmHandleClose(h_ws);
        return -1;
    }
    cout << "[OK] WsKmSetSamplingFreq" << endl;

    /*-----------------------------------------------------------*/
    /* 接続相手の識別コード取得                                  */
    /*-----------------------------------------------------------*/
    int target_type;
    ret = WsKmGetTargetType(h_ws, &target_type);
    if( ret != 0 )
    {
        cerr << "Cannot Get Target Type.\n";
        WsKmHandleClose(h_ws);
        return -1;
    }
    cout << "[OK] WsKmSetSamplingFreq=" << target_type << endl;
    if( target_type != TARGET_TYPE_KM_WL )
    {
        cerr << "Target Type  Not KANEKO.\n";
        WsKmHandleClose(h_ws);
        return -1;
    }

    /*-----------------------------------------------------------*/
    /* A/Dデータフォーマット設定                                 */
    /*-----------------------------------------------------------*/
    cout << "dev_type == KANEKO "  << endl;
    ret = WsKmSetAdDataFormat(h_ws , DATA_FORMAT_16BIT_PCM);
    sample_bytes = 2;
    if( ret != 0 )
    {
        cerr << "Cannot Set A/D Data Format: sample_bytes=" << sample_bytes << endl;
        WsKmHandleClose(h_ws);
        return -1;
    }
    cout << "[OK] WsKmSetAdDataFormat : sample_bytes=" << sample_bytes << endl;

    /*-----------------------------------------------------------*/
    /* chan_count から 同時サンプリング数を取得                  */
    /*-----------------------------------------------------------*/
    //int board_num=0;
    //int board_no =0;
    //int max_bord_num=0;
    //int mode9ch = 0;
    //if(chan_count % 9 == 0){
    //  board_num = abs((chan_count-1)/9); //使用ボード番号取得
    //  mode9ch = 1;
    //}else{
    //  board_num = abs((chan_count-1)/8); //使用ボード番号取得
    //}
    //cout << "board_num=" << board_num << endl;

    //ret=WsKmGetBoardNum(h_ws , &max_bord_num);
    //if( ret != 0 )
    //{
    //	cerr << "Cannot Get Board Num.\n";
    //	WsKmHandleClose(h_ws);
    //	return -1;
    //}
    //if( board_num < 0 )
    //{
    //    cerr << "Number of channels was lower than zero.\n";
    //    WsKmHandleClose(h_ws);
    //    return -1;
    //}
    //else if( max_bord_num <= board_num )
    //{
    //    cerr << "Number of channels has exceeded the upper limit of hardware.\n";
    //    WsKmHandleClose(h_ws);
    //    return -1;
    //}
    //cout << "[OK] WsKmGetBoardNum max_bord_num=" << max_bord_num << endl;

    //for(board_no=0 ; board_no < board_num+1 ; board_no++)
    //{
    //  if(mode9ch == 1)
    //    ret = WsKmSetCaptureChannels(h_ws, board_no, AD_RASP24AD_CAPTURE_CHANNELS);
    //  else
    //    ret = WsKmSetCaptureChannels(h_ws, board_no, AD_8CH_MODE_CAPTURE_CHANNELS);
    //	if( ret != 0 )
    //	{
    //	        cerr << "Cannot Set Capture Channels"  << endl;
    //		WsKmHandleClose(h_ws);
    //		return -1;
    //	}
    //	cout << "[OK] WsKmSetCaptureChannels board_no=" << board_no << endl;
    //}

    //未使用分初期化
    //int loop = max_bord_num - (board_num+1);
    //for( board_no=1 ; board_no <= loop ; board_no++)
    //{
    //    ret = WsKmSetCaptureChannels(h_ws, board_no, 0);
    //	if( ret != 0 )
    //	{
    //		cerr << "Cannot Set Capture Channels for Init"  << endl;
    //		WsKmHandleClose(h_ws);
    //		return -1;
    //	}
    //}

    /*-----------------------------------------------------------*/
    /* A/Dデータのヘッダ情報を付加する（金子マイクは付与固定）   */
    /*-----------------------------------------------------------*/
    ret = WsKmSetHeaderFlag(h_ws, 1);
    if( ret != 0 )
    {
        cerr << "Cannot Set Header Flag.\n";
        WsKmHandleClose(h_ws);
        return -1;
    }
    cout << "[OK] WsKmSetHeaderFlag" << endl;

    /*-----------------------------------------------------------*/
    /* debug                                                     */
    /*-----------------------------------------------------------*/
    int get_val;
    WsKmGetSamplingFreq(h_ws, &get_val);
    cout << "[OK] WsKmGetSamplingFreq=" << get_val << endl;

    WsKmGetAdDataFormat(h_ws, &get_val);
    cout << "[OK] WsKmGetAdDataFormat format=" << get_val << endl;

    int i=TARGET_TYPE_KM_WL;
    WsKmGetSystemInfo(h_ws, &i);
    WsKmHandleShow(h_ws);

    WsKmADStop(h_ws);
    recorderThread.reset(new thread(ref(*this)));
    return 0;
}
