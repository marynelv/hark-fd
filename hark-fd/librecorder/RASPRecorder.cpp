#include <iostream>

#include "RASPRecorder.hpp"

#include <boost/shared_ptr.hpp>

#include "endian.hpp"

using namespace boost;

RASPRecorder* RASPRecorder::instance = 0;

// This function is executed in another thread, and
// records acoustic signals into circular buffer.
void RASPRecorder::operator()()
{
    const int header_period = 384;
	read_buf.resize(channel_count * (header_period - 1) * 20);
    std::vector<short> buf(16 * header_period * 20);
    int header_count = 0;
    int read_size;

    for (;;) {
        usleep(sleep_time);
		std::cout << "thread\n";
        mutex::scoped_lock lk(mutex_buffer);
        if (state == PAUSE) {
            continue;
        }
        else if (state == STOP) {
            break;
        }
        else if (state == RECORDING) {
            read_size = read(fd_adc, (char*)&buf[0],
                             buf.size() * sizeof(short));
            std::cout << "read_size = " << read_size << std::endl;
            if (read_size <= 0) {
                break;
            }

            int frame_count = read_size / 16 / sizeof(short);
            int i = 0;
            int j = 0;
            while (j < frame_count) {
                if (header_count != 0) {
                    int start_rb = i * channel_count;
                    int start_buf = j * 16;
                    read_buf[start_rb + 0] = swap_endian(buf[start_buf + 8]);
                    read_buf[start_rb + 1] = swap_endian(buf[start_buf + 10]);
                    read_buf[start_rb + 2] = swap_endian(buf[start_buf + 12]);
                    read_buf[start_rb + 3] = swap_endian(buf[start_buf + 14]);
                    read_buf[start_rb + 4] = swap_endian(buf[start_buf + 9]);
                    read_buf[start_rb + 5] = swap_endian(buf[start_buf + 11]);
                    read_buf[start_rb + 6] = swap_endian(buf[start_buf + 13]);
                    read_buf[start_rb + 7] = swap_endian(buf[start_buf + 15]);

                    cur_time++;
                    i++;
                    j++;
                }
                else {
                    j++;
                }
                
                header_count++;
                if (header_count == header_period) {
                    header_count = 0;
                }
            }
            std::vector<short>::iterator it = read_buf.begin();
            advance(it, i * channel_count);
            buffer.insert(buffer.end(), read_buf.begin(), it);
        }

        lk.unlock();
    }
}

// A device for recording is initialized.
//   device_name : Audio device name.  ex. "plughw:0,0"
//   chan_count : A number of channels. ex. 8 if eight microphones are used.
//   samp_rate : A frequency of sampling.
//   buf_size : A size of circular buffer.
int RASPRecorder::Initialize(const std::string& device_name,
                             int chan_count, int samp_rate,
                             size_t buf_size)
{
    mutex::scoped_lock lk(mutex_buffer);

    buffer_size = buf_size;
    channel_count = chan_count;
    buffer.set_capacity(buffer_size * channel_count);
    buffer.clear();
    unsigned long sampling_rate = (unsigned long)((float)ADC_BASE_FREQ/samp_rate + 0.5);
    sleep_time = read_buf_size * 1000000 / sampling_rate / 3;

    int target = 0;

    fd_adc = open(device_name.c_str(), O_RDWR, 0666);
    if (fd_adc < 0) {
        std::cerr << device_name << " Open Error.\n";
        return -1;
    }

    ioctl(fd_adc, FIO_ADC_SAMPLE_RATE, sampling_rate);
    
    recorderThread.reset(new thread(ref(*this)));

    return 0;
}
