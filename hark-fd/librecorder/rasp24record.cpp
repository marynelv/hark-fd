#include <iostream>
#include <fstream>

using namespace std;


#include <boost/thread.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "RASP24Recorder.hpp"

using namespace boost;
using namespace boost::posix_time;

void WriteWaveHeader(ostream& out, int ch, unsigned sampling_rate,
                     unsigned short bytes_per_sample);
void WriteSize(ostream& out);


int main(int argc, char* argv[])
{


	if (!(argc == 7 || argc == 8)) 
	{
		cout << "usage: rasp24recorder ip_addr channel sampling_rate duration bit_mode gain filename [logfile]\n";
		return 0;
	}

	Recorder* recorder(RASP24Recorder::GetInstance());
	int channel = lexical_cast<int>(argv[2]);
	unsigned sampling_rate = lexical_cast<unsigned>(argv[3]);
	double duration = lexical_cast<double>(argv[4]);

	string ip_addr;
	string ip_bit;
	string bit_mode;
	string  ad_gain;
		
	ip_addr  = argv[1];
	bit_mode = argv[5];
	ad_gain = argv[6];
	
	if( bit_mode == "16" )
	{
		ip_bit = ip_addr + ",RASP24-16" + "," + ad_gain;
	}
	else if( bit_mode == "32" )
	{
		ip_bit = ip_addr + ",RASP24-32" + "," + ad_gain;
	}
	else
	{
		cerr << "bit_mode error : " << bit_mode << endl;
		return 0;
	}
	
	
	ofstream fout(argv[7], ios::binary);
	ofstream fout_logfile;

	bool logfile_is_valid = false;

	if (argc == 9) 
	{
		fout_logfile.open(argv[8]);
		logfile_is_valid = true;
	}
  
	int buffer_length = 160;
	int sleep_time_usec = buffer_length * 1000000 / sampling_rate;
	int log_period = sampling_rate * 60;
	vector<int> buffer(buffer_length * channel);
	unsigned short bytes_per_sample = (channel == 10 ? 3 : 2);

//#ifdef DEBUG
	cout << "channel="          << channel << endl;
	cout << "sampling_rate="    << sampling_rate << endl;
	cout << "duration="         << duration << endl;
	cout << "buffer_length="    << buffer_length << endl;
	cout << "sleep_time_usec="  << sleep_time_usec << endl;
	cout << "log_period="       << log_period << endl;
	cout << "bytes_per_sample=" << bytes_per_sample << endl;
	cout << "ip_bit="           << ip_bit << endl;
	cout << "ad_gain="           << ad_gain<< endl;
//#endif

	WriteWaveHeader(fout, channel, sampling_rate, bytes_per_sample);

	int result = recorder->Initialize( ip_bit.c_str() , channel, sampling_rate, sampling_rate * 10);

	if (result) 
	{
		cerr << "Cannot open Wireless System.\n";
		return 0;
	}

	ptime start_time;
	ptime end_time;
	ptime cur_time;

	recorder->Start();

	unsigned time = 0;

	while (time < sampling_rate * duration) 
	{
		if (logfile_is_valid && time % log_period == 0) 
		{
			cur_time = microsec_clock::local_time();
			fout_logfile << time << " " << cur_time << "\n";
		}

		Recorder::BUFFER_STATE state;
		for (;;) 
		{
			state = recorder->ReadBuffer(time, buffer_length, buffer.begin());

			if (state == Recorder::OK) 
			{
				break;
			}
			else if (state == Recorder::UNDERRUN) 
			{
				usleep(sleep_time_usec);
				continue;
			}
			else {
			}
		}

		time += buffer_length;

		for (int i = 0; i < buffer_length * channel; i++) 
		{
			fout.write((char*)&buffer[i], bytes_per_sample);
		}
	}

	if (logfile_is_valid) 
	{
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
  unsigned byte_format = 32;
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
