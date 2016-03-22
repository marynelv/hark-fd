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

using namespace std;
using namespace FD;
using namespace boost;


class SaveWavePCM;

DECLARE_NODE(SaveWavePCM)
/*Node
 *
 * @name SaveWavePCM
 * @category HARK:AudioIO
 * @description Save audio signals as a wav file format.
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
 * @parameter_description Basename of files. [default: sep_]
 *
 * @parameter_name ADVANCE
 * @parameter_type int
 * @parameter_value 160
 * @parameter_description The shift length beween adjacent frames (in samples)[default: 160].
 *
 * @parameter_name SAMPLING_RATE
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_description Sampling rate (in samples)[default: 16000].
 *
 * @parameter_name BITS
 * @parameter_type string
 * @parameter_value int16 
 * @parameter_list int16:int24
 * @parameter_description Bit format of samples. int16 , int24 and float32 bits are supported.
 *
 END*/

class SaveWavePCM : public BufferedNode {

	typedef struct WaveHeader{
		const char *pRIFF; //={'R','I','F','F'};
		const char *pWAVE; //={'W','A','V','E'};
		const char *pFmt;  //={'f','m','t',' '};
		const char *pData; //={'d','a','t','a'};
		const int nFmtSize; //=16;
		const short nPCM; //=1 (PCMなら1）
		int nSize;
		short nCh;
		int nRate;//サンプリングレート
		int nAvgBytesPerSec;//速度
		short nBitsPerSamp; //量子化ビット数
		short nBlockAlign;//ブロックサイズ
		size_t sz;
		int i;
		char buf;
	public:
		WaveHeader(): pRIFF("RIFF"),pWAVE("WAVE"),
									pFmt("fmt "),pData("data"),
									nFmtSize(16),nPCM(1)
		{
		};
		~WaveHeader(){};
	}WaveHeader;

	int inputID;
	int outputID;
        int sourcesID;

	int bits;
	int advance;
	int file_count;
	int sampling_rate;
	string basename;
	string bits_name;	
	map<int, shared_ptr<ofstream> > streams;
	shared_ptr<ofstream> fout;
	vector<char> buf;
	vector<string> file_list;
	int bytes_per_sample;
	WaveHeader waveheader;


public:
	SaveWavePCM(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params), file_count(0), sourcesID(-1)
	{
		inputID  = addInput("INPUT");
		outputID = addOutput("OUTPUT");

		basename      = object_cast<String>(parameters.get("BASENAME"));
		bits_name     = object_cast<String>(parameters.get("BITS"));
		advance       = dereference_cast<int>(parameters.get("ADVANCE"));
		sampling_rate = dereference_cast<int>(parameters.get("SAMPLING_RATE"));

		if(bits_name == "int16")
			bits = 16;
		else if(bits_name == "int24")
			bits = 24;
		else if(bits_name == "float32")
			bits = 32;

		bytes_per_sample = bits / 8;
		inOrder = true;
	}

	~SaveWavePCM() 
	{
	}

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

	void setRIFFheader(shared_ptr<ofstream> fout, int nch, int nrate, int bits ){
		waveheader.nCh             = nch;
		waveheader.nRate           = nrate;
		waveheader.nAvgBytesPerSec = nch * nrate * bits / 8; //データ速度
		waveheader.nBitsPerSamp    = bits;
		waveheader.nBlockAlign     = nch * bits / 8; //ブロックサイズ

		fout->write( waveheader.pRIFF,4 ); /* RIFF (12)*/
		fout->write( (char*)(&waveheader.nSize),4 );
		fout->write( waveheader.pWAVE,4 ); /* WAVE */

		fout->write( waveheader.pFmt,4 ); /* fmt (24)*/
		fout->write( (char*)&waveheader.nFmtSize,4 ); /*format chunk size */
		fout->write( (char*)&waveheader.nPCM,2 );
		fout->write( (char*)&waveheader.nCh,2 );
		fout->write( (char*)&waveheader.nRate,4 );
		fout->write( (char*)&waveheader.nAvgBytesPerSec,4 );
		fout->write( (char*)&waveheader.nBlockAlign,2 );
		fout->write( (char*)&waveheader.nBitsPerSamp,2 );
		fout->write( (char*)waveheader.pData,4 ); /* data */
		fout->write( (char*)&waveheader.sz,4 ); /* data chunk size */
	}
	void updateRIFFHeader( void )
	{
		fout->flush();
		// 実際のファイルサイズを取得する
		fout->seekp(0,ios::end);
		waveheader.nSize = fout->tellp();
		fout->seekp(0,ios::beg);
		waveheader.nSize -= fout->tellp();

		// 実際のファイルサイズが N のとき、
		// nSize = N - 8;
		// sz = N - 44 = nSize - 36

		// ヘッダには実際のファイルサイズ -8 を書き込む（RIFFの分を引く)
		waveheader.nSize -= 8;

		// 波形のサイズはリニアPCMの場合，ファイルサイズより36小さい(一行前で8引いているので)
		waveheader.sz = waveheader.nSize - 36;

		// ファイルのヘッダの内容を書き直す
		fout->seekp( 4, ios::beg );
		fout->write( (char*)(&waveheader.nSize),4 );
		fout->seekp( 40, ios::beg );
		fout->write( (char*)(&waveheader.sz),4 );
		fout->seekp(0,ios::end);

	}
	void calculate(int output_id, int count, Buffer &out)
	{
		// インプットを受けとる
		ObjectRef in = getInput(inputID, count);

		// インプットがMAP形式のときの処理
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
	
				// shared_ptr<ofstream> fout;
				if (streams.find(it->first) == streams.end()) 
				{
					// new ID, increment counter
					if( file_count < it->first ) file_count=it->first;

					ostringstream name_str;
					name_str << basename << it->first << ".wav";
					file_list.push_back( name_str.str() );

					fout.reset(new ofstream(name_str.str().c_str()));
					setRIFFheader( fout, 1, sampling_rate, bits );


					streams[it->first] = fout;
					convertBitFormat(x, buf, 0, x.size());
				}
				else 
				{
					convertBitFormat(x, buf, x.size() - advance, x.size());
					fout = streams[it->first];
				}
				// データを書き込む
				fout->write((char*)&buf[0], buf.size());

				updateRIFFHeader();

				fout->flush();
			}
			}else{
			  
                       	for (int i = 0; i < sources->size(); i++) {
                       	  RCPtr<Source> src = (*sources)[i];
                       	  Map<int, ObjectRef>::iterator it = input->find(src->id);
                       	  if (it == input->end()) continue;
                       	  Vector<float>& x = object_cast<Vector<float> >(it->second);
                       	  if (streams.find(src->id) == streams.end()) {
			    if( file_count < src->id ) file_count=src->id;
			    ostringstream name_str;
			    name_str << basename << src->id << ".wav";
			    file_list.push_back( name_str.str() );
			    fout.reset(new ofstream(name_str.str().c_str()));
			    setRIFFheader( fout, 1, sampling_rate, bits );
			    streams[src->id] = fout;
			    convertBitFormat(x, buf, 0, x.size());
                       	  }else{
                       	    convertBitFormat(x, buf, x.size() - advance, x.size());
                       	    fout = streams[src->id];
                       	  }
                       	  if (it != input->end()) {
                       	    fout->write((char*)&buf[0], buf.size());
			    updateRIFFHeader();
                       	    fout->flush();
                       	  }
                       	}

			}

			if (count % 100 == 0) {
				closeFiles(input);
			}
		}
		// インプットが行列だったときの処理
		else if (typeid(*in) == typeid(Matrix<float>)) 
		{
 	
			Matrix<float>& x = object_cast<Matrix<float> >(in);
			int num_chan = x.nrows();
			int length = x.ncols();
			// foutが何も指してない＝準備が必要
			if (!fout) 
			{
				// foutがファイルを指し示すようにする（準備)
				ostringstream name_str;
				name_str << basename << "0.wav";
				fout.reset( new ofstream( name_str.str().c_str(), ios::binary) );

				setRIFFheader( fout, num_chan, sampling_rate, bits );

				// 準備が終わったから、データを書き込む
				convertBitFormat( x, buf, 0, length - advance );
				fout->write( (char*)&buf[0], buf.size() );

				updateRIFFHeader();


				fout->flush();
			}

			convertBitFormat(x, buf, length - advance, length);
			fout->write((char*)&buf[0], buf.size());

			updateRIFFHeader();

			fout->flush();
		}
		else 
		{
			throw new NodeException(this, string("Only Map<int, ObjectRef> and Matrix<float> is supported."),
															__FILE__, __LINE__);
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

/*
	bool checkClipping(float x)
	{
		if (x > SHRT_MAX || x < SHRT_MIN) 
		{
			cout << "checkClipping =true" << endl; 	
			return true;
		}
		else 
		{
			return false;
		}
	}
*/


	void convertBitFormat(Matrix<float>& x, vector<char>& buf,
												int t_first, int t_last)
	{

		buf.resize(x.nrows() * (t_last - t_first) * bytes_per_sample);
		vector<char>::iterator it = buf.begin();

	
		int clipmax, clipmin, clipped;
		clipped = 0;
		switch(bytes_per_sample)
		{
		case 2:
			clipmax=SHRT_MAX; clipmin=SHRT_MIN;
			break;
		case 3:
			clipmax=(((SHRT_MAX+1)*(UCHAR_MAX+1))-1); clipmin=(-(SHRT_MAX+1)*(UCHAR_MAX+1));
			break;
		case 4:
		default:
			clipmax=(INT_MAX-100); clipmin=INT_MIN; // FixMe from SaveRawPCM
			break;
		}
		for(int j = t_first; j < t_last; j++){
			for(int i = 0; i < x.nrows(); i++){
				int l = (x(i,j)>clipmax ? 1 : (x(i,j)<clipmin ? -1 : 0));
				clipped |= l;
				int v = (l>0 ? clipmax : (l<0 ? clipmin : (int)x(i,j)));
				char* p = (char*)&v;
				copy(p, p + bytes_per_sample, it);
				it += bytes_per_sample;
			}
		}
		if(clipped){ cout << "Clip has been detected !!" << endl; }

/*
		switch(bytes_per_sample)
		{
#if 0
		case 4:
			for (int j = t_first; j < t_last; j++) 
			{
				for (int i = 0; i < x.nrows(); i++) 
				{
					float v = (float)x(i, j);
					char* p = (char*)&v;
					copy(p, p + bytes_per_sample, it);
					it += bytes_per_sample;
				}
			}
			break;
#endif
		default:
			for (int j = t_first; j < t_last; j++) 
			{
				for (int i = 0; i < x.nrows(); i++) 
				{
					int v = (int)x(i, j);
					char* p = (char*)&v;
					copy(p, p + bytes_per_sample, it);
					it += bytes_per_sample;
				}
			}
			break;
		}
*/
	
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

		int clipmax, clipmin, clipped;
		clipped = 0;
		switch(bytes_per_sample)
		{
		case 2:
			clipmax=SHRT_MAX; clipmin=SHRT_MIN;
			break;
		case 3:
			clipmax=(((SHRT_MAX+1)*(UCHAR_MAX+1))-1); clipmin=(-(SHRT_MAX+1)*(UCHAR_MAX+1));
			break;
		case 4:
		default:
			clipmax=(INT_MAX-100); clipmin=INT_MIN; // FixMe from SaveRawPCM
			break;
		}
		for(int i = t_first; i < t_last; i++){
			int l = (x[i]>clipmax ? 1 : (x[i]<clipmin ? -1 : 0));
			clipped |= l;
			int v = (l>0 ? clipmax : (l<0 ? clipmin : (int)x[i]));
			char* p = (char*)&v;
			copy(p, p + bytes_per_sample, it);
			it += bytes_per_sample;
		}
		if(clipped){ cout << "Clip has been detected !!" << endl; }

/*
		switch(bytes_per_sample)
		{
#if 0
		case 4:
			for (int i = t_first; i < t_last; i++) 
			{
				float v = (float)x[i];
				char* p = (char*)&v;
				copy(p, p + bytes_per_sample, it);
				it += bytes_per_sample;
			}
			break;
#endif
		default:
			for (int i = t_first; i < t_last; i++) 
			{
				int v = (int)x[i];
				char* p = (char*)&v;
				copy(p, p + bytes_per_sample, it);
				it += bytes_per_sample;
			}
			break;
		}
*/
	}

	IN_ORDER_NODE_SPEEDUP(SaveWavePCM);
};
