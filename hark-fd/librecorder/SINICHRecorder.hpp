/*
 * SINICHRecorder.h
 *
 *  Created on: 2009/08/11
 *      Author: yoshida
 */

#ifndef SINICHRECORDER_H_
#define SINICHRECORDER_H_


#include "Recorder.hpp"
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// for TDBD16ADUSB
extern "C"{
#include <sinich_cmn.h>
#include <sinich_prm.h>
#include <sinich_reg.h>
#include <sinich_lib.h>
}

#ifndef O_BINARY
#define O_BINARY					0
#endif

#define _O_BINARY					O_BINARY
#define _O_RDONLY					O_RDONLY
#define Sleep(x)					usleep(1000*x)
typedef struct _ICH_FILE_HEADER
{
	ulong	file_version;
	ulong	sampling_rate;
	ulong	channel_num;
	ulong	reserved;
} ICH_FILE_HEADER_INFO;

typedef struct _OCH_FILE_HEADER
{
	ulong	file_version;
	ulong	sampling_rate;
	ulong	channel_num;
	ulong	reserved;
} OCH_FILE_HEADER_INFO;

// for BOOST
#include <boost/thread.hpp>

/*----------------------------------------*/
// Definition for TDBD-16ADUSB            //
/*----------------------------------------*/
#define ENDIAN_SET            SINICH_LITTLE_ENDIAN
#define ICH_SYS_DEFAULT_DELAY 1000
#define	MODE_16CH             0
#define SINICH_LIB_DEBUG      0



class SINICHRecorder: public Recorder {

private:
  
  static SINICHRecorder* instance;
  int i, ret;
  int buff_len;                     // received data size
  int samp_index;                   // index for in_pkt_size
  struct ich_io_t *ted_usb_sys;     // struct for device
  
  ushort buff_status;               // buffer status
  uchar ep2_bufffill_num;           // filled buffer size
  int audio_inpkt_siz;              // input packet size
  int set_gain_val;                 // gain value
  UCHAR *audio_in_buf;              // buffer
  unsigned char sample_rate_ch_set; // samplingrate and ch
  uchar fpga_rdata;
  int read_buf_size;
  std::vector<short> read_buf;  
  boost::shared_ptr<boost::thread> recorderThread;

  int ich2mat(void);             // convert data format

public:

  // constructor does nothing
  SINICHRecorder(): read_buf(){
  }

  static Recorder* GetInstance() {
    if (!instance) {
      instance = new SINICHRecorder;
    }
    return instance;
  }

  // main process: done in other thread
  void operator()();

  virtual int Initialize(const std::string& device_name,
			 int channel_count, int sampling_rate,
			 size_t buf_size);

  // start recording
  virtual void Start() {
	boost::mutex::scoped_lock lk(mutex_buffer);
    state = RECORDING;
  }

  // pause in recording
  virtual void Pause() {
	boost::mutex::scoped_lock lk(mutex_buffer);
    state = PAUSE;
  }

  // stop and finish recording
  virtual void Stop() {
	boost::mutex::scoped_lock lk(mutex_buffer);
    state = STOP;
  }

  virtual void Close() {
  }

};
#endif /* SINICHRECORDER_H_ */
