#include <iostream>
#include <fstream>

using namespace std;


#include <boost/thread.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "HARKNETAPIRecorder.hpp"

using namespace boost;
using namespace boost::posix_time;

void WriteWaveHeader(ostream& out, int ch, unsigned sampling_rate,
                     unsigned short bytes_per_sample);

void WriteSize(ostream& out);


//***************************************************************************************//
//
//  main
//
//***************************************************************************************//
int main(int argc, char* argv[])
{


	if (!(argc == 6 || argc == 7)) 
	{
		cout << "usage: harknetapirecorder ip_addr channel sampling_rate duration filename [logfile]\n";
		return 0;
	}

	Recorder* recorder(HARKNETAPIRecorder::GetInstance());
	int channel            = lexical_cast<int>(argv[2]);
	unsigned sampling_rate = lexical_cast<unsigned>(argv[3]);
	double duration        = lexical_cast<double>(argv[4]);

	
	ofstream fout(argv[5], ios::binary);
	ofstream fout_logfile;
	ofstream fout2("data.raw", ios::binary);
	
	bool logfile_is_valid = false;

	if (argc == 7) 
	{
		fout_logfile.open(argv[6]);
		logfile_is_valid = true;
	}
  
	int buffer_length   = 160;
//	int buffer_length   = 640;
	int sleep_time_usec = buffer_length * 1000000 / sampling_rate;
	int log_period      = sampling_rate * 60;
///	vector<int> buffer(buffer_length * channel);
	vector<float> buffer(buffer_length * channel);
	unsigned short bytes_per_sample = 4;
//	unsigned short bytes_per_sample = (channel == 10 ? 3 : 2);


	cout << "<>channel="          << channel << endl;
	cout << "<>sampling_rate="    << sampling_rate << endl;
	cout << "<>duration="         << duration << endl;
	cout << "<>buffer_length="    << buffer_length << endl;
	cout << "<>sleep_time_usec="  << sleep_time_usec << endl;
	cout << "<>log_period="       << log_period << endl;
	cout << "<>bytes_per_sample=" << bytes_per_sample << endl;


	//=================================================================================================//
	//
	//wave Header make
	//
	//=================================================================================================//
	WriteWaveHeader(fout, channel, sampling_rate, bytes_per_sample);



	//=================================================================================================//
	//
	//   initialize
	//	
	//=================================================================================================//
	cout << "main:initialize  START" << endl;




	int result = recorder->Initialize( argv[1] , channel, sampling_rate, sampling_rate * 100);

	if (result) 
	{
		cerr << "Cannot open Wireless System.\n";
		return 0;
	}

	ptime start_time;
	ptime end_time;
	ptime cur_time;



	cout << "main:initialize  END" << endl;
	//=================================================================================================//
	//
	// record start
	//
	//=================================================================================================//
	cout << "main:record   START" << endl;

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
		state = Recorder::OK;
		for (;;) 
		{
			state = recorder->ReadBuffer(time, buffer_length, buffer.begin());
			//cout << "<<<<<<<<<< Recorder state-> " << state << endl;

			if (state == Recorder::OK) 
			{
				cout << "<<<<<<<<< Recorder::OK" << endl;
				break;
			}
			else if (state == Recorder::UNDERRUN) 
			{
				//cout << "xxxxxxxxxxx sleep xxxxxxxxx" << state << endl;
				usleep(sleep_time_usec);
				continue;
			}
			else {
			}
		}

		time += buffer_length;
		cout << "time += buffer_length->" << time << endl;

		for (int i = 0; i < buffer_length * channel; i++) 
		{
	
			fout.write((char*)&buffer[i], bytes_per_sample);
			fout2.write((char*)&buffer[i], bytes_per_sample);
//			cout << buffer[i] << endl;
		}
	}

	if (logfile_is_valid) 
	{
		end_time = microsec_clock::local_time();
		fout_logfile << time << " " << end_time << endl;
	}



	//=================================================================================================//
	//
	// record stop
	//
	//=================================================================================================//
	recorder->Stop();

	//cout << "End\n";

	WriteSize(fout);
fout.close();
	return 0;
}



//=================================================================================================//
//
//   WriteWaveHeader
//
//=================================================================================================//
void WriteWaveHeader(ostream& out, int ch, unsigned sampling_rate,
                     unsigned short bytes_per_sample)
{
  char riff[]              = "RIFF";
  unsigned filesize        = 0;
  char wave[]              = "WAVE";
  char format[]            = "fmt ";
//  unsigned byte_format     = 16;
  unsigned byte_format     = 32;
  unsigned short format_id = 1;
  unsigned short channel   = ch;
  unsigned bytes_per_sec   = sampling_rate * bytes_per_sample * channel;
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



//=================================================================================================//
//
//   WriteSize
//
//=================================================================================================//
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
