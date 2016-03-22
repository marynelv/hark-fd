/*
 * SINICHRecorder.cpp
 *
 *  Created on: 2009/08/11
 *      Author: yoshida
 */


#include "SINICHRecorder.hpp"

using namespace std;
using namespace boost;

unsigned long	audio_inpkt_tbl[7]={
  6144,	// 48k
  4096,	// 32k
  2048,	// 16k
  1024,	// 8k
  6144,	// 44k
  3072,	// 22k
  1536	// 11k
};
unsigned char audio_board_num_table[] = { 0x5A, 0xA5, 0x4B, 0xB4 };

int codec_control_set(uchar msg_flag, struct ich_io_t *io,int set_gain_val);
// Delay,Hex checker,ASCII to Integer change
void insert_delay(uint delay);
// Alphabet Hexadecimal number charactor to Integer
int ahextoi(char hex);
// Alphabet Hexadecimal number charactor to Integer
int ahextoi2(char *hex_char, int digit);
// Check hex number for gain
int hex_check(char *hex_char);



SINICHRecorder* SINICHRecorder::instance = 0;

// This function is executed in another thread, and
// records acoustic signals into circular buffer.
void SINICHRecorder::operator()()
{
  int read_size;            // a read frame size
  
  for(;;){

    // wait during less than (read_buf_size/sampling_rate) [ms]
    usleep(sleep_time);
    
    mutex::scoped_lock lk(mutex_buffer);

    if (state == PAUSE) {
      continue;
    }
    else if (state == STOP) {
      break;
    }
    else if (state == RECORDING) {
      lk.unlock();

      // device error
    
      if((buff_status = sinich_sys_status(ted_usb_sys)) == 0xFFFF){
		cout <<"Status read error !" << endl;
		goto END_PROC;
      }

      //-- get buff status --//
      ep2_bufffill_num = (uchar)buff_status;

      //-- PCM data receive process(EP2) --//
      if(ep2_bufffill_num){

		//-- read data from device
		ret = sinich_pcm_read( ted_usb_sys,
					   audio_in_buf,
					   audio_inpkt_siz*ep2_bufffill_num);

		if(ret < 0){
		  printf("PCM read size error %d\n",ret);
		  break;
		}

      }
	  // convert data format: remove headers inserted every 4[ms] and sort
      buff_len = ich2mat();
	  //      cur_time += (ret/audio_inpkt_siz) * 4 * sampling_rate /1000;

      cur_time += (ret/audio_inpkt_siz) * sampling_rate * 4/1000;

      mutex::scoped_lock lk(mutex_buffer);
      buffer.insert(buffer.end(),
			read_buf.begin(),
			read_buf.begin() + buff_len);

      lk.unlock();
    }
  }

 END_PROC:
  if(ted_usb_sys!=NULL){	sinich_sys_stop(ted_usb_sys);}
  if(audio_in_buf !=NULL)       free(audio_in_buf);
  state = STOP;
  return;

}


// A device for recording is initialized.
//   device_name : Audio device name.  ex. "plughw:0,0"
//   chan_count : A number of channels. ex. 8 if eight microphones are used.
//   samp_rate : A frequency of sampling.
//   buf_size : A size of circular buffer.
int SINICHRecorder::Initialize(const string& device_name,
							   int chan_count, int samp_rate,
							   size_t buf_size)
{

  i=1;
  ret=1;
  fpga_rdata=0;
  sample_rate_ch_set=0;
  buff_status=0;
  audio_inpkt_siz=0;

  ep2_bufffill_num=0;

  buffer_size = buf_size;
  channel_count = chan_count;
  sampling_rate=samp_rate;

  buffer.set_capacity(buf_size);

  sleep_time = read_buf_size * 1000000 / sampling_rate / 3;
  frame_size = read_buf.size() / channel_count;
  // Allocate read buffer
  if ((audio_in_buf = (UCHAR *)malloc(32*2048))==NULL){
    cerr << "cannot allocate read buffer" << "\n";
    exit(1);
  }


  ted_usb_sys=NULL;
  switch( sampling_rate ){
  case 48000:
    samp_index = 0;
    break;
  case 32000:
    samp_index = 1;
    break;
  case 16000:
    samp_index = 2;
    break;
  case 8000:
    samp_index = 3;
    break;
  case 44100:
    samp_index = 4;
    break;
  case 22000:
    samp_index = 5;
    break;
  case 11000:
    samp_index = 6;
    break;
  default:
    cerr << "sampling rate("
		 << samp_rate << ") is not implimented"
		 << endl;
    exit(1);

  }

  // 16adusb can store less than 32 packets, one data is 2byte
  read_buf.resize( 32 * audio_inpkt_tbl[samp_index] );

  //*********************************************************
  // From SINICH sample sources
  //*********************************************************

  int k=0;
  //-- Driver open for control transfer(EP0) --//
  uchar dev_name[64] = "/dev/sinichusb0";
#if 0
  // FOR WINDOWS
  uchar dev_name[64] = "Ezusb-0";
#endif

  //-- Open A/D driver --//
  if((ret = sinich_sys_open(&ted_usb_sys, dev_name )) != 0){
    cerr << "Driver open error!\n";
    goto END_PROC;
  }

  //-- Version read --//
  if((ret=sinich_sys_get_fpga_version(ted_usb_sys))==-1){
    cerr << "Get fpga-version error!\n";
    goto END_PROC;
  }else printf("Fpga   version             : 0x%02X\n", (uchar)ret);

  if((ret=sinich_sys_get_driver_version(ted_usb_sys))==-1){
    cerr << "Get driver-version!\n";
    goto END_PROC;
  }else printf("Driver version             : 0x%08X\n", (uint)ret);

  /*---------------------------------------------------------------*/
  /*--------------------------*/
  /*	Prepare audio in-data 	*/
  /*--------------------------*/

  /* Get input pkt size */
  audio_inpkt_siz=audio_inpkt_tbl[samp_index];
  audio_inpkt_siz+=4;

  /*---------------------------------------------------------------*/
  //-- Set sampling rate & channel(Audio) --//
  sample_rate_ch_set |= samp_index;
  sample_rate_ch_set |= (MODE_16CH<<3);

  if((ret = sinich_pcm_ctl(ted_usb_sys,sample_rate_ch_set)) != 0){
    cerr << "We can't set sampling status!";
    goto END_PROC;
  }
  if((ret = sinich_sys_get_reg( ted_usb_sys, 0x01, &fpga_rdata)) != 0){
    cerr << "We can't get sampling status!";
    goto END_PROC;
  }

  //-- Set codec parameter --//
  if((ret = sinich_sys_set_reg(ted_usb_sys,0x0B,0x01)) != 0){
    cerr << "Register set error!\n";
    goto END_PROC;
  }

  //-- Check slave board connection --//
  // set max gain  
  ret = codec_control_set(0, ted_usb_sys,6*16);

  if(ret < 0) goto END_PROC;

  if(fpga_rdata){
    ret = sinich_sys_set_reg( ted_usb_sys, 0x04, 0x00);
    if (ret!=0){
      cerr << "Set audio in enable register failed\n";
      goto END_PROC;
    }
  }

  //-- If already EP2 START, EP2 STOP --//
  if((ret = sinich_sys_get_reg( ted_usb_sys, 0x03, &fpga_rdata)) != 0){
    cerr << "Reg Read Error!\n";
    goto END_PROC;
  }
  if(fpga_rdata){
    ret = sinich_sys_set_reg( ted_usb_sys, 0x03, 0x00);
    if (ret!=0){
      cerr << "Set audio out enable register failed\n";
      goto END_PROC;
    }
  }

  if(sinich_sys_start(ted_usb_sys)==-1){
    cerr << "Audio in start error !\n";
    goto	END_PROC;
  }

  recorderThread.reset(new thread(ref(*this)));

  return 0;


 END_PROC:
  if(ted_usb_sys!=NULL){	sinich_sys_stop(ted_usb_sys);}
  if(audio_in_buf !=NULL)		free(audio_in_buf);

  exit(1);

}



// convert sinich data format to hark format
int SINICHRecorder::ich2mat(void){

  int nsamp = 2 * sampling_rate * 2 / 1000; // 2byte*sampRate*2ms
  //int nsamp = sampling_rate * 2 / 1000; // 2byte*sampRate*2ms
  unsigned short m;
  short *temp1, *temp2; // just for simplify

  int b, t, c, s;  // loop counters for
  // b: block current packet number
  // t: time  first 2ms or last 2ms
  // c: chan  channel number
  // s: sampl sampling rate


  int harf_pkt = sampling_rate*2/1000;    // デバイスは2ms毎のまとまりで扱っているため

  m=0;
  for( b=0;b<ret/audio_inpkt_siz;b++ ){
    temp1 = (short *)(audio_in_buf + b * audio_inpkt_siz + 4);
    for( t=0;t<2;t++ ){
      // 後半の始まりは2(byte)*channel_count(ch)*samprate(Hz)*2/1000(s)
      temp2 = &temp1[t*channel_count*harf_pkt];
      for( s=0;s<nsamp/2;s++ ){
		for( c=0;c<channel_count;c++ ){
		  read_buf[m++] = (temp2[c*harf_pkt+s]);
		} // loop for sample number
      } // loop for channels
    } // loop for first 2ms or last 2ms
  } // loop for packets

  return m;
}
/*----------------------------------------------------*/
// Display connect audio board                        //
/*----------------------------------------------------*/
int codec_control_set(uchar msg_flag, struct ich_io_t *io,int set_gain_val)
{
  
  int ret;
  struct codec_param_t	codec[4];
  uchar fpga_rdata;
  cout << "============================" << endl;
  if((ret	= sinich_sys_act_info(io ,0 ,&fpga_rdata)) < 0){
    cout << "Audio board nothing" << endl;
    cerr << "===== Audio board nothing! (" << ret << ")<0) ===== \n";
  }else{
    if( (unsigned char)fpga_rdata == audio_board_num_table[0] ){
      codec[0].act_flag		= 0x0002;	// Bit1 set
      codec[0].adc_power_down	= 0;
      if((ret	= sinich_pcm_set_codec(io ,0 ,&codec[0])) < 0 ){
		cerr << "Codec set error!\n";
		return(-1);
      }
      Sleep(10);	// 10ms sleep
	  // Power down off
      codec[0].act_flag		= 0x0002;	// Bit1 set
      codec[0].adc_power_down	= 1;
      if((ret	= sinich_pcm_set_codec(io ,0 ,&codec[0])) < 0 ){
		cerr << "Codec set error!\n";
		return(-1);
      }

      // Wait for ADC initialize end(Addtional function 2006/06/23)
      codec[0].act_flag = 0x0200;

      while(1){
		ret	= sinich_pcm_get_codec(io , 0 ,codec);
		if(ret < 0) {
		  cerr << "Codec initialize status read error!!\n";
		  return(-1);
		}
		if(((codec[0].operation_mode) & 0x08) != 0){
		  break;
		}
      }

      if(set_gain_val != 0){
		// Set Gain(IPGA/ALC Mode Control 2)
		Sleep(100);	// 100ms sleep
		codec[0].act_flag		   = 0x0500;	// Set gain bit
		codec[0].input_pga_control = set_gain_val;      // IPGA data
		codec[0].alc_mode_control2 = set_gain_val;	// ALC Mode Control2 data
		if((ret	= sinich_pcm_set_codec(io ,0 ,&codec[0])) < 0 ){
		  cerr << "Codec set error!\n";
		  return(-1);
		}
      }

      if (msg_flag)
		{// For debug
		  int		cnt1;
		  int		cnt2;
		  codec[0].act_flag		= 0x1FFF;	// All read
		  codec[1].act_flag		= 0x1FFF;	// All read
		  codec[2].act_flag		= 0x1FFF;	// All read
		  codec[3].act_flag		= 0x1FFF;	// All read
		  ret	= sinich_pcm_get_codec(io , 0 ,codec);
		  if( ret < 0 ){
			cerr << "Get codec info error!\n";
			return(-1);
		  }else{
			for( cnt1=0 ; cnt1<4 ; cnt1++ ){
			  for( cnt2=0 ; cnt2<0x0C ; cnt2++ ){
				printf("Address 0x%02X : Data 0x%02X\n",
					   cnt2,*(((unsigned char *)&codec[cnt1])+cnt2));
			  }
			}
		  }
		}// For debug
    }
  }


  return 0;
}

/*----------------------------------------------------*/
// Delay,Hex checker,ASCII to Integer change          //
/*----------------------------------------------------*/
void insert_delay(uint delay)
{
  volatile uint count_down;

  for (count_down = delay; count_down > 0; count_down--);
}

// Alphabet Hexadecimal number charactor to Integer
int ahextoi(char hex)
{
  if ((hex >= '0') && (hex <= '9')) return (hex - '0');
  else if ((hex >= 'a') && (hex <= 'f')) return (hex + 10 - 'a');
  else if ((hex >= 'A') && (hex <= 'F')) return (hex + 10 - 'A');

  return -1;
}

// Alphabet Hexadecimal number charactor to Integer
int ahextoi2(char *hex_char, int digit)
{
  int base = 1, var, ret = 0;

  digit++;
  while(digit != 1) {
    var = ahextoi(hex_char[digit--]);
    if ((var<0) || (var>15)) {
      return -1;
    }
    ret += var * base;
    base <<= 4;
  }

  return ret;
}

// check argument header is hexadecimal or not
int hex_check(char *p)
{
  if (('0' != p[0]) || !(('x' == p[1]) || 'X' == p[1])) {
    return -1;
  }
	
  return 0;
}
