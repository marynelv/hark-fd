/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Map.h"
#include "Matrix.h"

#include "SDLsound.h"

#include <algorithm>

using namespace std;
using namespace FD;

class PlayAudio;

DECLARE_NODE(PlayAudio)
/*Node
 *
 * @name PlayAudio
 * @category HARK:AudioIO
 * @description Play audio signals. You can place only one PlayAudio.
 *
 * @input_name INPUT1
 * @input_type any
 * @input_description Audio signals in time domain for 1ch. Supported types are Map<int,ObjectRef>, Matrix<float> and Vector<float>. In case of Map<int,ObjectRef>, the key is a source ID, and the value is a signal vector (Vector<complex<float> >). In case of Matrix<float>, each row including a signal vector and multi-channel audio signals are saved.
 *
 * @input_name INPUT2
 * @input_type any
 * @input_description Audio signals in time domain for 2ch. Supported types are Map<int,ObjectRef>, Matrix<float> and Vector<float>. In case of Map<int,ObjectRef>, the key is a source ID, and the value is a signal vector (Vector<complex<float> >). In case of Matrix<float>, each row including a signal vector and multi-channel audio signals are saved.
 *
 * @input_name INPUT_MULTI_CHANNEL
 * @input_type any
 * @input_description Audio signals in time domain. Supported types are Map<int,ObjectRef> and Matrix<float>. In case of Map<int,ObjectRef>, the key is a channel index, and the value is a signal vector (Vector<complex<float> >). In case of Matrix<float>, each row including a signal vector and multi-channel audio signals are saved. 
 *
 * @output_name OUTPUT
 * @output_type Matrix<float>
 * @output_description Output signal
 *
 * @parameter_name MASTER_VOLUME
 * @parameter_type float
 * @parameter_value 0
 * @parameter_description Value of the master volume (dB) [default: 0]
 *
 * @parameter_name DEVICE
 * @parameter_type int
 * @parameter_value 
 * @parameter_description Device Index. PlayAudio uses the default device, when this parameter is empty. [default: ]
 *
 * @parameter_name CHANNEL_COUNT
 * @parameter_type int
 * @parameter_value 2
 * @parameter_description Number of Channel for playing. [default: 2]
 *
 * @parameter_name MULTI_CHANNEL_ASSIGN
 * @parameter_type object
 * @parameter_value 
 * @parameter_description Channel assign for INPUT_MULTI_CHANNEL by Vector<int>. For example, <Vector<int> 1 2>. [default: ]
 *
 * @parameter_name LATENCY
 * @parameter_type int
 * @parameter_value 1000
 * @parameter_description Latency of this node (msec) [default: 1000].
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description The length of a frame (per channel).
 * 
 * @parameter_name ADVANCE
 * @parameter_type int
 * @parameter_value 160
 * @parameter_description The length in sample between a frame and a previous frame. [default: 160]
 *
 * @parameter_name SAMPLING_RATE
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_description Sampling rate (Hz) [default: 16000].
 *
 END*/

class PlayAudio : public BufferedNode {
	bool initialized;

	vector<int>	vinID;
	int inputmchID;
	int outID;
  int paramsID;

	int nch;
	Vector<int> vmulchassign;
	bool mulchassigned;
	float mastervlm;
	float mastergain;
	bool use_default_dev;
	int device_idx;
  int latency;
  int length;
  int advance;
  int sampling_rate;

	// 出力データ作成
  vector<vector<int> >	vloadedID;	// Map入力時の入力済みID。
  vector<vector<float> >	vcurrentoutput; // 出力データ保管用

	// DEBUG出力
	bool bDebugOutput;
	vector<ofstream*> fosm;
	vector<float *> pbuffer;
	int datacount;

public:
	PlayAudio(string nodeName, ParameterSet params)
	    : BufferedNode(nodeName, params),
		inputmchID(-1),paramsID(-1),outID(-1),
		nch(2),mulchassigned(false),mastervlm(0),mastergain(1.0),
		latency(1000),use_default_dev(true),device_idx(0),
		length(512),advance(160),sampling_rate(16000),
		initialized(false),
		bDebugOutput(false)
	{
		vinID.clear();
		vmulchassign.clear();
		vloadedID.clear();
		vcurrentoutput.clear();
		fosm.clear();
		pbuffer.clear();

	  mastervlm = dereference_cast<float>(parameters.get("MASTER_VOLUME"));
		if (parameters.exist("DEVICE")){
			use_default_dev = false;
			device_idx = dereference_cast<int>(parameters.get("DEVICE"));
		}
		else {
			use_default_dev = true;
		}
		nch = dereference_cast<int>(parameters.get("CHANNEL_COUNT"));
		if (parameters.exist("MULTI_CHANNEL_ASSIGN")){
			mulchassigned = true;
			vmulchassign = object_cast<Vector<int> >(parameters.get("MULTI_CHANNEL_ASSIGN"));
		}
		else {
			mulchassigned = false;
		}
	  latency = dereference_cast<int>(parameters.get("LATENCY"));
	  length = dereference_cast<int>(parameters.get("LENGTH"));
	  advance = dereference_cast<int>(parameters.get("ADVANCE"));
	  sampling_rate = dereference_cast<int>(parameters.get("SAMPLING_RATE"));

		// ボリュームをゲインに変換
		mastergain = ConvertVlm2Gain(mastervlm, false);

		// 新たなチャンネル数で初期化
		vinID.resize(nch, -1);
		vmulchassign.resize(nch, -1);
		vloadedID.resize(nch);
		vcurrentoutput.resize(nch);

		// 入力データ管理用
		for (int i = 0 ; i < nch ; i++){
			vcurrentoutput[i].resize(length, 0);	// 初期化
		}

		inOrder = true;
	}
// --------------------------------------------------------
	~PlayAudio()
	{
		if (initialized){
			//
			// 音声出力の停止
			//
			sdl_stopaudio();

			// DEBUG出力
			if (bDebugOutput){
				for (int i = 0 ; i < nch ; i++){
					if (fosm[i]){
						fosm[i]->flush();
						fosm[i]->close();
						delete fosm[i];
					}
					if (pbuffer[i]){
						delete [] pbuffer[i];
					}
				}
				cout<< "datacount" << datacount << endl;
			}
		}
		sdl_closedbgfile();
	}
// --------------------------------------------------------
  virtual void initialize()
{
		this->BufferedNode::initialize();

		//
		// 音声出力の準備
		//
		int rtn;
#ifdef SDLSND_USE_NEWAPI
		rtn = sdl_buildaudio(sampling_rate, advance, latency, -1, !use_default_dev, device_idx, nch);
#else
		rtn = sdl_buildaudio(sampling_rate, advance, latency, nch);
#endif

		if (rtn == SDLSND_ERR_SDL_USED){
			throw new NodeException(this, "You can place only one PlayAudio.[PlayAudio]", __FILE__, __LINE__);		
		}
		else if (rtn == SDLSND_ERR_CHK_AUDIODEV){
			throw new NodeException(this, "Unable to check audio device.[PlayAudio]", __FILE__, __LINE__);		
		}
		else if (rtn == SDLSND_ERR_INVALID_DEVIDX){
			throw new NodeException(this, "Invalid device index.[PlayAudio]", __FILE__, __LINE__);		
		}
		else if (rtn == SDLSND_ERR_INITSDL){
			throw new NodeException(this, "Unable to initialize SDL.[PlayAudio]", __FILE__, __LINE__);		
		}
		else if (rtn == SDLSND_ERR_OPENAUDIO){
			throw new NodeException(this, "Couldn't open audio. You can place only one PlayAudio.[PlayAudio]", __FILE__, __LINE__);		
		}
		else if (rtn == SDLSND_ERR_BUILDCVT){
			throw new NodeException(this, "Couldn’t build converter.[PlayAudio]", __FILE__, __LINE__);		
		}
		else if (rtn == SDLSND_ERR_NCH){
			throw new NodeException(this, "Number of channel must be less than 2.[PlayAudio]", __FILE__, __LINE__);
		}


		// DEBUG出力
		bDebugOutput = false;
		fosm.resize(nch, NULL);
		pbuffer.resize(nch, NULL);
		if (bDebugOutput){
			for (int i = 0 ; i < nch ; i++){
				ostringstream name_str;
				name_str << "PlayAudioDBG" << i << ".dat";
				fosm[i] = new ofstream;
				fosm[i]->open(name_str.str().c_str(), ios::out|ios::binary|ios::trunc);
				pbuffer[i] = new float[advance];
			}
			datacount = 0;
		}
		sdl_opendbgfile(bDebugOutput);

		initialized = true;
}
// --------------------------------------------------------
	virtual int translateInput(string inputName)
{
		for (int i = 0 ; i < nch ; i++){
			ostringstream name_str;
			name_str << "INPUT" << i+1;
			if (inputName.compare(name_str.str()) == 0){
				vinID[i] = addInput(inputName);
				return (vinID[i]);
			}
		}

		if (inputName == "INPUT_MULTI_CHANNEL") {
    		inputmchID = addInput(inputName);
    		return inputmchID;
    }
//		else if (inputName == "PARAMS") {
//    		paramsID = addInput(inputName);
//    		return paramsID;
//    }
    else {throw new NodeException(this, inputName+ " is not supported.[PlayAudio]", __FILE__, __LINE__);}
  }
// --------------------------------------------------------
  virtual int translateOutput(string outputName) 
{
   	if (outputName == "OUTPUT") {
   		outID = addOutput(outputName);
   		return outID;
   	}
   	else {throw new NodeException(this, outputName+ " is not supported.[PlayAudio]", __FILE__, __LINE__);}
  }
// --------------------------------------------------------
	void calculate(int output_id, int count, Buffer &out)
 	{
		if(paramsID != -1){
			RCPtr<Vector<ObjectRef> > param_ptr = getInput(paramsID, count);
			const Vector<ObjectRef>& param_vec = *param_ptr;      
			if(param_vec.size() != 0){	
/*
			RCPtr<ParamsLocalizeMUSIC> value_LocalizeMUSIC = param_vec[0];
			num_source = (*value_LocalizeMUSIC).num_source;
			localizer->SetNumSource(num_source);
			mindeg = (*value_LocalizeMUSIC).min_deg;
			maxdeg = (*value_LocalizeMUSIC).max_deg;
			lower_bound = (*value_LocalizeMUSIC).lower_bound_frequency;
			upper_bound = (*value_LocalizeMUSIC).upper_bound_frequency;
			localizer->SetUsedFrequency(lower_bound, upper_bound);
*/
			}
		}

		//
		// 出力先の確保(length)
		//
		ObjectRef outdata;
		if (outID >= 0){
			outdata = ObjectRef(new Matrix<float>(nch, length)) ;
			(*(outputs[outID].buffer))[count] = outdata;

			// 初期化
			RCPtr<Matrix<float> > outmat = outdata;
			for (int m = 0 ; m < nch ; m++){
				for (int k = 0 ; k < length ; k++){
					(*outmat)(m, k) = 0.0;
				}
			}
		}

		//
		// vcurrentoutputのlength-advance以降にデータを合成コピーし、出力信号を作成する
		//
		for (int i = 0 ; i < nch ; i++){	// channel
			if (vinID[i] >= 0){	// INPUT#の入力
			  ObjectRef in = getInput(vinID[i], count);
	
			  if (typeid(*in) == typeid(Matrix<float>)) {

					// Matrixの場合

				  RCPtr<Matrix<float> > inmat = in;
					if (inmat->nrows() == 0 || inmat->ncols() == 0){
						continue;
					}
					// フレーム長エラー
					if (inmat->ncols() != length){
						throw new NodeException(this, "The datalength of Matrix should be same as LENGTH.", __FILE__, __LINE__);
					}
					// count==0のとき0～length-advance-1も登録
					if (count == 0){
						SetData2currentoutput(inmat, 0, length-advance-1, i, 0);
					}
					// 最後のadvanceを登録(SaveWavePCM, SaveRawPCMを参考)
					SetData2currentoutput(inmat, length-advance, length, i, length-advance);

				} // <---- Matrixの場合終わり
			  else if (typeid(*in) == typeid(Map<int, ObjectRef>)) {

					// Mapの場合

				  RCPtr<Map<int, ObjectRef> > inmap = in;
					if (inmap->size() == 0){
						continue;
					}
					Map<int, ObjectRef>::iterator itinmap = inmap->begin();
					vector<int> vnewIDs;	// 新しいID
					while(1){
						vnewIDs.push_back(itinmap->first);
						RCPtr<Vector<float> > invec = itinmap->second;
	
						// フレーム長エラー
						if (invec->size() != length){
							throw new NodeException(this, "The vector size of Map should be same as LENGTH.", __FILE__, __LINE__);
						}

						// 新しいIDのとき0～length-advance-1も登録(加算)
						vector<int>::iterator itfound = find(vloadedID[i].begin(), vloadedID[i].end(), itinmap->first);
//						if (itfound != vloadedID[i].end()){	// 2015/10/22 changed by N.Tanaka
						if (itfound == vloadedID[i].end()){
							// ID登録
							vloadedID[i].push_back(itinmap->first);
							// 0～length-advance-1も加算
							SetData2currentoutput(invec, 0, length-advance-1, i, 0);
						}
						// 残りのadvanceを加算(SaveWavePCM, SaveRawPCMを参考)
						SetData2currentoutput(invec, length-advance, length, i, length-advance);
	
						itinmap++;
						if (itinmap == inmap->end()){
							break;
						}
					}
					vloadedID[i].resize(vnewIDs.size());
					vloadedID[i].assign(vnewIDs.begin(), vnewIDs.end());
	
				}// <---- Mapの場合終わり
			  else if (typeid(*in) == typeid(Vector<float>)) {

					// Vectorの場合

					RCPtr<Vector<float> > invec = in;
	
					// フレーム長エラー
					if (invec->size() != length){
						throw new NodeException(this, "The vector size should be same as LENGTH.", __FILE__, __LINE__);
					}
	
					// count==0のとき0～length-advance-1も登録
					if (count == 0){
						SetData2currentoutput(invec, 0, length-advance-1, i, 0);
					}
					// 最後のadvanceを登録(SaveWavePCM, SaveRawPCMを参考)
					SetData2currentoutput(invec, length-advance, length, i, length-advance);
	
				}// <---- Vectorの場合終わり
			} // <---- vinIDの終わり

		} // <---- channelごと処理の終わり

		if (inputmchID >= 0){ // multichannel処理
			  ObjectRef in = getInput(inputmchID, count);
	
			  if (typeid(*in) == typeid(Matrix<float>)) {

					// Matrixの場合

				  RCPtr<Matrix<float> > inmat = in;
					if (inmat->nrows() > 0 && inmat->ncols() > 0){
						// フレーム長エラー
						if (inmat->ncols() != length){
							throw new NodeException(this, "The datalength of Matrix should be same as LENGTH.", __FILE__, __LINE__);
						}
						if (inmat->nrows() > 0){
							// count==0のときアサインを確認する
							if (count == 0){
								cout << "[PlayAudio] MULTI_CHANNEL_ASSIGN: ";
								for (int i = 0 ; i < nch ; i++){
									vmulchassign[i] = mulchassigned ? vmulchassign[i] : i;
									if (vmulchassign[i] < 0 || vmulchassign[i] >= inmat->nrows()){
										vmulchassign[i] = -1;
									}
									cout << "(" << i+1 << "ch)";
									if (vmulchassign[i] < 0){ cout << "None, "; }
									else { cout << vmulchassign[i] << ", "; }
								}
								cout << endl;
							}
							// アサインのとおりに出力
							for (int i = 0 ; i < nch ; i++){
								if (vmulchassign[i] < 0){
									continue;
								}
								// count==0のとき0～length-advance-1も登録
								if (count == 0){
									SetSpecificData2currentoutput(inmat, vmulchassign[i], 0, length-advance-1, i, 0);
								}
								// 最後のadvanceを登録(SaveWavePCM, SaveRawPCMを参考)
								SetSpecificData2currentoutput(inmat, vmulchassign[i], length-advance, length, i, length-advance);
							}
						}
					}

				} // <---- Matrixの場合終わり
			  else if (typeid(*in) == typeid(Map<int, ObjectRef>)) {

					// Mapの場合

				  RCPtr<Map<int, ObjectRef> > inmap = in;
					if (inmap->size() > 0){
						// count==0のときアサインを確認する
						if (count == 0){
							cout << "[PlayAudio] MULTI_CHANNEL_ASSIGN: ";
							for (int i = 0 ; i < nch ; i++){
								vmulchassign[i] = mulchassigned ? vmulchassign[i] : i;
								if (vmulchassign[i] >= 0){
									Map<int, ObjectRef>::iterator itfound = inmap->find(vmulchassign[i]);
									if (itfound == inmap->end()){
										vmulchassign[i] = -1;
									}
								}
								else {
									vmulchassign[i] = -1;
								}

								cout << "(" << i+1 << "ch)";
								if (vmulchassign[i] < 0){ cout << "None, "; }
								else { cout << vmulchassign[i] << ", "; }
							}
							cout << endl;
						}
						// アサインのとおりに出力
						for (int i = 0 ; i < nch ; i++){
							if (vmulchassign[i] < 0){
								continue;
							}
							Map<int, ObjectRef>::iterator itfound = inmap->find(vmulchassign[i]);
							if (itfound == inmap->end()){
								continue;
							}
							RCPtr<Vector<float> > invec = itfound->second;
							// count==0のとき0～length-advance-1も登録
							if (count == 0){
								SetData2currentoutput(invec, 0, length-advance-1, i, 0);
							}
							// 最後のadvanceを登録(SaveWavePCM, SaveRawPCMを参考)
							SetData2currentoutput(invec, length-advance, length, i, length-advance);
						}
					}	
				}// <---- Mapの場合終わり
		} // <---- multichannel処理の終わり

		// <---- 出力信号の作成終わり

		//
		// ゲインをかけて、outputsに登録する(length)
		//
		RCPtr<Matrix<float> > outmat = outdata;	// 出力先
		for (int m = 0 ; m < nch ; m++){
			vector<float>::iterator itvcurrentoutput = vcurrentoutput[m].begin();
			for (int k = 0 ; k < length ; k++,itvcurrentoutput++){
				(*outmat)(m,k) = (*itvcurrentoutput)*mastergain;
			}
		}

		//
		// ゲインをかけて、音声出力用バッファに登録する(advance)
		//
		sdl_inputdata(advance, vcurrentoutput, mastergain);

		// DEBUG出力
		if (bDebugOutput){
			for (int i = 0 ; i < nch ; i++){
				for (int n = 0 ; n < advance ; n++){
					pbuffer[i][n] = vcurrentoutput[i][n]*mastergain;
				}
				fosm[i]->write((char*)pbuffer[i], advance*sizeof(float));
				fosm[i]->flush();
			}
			datacount = datacount + advance;
		}


		// vcurrentoutputを1フレーム分送る
		vector<float> vzero;
		vzero.resize(advance, 0);
		for (int i = 0 ; i < nch ; i++){
			copy(vcurrentoutput[i].begin()+advance, vcurrentoutput[i].end(), vcurrentoutput[i].begin());
			copy_backward(vzero.begin(), vzero.end(), vcurrentoutput[i].end());	// 末尾にゼロ埋め
			
		}

		// 再生開始
		if (count == 0){
			sdl_startaudio();
		}

 	}
// --------------------------------------------------------
	float ConvertVlm2Gain(float vlm, bool mt)
	{
		float rtn = pow(10.0, vlm/20);
		if (mt){
			rtn = 0.0;
		}
		return (rtn);
	}
// --------------------------------------------------------
	// Matrixからvcurrentoutputにデータをセット
	void SetData2currentoutput(RCPtr<Matrix<float> > inmat, int mat_first, int mat_end, int out_idx, int out_first)
	{
		for (int m = 0 ; m < inmat->nrows() ; m++){	// ch数
			vector<float>::iterator itvcurrentoutput = vcurrentoutput[out_idx].begin() + out_first;
			for (int n = mat_first ; n < mat_end ; n++, itvcurrentoutput++){ // サンプル
				*itvcurrentoutput = *itvcurrentoutput + (*inmat)(m,n);
			}
		}
	}
// --------------------------------------------------------
	// Matrixの特定rowのデータからvcurrentoutputにデータをセット
	void SetSpecificData2currentoutput(RCPtr<Matrix<float> > inmat, int mat_targetrow, int mat_first, int mat_end, int out_idx, int out_first)
	{
		if (mat_targetrow < 0 || mat_targetrow >= inmat->nrows()){
			return;
		}
		vector<float>::iterator itvcurrentoutput = vcurrentoutput[out_idx].begin() + out_first;
		for (int n = mat_first ; n < mat_end ; n++, itvcurrentoutput++){ // サンプル
			*itvcurrentoutput = *itvcurrentoutput + (*inmat)(mat_targetrow,n);
		}	
	}
// --------------------------------------------------------
	// Vectorからvcurrentoutputにデータをセット
	void SetData2currentoutput(RCPtr<Vector<float> > invec, int vec_first, int vec_end, int out_idx, int out_first)
	{
		Vector<float>::iterator itinvec = invec->begin() + vec_first;
		vector<float>::iterator itvcurrentoutput = vcurrentoutput[out_idx].begin() + out_first;
		for (int n = 0 ; n < vec_end-vec_first+1 ; n++, itvcurrentoutput++, itinvec++){ // サンプル
			*itvcurrentoutput = *itvcurrentoutput + *itinvec;
		}
	}
// --------------------------------------------------------
  IN_ORDER_NODE_SPEEDUP(PlayAudio);
};
 
