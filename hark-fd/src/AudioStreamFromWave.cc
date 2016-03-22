/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <iostream>
#include <BufferedNode.h>
#include <Buffer.h>
#include <Matrix.h>
#include <Stream.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Time.h"

using namespace FD;
using namespace std;
using namespace boost::posix_time;

class AudioStreamFromWave;

DECLARE_NODE(AudioStreamFromWave)
/*Node
 *
 * @name AudioStreamFromWave
 * @category HARK:AudioIO
 * @description Read sound data from a multi-channel wave file.
 *
 * @input_name INPUT
 * @input_type Stream
 * @input_description An audio input stream (IStream).
 *
 * @output_name AUDIO
 * @output_type Matrix<float>
 * @output_description Windowed sound data. A row index is channel, and a column index is time.
 *
 * @output_name NOT_EOF
 * @output_type bool
 * @output_description True unless End-of-File
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description The frame length of each channel (in samples) [default: 512].
 *
 * @parameter_name ADVANCE
 * @parameter_type int
 * @parameter_value 160
 * @parameter_description The shift length beween adjacent frames (in samples)[default: 160].
 *
 * @parameter_name USE_WAIT
 * @parameter_type bool
 * @parameter_list true:false
 * @parameter_value false
 * @parameter_description If true, real recording is simulated [default: false].
 *
 END*/

class AudioStreamFromWave : public BufferedNode {
  int inputID;
  int audioID;
  int eofID;
  int timeID;
  int length;
  int advance;
  int num_channels;
  int nbits;
  int nbytes;

  vector<char> tmpBuffer;

  bool use_wait;
  ptime start_time;
  int length_ms;
  int advance_ms;
  int overlapped_usec;
    
public:
  AudioStreamFromWave(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params), num_channels(0)
  {
    inputID = addInput("INPUT");
    audioID = addOutput("AUDIO");
    eofID = addOutput("NOT_EOF");
    timeID = addOutput("TIME");

    length = dereference_cast<int>(parameters.get("LENGTH"));
    if (parameters.exist("ADVANCE")) {
      advance = dereference_cast<int>(parameters.get("ADVANCE"));
    }
    else advance = length;
    use_wait = dereference_cast<bool>(parameters.get("USE_WAIT"));

    inOrder = true;
  }

  virtual void initialize()
  {
    outputs[audioID].lookAhead
      = outputs[eofID].lookAhead
      = outputs[timeID].lookAhead
      = 1 + max(max(outputs[audioID].lookAhead, outputs[eofID].lookAhead),
                outputs[timeID].lookAhead);
    //outputs[audioID].lookBack += 1;

    outputs[audioID].lookBack
      = outputs[eofID].lookBack
      = outputs[timeID].lookBack
      = 1 + max(max(outputs[audioID].lookBack, outputs[eofID].lookBack),
                outputs[timeID].lookBack);
    //outputs[audioID].lookBack += 1;
    
    this->BufferedNode::initialize();
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    Buffer &audioBuffer = *(outputs[audioID].buffer);
    Buffer &eofBuffer = *(outputs[eofID].buffer);
    eofBuffer[count] = TrueObject;

    ObjectRef inputValue = getInput(inputID, count);

    RCPtr<Matrix<float> > outputp(new Matrix<float>(num_channels,
                                                    length));
    audioBuffer[count] = outputp;
    Matrix<float>& output = *outputp;

    RCPtr<Time> cur_time(new Time());
    (*(outputs[timeID].buffer))[count] = cur_time;

    if (count == 0) {
      vector<char> buf;

      checkHeader(inputValue);
      start_time = microsec_clock::local_time();
      buf.resize(num_channels * length * nbytes);
      tmpBuffer.resize(num_channels * advance * nbytes);
      output.resize(num_channels, length);

      if (use_wait) {
        wait(count);
      }
      if (!readStream(buf, num_channels * length, inputValue))
        {
          eofBuffer[count] = FalseObject;
          for (int r = 0; r < output.nrows(); r++) {
            for (int c = 0; c < output.ncols(); c++) {
              output(r, c) = 0;
            }
          }
        }
      else {
        convertVectorToMatrix(buf, output, 0);
      }
      cur_time->setTime(microsec_clock::local_time());
    }
    else {
      if (advance < length) {
        Matrix<float>& previous =
          object_cast<Matrix<float> >(audioBuffer[count - 1]);
                
        for (int c = 0; c < length - advance; c++) {
          for (int r = 0; r < output.nrows(); r++) {
            output(r, c) = previous(r, c + advance);
          }
        }
      }
      /*
      else {
        for (int c = 0; c < length - advance; c++) {
          for (int r = 0; r < output.nrows(); r++) {
            output(r, c) = 0;
          }
        }
      }
      */

      if (use_wait) {
        wait(count);
      }
      if (!readStream(tmpBuffer, num_channels * advance, inputValue)) {
        eofBuffer[count] = FalseObject;

        for (int c = 0; c < length; c++) {
          for (int r = 0; r < output.nrows(); r++) {
            output(r, c) = 0.0f;
          }
        }
      }
      else {
        convertVectorToMatrix(tmpBuffer, output,
                              length - min(advance, length));
      }
      cur_time->setTime(microsec_clock::local_time() - microseconds(overlapped_usec));
    }
  }

protected:
  void convertVectorToMatrix(const vector<char>& in, Matrix<float>& out,
                             int first_col)
  {
    const unsigned negative_mask_16 = 0xffff0000;
    const unsigned negative_mask_24 = 0xff000000;
    const unsigned sign_bit_16 = 0x00008000;
    const unsigned sign_bit_24 = 0x00800000;
    
    int buf;
    char* ptr = (char*)&buf;

    for (int i = 0; i < out.nrows(); i++) {
      for (int j = first_col; j < out.ncols(); j++) {
        int start = (i + (j - first_col) * out.nrows()) * nbytes;
        buf = 0;
        switch (nbits) {
        case 16:
          ptr[0] = in[start];
          ptr[1] = in[start + 1];
          if (buf & sign_bit_16) {
            buf |= negative_mask_16;
          }
          break;
        case 24:
          ptr[0] = in[start];
          ptr[1] = in[start + 1];
          ptr[2] = in[start + 2];
          if (buf & sign_bit_24) {
            buf |= negative_mask_24;
          }
          break;
        default:
          throw new NodeException(NULL, string("Not supported bits"),
                                  __FILE__, __LINE__);
        }
          
        out(i, j) = (float)buf;
      }
    }
  }

  void checkHeader(ObjectRef inputValue)
  {
    IStream& file = object_cast<IStream>(inputValue);
    char buf[5];
    buf[4] = '\0';
    unsigned size;
    short wFormatID;
    short wChannels;
    short wBits;
    unsigned dwSamplingRate;
        

    file.read(buf, 4);
    if (string("RIFF") != buf) {
      throw new NodeException(NULL, string("Not RIFF file"),
                              __FILE__, __LINE__);
    }
    file.read((char*)&size, 4);
    file.read(buf, 4);
    if (string("WAVE") != buf) {
      throw new NodeException(NULL, string("Not WAVE file"),
                              __FILE__, __LINE__);
    }

    while (!file.eof()) {
      file.read(buf, 4);

      if (string("fact") == buf || string("LIST") == buf) {
        file.read((char*)&size, 4);
        file.seekg(size, ios::cur);
      }
      else if (string("fmt ") == buf) {
        file.read((char*)&size, 4);
        file.seekg(2, ios::cur);
        file.read((char*)&wChannels, sizeof(short));
        num_channels = wChannels;
        file.read((char*)&dwSamplingRate, sizeof(unsigned));

        length_ms = (length * 1000) / dwSamplingRate;
        advance_ms = (advance * 1000) / dwSamplingRate;
        overlapped_usec = (length - advance) * 1000000 / dwSamplingRate;

        file.seekg(6, ios::cur);
        file.read((char*)&wBits, sizeof(short));
        nbits = wBits;
        nbytes = nbits / 8;
        file.seekg(size - 16, ios::cur);
      }
      else if (string("data") == buf) {
        file.seekg(4, ios::cur);
        break;
      }
      else {
        throw new NodeException(NULL, string("Bad WAVE header"),
                                __FILE__, __LINE__);
      }
    }
  }

  bool readStream(vector<char>& buffer, int length, ObjectRef inputValue)
  {
    IStream& file = object_cast<IStream>(inputValue);

    file.read((char*)&buffer[0], nbytes * length);
    if (file.eof()) {
      return false;
    }
        
    return true;
  }

  void wait(int count)
  {
    long elapsed_time;
    ptime cur_time;
    while (cur_time = microsec_clock::local_time(),
           elapsed_time = (cur_time - start_time).total_milliseconds(),
           elapsed_time <= length_ms + count * advance_ms) {
      usleep(1000);
    }
  }
};
