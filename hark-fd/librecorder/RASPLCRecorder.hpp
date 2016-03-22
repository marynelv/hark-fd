#ifndef ___RASPLCRECORDER_HPP___
#define ___RASPLCRECORDER_HPP___

#include "Recorder.hpp"

#include "librasp-netapi.h"
#include "rasp-netapi-arg.h"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>


#define LC_SAMPLE_NUM      58
#define LC_CH_NUM           4
#define LC_LOCAL_BUF_SIZE  ( LC_SAMPLE_NUM * sizeof(int) * LC_CH_NUM )

class RASPLCRecorder : public Recorder 
{

private:
	static boost::scoped_ptr<RASPLCRecorder> instance;
	boost::shared_ptr<boost::thread> recorderThread;
	boost::posix_time::time_duration latency;
	std::vector<float> read_buf;
	int header_period;
	int read_buf_size;
	int sample_bytes;
	int local_buf_size;
	WS_HANDLE_t* h_ws;


public:
	RASPLCRecorder() : h_ws(0), header_period(0), sample_bytes(4), read_buf_size(2560), local_buf_size(LC_LOCAL_BUF_SIZE)   
	{
		
	}

	//-----------------------------------------------------//
	//
	//-----------------------------------------------------//
	virtual ~RASPLCRecorder() 
	{
		if (h_ws) 
		{
			
			std::cerr << "RASPLCRecorder() exit \n";
			WsLcADStop(h_ws);
			WsLcHandleClose(h_ws);
		}
	}


	//-----------------------------------------------------//
	//
	//-----------------------------------------------------//
	static
	Recorder* GetInstance() 
	{
		if (!instance) 
		{
			instance.reset(new RASPLCRecorder);
    	}
		return instance.get();
	}

	//-----------------------------------------------------//
	//
	//-----------------------------------------------------//
	void operator()();

	//-----------------------------------------------------//
	//
	//-----------------------------------------------------//
	virtual int Initialize( const std::string& device_name,
                            int channel_count,
                            int sampling_rate,
                            size_t buf_size);



	//-----------------------------------------------------//
	// start recording
	//-----------------------------------------------------//
	virtual void Start() 
	{
		std::cout << "start recording" << std::endl;
		boost::mutex::scoped_lock lk(mutex_buffer);
		int ret = WsLcADStart(h_ws);
		state = RECORDING;
	}


	//-----------------------------------------------------//
	// pause in recording
	//-----------------------------------------------------//
	virtual void Pause() 
	{
		boost::mutex::scoped_lock lk(mutex_buffer);
		state = PAUSE;
	}
 

	//-----------------------------------------------------//
	// stop and finish recording
	//-----------------------------------------------------//
	virtual void Stop() 
	{
		std::cout << "stop recording" << std::endl;
		boost::mutex::scoped_lock lk(mutex_buffer);
		state = STOP;
		
		if (h_ws) 
		{
			WsLcADStop(h_ws);
			WsLcHandleClose(h_ws);//add
			h_ws = 0; //add
		}

	}

 
	//-----------------------------------------------------//
	// Close
	//-----------------------------------------------------//
	virtual void Close() 
	{
		std::cout << "close connection" << std::endl;
		if (h_ws) 
		{
			WsLcADStop(h_ws);
			WsLcHandleClose(h_ws);
			h_ws = 0;
		}
	}
};


#endif
