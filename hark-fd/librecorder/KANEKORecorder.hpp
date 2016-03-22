#ifndef ___KANEKORECORDER_HPP___
#define ___KANEKORECORDER_HPP___

#include "Recorder.hpp"
#include "librasp-netapi.h"
//#include "rasp-netapi-arg.h"
#include "libkm-netapi.h"

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>

class KANEKORecorder : public Recorder 
{

private:
    static boost::scoped_ptr<KANEKORecorder> instance;
    WS_KM_HANDLE_t* h_ws;
    boost::shared_ptr<boost::thread> recorderThread;
    int header_period;
    std::vector<float> read_buf;
    int read_buf_size;
    int sample_bytes;
    boost::posix_time::time_duration latency;

public:
    KANEKORecorder() : h_ws(0), header_period(0), sample_bytes(4), read_buf_size(0) 
    {
        //cerr << "KANEKORecorder\n";
    }

    virtual ~KANEKORecorder() 
    {
        if(h_ws) 
        {
            WsKmADStop(h_ws);
            WsKmHandleClose(h_ws);
        }
    }

    static
    Recorder* GetInstance() 
    {
        if(!instance) 
        {
            instance.reset(new KANEKORecorder);
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
        int ret = WsKmADStart(h_ws);
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
        WsKmADStop(h_ws);
    }

    //-----------------------------------------------------//
    // Close
    //-----------------------------------------------------//
    virtual void Close() 
    {
        if(h_ws) 
        {
            WsKmADStop(h_ws);
            WsKmHandleClose(h_ws);
            h_ws = 0;
        }
    }
};

#endif // ___KANEKORECORDER_HPP___

