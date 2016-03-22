// ---------------------------------------------------------------------
/**
 Copyright (c) 2004-2006 Honda Research Institute Japan, Co., Ltd.

 \file MICARY_PF_Struct.h
 \brief \~English Header file for Post Filter Struct
 \brief \~Japanese MICARYポストフィルタの共通構造体のためのヘッダファイル(Post Filter)
 \author Masatoshi Tsutsumi
 \date 15 Nov 2006
 \version $Id: MICARY_PF_Struct.h 450 2009-03-16 10:12:38Z suzuki $
*/

// ---------------------------------------------------------------------
#ifndef	MICARY_PF_Struct_h_defined
#define	MICARY_PF_Struct_h_defined
// ---------------------------------------------------------------------
//#include <string>
#include "MICARYHA_struct.h"
#include "MICARY_struct.h"


// ---------------------------------------------------------------------

/**
 Gain Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_Float	ha_fMinGain;	///\~Japanese 無音声時の最小ゲイン
} MICARY_PrmGain;

/**
 Smoothing Priori SNR Parameters struct
 \author	Masatoshi Tsutsumi
 \date	29 Nov 2006
*/
typedef struct{
	HA_Float	ha_fB;	///\~Japanese 時間平滑化係数(b)
	HA_Integer	ha_fBwFst;	///\~Japanese フレーム平均で矩形窓を適用する下限周波数ビン(bwFst)
	HA_Integer	ha_fBwFen;	///\~Japanese フレーム平均で矩形窓を適用する上限周波数ビン(bwFen)
	HA_Integer	ha_fBwG;	///\~Japanese 広帯域平均幅(bwG)
	HA_Integer	ha_fBwL;	///\~Japanese 狭帯域平均で矩形窓幅(bwL)
} MICARY_PrmZeta;

/**
 Voice Power Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_Float	ha_fB;	///\~Japanese 時間平滑化係数(b)
	HA_Integer	ha_fBwFst;	///\~Japanese フレーム平均で矩形窓を適用する下限周波数ビン(bwFst)
	HA_Integer	ha_fBwFen;	///\~Japanese フレーム平均で矩形窓を適用する上限周波数ビン(bwFen)
	HA_Integer	ha_fBwG;	///\~Japanese 広帯域平均幅(bwG)
	HA_Integer	ha_fBwL;	///\~Japanese 狭帯域平均で矩形窓幅(bwL)
	HA_Float	ha_fZminF;	///\~Japanese 事前SN比の最小値 (Frame)
	HA_Float	ha_fZmaxF;	///\~Japanese 事前SN比の最大値 (Frame)
	HA_Float	ha_fZminG;	///\~Japanese 事前SN比の最小値 (Global)
	HA_Float	ha_fZmaxG;	///\~Japanese 事前SN比の最大値 (Global)
	HA_Float	ha_fZminL;	///\~Japanese 事前SN比の最小値 (Local)
	HA_Float	ha_fZmaxL;	///\~Japanese 事前SN比の最大値 (Local)
	HA_Float	ha_fZthF;	///\~Japanese zetaF0の係数
	HA_Float	ha_fZpmin;	///\~Japanese zpeakの下限値
	HA_Float	ha_fZpmax;	///\~Japanese zpeakの上限値
	HA_Float	ha_fAmpL;	///\~Japanese 音声存在確率計算時のLocalの対する係数
	HA_Float	ha_fAmpG;	///\~Japanese 音声存在確率計算時のGlobalの対する係数
	HA_Float	ha_fAmpF;	///\~Japanese 音声存在確率計算時のFrameの対する係数
	HA_Float	ha_fQmax;	///\~Japanese 音声存在確率の上限値
	HA_Float	ha_fQmin;	///\~Japanese 音声存在確率の下限値
} MICARY_PrmVoiceP;

/**
 Saved Voice Power Spectrum struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsSmSNR;	///\~Japanese 前フレームの平滑化SN比
	MICARY_FloatArray	micary_faSmSNRF;	///\~Japanese 前フレームの平滑化SN比 Frame
	MICARY_FloatArray	micary_faSmSNRP;	///\~Japanese 前フレームの平滑化SN比 Peak
} MICARY_SavedVoiceP;

/**
 Estimate S/N Parameters struct
 \author	Masatoshi Tsutsumi
 \date	23 Nov 2006
*/
typedef struct{
	HA_Float	ha_fAlp_pmin;	///\~Japanese 最小音声含有率(alp_pmin)
	HA_Float	ha_fAlp_pmag;	///\~Japanese 音声含有率の係数(alp_pmag)
	HA_Float	ha_fA;	///\~Japanese 事前SN比の更新率(a)
	HA_Float	ha_fXi_max;	///\~Japanese 事前SN比のクリップ(OP.xi_max)
	HA_Float	ha_fTheta_max;	///\~Japanese GH1を求める際のvの最大値(OP.theta_max)
	HA_Float	ha_fTheta_min;	///\~Japanese GH1を求める際のvの最小値(OP.theta_min)
} MICARY_PrmEstSN;

/**
 Saved Estimate S/N Values struct
 \author	Masatoshi Tsutsumi
 \date	23 Nov 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsInputPSpec;	///\~Japanese 入力パワー(Shatp)
	MICARY_FloatSpecArray	micary_fsSN;	///\~Japanese 事前SN比(xi0)
} MICARY_SavedEstSN;

/**
 reverberation Noise Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_Float	ha_fGamma;	///\~Japanese 残響減衰定数(gamma)
	HA_Float	ha_fDelta;	///\~Japanese 直接音成分比(SRR)(delta)
} MICARY_PrmRevNoise;

/**
 Saved Reverberation Noise Values struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsRevPSpec;	///\~Japanese 前フレームの残響成分パワースペクトル
} MICARY_SavedEstRev;

/**
 Leak Noise Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_Float	ha_fAlp_leak;	///\~Japanese Leak parameter (used like 'eta')
} MICARY_PrmLeakNoise;

/**
 Initial noise of MCRA Parameters struct
 \author	Masatoshi Tsutsumi
 \date	13 Dec 2006
*/
typedef struct{
	HA_Float	ha_fDelta;	///\~Japanese 音声確率閾値
	HA_Integer	ha_iL;	///\~Japanese 更新のフレーム長
	HA_Float	ha_fAlp_s;	///\~Japanese 入力スペクトルの平滑化係数
	HA_Float	ha_fNoise_compense;	///\~Japanese valin spetical parameter (1,1) [1?]
	HA_Float	ha_fAlp_dmin;	///~\Japanese Minimum speech probability (1,1) [0.05]
	HA_Integer	ha_iInit_frame;	///~\Japanese Initialize update frams (1,1) [100]
} MICARY_PrmInitMCRA;

/**
 Saved Initial noise of MCRA Values struct
 \author	Masatoshi Tsutsumi
 \date	13 Dec 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsStateNoisePSpec;	///\~Japanese 定常雑音パワースペクトル
	MICARY_FloatSpecArray	micary_fsSmin;	///\~Japanese 入力信号のパワースペクトルの最小値
	MICARY_FloatSpecArray	micary_fsStmp;	///\~Japanese 入力信号のパワースペクトルの一時最小値
	MICARY_FloatSpecArray	micary_fsS;	///\~Japanese 入力信号の平滑化パワースペクトル
	MICARY_FloatSpecArray	micary_fsSf;	///\~Japanese Frequency smoothed spectrum (N, M) [0?] for debug output
	HA_Num	ha_numProc;	///\~Japanese 処理回数
} MICARY_SavedInitMCRA;

/**
 MCRA Parameters struct
 \author	Masatoshi Tsutsumi
 \date	5 Dec 2006
*/
typedef struct{
	HA_Float	ha_fAlp_d;				///\~Japanese 雑音の平滑化係数
	HA_Float	ha_fDelta;				///\~Japanese 音声確率閾値
	HA_Integer	ha_iL;					///\~Japanese 更新のフレーム長
	HA_Float	ha_fAlp_s;				///\~Japanese 入力スペクトルの平滑化係数
	HA_Float	ha_fLeak_ampleak_factor;	///\~Japanese 漏れ雑音振幅係数
	HA_Float	ha_fSs_floor;			///\~Japanese　Floor係数
	HA_Float	ha_fNoise_compense;		///\~Japanese 雑音補正値
	HA_Float	ha_fIn_Noise_compense;	///\~Japanese 入力信号の定常雑音混入率。追加森尾2008.12.3
	HA_Integer	ha_iResetL;				///\~Japanese 音源変更時に処理回数をリセットするか 1:変更する，0:変更しない 追加森尾2009.3.12
} MICARY_PrmMCRA;



typedef struct{///\~Japanese 追加（森尾）2008.11.14
	HA_Char		*ha_pcSavename;			///\~Japanese データ保存名 
	HA_Char		*ha_iInitFromfilename;	///\~Japanese 初期値を読み出すファイルの名前
	HA_Integer	ha_iInitFromfile_ID;	///\~Japanese 初期値を読み出すファイルIDの上限
	HA_Integer	ha_iFileSave;			///\~Japanese ファイルを保存するかどうか？1:保存する，0:保存しない
	HA_Integer  ha_iKeepPeriod;			///\~Japanese データ維持フレーム数
	
} MICARY_PrmSave;





typedef struct{
	HA_Integer iID;							//音源ID
	HA_Integer iSize;							//保存値の配列のサイズ
	HA_Integer ha_iLowerFreq;					//下限周波数
	HA_Integer ha_iUpperFreq;					//上限周波数
	HA_Integer	ha_numCh;						//同時音源数
    HA_Char cPad[100];						//パッド
}MICARY_SavedFileHeader;///\~Japanese 追加（森尾）2008.11.14


/**
 Saved MCRA Values struct
 \author	Masatoshi Tsutsumi
 \date	5 Dec 2006
*/
typedef struct{
	MICARY_FloatSpecArray	micary_fsStateNoisePSpec;	///\~Japanese 定常雑音パワースペクトル
	MICARY_FloatSpecArray	micary_fsSmin;	///\~Japanese 入力信号のパワースペクトルの最小値
	MICARY_FloatSpecArray	micary_fsStmp;	///\~Japanese 入力信号のパワースペクトルの一時最小値
	HA_Num	ha_numProc;	///\~Japanese 処理回数
	MICARY_FloatSpecArray	micary_fsS;	///\~Japanese 入力信号の平滑化パワースペクトル
} MICARY_SavedMCRA;



/**
 Post Filter Parameters struct
 \author	Masatoshi Tsutsumi
 \date	27 Dec 2006
*/
typedef struct{
	MICARY_PrmMCRA sPrmMCRA;	///\~Japanese MCRA計算パラメータ
	HA_Float	ha_fAlp_d;	///\~Japanese 雑音の平滑化係数
	HA_Float	ha_fDelta;	///\~Japanese 音声確率閾値
	HA_Integer	ha_iL;	///\~Japanese 更新のフレーム長
	HA_Float	ha_fAlp_s;	///\~Japanese 入力スペクトルの平滑化係数
	HA_Float	ha_fLeak_ampleak_factor;	///\~Japanese
	HA_Float	ha_fSs_floor;	///\~Japanese
	HA_Float	ha_fNoise_compense;	///\~Japanese 
	
} MICARY_PrmPFExec;

/**
 Saved Post Filter Values struct
 \author	Masatoshi Tsutsumi
 \date	27 Dec 2006
*/
typedef struct{
	MICARY_SavedMCRA	*psSavedMCRA;	///\~Japanese 定常雑音計算の保存値
} MICARY_SavedPFExec;

/**
 Command Line Parameters struct
 \author	Masatoshi Tsutsumi
 \date	15 Nov 2006
*/
typedef struct{
	HA_String		cOriginalWavFile;	///<\~Japanese 分析ファイル名
	HA_Float		ha_fStartTime;	///<\~Japanes 開始時間[sec]
	HA_Float		ha_fEndTime;	///<\~Japanes 終了時間[sec]
	HA_String		cSeparateWavFile;	///<\~Japanese 分離ファイル名
	HA_String		cOriginalSpecFile;	///<\~Japanese 入力スペクトルファイル名
	HA_Integer		ha_iStartFrame;	///<\~Japanese 分析開始フレーム[frame]
	HA_Integer		ha_iEndFrame;	///<\~Japanese 分析終了フレーム[frame]
	HA_String		cSeparateSpecFile;	///<\~Japanese 出力スペクトルファイル名
	HA_String		cNoiseSpecFile;	///<\~Japanese 雑音スペクトルファイル名
	HA_Integer		ha_iNFFT;			///<\~Japanese FFT長[point]
	HA_Integer		ha_iWindow;			///<\~Japanese 窓長[sample]
	MICARY_WindowEnum	micary_nwnWindow;	///<\~Japanese 窓
	HA_Integer		ha_iShift;			///<\~Japanese シフト数[sample]
	HA_Float		ha_fUpperFreq;	///<\~Japanes 分析上限周波数[Hz]
	MICARY_PrmMCRA		micary_sPrmMCRA;	///<\~Japanes MCRA設定パラメータ
	MICARY_PrmLeakNoise		micary_sPrmLeakNoise;	///<\~Japanes 漏れ設定パラメータ
	MICARY_PrmRevNoise		micary_sPrmRevNoise;	///<\~Japanes 残響成分パラメータ
	MICARY_PrmEstSN		micary_sPrmEstSN;	///<\~Japanes SN推定設定パラメータ
	MICARY_PrmVoiceP		micary_sPrmVoiceP;	///<\~Japanes 音声存在確率設定パラメータ
	MICARY_PrmGain		micary_sPrmGain;	///<\~Japanes 最適ゲイン推定パラメータ
} MICARY_PFCmdArgs;
// ---------------------------------------------------------------------
#endif	// MICARY_PF_Struct_h_defined
