#ifndef ___RASP24RECORDER_HPP___
#define ___RASP24RECORDER_HPP___

#include "Recorder.hpp"
#include "librasp-netapi.h"
#include "rasp-netapi-arg.h"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

#define AD_RASP24AD_CAPTURE_CHANNELS  (AD_CH0_CAPTURE | \
                                       AD_CH1_CAPTURE | \
                                       AD_CH2_CAPTURE | \
                                       AD_CH3_CAPTURE | \
                                       AD_CH4_CAPTURE | \
                                       AD_CH5_CAPTURE | \
                                       AD_CH6_CAPTURE | \
                                       AD_CH7_CAPTURE | \
                                       AD_CH8_CAPTURE )

#define AD_8CH_MODE_CAPTURE_CHANNELS  (AD_CH0_CAPTURE | \
                                       AD_CH1_CAPTURE | \
                                       AD_CH2_CAPTURE | \
                                       AD_CH3_CAPTURE | \
                                       AD_CH4_CAPTURE | \
                                       AD_CH5_CAPTURE | \
                                       AD_CH6_CAPTURE | \
                                       AD_CH7_CAPTURE )



class RASP24Recorder : public Recorder 
{

private:
	static boost::scoped_ptr<RASP24Recorder> instance;
	WS_HANDLE_t* h_ws;
	boost::shared_ptr<boost::thread> recorderThread;
	int header_period;
	std::vector<float> read_buf;
	int read_buf_size;
	int sample_bytes;
	boost::posix_time::time_duration latency;

public:
	RASP24Recorder() : h_ws(0), header_period(0), sample_bytes(4), read_buf_size(0) 
	{
		//cerr << "RASP24Recorder\n";
	}

	virtual ~RASP24Recorder() 
	{
		if (h_ws) 
		{
			WsADStop(h_ws);
			WsHandleClose(h_ws);
		}
	}


	static
	Recorder* GetInstance() 
	{
		if (!instance) 
		{
			instance.reset(new RASP24Recorder);
    	}
		return instance.get();
	}

	void operator()();

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
		int ret = WsADStart(h_ws);
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
		WsADStop(h_ws);
	}

 
	//-----------------------------------------------------//
	// Close
	//-----------------------------------------------------//
	virtual void Close() 
	{
		if (h_ws) 
		{
			WsADStop(h_ws);
			WsHandleClose(h_ws);
			h_ws = 0;
		}
	}
};


#endif
