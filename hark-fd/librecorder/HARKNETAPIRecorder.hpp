#ifndef ___HARKNETAPIRECORDER_HPP___
#define ___HARKNETAPIRECORDER_HPP___

#include "Recorder.hpp"

#include "hark-netapi.h"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>



class HARKNETAPIRecorder : public Recorder 
{

private:
	static boost::scoped_ptr<HARKNETAPIRecorder> instance;
	boost::shared_ptr<boost::thread> recorderThread;
	boost::posix_time::time_duration latency;
	std::vector<float> read_buf;
	int header_period;
	int read_buf_size;
	int sample_bytes;
	KC_HANDLE_t* h_ws;

public:
	HARKNETAPIRecorder() : h_ws(0), header_period(0), sample_bytes(4), read_buf_size(2560) 
	{
		std::cerr << "HARKNETAPIRecorder\n";
	}

	//-----------------------------------------------------//
	//
	//-----------------------------------------------------//
	virtual ~HARKNETAPIRecorder() 
	{
		if (h_ws) 
		{
			KcADStop(h_ws);
			KcHandleClose(h_ws);
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
			instance.reset(new HARKNETAPIRecorder);
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
		boost::mutex::scoped_lock lk(mutex_buffer);
		int ret = KcADStart(h_ws);
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
		boost::mutex::scoped_lock lk(mutex_buffer);
		state = STOP;
		KcADStop(h_ws);
	}

 
	//-----------------------------------------------------//
	// Close
	//-----------------------------------------------------//
	virtual void Close() 
	{
		if (h_ws) 
		{
			KcADStop(h_ws);
			KcHandleClose(h_ws);
			h_ws = 0;
		}
	}
};


#endif
