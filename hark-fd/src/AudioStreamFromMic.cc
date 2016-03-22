/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <climits>

#include <iostream>
#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include <math.h>
#include <csignal>
#include <string>
#include <Matrix.h>
#include "Time.h"
#include "TimeStamp.h"
#include <sys/time.h>

#include "../config.h"

#include <ALSARecorder.hpp>
#if ENABLE_TDBD8CSUSB
#include <ASIORecorder.hpp>
#endif
#if ENABLE_TDBD16ADUSB
#include <SINICHRecorder.hpp>
#endif
#if ENABLE_RASP2
#include <RASPRecorder.hpp>
#endif
#if ENABLE_WS
#include <WSRecorder.hpp>
#endif
#if ENABLE_ASRASP
#include <ASRASPRecorder.hpp>
#endif
#if ENABLE_RASP24
#include <RASP24Recorder.hpp>
#endif
#if ENABLE_HARKNETAPI
#include <HARKNETAPIRecorder.hpp>
#endif
#if ENABLE_RASPLC
#include <RASPLCRecorder.hpp>
#endif
#if ENABLE_KANEKO
#include <KANEKORecorder.hpp>
#endif




using namespace std;
using namespace FD;
using namespace boost::posix_time;

class AudioStreamFromMic;

DECLARE_NODE(AudioStreamFromMic);
/*Node
 *
 * @name AudioStreamFromMic
 * @category HARK:AudioIO
 * @description Capture sound data synchronously from multiple microphones.
 *
 * @output_name AUDIO
 * @output_type Matrix<float>
 * @output_description Windowed multi-channel sound data. A row index is a channel, and a column index is time.
 *
 * @output_name NOT_EOF
 * @output_type bool
 * @output_description Always true, this is for "condition" setting
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description The frame length of each channel (in samples) [default: 512].
 *
 * @parameter_name ADVANCE
 * @parameter_type int
 * @parameter_value 160
 * @parameter_description The shift length beween adjacent frames (in samples)[default: 160].
 *
 * @parameter_name CHANNEL_COUNT
 * @parameter_type int
 * @parameter_value 8
 * @parameter_description The number of channels.
 *
 * @parameter_name SAMPLING_RATE
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_description Sampling rate (Hz) [default: 16000].
 *
 * @parameter_name DEVICETYPE
 * @parameter_type string
 * @parameter_list ALSA:RASP:WS:TDBD16ADUSB:RASP24-16:RASP24-32:RASP-LC:KANEKO:NETWORK
 * @parameter_value WS
 * @parameter_description Device type [default: WS].
  *
 * @parameter_name GAIN
 * @parameter_type string
 * @parameter_list 0dB:12dB:24dB:36dB:48dB
 * @parameter_value 0dB
 * @parameter_valid DEVICETYPE=RASP24-16:RASP24-32
 * @parameter_description  capture gain (dB)  [default: 0dB].
 *
 * @parameter_name DEVICE
 * @parameter_type string
 * @parameter_value 127.0.0.1
 * @parameter_description Device name or IP address [default: 127.0.0.1]
 END*/

bool recorder_stop_flag2 = false;

 void signal_handler2(int s)
 {
   recorder_stop_flag2 = true;
 }

class AudioStreamFromMic : public BufferedNode {

  int audioID;
  int eofID;
  int timeID;
  int timestampID;
  int length;
  int advance;
  int channel_count;
  int sampling_rate;
  string device_type;
  string device;
  string device_gain;
  string dv_gain;	


  Recorder* recorder;
  bool hasTimestamp;
  int overlapped_usec;
  int sleep_read_usec;
  float amp_max;
  float amp_min;

  vector<float> buffer;

public:

  AudioStreamFromMic(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params), recorder(0), hasTimestamp(false),
      overlapped_usec(0), amp_max(SHRT_MAX), amp_min(SHRT_MIN)
  {
    audioID = addOutput("AUDIO");
    eofID = addOutput("NOT_EOF");
    timeID = addOutput("TIME");
    timestampID = addOutput("TIMESTAMP");
    if (parameters.exist("LENGTH")) {
      length = dereference_cast<int>(parameters.get("LENGTH"));
    }
    if (parameters.exist("ADVANCE")) {
      advance = dereference_cast<int>(parameters.get("ADVANCE"));
    }
    else {
      advance = length;
    }

    channel_count = dereference_cast<int>(parameters.get("CHANNEL_COUNT"));
    sampling_rate = dereference_cast<int>(parameters.get("SAMPLING_RATE"));
    device_type = object_cast<String>(parameters.get("DEVICETYPE"));
    device = object_cast<String>(parameters.get("DEVICE"));
    device_gain = object_cast<String>(parameters.get("GAIN"));

    overlapped_usec = (length - advance) * 1000000 / sampling_rate;
    sleep_read_usec = advance / 2 * 1000000 / sampling_rate;

    signal(SIGINT, signal_handler2);
    signal(SIGHUP, signal_handler2);


    if (device_type == "ALSA") {
      // device = "plughw:0";
      recorder = ALSARecorder::GetInstance();
      recorder->Initialize(device.c_str(), channel_count, sampling_rate,
                           length * 1024);
    }
#if ENABLE_TDBD8CSUSB
    else if (device_type == "ASIO") {
      // device = "plughw:0";
      recorder = ASIORecorder::GetInstance();
      recorder->Initialize(device.c_str(), channel_count, sampling_rate,
                           length * 1024);
    }
#endif
#if ENABLE_TDBD16ADUSB
    else if (device_type == "TDBD16ADUSB") {
      // device = "plughw:0";
      recorder = SINICHRecorder::GetInstance();
      recorder->Initialize(device.c_str(), channel_count, sampling_rate,
			   length * 2048);
    }
#endif
#if ENABLE_RASP2
    else if (device_type == "RASP") {
      // device = "/dev/rasp/adc0";
      recorder = RASPRecorder::GetInstance();
      recorder->Initialize(device.c_str(), channel_count, sampling_rate,
                           length * 1024);
    }
#endif
#if ENABLE_WS
    else if (device_type == "WS") {
      // device = "192.168.32.228";
      recorder = WSRecorder::GetInstance();
      int ret = recorder->Initialize(device.c_str(), channel_count,
                                     sampling_rate, length * 1024);
      if (ret == -1) {
        throw new NodeException(NULL, string("Device can not be initialized."),
                                __FILE__, __LINE__);
      }
      hasTimestamp = true;
      if (channel_count == 8 || channel_count == 10) {
        amp_max = 8388607;
        amp_min = -8388608;
      }
    }
#endif
#if ENABLE_ASRASP
    else if (device_type == "ASRASP") {
      recorder = ASRASPRecorder::GetInstance();
      int ret = recorder->Initialize(device.c_str(), channel_count,
                                     sampling_rate, length * 1024);
      if (ret == -1) {
        throw new NodeException(NULL, string("Device can not be initialized."),
                                __FILE__, __LINE__);
      }
      hasTimestamp = true;
      if (channel_count >= 8 && channel_count <= 10) {
        amp_max = 8388607;
        amp_min = -8388608;
      }
    }
#endif
#if ENABLE_RASP24
	else if (device_type == "RASP24-16" || device_type == "RASP24-32") 
	{

		string tmp_device;

		recorder = RASP24Recorder::GetInstance();

		dv_gain="0";

		if(strcmp("0dB",device_gain.c_str())==0)
		{
			dv_gain="0";
		}
		if(strcmp("12dB",device_gain.c_str())==0)
		{
			dv_gain="1";
		}
		else if(strcmp("24dB",device_gain.c_str())==0)
		{
			dv_gain="2";
		}
		else if(strcmp("36dB",device_gain.c_str())==0)
		{
			dv_gain="3";
		}
		else if(strcmp("48dB",device_gain.c_str())==0)
		{
			dv_gain="4";
		}


		recorder = RASP24Recorder::GetInstance();

		tmp_device = device + "," +  device_type + "," +  dv_gain;
				

		int ret = recorder->Initialize( tmp_device.c_str(),
                                        channel_count,
                                        sampling_rate,
                                        length * 1024);


		if (ret == -1) 
		{

			throw new NodeException(NULL, string("Device can not be initialized."),__FILE__, __LINE__);
		}

		hasTimestamp = true;
		amp_max =  8388607;
		amp_min = -8388608;
 


	}
#endif
#if ENABLE_HARKNETAPI
	else if (device_type == "NETWORK") 
	{


		recorder = HARKNETAPIRecorder::GetInstance();

		int ret = recorder->Initialize( device.c_str(),
                                                 channel_count,
                                                 sampling_rate,
                                                length * 1024);

		if (ret == -1) 
		{
			throw new NodeException(NULL, string("Device can not be initialized."),__FILE__, __LINE__);
		}

		hasTimestamp = true;

		amp_max =  2147483647;      //32bit整数時振の幅データMAX
		amp_min = -2.147483648e+09; //32bit整数時の振幅データMin
//		amp_max =  8388607;         //24bit整数時の振幅データMAX
//		amp_min = -8388608;         //24bit整数時の振幅データMin
//		cout<< "HARKNETAPI END" << endl;
 
	}
#endif
#if ENABLE_RASPLC
	else if (device_type == "RASP-LC") 
	{

 		recorder = RASPLCRecorder::GetInstance();

		channel_count = LC_CH_NUM;
		sampling_rate = CLOCK_FREQ_16K;

		int ret = recorder->Initialize( device.c_str(),
                                        channel_count,
                                        sampling_rate,
                                        length * 1024);


		if (ret == -1) 
		{
			throw new NodeException(NULL, string("Device can not be initialized."),__FILE__, __LINE__);
		}

		hasTimestamp = true;

		amp_max =  2147483647;      //32bit整数時振の幅データMAX
		amp_min = -2.147483648e+09; //32bit整数時の振幅データMin
//		amp_max =  8388607;         //24bit整数時の振幅データMAX
//		amp_min = -8388608;         //24bit整数時の振幅データMin
 
	}
#endif
#if ENABLE_KANEKO
	else if (device_type == "KANEKO") 
	{

		string tmp_device;

		recorder = KANEKORecorder::GetInstance();

		channel_count = /*LC_CH_NUM*/4;
		sampling_rate = CLOCK_FREQ_16K;

		tmp_device = device + "," +  device_type;

		int ret = recorder->Initialize( tmp_device.c_str(),
                                        channel_count,
                                        sampling_rate,
                                        length * 1024);

		if (ret == -1) 
		{
			throw new NodeException(NULL, string("Device can not be initialized."),__FILE__, __LINE__);
		}

		hasTimestamp = true;

		amp_max =  2147483647;      //32bit整数時振の幅データMAX
		amp_min = -2.147483648e+09; //32bit整数時の振幅データMin
//		amp_max =  8388607;         //24bit整数時の振幅データMAX
//		amp_min = -8388608;         //24bit整数時の振幅データMin
 
	}
#endif
    else 
    {
      throw new NodeException(NULL, string("Device type " + device_type + " is not supported."), __FILE__, __LINE__);
    }
    inOrder = true;
  }

  ~AudioStreamFromMic()
  {
    recorder->Stop();
  }

  virtual void initialize()
  {
  
    outputs[audioID].lookAhead
      = outputs[eofID].lookAhead
      = outputs[timeID].lookAhead
      = outputs[timestampID].lookAhead
      = 1 + max(max(max(outputs[audioID].lookAhead, outputs[eofID].lookAhead),
		    outputs[timeID].lookAhead),outputs[timestampID].lookAhead);
    //outputs[audioID].lookBack += 1;

    outputs[audioID].lookBack
      = outputs[eofID].lookBack
      = outputs[timeID].lookBack
      = outputs[timestampID].lookBack
      = 1 + max(max(max(outputs[audioID].lookBack, outputs[eofID].lookBack),
		    outputs[timeID].lookBack),outputs[timestampID].lookBack);
    this->BufferedNode::initialize();
  }


  virtual void stop()
  {
    recorder->Stop();
  }

  void calculate(int output_id, int count, Buffer &out)
  {
  
  
    Buffer &audioBuffer = *(outputs[audioID].buffer);
    Buffer &eofBuffer = *(outputs[eofID].buffer);

    eofBuffer[count] = (recorder_stop_flag2 ? FalseObject : TrueObject);

    //Vector<float>& output = *Vector<float>::alloc(length * channel_count);

    RCPtr<Matrix<float> > outputp(new Matrix<float>(channel_count,length));
    audioBuffer[count] = outputp;
    Matrix<float>& output = *outputp;

    RCPtr<Time> cur_time(new Time());
    (*outputs[timeID].buffer)[count] = cur_time;

    RCPtr<TimeStamp> cur_timestamp(new TimeStamp());
    (*outputs[timestampID].buffer)[count] = cur_timestamp;

    if (count == 0) 
    { //Done only the first time

		
      recorder->Start();
	  

      buffer.resize(length * channel_count);



      Recorder::BUFFER_STATE state;

      if (hasTimestamp) 
      {
        ptime sample_time;
        do 
        {
          usleep(sleep_read_usec);
          state = recorder->ReadBuffer(0, length, buffer.begin(), sample_time);
          
          if (state == Recorder::STOPPING) 
          {
            eofBuffer[count] = FalseObject;
            return;
          }
        } while (state != Recorder::OK);
        cur_time->setTime(sample_time);
        cur_timestamp->setTime(to_timeval(sample_time));
      }
      else 
      {

        do 
        {
          usleep(sleep_read_usec);
          state = recorder->ReadBuffer(0, length, buffer.begin());
          if (state == Recorder::STOPPING) {
            eofBuffer[count] = FalseObject;
            return;
          }
        } while (state != Recorder::OK);
        cur_time->setTime(microsec_clock::local_time());
        cur_timestamp->setTime(to_timeval(microsec_clock::local_time()));
      }

      //ShortToFloat(&buffer[0], &output[0], length * channel_count);


	// networkデータをfloatに変換する(2013/02/01)
	if (device_type == "NETWORK") 
	{
		float fl1;		
		for(int i=0;i<buffer.size();i++)
		{	
			int32_t v = (int32_t)buffer[i];
			char* p = (char*)&v;
			memcpy( (char*)&fl1 , p , 4);
			buffer[i] = fl1 * 32767;
		}
	}


      convertVectorToMatrix(buffer, output, 0);

    }
    // Normal case (not at start of file)
    else 
    { 

	            
      if (advance < length)
        {

          // Vector<float>& previous  = object_cast<Vector<float> >(audioBuffer[count - 1]);
          Matrix<float>& previous = 
            object_cast<Matrix<float> >(audioBuffer[count - 1]);

          for (int c = 0; c < length - advance; c++) {
            for(int r = 0; r < output.nrows(); r++) {
              output(r,c) = previous(r,c + advance);
            }
          }
        }
      /*
      else 
      {

        //FIXME: should skip data if (advance > length)
        for (int c = 0; c < length - advance; c++) {
          for(int r = 0; r < output.nrows(); r++) {
            output(r,c) = 0;
          }
        }
      }
      */

      buffer.resize(advance * channel_count);

      Recorder::BUFFER_STATE state;

      if (hasTimestamp) 
      {

        ptime sample_time;
        for (;;) 
        {
          state = recorder->ReadBuffer((count - 1) * advance + length,
                                       advance, buffer.begin(), sample_time);
          if (state == Recorder::OK) {
            break;
          }
          else if (state == Recorder::STOPPING) {
            eofBuffer[count] = FalseObject;
            return;
          }
          else {
            usleep(sleep_read_usec);
          }
        }
        cur_time->setTime(sample_time - microseconds(overlapped_usec));
        cur_timestamp->setTime(to_timeval(sample_time - microseconds(overlapped_usec)));
      }
      else 
      {


        for (;;) {
          state = recorder->ReadBuffer((count - 1) * advance + length,
                                       advance, buffer.begin());
          if (state == Recorder::OK) {
            break;
          }
          else if (state == Recorder::STOPPING) {
            eofBuffer[count] = FalseObject;
            return;
          }
          else {
            //cout << "Record state is not OK.state=" << state << endl;
            usleep(sleep_read_usec);
          }
        }
        cur_time->setTime(microsec_clock::local_time() - microseconds(overlapped_usec));
        cur_timestamp->setTime(to_timeval(microsec_clock::local_time() - microseconds(overlapped_usec)));
      }

      //int first_output = output.size() - advance * channel_count;
      int first_output = length - advance;
      // ShortToFloat(&buffer[0],&output[first_output],advance * channel_count);



	// networkデータをfloatに変換する(2013/02/01)
//	ofstream fout;
//	fout.open("audio.txt", ios::out | ios::app | ios::binary);
	if (device_type == "NETWORK") 
	{
		float fl1;		
		for(int i=0;i<buffer.size();i++)
		{	
			int32_t v = (int32_t)buffer[i];
			char* p = (char*)&v;
			memcpy( (char*)&fl1 , p , 4);
			buffer[i] = fl1 * 32767;
//			fout << buffer[i] << '\n';
		}
	}
//	fout.close();
	
      convertVectorToMatrix(buffer,output,first_output);
    }


    bool is_clipping = false;
    for (int i = 0; i < buffer.size(); i++) {
      if (!is_clipping && checkClipping(buffer[i])) {

        is_clipping = true;
      }
    }
    if (is_clipping) {
      cerr << "["<< count << "][" << getName() << "] clipping" << endl;
    }
  }

protected:
  void ShortToFloat(const short* in, float* out, size_t n) {
    for (int i = 0; i < n; i++) {

      out[i] = (float)in[i];
    }
  }


  void convertVectorToMatrix(const vector<float>& in, Matrix<float>& out,
                             int first_col)
  {
    for(int i = 0 ; i < out.nrows();i++){
      for(int j = first_col; j < out.ncols();j++){
        out(i,j) = (float)in[i + (j-first_col) *out.nrows()] ;
      }
    }
  }


  bool checkClipping(float x)
  {

//	if (device_type == "HARKNETAPI") 
	if (device_type == "NETWORK2") 
	{
		int32_t xx; 
		memcpy((char*)&xx , (char*)&x , 4) ;
	
		if (xx >= amp_max || xx <= amp_min) 
		{
		  return true;
		}
		else {
		  return false;
		}
	}
	else
	{
	    if (x >= amp_max || x <= amp_min) {
	      return true;
	    }
	    else {
	      return false;
	    }
	}
  }

  timeval to_timeval(const ptime &t)
  {
    ptime timet_start(boost::gregorian::date(1970,1,1));
    time_duration diff = t - timet_start;
    timeval tv;
    //drop off the fractional seconds...
    tv.tv_sec = diff.ticks()/time_duration::rep_type::res_adjust();
    //The following only works with microsecond resolution!
    tv.tv_usec = diff.fractional_seconds();
    return tv;
  }

};
