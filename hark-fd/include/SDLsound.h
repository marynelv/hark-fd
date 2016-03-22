/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <iostream>
#include <SDL2/SDL.h>
#include <algorithm>
#include <vector>

#ifdef __MINGW32__
#include <windows.h>
#define sleep(n) Sleep(1000 * n)
#else
#include <unistd.h>
#endif

#define SDLSND_USE_NEWAPI

#define SDLSND_SUCCEEDED	(0)
#define SDLSND_ERR_INITSDL	(10)
#define SDLSND_ERR_OPENAUDIO	(20)
#define SDLSND_ERR_BUILDCVT	(30)
#define SDLSND_ERR_INVALID_DEVIDX	(40)
#define SDLSND_ERR_CHK_AUDIODEV	(50)
#define SDLSND_ERR_SDL_USED	(60)
#define SDLSND_ERR_NCH (70)

#define SDLSND_SAMPLES	(4096)
#define SDLSND_BFR_FRMSIZE (1024)

#define SDLSND_MAX_NCH	(2)


using namespace std;

// SDLのセットアップ
#ifdef SDLSND_USE_NEWAPI
int sdl_buildaudio(int sampling_rate, int advance, int latency, int driver_idx, bool busedevidx, int devidx, int nch);
#else
int sdl_buildaudio(int sampling_rate, int advance, int latency, int nch);
#endif
// データの入力
void sdl_inputdata(int advance, vector<vector<float> > vcurrentoutput, float mastergain);
// 音声出力の開始
void sdl_startaudio();
// 音声出力の停止
void sdl_stopaudio();

#ifdef SDLSND_USE_NEWAPI
// ドライバ情報と初期化可否の取得
int sdl_getAudioDriverInformation(vector<string> &drivers, vector<bool> &driverinit);
// 現在のドライバ名を取得
int sdl_getCurrentAudioDriver(string & currentdriver);
// ドライバを指定してデバイス情報の取得(type==0:再生)
int sdl_getAudioDeviceInformation(int driver_idx, vector<string> &devices, int type);
// driver, device情報の表示(type==0:再生)
int sdl_showAudioDevInformation(int type);
#endif

// 出力データのデバッグ出力
void sdl_opendbgfile(bool output);
void sdl_closedbgfile();

