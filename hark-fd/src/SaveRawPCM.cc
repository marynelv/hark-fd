/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <climits>
#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Matrix.h"
#include "Source.h"
#include "Map.h"
#include <iostream>
#include <cstring>
#include <fstream>

#include <boost/shared_ptr.hpp>

#include "fft.hpp"

using namespace std;
using namespace FD;
using namespace boost;

class SaveRawPCM;

DECLARE_NODE(SaveRawPCM)
/*Node
 *
 * @name SaveRawPCM
 * @category HARK:AudioIO
 * @description Save audio signals as a raw PCM file.
 *
 * @input_name INPUT
 * @input_type any
 * @input_description Audio signals in time domain. Supported types are Map<int,ObjectRef> and Matrix<float>. In case of Map<int,ObjectRef>, the key is a source ID, and the value is a signal vector (Vector<complex<float> >). In case of Matrix<float>, each row including a signal vector and multi-channel audio signals are saved.
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description The same as input.
 *
 * @parameter_name BASENAME
 * @parameter_type string
 * @parameter_value sep_
 * @parameter_description Basename of raw PCM files. [default: sep_]
 *
 * @parameter_name ADVANCE
 * @parameter_type int
 * @parameter_value 160
 * @parameter_description The shift length beween adjacent frames (in samples)[default: 160].
 *
 * @parameter_name BITS
 * @parameter_type int
 * @parameter_value 16 
 * @parameter_list 16:24
 * @parameter_description Bit format of samples. 16, 24, and 32 bits are supported.
 *
 END*/


class SaveRawPCM : public BufferedNode {
  int inputID;
  int outputID;
  int sourcesID;

  string basename;
  int advance;
  int bits_name;	
  map<int, shared_ptr<ofstream> > streams;
  shared_ptr<ofstream> fout;
  vector<char> buf;
  int bytes_per_sample;



public:
  SaveRawPCM(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params), sourcesID(-1)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");

    basename = object_cast<String>(parameters.get("BASENAME"));
    advance = dereference_cast<int>(parameters.get("ADVANCE"));
    bits_name = dereference_cast<int>(parameters.get("BITS"));

    int bits = 16;
    if(bits_name == 16)
    {
		bits = 16;
    }
    else if(bits_name == 24)
	{
		bits = 24;
	}
	else if	(bits_name == 32)
	{
		bits = 32;
	}

  	bytes_per_sample = bits / 8;

    inOrder = true;
  }

  ~SaveRawPCM() {}
    
  virtual int translateInput(string inputName)
  {
    for (unsigned int i = 0; i < inputs.size(); i++) {
      if (inputs[i].name == inputName) {
        return i;
      }
    }
    if (inputName == "SOURCES") {
      return sourcesID = addInput(inputName);
    }
    else {
      throw new NodeException(this, inputName + " is not supported.", __FILE__, __LINE__);
    }
  }

  void reset()
  {
    streams.clear();
    fout.reset();
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    ObjectRef in = getInput(inputID, count);

    // cout << "SaveRawPCM start count->" << count << endl; 

    if (typeid(*in) == typeid(Map<int, ObjectRef>)) 
    {

      RCPtr<Vector<ObjectRef> > sources;
      if (sourcesID != -1) {
	sources = getInput(sourcesID, count);
      }

      RCPtr<Map<int, ObjectRef> > input = in;

      if (sources.isNil()) {
      Map<int, ObjectRef>::iterator it = input->begin();
      for (; it != input->end(); ++it) 
      {
        Vector<float>& x = object_cast<Vector<float> >(it->second);
        
        shared_ptr<ofstream> fout;
        if (streams.find(it->first) == streams.end()) 
        {
			ostringstream name_str;
			name_str << basename << it->first << ".sw";
			fout.reset(new ofstream(name_str.str().c_str()));
			streams[it->first] = fout;
			convertBitFormat(x, buf, 0, x.size());
        }
        else 
        {
          convertBitFormat(x, buf, x.size() - advance, x.size());
          fout = streams[it->first];
        }

        fout->write((char*)&buf[0], buf.size());
        fout->flush();
      }
      }else{

	for (int i = 0; i < sources->size(); i++) {
	  RCPtr<Source> src = (*sources)[i];
	  Map<int, ObjectRef>::iterator it = input->find(src->id);
	  if (it == input->end()) continue;
	  Vector<float>& x = object_cast<Vector<float> >(it->second);
	  shared_ptr<ofstream> fout;
	  if (streams.find(src->id) == streams.end()) {
	    ostringstream name_str;
	    name_str << basename << src->id << ".sw";
	    fout.reset(new ofstream(name_str.str().c_str()));
	    streams[src->id] = fout;
	    convertBitFormat(x, buf, 0, x.size());
	  }else{
	    convertBitFormat(x, buf, x.size() - advance, x.size());
	    fout = streams[src->id];
	  }
	  if (it != input->end()) {
	    fout->write((char*)&buf[0], buf.size());
	    fout->flush();
	  }
	}

      }
      
      if (count % 100 == 0) {
        closeFiles(input);
      }
    }
    else if (typeid(*in) == typeid(Matrix<float>)) 
    {
   	  
      Matrix<float>& x = object_cast<Matrix<float> >(in);
      int num_chan = x.nrows();
      int length = x.ncols();
      if (!fout) 
      {
        fout.reset(new ofstream((basename + ".sw").c_str(), ios::binary));

        convertBitFormat(x, buf, 0, length - advance);
        fout->write((char*)&buf[0], buf.size());
        fout->flush();
      }



      convertBitFormat(x, buf, length - advance, length);
      fout->write((char*)&buf[0], buf.size());
      fout->flush();
    }
    else 
    {
      throw new NodeException(this, string("Only Map<int, ObjectRef> and Matrix<float> is supported."), __FILE__, __LINE__);
    }

    out[count] = in;
  }

  void closeFiles(RCPtr<Map<int, ObjectRef> > input)
  {
    map<int, shared_ptr<ofstream> >::iterator streams_it = streams.begin();
    while (streams_it != streams.end()) {
      if (input->find(streams_it->first) == input->end()) {
        streams.erase(streams_it++);
      }
      else {
        ++streams_it;
      }
    }
  }

  float floorClipping(float x, int bps)
  {
    if (bps==2){
      x = (x >= SHRT_MAX ? (float)SHRT_MAX : x);
      x = (x <= SHRT_MIN ? (float)SHRT_MIN : x);
    }
    if (bps==3){
      x = (x >= (((SHRT_MAX + 1) * (UCHAR_MAX + 1)) - 1) ? (float)(((SHRT_MAX + 1) * (UCHAR_MAX + 1)) - 1) : x);
      x = (x <= (-(SHRT_MAX + 1) * (UCHAR_MAX + 1))      ? (float)(-(SHRT_MAX + 1) * (UCHAR_MAX + 1))      : x);
    }
    if (bps==4){
      x = (x > INT_MAX - 100 ? (float)INT_MAX - 100 : x); // fix me
      x = (x < INT_MIN ? (float)INT_MIN : x);
    }    
    return x;
  }



  void convertBitFormat(Matrix<float>& x, vector<char>& buf,
                        int t_first, int t_last)
  {

    buf.resize(x.nrows() * (t_last - t_first) * bytes_per_sample);
    vector<char>::iterator it = buf.begin();

	
	switch(bytes_per_sample)
	{

	case 4:

		for (int j = t_first; j < t_last; j++) 
		{
		  for (int i = 0; i < x.nrows(); i++) 
		  {
		        float x2 = floorClipping(x(i, j),bytes_per_sample);
			int32_t v = (int32_t)x2;
		        // x(i, j) = floorClipping(x(i, j),bytes_per_sample);
			// int32_t v = (int32_t)x(i, j);
			char* p = (char*)&v;
			copy(p, p + bytes_per_sample, it);
			it += bytes_per_sample;
		  }
		}
		break;

/*
	//windowsのkinectは、これが必要
	case 4:

		for (int j = t_first; j < t_last; j++) 
		{
		  for (int i = 0; i < x.nrows(); i++) 
		  {
			float v = (float)x(i, j);

			//cout << "x(i, j)->" << x(i, j) <<  "  float v->" <<  v << endl;	

			char* p = (char*)&v;
			copy(p, p + bytes_per_sample, it);
			it += bytes_per_sample;
		  }
		}
		break;
*/


	default:
		for (int j = t_first; j < t_last; j++) 
		{
		  for (int i = 0; i < x.nrows(); i++) 
		  {
		        float x2 = floorClipping(x(i, j),bytes_per_sample);
			int v = (int)x2;
		        // x(i, j) = floorClipping(x(i, j),bytes_per_sample);
			// int v = (int)x(i, j);
			char* p = (char*)&v;
			copy(p, p + bytes_per_sample, it);
			it += bytes_per_sample;
		  }
		}
		break;
	}
	
    if (it != buf.end()) 
    {
      cout << distance(it, buf.end()) << endl;
    }
  }



  void convertBitFormat(Vector<float>& x, vector<char>& buf,
                        int t_first, int t_last)
  {
    buf.resize((t_last - t_first) * bytes_per_sample);
    vector<char>::iterator it = buf.begin();

	switch(bytes_per_sample)
	{

	case 4:
		for (int i = t_first; i < t_last; i++) 
		{
		        float x2 = floorClipping(x[i],bytes_per_sample);
			int32_t v = (int32_t)x2;
		        // x[i] = floorClipping(x[i],bytes_per_sample);
			// int32_t v = (int32_t)x[i];
			char* p = (char*)&v;
			copy(p, p + bytes_per_sample, it);
			it += bytes_per_sample;
		}
		break;


/*
   //window版kinectで必要
	case 4:
		for (int i = t_first; i < t_last; i++) 
		{
			float v = (float)x[i];
			char* p = (char*)&v;
			copy(p, p + bytes_per_sample, it);
			it += bytes_per_sample;
		}
		break;
*/
	default:
		for (int i = t_first; i < t_last; i++) 
		{
		        float x2 = floorClipping(x[i],bytes_per_sample);
			int v = (int)x2;
		        // x[i] = floorClipping(x[i],bytes_per_sample);
			// int v = (int)x[i];
			char* p = (char*)&v;
			copy(p, p + bytes_per_sample, it);
			it += bytes_per_sample;
		}
		break;
	}
  }
  
  IN_ORDER_NODE_SPEEDUP(SaveRawPCM);

};


