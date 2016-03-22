#include <iostream>
#include <fstream>

using namespace std;


#include <boost/thread.hpp>
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "ALSARecorder.hpp"
#include "ASIORecorder.hpp"

using namespace boost;

void WriteWaveHeader(ostream& out, unsigned sampling_rate);
void WriteSize(ostream& out);

int main(int argc, char* argv[])
{
    if (argc != 4) {
        cout << "usage: recorder sampling_rate duration filename\n";
        return 0;
    }
    
	Recorder* recorder(ASIORecorder::GetInstance());
    //Recorder* recorder(new ALSARecorder);
    vector<short> buffer(1024 * 8);
    unsigned sampling_rate = lexical_cast<unsigned>(argv[1]);
    double duration = lexical_cast<double>(argv[2]);
    ofstream fout(argv[3], ios::binary);
    WriteWaveHeader(fout, sampling_rate);

	//recorder->Initialize("plughw:0", 8, 48000, 1024 * 1024);
    recorder->Initialize("plughw:0", 8, sampling_rate, 1024 * 1024);

	recorder->Start();

    unsigned time = 0;
    while (time < sampling_rate * duration) {
        Recorder::BUFFER_STATE state;
        for (;;) {
            state = recorder->ReadBuffer(time, 1024, buffer.begin());

            if (state == Recorder::OK) {
                break;
            }
            else if (state == Recorder::UNDERRUN) {
                usleep(40000);
                continue;
            }
            else {
            }
        }

        //cout << recorder->GetCurrentTime() << endl;
        time += 1024;
        fout.write((char*)&buffer[0], 1024 * 8 * sizeof(short));
    }

	recorder->Stop();

    //cout << "End\n";

    WriteSize(fout);

	return 0;
}

void WriteWaveHeader(ostream& out, unsigned sampling_rate)
{
    char riff[] = "RIFF";
    unsigned filesize = 0;
    char wave[] = "WAVE";
    char format[] = "fmt ";
    unsigned byte_format = 16;
    unsigned short format_id = 1;
    unsigned short channel = 8;
    //unsigned sampling_rate = sampling_rate;
    unsigned bytes_per_sec = sampling_rate * 2 * 8;
    unsigned short block_align = 2 * 8;
    unsigned short bits_per_sample = 16;
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
