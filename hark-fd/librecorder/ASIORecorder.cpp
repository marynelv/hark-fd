#include "ASIORecorder.hpp"

extern "C" {
#include "asio.h"
}

using namespace std;
using namespace boost;

scoped_ptr<ASIORecorder> ASIORecorder::instance;

ASIORecorder::~ASIORecorder()
{
    ASIOStop();
    ASIODisposeBuffers();
    ASIOExit();
}

Recorder* ASIORecorder::GetInstance() {
  if (!instance) {
    instance.reset(new ASIORecorder);
  }
  return instance.get();
}

int ASIORecorder::Initialize(const string& device_name,
                             int chan_count, int samp_rate,
                             size_t buf_size)
{
    mutex::scoped_lock lk(mutex_buffer);
    
    buffer_size = buf_size;
    channel_count = chan_count;
    sampling_rate = samp_rate;
    buffer.set_capacity(buffer_size * channel_count);
    buffer.clear();
    read_buf.resize(read_buf_size * channel_count);
    cur_time = 0;
    sleep_time = read_buf_size * 1000000 / sampling_rate / 3;
    frame_size = read_buf.size() / channel_count;
    //cout << "sleep_time = " << sleep_time << "\n";
    //cout << "frame_size = " << frame_size << "\n";

    
    ASIODriverInfo info;
    ASIOError result;

    info.asioVersion = 2;
    info.sysRef = NULL;

    result = ASIOInit(&info);

    //cout << info.name << " (" << info.driverVersion << ")" << endl;

    callbacks.bufferSwitch = BufferSwitch;
    callbacks.sampleRateDidChange = SampleRateDidChanged;
    callbacks.asioMessage = ASIOMessages;
    callbacks.bufferSwitchTimeInfo = BufferSwitchTimeInfo;

    result = ASIOGetChannels(&inputChannelCount, &outputChannelCount);
    bufferInfo.resize(inputChannelCount + outputChannelCount);
    channelInfo.resize(inputChannelCount + outputChannelCount);

    int index = 0;
    for (int channelCount = 0; channelCount < inputChannelCount; channelCount++) {
        bufferInfo[index].isInput = ASIOTrue;
        bufferInfo[index].channelNum = channelCount;
        bufferInfo[index].buffers[0] = bufferInfo[index].buffers[1] = NULL;

        channelInfo[index].isInput = bufferInfo[index].isInput;
        channelInfo[index].channel = bufferInfo[index].channelNum;
        ASIOGetChannelInfo(&channelInfo[index]);
        index++;
    }

    for (int channelCount = 0; channelCount < outputChannelCount; channelCount++) {
        bufferInfo[index].isInput = ASIOFalse;
        bufferInfo[index].channelNum = channelCount;
        bufferInfo[index].buffers[0] = bufferInfo[index].buffers[1] = NULL;

        channelInfo[index].isInput = bufferInfo[index].isInput;
        channelInfo[index].channel = bufferInfo[index].channelNum;
        ASIOGetChannelInfo(&channelInfo[index]);
        index++;
    }

    result = ASIOSetSampleRate(sampling_rate);

    long minSize;
    long maxSize;
    long granularity;
    ASIOGetBufferSize(&minSize, &maxSize, &bufferSize, &granularity);

    long inputLatency;
    long outputLatency;
    result = ASIOGetLatencies(&inputLatency, &outputLatency);

    result = ASIOCreateBuffers(&bufferInfo[0], inputChannelCount + outputChannelCount,
                               bufferSize, &callbacks);
    for (int i = 0; i < inputChannelCount; i++) {
        //cout << bufferInfo[i].buffers[0] << ", ";
        //cout << bufferInfo[i].buffers[1] << endl;
    }

    read_buf.resize(bufferSize * inputChannelCount);

    result = ASIOStart();
    //cout << result << endl;
}

void ASIORecorder::BufferSwitch(long doubleBufferIndex, ASIOBool directProcess)
{
    //cout << "aok\n";
	ASIOTime  timeInfo;

 	memset(&timeInfo, 0, sizeof(ASIOTime));
 	if (ASIOGetSamplePosition(&timeInfo.timeInfo.samplePosition,
                              &timeInfo.timeInfo.systemTime) == ASE_OK){
 		timeInfo.timeInfo.flags = kSystemTimeValid | kSamplePositionValid;
 	}
  	BufferSwitchTimeInfo(&timeInfo, doubleBufferIndex, directProcess);
}

void ASIORecorder::SampleRateDidChanged(ASIOSampleRate sRate)
{
}

long ASIORecorder::ASIOMessages(long selector, long value, void *message, double *opt)
{
  	long ret = 0;
  	switch (selector){
 	case kAsioSelectorSupported:
  		if(value == kAsioResetRequest
  		|| value == kAsioEngineVersion
  		|| value == kAsioResyncRequest
  		|| value == kAsioLatenciesChanged
  		// the following three were added for ASIO 2.0, you don't necessarily have to support them
  		|| value == kAsioSupportsTimeInfo
  		|| value == kAsioSupportsTimeCode
		|| value == kAsioSupportsInputMonitor){
  			ret = 1L;
		}
 		break;
 	case kAsioResetRequest:
 		ret = 1L;
 		break;
 	case kAsioResyncRequest:
 		ret = 1L;
 		break;
 	case kAsioLatenciesChanged:
 		ret = 1L;
 		break;
 	case kAsioEngineVersion:
 		ret = 2L;
 		break;
 	case kAsioSupportsTimeInfo:
 		ret = 1L;
 		break;
 	case kAsioSupportsTimeCode:
 		ret = 0L;
 		break;
	default:
		ret = 0L;
		break;
 	}
  	return ret;
}

ASIOTime* ASIORecorder::BufferSwitchTimeInfo(ASIOTime *params, long doubleBufferIndex,
                                             ASIOBool directProcess)
{
    //cout << params->timeInfo.samplePosition << endl;

    for (int i = 0, inputChannel = 0; i < 16; i++) {
        if (instance->channelInfo[i].isInput == ASIOFalse) {
        }
        else {
            for (int t = 0; t < instance->bufferSize; t++) {
                instance->read_buf[t * instance->inputChannelCount + inputChannel] =
                    ((short*)instance->bufferInfo[i].buffers[doubleBufferIndex])[t];
            }
            inputChannel++;
        }
    }


    mutex::scoped_lock lk(instance->mutex_buffer);
    instance->cur_time = (int)params->timeInfo.samplePosition;
    instance->buffer.insert(instance->buffer.end(),
                            instance->read_buf.begin(),
                            instance->read_buf.begin() +
                            instance->bufferSize * instance->inputChannelCount);

    return 0L;
}
