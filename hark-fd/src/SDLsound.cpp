/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include "SDLsound.h"
#include <fstream>

static int sdl_count = 0;	// SDLの個数

static vector<vector<float> > vvdatabuffer;	// 計算結果用データバッファ
static vector<float>	vtempdatabuffer;	// テンポラリ用データバッファ(callback時優先使用)
static int	dtbfr_inidx;	// 計算結果用データバッファへ次に入力するidx
static int	dtbfr_outidx;	// 計算結果用データバッファから次に出力するidx
static int tmpdtbfr_loadedsize;	// テンポラリ用データバッファの入力済みサイズ
static SDL_AudioCVT wav_cvt;	// データコンバータ
static int cvtbfr_size;	// データコンバータのバッファサイズ

#ifdef SDLSND_USE_NEWAPI
static SDL_AudioFormat format;
static SDL_AudioDeviceID dev;
#endif

// DEBUG出力
static bool bwDebugOutput;
static ofstream wfosm;
static int wdatacount;

// --------------------------------------------------------
/* The audio function callback takes the following parameters:
   stream:  A pointer to the audio buffer to be filled
   len:     The length (in bytes) of the audio buffer
*/
#ifdef SDLSND_USE_NEWAPI
static void SDLCALL sdl_fill_audio(void *udata, Uint8 *stream, int len)
#else
static void sdl_fill_audio(void *udata, Uint8 *stream, int len)
#endif
{
	// wav_cvtバッファサイズ調整
	int ndsize = ((float)len/wav_cvt.rate_incr+0.5)/(sizeof(short));	// 必要なデータ数
	if (wav_cvt.len_cvt < len){
		if (wav_cvt.buf != NULL){
			free(wav_cvt.buf);
		}
		wav_cvt.buf = (Uint8 *)malloc(ndsize*sizeof(short)*wav_cvt.len_mult);	// 初期化
		memset(wav_cvt.buf, 0, ndsize*sizeof(short)*wav_cvt.len_mult);
	}

	// データ入力
	int inidx = 0;
	short *pstemp = (short *)wav_cvt.buf;
	float fvalue;
	int crt_dtidx = 0;
	int tempdtbfr_usedsize = 0;
	int crt_dtbfr_outidx = dtbfr_outidx % vvdatabuffer.size();
	for (inidx = 0 ; inidx < ndsize ; inidx++,pstemp++){
		if (tmpdtbfr_loadedsize > 0){
			// テンポラリデータを優先使用
			fvalue = (short)vtempdatabuffer[inidx];	// すでにgain調整済み
			tmpdtbfr_loadedsize--;
			tempdtbfr_usedsize++;
		}
		else {
			// データが追いついてしまった
			if (dtbfr_inidx < dtbfr_outidx){
				cout << "[SDLsound] Not in time!" << endl;
				pstemp--;
				inidx--;
				break;
			}
			// データバッファのデータを使用
			fvalue = vvdatabuffer[crt_dtbfr_outidx][crt_dtidx];	// すでにgain調整済み
			crt_dtidx++;
			if (crt_dtidx >= vvdatabuffer[crt_dtbfr_outidx].size()){ // 次のバッファ？
				crt_dtidx = 0;
				dtbfr_outidx++;
				crt_dtbfr_outidx = (crt_dtbfr_outidx+1 < vvdatabuffer.size()) ? crt_dtbfr_outidx+1 : 0;
			}
		}
		fvalue = fvalue > -32768.0 ? fvalue : -32768.0;
		fvalue = fvalue < 32768.0 ? fvalue : 32768.0;
		*pstemp = fvalue;
	}
	// テンポラリデータのデータを詰める
	if (tmpdtbfr_loadedsize > 0){
		copy(vtempdatabuffer.begin()+tempdtbfr_usedsize, vtempdatabuffer.begin()+tempdtbfr_usedsize+tmpdtbfr_loadedsize, vtempdatabuffer.begin());
	}
	// データバッファのあまりデータ --> テンポラリバッファへ移動しておく
	if (crt_dtidx > 0){
		copy(vvdatabuffer[crt_dtbfr_outidx].begin()+crt_dtidx, vvdatabuffer[crt_dtbfr_outidx].end(), vtempdatabuffer.begin()+tmpdtbfr_loadedsize);
		tmpdtbfr_loadedsize = vvdatabuffer[crt_dtbfr_outidx].size()-crt_dtidx;
		dtbfr_outidx++;
	}

	// dtbfr_inidx, dtbfr_outidxをループする
	if (((int)(dtbfr_inidx/vvdatabuffer.size()) > 0) &&
			((int)(dtbfr_outidx/vvdatabuffer.size()) > 0)){
		dtbfr_inidx = dtbfr_inidx - (dtbfr_inidx/vvdatabuffer.size())*vvdatabuffer.size();
		dtbfr_outidx = dtbfr_outidx - (dtbfr_outidx/vvdatabuffer.size())*vvdatabuffer.size();
	}

	if (bwDebugOutput){
		wfosm.write((char*)wav_cvt.buf, inidx*sizeof(short));
		wfosm.flush();
		wdatacount = wdatacount + inidx;
	}

	// データコンバート
	wav_cvt.len = inidx*sizeof(short);
	SDL_ConvertAudio(&wav_cvt);
		
	// Stream initialization
	SDL_memset(stream, 0, len);

  // Mix as much data as possible
  len = ( (Uint32)len > wav_cvt.len_cvt ? wav_cvt.len_cvt : len );
#ifdef SDLSND_USE_NEWAPI
  SDL_MixAudioFormat(stream, wav_cvt.buf, format, len, SDL_MIX_MAXVOLUME);
#else
  SDL_MixAudio(stream, wav_cvt.buf, len, SDL_MIX_MAXVOLUME);
#endif
};

// --------------------------------------------------------
#ifdef SDLSND_USE_NEWAPI
/* Check audio device and driver */
int sdl_chkAudioDev(int driver_idx)
{
	int i;

	int numAudioDrivers = SDL_GetNumAudioDrivers();
	if (driver_idx < 0){
		cout << "[SDLsound] Current audio driver: " << SDL_GetCurrentAudioDriver() << endl;
		return (0);
	}
	if (driver_idx >= numAudioDrivers){
		cout << "[SDLsound] Invalid audio driver index: " << driver_idx << endl;
		return (1);
	}

	if (!SDL_AudioInit(SDL_GetAudioDriver(driver_idx)) != 0){
		cout << "[SDLsound] Failed to init audio: " << SDL_GetError() << endl;
		return (1);
	}

	// print the audio driver we are using
	cout << "[SDLsound] Current audio driver: " << SDL_GetCurrentAudioDriver() << endl;

	return(0);
/*
	int i;

//	if(SDL_Init(SDL_INIT_AUDIO)) {
//		cout << "[SDLsound] Failed to initialize: " << SDL_GetError() << endl;
//		return(1);
//	}

	int numAudioDrivers = SDL_GetNumAudioDrivers();

	// print the list of audio backends
	cout << "[SDLsound] " << numAudioDrivers << " audio backends compiled into SDL:";
	for(i=0;i<numAudioDrivers;i++)
		cout << " \'" << SDL_GetAudioDriver(i) << "\'";
	cout << endl;

	// attempt to init audio
	for(i=0;i<numAudioDrivers;i++) {
		if(!SDL_AudioInit(SDL_GetAudioDriver(i))) break;
	}
	if(i == numAudioDrivers) {
		cout << "[SDLsound] Failed to init audio: " << SDL_GetError() << endl;
//		SDL_Quit();
		return 1;
	}

	// print the audio driver we are using
	cout << "[SDLsound] Current audio driver: " << SDL_GetCurrentAudioDriver() << endl;

	// pass it 0 for playback
	int numAudioDevices = SDL_GetNumAudioDevices(0);

	// print the audio devices that we can see
	cout << "[SDLsound] " << numAudioDevices << " audio devices:";
	for(i = 0; i < numAudioDevices; i++)
		// again, 0 for playback
        cout << " \'" << SDL_GetAudioDeviceName(i, 0) << "\'";
	cout << endl;

//	SDL_Quit();

	return(0);
*/
};
#endif

// --------------------------------------------------------
// 音声出力の準備
#ifdef SDLSND_USE_NEWAPI
int sdl_buildaudio(int sampling_rate, int advance, int latency, int driver_idx, bool busedevidx, int devidx, int nch)
#else
int sdl_buildaudio(int sampling_rate, int advance, int latency, int nch)
#endif
{
	// Initialize SDL
	SDL_AudioSpec desired, obtained;
	int ret;

	bwDebugOutput = false;
	
	if (nch > SDLSND_MAX_NCH){
		return (SDLSND_ERR_NCH);
	}

	if (sdl_count > 0){
		return (SDLSND_ERR_SDL_USED);
	}
	sdl_count += 1;

	if(SDL_Init(SDL_INIT_AUDIO) != 0){
		return (SDLSND_ERR_INITSDL);
	}

#ifdef SDLSND_USE_NEWAPI
	if(sdl_chkAudioDev(driver_idx) != 0){
		return (SDLSND_ERR_CHK_AUDIODEV);
	}
	if (busedevidx){
		if ((devidx >= SDL_GetNumAudioDevices(0)) || (devidx < 0)){
			return (SDLSND_ERR_INVALID_DEVIDX);
		}
		cout << "[SDLsound] Use device index=" << devidx << ": \'" << SDL_GetAudioDeviceName(devidx, 0) << "\'" << endl;
	}
	else {
		cout << "[SDLsound] Use the default audio device." << endl;
	}
#endif	

	// 出力フォーマットの設定
	SDL_zero(desired);
	SDL_zero(obtained);
	desired.freq=sampling_rate;
	desired.format=AUDIO_S16;
	desired.channels=nch;
	desired.samples=SDLSND_SAMPLES;	// 2のべき乗
	desired.callback=sdl_fill_audio;
	desired.userdata=NULL;
	// Open the audio device
#ifdef SDLSND_USE_NEWAPI
	dev = SDL_OpenAudioDevice((busedevidx?SDL_GetAudioDeviceName(devidx, 0):NULL), 0,
				&desired, &obtained, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if (dev == 0){
		return (SDLSND_ERR_OPENAUDIO);
	}
#else
	if ( SDL_OpenAudio(&desired, &obtained) < 0 ){
		return (SDLSND_ERR_OPENAUDIO);
	}
#endif

#ifdef SDLSND_USE_NEWAPI
	format = obtained.format;
#endif


	// データコンバータの作成
	ret = SDL_BuildAudioCVT(&wav_cvt,
													AUDIO_S16, nch, sampling_rate,
													obtained.format, obtained.channels, obtained.freq);
	if(ret==-1){
#ifdef SDLSND_USE_NEWAPI
	  SDL_CloseAudioDevice(dev);
#else
	  SDL_CloseAudio();
#endif
		return (SDLSND_ERR_BUILDCVT);
	}
	cvtbfr_size	= desired.samples * desired.channels * sizeof(short);
	wav_cvt.buf = (Uint8 *)malloc(cvtbfr_size*wav_cvt.len_mult);	// 初期化
	wav_cvt.len = cvtbfr_size;

#ifdef SDLSND_USE_NEWAPI
	SDL_LockAudioDevice(dev);	// callbackのLock
#else
	SDL_LockAudio();	// callbackのLock
#endif

	// テンポラリデータの作成(latency x チャンネル数分のデータを作成)
	tmpdtbfr_loadedsize = (float)latency/1000.0*(float)sampling_rate * nch;
	vtempdatabuffer.resize(tmpdtbfr_loadedsize>advance*nch?tmpdtbfr_loadedsize:advance*nch, 0.0);

	vvdatabuffer.resize(latency/1000.0*(float)sampling_rate/advance+SDLSND_BFR_FRMSIZE);
	for (int i = 0 ; i < vvdatabuffer.size() ; i++){
		vvdatabuffer[i].resize(advance*nch, 0.0);
	}
	dtbfr_inidx = 0;
	dtbfr_outidx = 0;

#ifdef SDLSND_USE_NEWAPI
	SDL_UnlockAudioDevice(dev);	// callbackのUnlock
#else
	SDL_UnlockAudio();	// callbackのUnlock
#endif

	return (SDLSND_SUCCEEDED);
}
// --------------------------------------------------------
// 音声データの入力
void sdl_inputdata(int advance, vector<vector<float> > vcurrentoutput, float mastergain)
{
#ifdef SDLSND_USE_NEWAPI
	SDL_LockAudioDevice(dev);	// callbackのLock
#else
	SDL_LockAudio();	// callbackのLock
#endif
	int crt_dtbfr_inidx = dtbfr_inidx % vvdatabuffer.size();
	for (int n = 0 ; n < advance ; n++){
		for (int i = 0 ; i < vcurrentoutput.size() ; i++){
			vvdatabuffer[crt_dtbfr_inidx][vcurrentoutput.size()*n+i] = vcurrentoutput[i][n]*mastergain;
		}
	}
	dtbfr_inidx++;
#ifdef SDLSND_USE_NEWAPI
	SDL_UnlockAudioDevice(dev);	// callbackのUnlock
#else
	SDL_UnlockAudio();	// callbackのUnlock
#endif
}
// --------------------------------------------------------
// 再生スタート
void sdl_startaudio()
{
#ifdef SDLSND_USE_NEWAPI
	SDL_PauseAudioDevice(dev, 0);
#else
	SDL_PauseAudio(0);
#endif
}
// --------------------------------------------------------
// 再生停止
void sdl_stopaudio()
{
	// 2015/10/22 added by N.Tanaka
	// 終了を待つ場合は以下を有効にしてください --->
#if 1
	while (dtbfr_outidx < dtbfr_inidx){
		usleep(0.5*1000000);
	}
#endif
	// <----終了を待つ場合

#ifdef SDLSND_USE_NEWAPI
	SDL_PauseAudioDevice(dev, 1);
#else
	SDL_PauseAudio(1);
#endif

#ifdef SDLSND_USE_NEWAPI
	while(SDL_GetAudioDeviceStatus(dev) == SDL_AUDIO_PLAYING){
#else
	while(SDL_GetAudioStatus() == SDL_AUDIO_PLAYING){
#endif
		sleep(1);
	}
	if (wav_cvt.buf){
		free(wav_cvt.buf);
	}
#ifdef SDLSND_USE_NEWAPI
	SDL_CloseAudioDevice(dev);
#else
	SDL_CloseAudio();
#endif
	SDL_Quit();
}
// --------------------------------------------------------
// ドライバ情報と初期化可否の取得
#ifdef SDLSND_USE_NEWAPI
int sdl_getAudioDriverInformation(vector<string> &drivers, vector<bool> &driverinit)
{
	int i;

	if(SDL_Init(SDL_INIT_AUDIO)) {
		cout << "[SDLsound] Failed to initialize: " << SDL_GetError() << endl;
		return(1);
	}

	int numAudioDrivers = SDL_GetNumAudioDrivers();

	/* The list of audio backends */
	drivers.resize(numAudioDrivers);
	driverinit.resize(numAudioDrivers, false);
	for(i=0;i<numAudioDrivers;i++)
		drivers[i] = SDL_GetAudioDriver(i);

	/* attempt to init audio */
	int driver_idx = -1;
	for(i=0;i<numAudioDrivers;i++) {
		driverinit[i] = (bool)(SDL_AudioInit(SDL_GetAudioDriver(i))==0);
		if (driverinit[i] && driver_idx<0){
			driver_idx = i;		
		}
		if (!driverinit[i]){
			cout << "[SDLsound] Failed to initialize audio driver: " << SDL_GetError() << endl;
		}
	}
	if (driver_idx < 0){
		cout << "[SDLsound] Cannot find the valid audio driver" << endl;
	}

	SDL_AudioInit(SDL_GetAudioDriver(driver_idx));

	SDL_Quit();

	return(0);
}
#endif
// --------------------------------------------------------
// 現在のドライバ名を取得
#ifdef SDLSND_USE_NEWAPI
int sdl_getCurrentAudioDriver(string & currentdriver)
{
	int i;

	if(SDL_Init(SDL_INIT_AUDIO)) {
		cout << "[SDLsound] Failed to initialize: " << SDL_GetError() << endl;
		return(1);
	}

	/* The audio driver we are using */
	currentdriver = SDL_GetCurrentAudioDriver();

	SDL_Quit();

	return(0);
}
#endif
// --------------------------------------------------------
// デバイス情報の取得
#ifdef SDLSND_USE_NEWAPI
int sdl_getAudioDeviceInformation(int driver_idx, vector<string> &devices, int type)
{
	int i;

	if(SDL_Init(SDL_INIT_AUDIO)) {
		cout << "[SDLsound] Failed to initialize: " << SDL_GetError() << endl;
		return(1);
	}

	if (SDL_AudioInit(SDL_GetAudioDriver(driver_idx)) != 0){
		cout << "[SDLsound] Failed to initialize the audio driver" << SDL_GetError() << endl;
	}

	/* pass it 0 for playback */
	int numAudioDevices = SDL_GetNumAudioDevices(type);

	/* The audio devices that we can see */
	devices.resize(numAudioDevices);
	for(i = 0; i < numAudioDevices; i++)
		devices[i] = SDL_GetAudioDeviceName(i, type);

	SDL_Quit();

	return(0);
}
#endif
// --------------------------------------------------------
#ifdef SDLSND_USE_NEWAPI
// デバイス情報の表示
int sdl_showAudioDevInformation(int type)
{
	int i;

	int device_idx;
	vector<string> drivers;
	vector<bool> driverinit;
	string currentdriver;
	vector<string> devices;
	if (sdl_getAudioDriverInformation(drivers, driverinit) != 0){
		return (1);
	}
	sdl_getCurrentAudioDriver(currentdriver);
	device_idx = -1;
	for (i = 0 ; i < driverinit.size() ; i++){
		if (driverinit[i]){
			device_idx = i;
			break;
		}
	}
	if (device_idx < 0){
		return (1);
	}
	if (sdl_getAudioDeviceInformation(device_idx, devices, type) != 0){
		return (1);
	}

	cout << "[SDLsound] (" << drivers.size() << ") Audio drivers: " << endl;
	for (i=0 ; i<drivers.size() ; i++){
		cout << "\t" << i << ":\'" << drivers[i] << "\'\t";
		if (driverinit[i]){
			cout << "OK" << endl;
		}
		else {
			cout << "NG" << endl;
		}
	}

	cout << "[SDLsound] The current audio driver is: " << currentdriver << endl;

	cout << "[SDLsound] (" << devices.size() << ") Audio devices: " << endl;
	for (i=0 ; i<devices.size() ; i++){
		cout << "\t" << i << ":\'" << devices[i] << "\'" << endl;
	}

	return(0);
};
#endif
// --------------------------------------------------------
// デバッグ出力開始
void sdl_opendbgfile(bool output)
{
	bwDebugOutput = output;
	if (bwDebugOutput){
		wfosm.open("PlaySoundDBG_all.dat", ios::out|ios::binary|ios::trunc);
		wdatacount = 0;
	}
}
// --------------------------------------------------------
// デバッグ出力終了
void sdl_closedbgfile()
{
	if (bwDebugOutput){
		wfosm.flush();
		wfosm.close();
		cout<< "wdatacount" << wdatacount << endl;
	}
}
// --------------------------------------------------------

