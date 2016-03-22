#include <iostream>
#include <fstream>

using namespace std;


#include <boost/thread.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "WSRecorder.hpp"

using namespace boost;
using namespace boost::posix_time;

void WriteWaveHeader(ostream& out, int ch, unsigned sampling_rate,
                     unsigned short bytes_per_sample);
void WriteSize(ostream& out);

int main(int argc, char* argv[])
{
  if (!(argc == 6 || argc == 7)) {
    cout << "usage: wsrecorder ip_addr channel sampling_rate duration filename [logfile]\n";
    return 0;
  }
    
  Recorder* recorder(WSRecorder::GetInstance());
  int channel = lexical_cast<int>(argv[2]);
  unsigned sampling_rate = lexical_cast<unsigned>(argv[3]);
  double duration = lexical_cast<double>(argv[4]);
  ofstream fout(argv[5], ios::binary);
  ofstream fout_logfile;
  bool logfile_is_valid = false;
  if (argc == 7) {
    fout_logfile.open(argv[6]);
    logfile_is_valid = true;
  }
  
  int buffer_length = 160;
  int sleep_time_usec = buffer_length * 1000000 / sampling_rate;
  int log_period = sampling_rate * 60;
  vector<int> buffer(buffer_length * channel);
  unsigned short bytes_per_sample = (channel == 10 ? 3 : 2);
  WriteWaveHeader(fout, channel, sampling_rate, bytes_per_sample);
  
  //recorder->Initialize("plughw:0", 8, 48000, 1024 * 1024);
  int result = recorder->Initialize(argv[1], channel, sampling_rate, sampling_rate * 10);
  if (result) {
    cerr << "Cannot open Wireless System.\n";
    return 0;
  }

  ptime start_time;
  ptime end_time;
  ptime cur_time;

  recorder->Start();
  //start_time = microsec_clock::local_time();

  unsigned time = 0;
  while (time < sampling_rate * duration) {
    if (logfile_is_valid && time % log_period == 0) {
      cur_time = microsec_clock::local_time();
      fout_logfile << time << " " << cur_time << "\n";
    }
        
    Recorder::BUFFER_STATE state;
    for (;;) {
      state = recorder->ReadBuffer(time, buffer_length, buffer.begin());

      if (state == Recorder::OK) {
        break;
      }
      else if (state == Recorder::UNDERRUN) {
        usleep(sleep_time_usec);
        continue;
      }
      else {
      }
    }
    //cout << recorder->GetCurrentTime() << endl;
    time += buffer_length;
    //copy(buffer.begin(), buffer.end(), buf_short.begin());
    /*
    transform(buffer.begin(), buffer.end(), buf_short.begin(),
	      float_to_int16);
    */
    //fout.write((char*)&buffer[0], buffer_length * channel * sizeof(short));
    for (int i = 0; i < buffer_length * channel; i++) {
      fout.write((char*)&buffer[i], bytes_per_sample);
    }
  }
  if (logfile_is_valid) {
    end_time = microsec_clock::local_time();
    fout_logfile << time << " " << end_time << endl;
  }

  recorder->Stop();

  //cout << "End\n";

  WriteSize(fout);

  return 0;
}

void WriteWaveHeader(ostream& out, int ch, unsigned sampling_rate,
                     unsigned short bytes_per_sample)
{
  char riff[] = "RIFF";
  unsigned filesize = 0;
  char wave[] = "WAVE";
  char format[] = "fmt ";
  unsigned byte_format = 16;
  unsigned short format_id = 1;
  unsigned short channel = ch;
  //unsigned sampling_rate = sampling_rate;
  unsigned bytes_per_sec = sampling_rate * bytes_per_sample * channel;
  unsigned short block_align = bytes_per_sample * channel;
  unsigned short bits_per_sample = bytes_per_sample * 8;
  char data[] = "data";
  unsigned byte_data = 0;
    
  out.write(riff, 4);
  out.write((char*)&filesize, 4);
  out.write(wave, 4);
  out.write(format, 4);
  out.write((char*)&byte_format, 4);
  out.write((char*)&format_id, 2);
  out.write((char*)&channel, 2);
  out.write((char*)&sampling_rate, 4);
  out.write((char*)&bytes_per_sec, 4);
  out.write((char*)&block_align, 2);
  out.write((char*)&bits_per_sample, 2);
  out.write(data, 4);
  out.write((char*)&byte_data, 4);
}

void WriteSize(ostream& out)
{
  unsigned filesize = out.tellp();
  unsigned size = filesize - 8;
  out.seekp(4, ios_base::beg);
  out.write((char*)&size, 4);
  unsigned data_size = filesize - 44;
  out.seekp(40, ios_base::beg);
  out.write((char*)&data_size, 4);
}
