// ---------------------------------------------------------------------
/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#ifndef	Cexec_h_defined
#define	Cexec_h_defined
// ---------------------------------------------------------------------

#include "../config.h"
#ifndef ENABLE_HARKIO_NH

///#include "ArrayInit.h"
///#include "MICARY_GHDSS_struct.h"
#include "GHDSS_SeparateMat.h"
///#include "GHDSS_MathMat2.h"
//#include "GHDSS_TFData.h"

#include <string>
#include <iostream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "MICARYHA_struct.h"	// ClassView によって追加されました。
#include "HarkFileIO.h"
// 2014/8/25 added by N.Tanaka libharkio3対応
#ifdef ENABLE_HARKIO3
extern "C"{
	#include "libharkio3.h"
}
#endif


#define __MICPOSITION_MOVE_ORIGINALPOINT	(1) // 0 : shift position center / 1 : fix

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define __SAVED_LOC_THETA_PITCH	(5)	//5[deg]ピッチ
#define __UPDATE_METHOD_ID	(0)
#define __UPDATE_METHOD_POS (1)
#define __UPDATE_METHOD_ID_POS (2)
#define __INVALID_SRC_ID (-32768)

#define __SRCID_FRONT_CREATE	(1)
#define __SRCID_FRONT_EXIST		(0)
#define __SRCID_FRONT_REMOVE	(-1)

#define __FILE_FORMAT_MICARY	(0)
#define __FILE_FORMAT_HARK	(1)

// Added by morio 2013/06/19
#define __COMPARE_MODE_DEG	(0)
#define __COMPARE_MODE_TFINDEX (1)

// ---------------------------------------------------------------------
/**
 \class Cexec
 \brief \~English Class for executing GHDSSprocess 
 \brief \~Japanese GHDSS処理実行クラス
*/
class Cexec{
public:
	
	/**
	 \brief \~Japanese コンストラクタ
	*/
	Cexec() ;
	
      /**
	 \brief \~Japanese デストラクタ
	*/
	~Cexec() ;

	/**
	 \brief \~Japanese クリア
	 \return \~English error code (0 is error ,1 is success)	
	*/
	int Clear() ;
      
	MICARY_SavedSeparation *m_psSepMat ;	//分離行列保存値
	
	
	/**
	 \brief \~Japanese 初期化
	 \return \~English error code (0 is error ,1 is success)	
	 \param iNch \~English Size of microphone channels.
	 \~Japanese マイクチャンネル数
	 \param iNpos \~English Size of sources. 
	 \~Japanese 音源数
	 \param iLower_freq \~English Index of lower bound frequency.
	 \~Japanese 下限周波数インデックス
	 \param iUpper_freq \~English Index of upper bound frequency.
	 \~Japanese 上限周波数インデックス
	 \param filename \~English mic position filename.
	 \~Japanese マイク位置座標ファイル
       \param fspeed_sound \~English speed of sound
	 \~Japanese 音速
	 \param filename_tf \~English Transfer function data file name.
       \~Japanese 伝達関数データファイル名
	 \param bFlgFixedNoise \~English The flag for fixed noise source separation.(true is separate ,false is no separate)
       \~Japanese 固定雑音源の分離フラグ(true 分離 ,false 分離なし)
	 \param filename_FixedNoise \~English Fixed noise source position file name.
       \~Japanese 固定雑音源位置座標ファイル名
	 \param filename_sm \~English Initial separate matrix file name.
	 \~Japanese 分離行列初期値データファイル名     
    */
	int Initialize(const char *filename,float fspeed_sound,int iFlgTf,const char *filename_tf ,
		bool bFlgFixedNoise,const char *filename_FixedNoise, int iFlgSm,const char *filename_sm) ;
	
	/**
	 \brief \~Japanese フレームパラメータの設定
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese フレームパラメータ
	 \param iFs \~English Sampling rate[Hz]
	 \~Japanese サンプリング周波数[Hz]
	 \param iLength \~English The length of a frame
	 \~Japanese フレーム長
	 \param iOverlap \~English Size of overlap sample
	 \~Japanese オーバーラップサンプル数
	 \param enumWin \~English The form of time window(0 is Hanning window ,1 is CONJ window)
     　\~Japanese 時間窓 (0 Hanning窓 ,1 CONJ窓)
	*/
	int SetPrmFrame(MICARY_PrmFrame *psPrmFrame,int iFs,int iLength,int iOverlap,int enumWin) ;
	
	/**
	 \brief \~Japanese 伝達関数の初期化
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter pointer
	 \~Japanese フレームパラメータのポインタ
	*/
	int InitTFMatrix(MICARY_PrmFrame *psPrmFrame) ;

	// 2012/01/13 added
	/**
	 \brief \~Japanese TFデータベース、分離行列初期値データベースの初期値に値を更新
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter pointer
	 \~Japanese フレームパラメータのポインタ
	 \param iSrcIndex \~English Index of the source to load
	 \~Japanese 読みだす音源番号
	*/
	int LoadInitialTFMatrix(MICARY_PrmFrame *psPrmFrame, int iSrcIndex);

	/**
	 \breif \~Japanese 音源位置データをクラス内部保存値に代入する
 	 \return \~English error code (0 is error ,1 is success)	
	 \param locaSrcPos \~English Source position array 
	 \~Japanese 音源位置座標データ
	*/
	int AssignSrcPos(MICARY_LocationArray locaSrcPos) ;

	/**
	 \brief \~Japanese 固定雑音源位置座標の作成
 	 \return \~English error code (0 is error ,1 is success)	
	 \param locaSrc \~English source position array with size of includ fixed noise source position
	 \~Japanese 固定雑音源を含めたサイズを持つ音源位置座標データ
	 */
	int MakeFixedNoise_Pos(MICARY_LocationArray &locaSrc);

	/**
	 \brief \~Japanese 固定雑音源の検出チェック
	 \return \~English error code (true is include ,false is no include)	
	 \param locaSrc \~English Source position array 
	 \~Japanese 音源位置座標データ
	*/
	bool CheckFixedNoise_noise(MICARY_LocationArray locaSrc);

	/**
	 \brief \~Japanese 分離行列の初期化
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese フレームパラメータ
	*/
	int InitSepMatrix(MICARY_PrmFrame *psPrmFrame) ;

	// 2012/01/13 added
	/**
	 \brief \~Japanese 音源インデックスで分離行列初期値を読み込み
 	 \return \~English error code (0 is error ,1 is success)	
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese フレームパラメータ
	 \param iSrcIndex \~English Index of the source to load
	 \~Japanese 読み込む音源IDインデックス
	*/
	int LoadInitialSepMatrix(MICARY_PrmFrame *psPrmFrame, int iSrcIndex);

	/**
	 \breif \~Japanese 最近傍位置のデータインデックスを取得
 	 \return \~English Output index 	
	 \param micary_loCur \~English Current source position
	 \~Japanese 現在の音源座標
	 \param micary_laDB \~English Database source position array
	 \~Japanese 検索元音源位置座標データベース

	*/
	HA_Integer GetNearestPos(MICARY_Location micary_loCur, MICARY_LocationArray *micary_laDB) ;
	
	/**
	 \breif \~Japanese データベースを全検索して最近傍位置のデータインデックスを取得
 	 \return \~English Output index 	
	 \param micary_loCur \~English Current source position
	 \~Japanese 現在の音源座標
	 \param micary_laDB \~English Database source position array
	 \~Japanese 検索元音源位置座標データベース

	*/
	HA_Integer GetNearestPos_ALLSearch(MICARY_Location micary_loCur, MICARY_LocationArray *micary_laDB) ;

	/**
	 \brief \~Japanese 伝達関数ファイルフォーマットパラメータの設定
	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese ファイルフォーマットタイプ(__FILE_FORMAT_MICARY または __FILE_FORMAT_HARK)
	*/	
	void SetTF_CONJ_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese 分離行列初期値ファイルフォーマットパラメータの設定
	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese ファイルフォーマットタイプ(__FILE_FORMAT_MICARY または __FILE_FORMAT_HARK)
	*/	
	void SetInitW_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese 分離行列出力ファイルフォーマットパラメータの設定
	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese ファイルフォーマットタイプ(__FILE_FORMAT_MICARY または __FILE_FORMAT_HARK)
	*/	
	void SetExportW_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese マイク位置座標ファイルフォーマットパラメータの設定
 	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese ファイルフォーマットタイプ(__FILE_FORMAT_MICARY または __FILE_FORMAT_HARK)
	*/	
	void SetMicPos_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese 固定雑音源ファイルフォーマットパラメータの設定
	 \return \~English void.
	 \param iFileFormat \~English File format type(__FILE_FORMAT_MICARY or __FILE_FORMAT_HARK)
	 \~Japanese ファイルフォーマットタイプ(__FILE_FORMAT_MICARY または __FILE_FORMAT_HARK)
	*/	
	void SetFixedNoisePos_FileFormat(int iFormat) ;

	/**
	 \brief \~Japanese 更新された全ての分離行列をファイルに出力
	 \return \~English error code (true is success ,false is error)
	 \~Japanese エラーコード(true 成功 ,false　エラー)
	 \param cFilename \~English Export file name
	 \~Japanese 出力ファイル名
	*/	
	bool ExportAllUpdateW(MICARY_PrmFrame *psPrmFrame,const char *cFilename);

	/**
	 \brief \~Japanese 更新された分離行列をファイルに追加出力
	 \return \~English error code (true is success ,false is error)
	 \~Japanese エラーコード(true 成功 ,false エラー)
	 \param cFilename \~English Export file name
	 \~Japanese 出力ファイル名
	*/	
	bool AddUpdateW(MICARY_PrmFrame *psPrmFrame,const char *cFilename,bool bExpW);

 	/**
	 \brief \~Japanese 更新用分離行列保存値の更新
	*/
	void SetUpdateW();

	/**
	 \brief \~Japanese 更新された分離行列をファイルに出力
	　\return \~English error code (true is success ,false is error)
	 \~Japanese エラーコード(true / false)
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese フレームパラメータ
	 \param cFilename \~English Export file name
	 \~Japanese 出力ファイル名
	*/
	bool ExportUpdateW(MICARY_PrmFrame *psPrmFrame,const char *cFilename);
		
	// 2012/01/13 added
	/**
	 \brief \~Japanese TF_CONJとWの引き継ぎ
	 \return \~English check code(0 is no change ,1 is change)
	 \param plaSrcPosCur \~English The durrent source position array
	 \~Japanese 現在の音源位置座標データ
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese フレームパラメータ
	*/
	// TF_CONJとWの引き継ぎ
	int ChangeSrcPos_TF_CONJ_W(MICARY_LocationArray plaSrcPosCur, MICARY_PrmFrame psPrmFrame);

	/**
	 \brief \~Japanese POSによる分離行列の変更確認
	 \return \~English check code(0 is no change ,1 is change)
	 \param plaSrcPosCur \~English The durrent source position array
	 \~Japanese 現在の音源位置座標データ
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese フレームパラメータ
	*/
	int ChangeSrcPos_W(MICARY_LocationArray plaSrcPosCur,MICARY_PrmFrame psPrmFrame);
	
	/**
	 \brief \~Japanese POSによる伝達関数の変更確認
	 \return \~English check code (0 is no change ,1 is change)
	 \param plaSrcPosCur \~English Source location array for current frame
	 \~Japanese 現フレームの音源座標配列
	 \param psPrmFrame \~English Frame parameter
	 \~Japanese フレームパラメータ
	*/
	int ChangeSrcPos_TF_CONJ(MICARY_LocationArray plaSrcPosCur,MICARY_PrmFrame psPrmFrame);
	
	/**
	 \brief \~Japanese 更新用分離行列保存値の初期化
	 \return \~English error code (0 is error ,1 is success)
	*/
	int InitUpdateW();

	/**
	 \brief \~Japanese 分離行列出力ファイルの初期化
	 \return \~English error code (0 is error ,1 is success)	
	 \param filename \~English Export separate matrix filename 
	 \~Japanese 分離行列出力ファイル名
	*/
	int InitExportW(const char *cFilename,MICARY_PrmFrame *psPrmFrame);


	//Changed by Morio 2013/01/20
	/**
	 \brief \~Japanese 更新用パラメータの設定
	 \return \~English error code (true is success ,false is error)
	 \param iUpdateMTF_CONJ \~English Update method for transfer function(ID os POS).
	 \~Japanese 伝達関数の更新方法(ID または POS)
	 \param iUpdateMW \~English Update method for separate matrix(ID or POS ,ID_POS).
	 \~Japanese 分離行列更新方法(ID または　POS ，ID_POS)
	 \param iSrc_Compare_mode \~English Compare method for source position(DEG or TFIndex).
	 \~Japanese 音源位置比較方法(DEG または TFIndex)
	 \param fThred_angle \~English The threshold angle[deg.] for check source position.
	 \~Japanese 音源位置確認のための許容角度[deg.]
	 \param fThred_angle \~English The search azimuth angle[deg.] for check source position.
	 \~Japanese 音源位置確認のための検索方位角[deg.]
	 \param fThred_angle \~English The search elevation angle[deg.] for check source position.
	 \~Japanese 音源位置確認のための検索仰角[deg.]
	 \param bCheck_distance \~English Check source distance when using TFINDEX.
	 \~Japanese TFINDEX使用時にSourceの距離が離れすぎていないか確認する，閾値はfThred_angle
	*/
	bool SetPrmUpdate(int iUpdateMTF_CONJ,int iUpdateMW,int iSrc_Compare_mode, float fThred_angle, int iUpdate_tfindex_distance, float fSearch_azimuth,float fSearch_elevation, bool bCheck_distance);

	/**
	 \brief \~Japanese マイク座標のシフトパラメータ設定
	 \param ivalue \~English Shift parameter for microphone position
	 \~Japanese shiftの実行フラグ
	*/
	void SetiMicPosShift(int ivalue);
	
	/**
	 \brief \~Japanese 初期化
	  \return \~English error code (true is success ,false is error)
	 \param iNch \~English Size of microphone channels.
	 \~Japanese マイクチャンネル数
	 \param iNpos \~English Size of sources.
	 \~Japanese 音源数
	 \param iLower_f \~English Index of lower bound frequency.
	 \~Japanese 下限周波数インデックス
	 \param iUpper_f \~English Index of upper bound frequency.
	 \~Japanese 上限周波数インデックス
	*/
	bool Initialize_Change(int iNch,int iNpos,int iLower_f,int iUpper_f);

private:
	// HarkFileFormat Class
	CHarkFileIO cHark ;
	
	/**
	 \brief \~Japanese 音源の出現・存在・消滅を確認
	 \return \~English Check current source if it creats, exists or remove. 
	 \param lcCurLoc \~English Source location.
	 \~Japanese 音源位置
	 \param iUpdateMethodW \~English Update method(ID / POS / ID_POS).
	 \~Japanese 更新方法(ID / POS / ID_POS).
	**/	
	int _CheckFrontSrcLoc(MICARY_Location lcCurLoc,int iUpdateMethodW);

	/**
	 \brief \~Japanese 消滅した音源の確認
	 **/	
	void _SetRemoveSrc();

	/**
	 \brief \~Japanese 更新用分離行列保存値のうち，更新履歴のある有効サイズを取得
	 \return \~English Valid source number
	 **/	
	int _GetNvalidUpdateW();

	/**
	 \brief \~Japanese 位置座標から内積を計算
	 \return \~English Result value
	 \param micary_Loc1 \~English location position1[x,y,z]
	 \~Japanese 位置座標1[x,y,z系]
	 \param micary_Loc2 \~English location position2[x,y,z]
	 \~Japanese 位置座標2[x,y,z系]
	**/	
	HA_Float _InnerProductLoc(MICARY_Location micary_Loc1,MICARY_Location micary_Loc2);

	/**
	 \brief \~Japanese 内積の計算
	 \return \~English Result value.
	 \param micary_faVec1 \~English input vector 1
	 \~Japanese ベクトル1
	 \param micary_faVec2 \~English input vector 2
	 \~Japanese ベクトル2
	*/
	HA_Float _InnerProductVec(MICARY_FloatArray micary_faVec1,MICARY_FloatArray micary_faVec2);
	
	// 2011/11/16 added by N.Tanaka
	/**
	 \brief \~Japanese 内積の計算
	 \return \~English Result value.
	 \param fVec1 \~English input vector 1
	 \~Japanese ベクトル1
	 \param fVec2 \~English input vector 2
	 \~Japanese ベクトル2
	*/
	HA_Float _InnerProductVec(float fVec1[3],float fVec2[3]);
	
	/**
	 \brief \~Japanese 角度[angle]からPosition[x,y,z]に変換
	 \return \~English location position[x,y,z]
	 \param plocPos \~English location position[x,y,z,angle]
	 \~Japanese 位置座標[水平角度が設定されている]
　 　 **/	
	void _Ang2Pos(MICARY_Location *plocPos);
	
	/**
	 \brief \~Japanese 2点のIFIndex距離を求める
	 \return \~English Calicurate TFIndex distance
	 \param laRefPos \~English Source location (MICARY_Location type)
	 \~Japanese 音源ベクトル
	 \param laObjPos \~English Source location (MICARY_Location type)
	 \~Japanese 音源ベクトル
	 **/	
	HA_Integer _tfindexDistance(MICARY_Location laRefPos,MICARY_Location laObjPos);

	/**
	 \brief \~Japanese 2点の位置がThreshold_angleで設定された範囲内であるかの判定
	 \return \~English check code(true is in range ,false is out of range)
	 \param laRefPos \~English reference(saved) position
	 \~Japanese 参照位置
　 　  \param laObjPos \~English objective position
	 \~Japanese 目的位置
	*/	
	bool _IsThreshPos(MICARY_Location laRefPos,MICARY_Location laObjPos);
	
	/**
	 \brief \~Japanese 分離行列の音源位置による変更の確認と引継ぎ
	 \return \~English check code(true is no change ,false is change)
	 \param iIdcur \~English The objective index of current source position array.    
	 \~Japanese 対象となる現在の音源位置データのインデックス
	*/
	bool _CheckSrcPos_W(int iIdcur);
	
	/**
	 \brief \~Japanese 分離行列の音源IDによる変更の確認と引継ぎ
	 \return \~English check code(true is no change ,false is change)
	 \param iIdcur \~English The objective index of current source position array.  
	 \~Japanese 対象となる現在の音源位置データのインデックス
	*/
	bool _CheckSrcID_W(int iIdcur);
	
	/**
	 \brief \~Japanese 伝達関数の音源IDによる変更の確認と引継ぎ
	 \return \~English check code(true is no change ,false is change)
	 \param caTFDBInit \~English The previous TF_CONJ data. 
	 \~Japanese 1フレーム前のTF_CONJデータ
	 \param laSrcPosInit \~English The previous source position array.
	 \~Japanese 1フレーム前の音源位置データ
	*/
	bool _CheckSrcID_TF_CONJ(MICARY_ComplexArray caTFDBInit,MICARY_LocationArray laSrcPosInit);
	
	/**
	 \brief \~Japanese (共役定義)伝達関数初期値の計算
	 \return \~English error code(0 is error ,1 is success)
	 \param iUpper_freq \~English The index of upper boundary frequency
	 \~Japanese 上限周波数インデックス
	 \param psPrmFrame \~English Parameter struct pointer for current frame
	 \~Japanese 現フレームのパラメータ構造体ポインタ
	*/
	int MakeTFMatrix(int iUpper_freq,MICARY_PrmFrame *psPrmFrame ) ;

	// 2012/01/13 added
	/**
	 \brief \~Japanese (共役定義)伝達関数初期値の計算
	 \return \~English error code(0 is error ,1 is success)
	 \param psPrmFrame \~English Parameter struct pointer for current frame
	 \~Japanese 現フレームのパラメータ構造体ポインタ
	 \param idsrc \~English the index of the source id
	 \~Japanese TFデータを作成する音源IDインデックス
	*/
	int MakeTFMatrix2(MICARY_PrmFrame *psPrmFrame, int idsrc) ;

	/**
	 \brief \~Japanese 伝達関数のファイル読み込み
	 \return \~English error code(0 is error, 1 is success)
	 \param psPrmFrame \~English Parameter struct for current frame
	 \~Japanese 現フレームのパラメータ構造体
	*/
	int ReadTFMatrix(MICARY_PrmFrame psPrmFrame) ;

	/**
	 \brief \~Japanese マイク位置座標のファイル読み込み
	 \return \~English error code(0 is error ,1 is success)
	*/
	int ReadMicPos() ;	

	/**
	 \brief \~Japanese 分離行列のファイル読み込み
	　\return \~English error code(0 is error, 1 is success)
	 \param psPrmFrame \~English Parameter struct for current frame
	 \~Japanese 現フレームのパラメータ構造体
	*/
	int ReadSepMatrix(MICARY_PrmFrame psPrmFrame);
	
	
	/**
	 \brief \~Japanese 保存値伝達関数データベースから最近傍位置音源データの読み込み
	 \return \~English The saved data index
	 \param OutTF \~English Read transfer function data
	 \~Japanese 読み込まれた伝達関数データ
	 \param TFDB \~English The saved transfer function data base
	 \~Japanese 保存されている伝達関数データベース
	*/
	int GetNearestTFMatrixFromDB(MICARY_TFDatabase *OutTF,MICARY_TFDatabase *TFDB);

	// 
	/**
	 \brief \~Japanese DBからデータの取得(最も近い座標を読む)
	 \return \~English The saved data index
	 \param OutTF \~English Read transfer function data
	 \~Japanese 読み込まれた伝達関数データ
	 \param TFDB \~English The saved transfer function data base
	 \~Japanese 保存されている伝達関数データベース
	 \param idsrc \~English Index of the source to load
	 \~Japanese 読み込む音源インデックス
	*/
	int GetNearestTFMatrixFromDB2(MICARY_TFDatabase *OutTF,MICARY_TFDatabase *TFDB, int idsrc);

	/**
	 \brief \~Japanese 保存値分離行列データベースから最近傍位置音源データの読み込み
	 \return \~English The saved data index
	 \param OutSM \~English Read separate matrix
	 \~Japanese 読み込まれた分離行列
	 \param SMDB \~English The saved separate matrix data base
	 \~Japanese 保存されている分離行列データベース
	*/
	int GetNearestSepMatrixFromDB(MICARY_SavedSeparation *OutSM,MICARY_SMDatabase *SMDB);

	/**
	 \brief \~Japanese 保存値分離行列データベースから最近傍位置音源データの読み込み
	 \return \~English The saved data index
	 \param OutSM \~English Read separate matrix
	 \~Japanese 読み込まれた分離行列
	 \param SMDB \~English The saved separate matrix data base
	 \~Japanese 保存されている分離行列データベース
	 \param idsrc \~English Index of the source to load
	 \~Japanese 読み込む音源インデックス
	*/
	int GetNearestSepMatrixFromDB2(MICARY_SavedSeparation *OutSM, MICARY_SMDatabase *SMDB, int idsrc);

	/**
	\brief \~Japanese POSによる確認時の許容範囲 (DEG)
	\~English　The threshold angle when it checks source change by POS. 
	*/
	HA_Float m_fThred_angle;	 
	
	/**
	\brief \~Japanese POSによる確認時の許容範囲 (TFIndex)
	\~English　The threshold angle when it checks source change by POS. (TFIndex)
	*/
	HA_Integer m_fThred_TFIndex;

	/**
	\brief \~Japanese データベース検索時の検索範囲方位角
	\~English　The search azimuth angle when it checks source change. 
	*/
	HA_Float m_fSearch_azimuth;

	/**
	\brief \~Japanese データベース検索時の検索範囲仰角
	\~English　The search elevation angle when it checks source change. 
	*/
	HA_Float m_fSearch_elevation;

	/**
	\brief \~Japanese 更新用分離行列保存値 
	\~English　The saved separate matrix for update. 
	*/
	MICARY_UpdateSeparationMat *m_updateW;	

	/**
	\brief \~Japanese 伝達関数の更新方法 
	\~English　The update method for the (conjugated)transfer function. 
	*/
	int m_iUpdateMethodTF_CONJ;

	/**
	\brief \~Japanese 分離行列の更新方法 
	\~English　The update method for the separate matrix. 
	*/
	int m_iUpdateMethodW;
	
	/**
	\brief \~Japanese Positionの比較方法 
	\~English The compare method for source position. 
	*/
	int m_iSrc_Compare_mode;
	
	/**
	\brief \~Japanese TFIndex使用時のSource距離確認フラグ
	\~English Check flag for confirm source distace when using TFIndex. 
	*/
	
	bool m_bCheck_distance;
	
	/**
	\brief \~Japanese 1フレーム前の音源位置データ 
	\~English　The previous source position array. 
	*/
	MICARY_LocationArray *m_laFrontSrcLoc;	

	/**
	\brief \~Japanese 分離行列の実際の更新方法 
	\~English　The real update method for the separate matrix. 
	*/
	MICARY_IntArray *m_iUpdateMethodWex ;

	/**
	\brief \~Japanese マイク位置座標ファイル名 
	\~English　The microphone position file name. 
	*/
	char *m_file ;

	/**
	\brief \~Japanese (共役定義)伝達関数ファイル名 
	\~English　The (conjugated) transfer function file name. 
	*/
	char *m_file_tf ;

	/**
	\brief \~Japanese 音速 
	\~English　The speed of sound. 
	*/
	float m_fspeed_sound ;	

	/**
	\brief \~Japanese 伝達関数初期化方法フラグ(0 理論値 ,1 ファイル読み込み)
	\~English　The initialize method flag for (conjugated) transfer function. (0 is calculate ,1 is read data file)
	*/
	int m_iFlgtf ;

	/**
	\brief \~Japanese 固定雑音源の分離フラグ(true 分離 , false 分離なし) 
	\~English　The separate flag for fixed noise source.(true is separate ,false is no separate) 
	*/
	bool m_bFlgFixedNoise ; 

	/**
	\brief \~Japanese 固定雑音源位置 
	\~English　The fixed noise source position. 
	*/ 
	MICARY_LocationArray locaSrcFixedNoise ; 

	/**
	\brief \~Japanese ファイルから読み込まれた(共役定義)伝達関数データ 
	\~English　The read (conjugated) transfer function from file. 
	*/
	MICARY_TFDatabase  *m_psTFDB ;

	/**
	\brief \~Japanese ファイルから読み込まれた分離行列データ 
	\~English　The read separate matrix from file. 
	*/
	MICARY_SMDatabase  *m_psSMDB ;

	/**
	\brief \~Japanese 分離行列初期化方法フラグ(0 理論値 ,1 ファイル読み込み) 
	\~English　The initialize method for separate matrix.(0 is calculate ,1 is read data file) 
	*/
	int m_iFlgsm ;	

	/**
	\brief \~Japanese 分離行列ファイル名 
	\~English　The separate matrix file name. 
	*/
	char *m_file_sm ;	

	/**
	\brief \~Japanese 固定雑音源位置座標ファイル名 
	\~English　The fixed noise source position file name. 
	*/
	char *m_file_FixedNoise ;

	/**
	\brief \~Japanese マイク位置 
	\~English　The microphone position array. 
	*/
	MICARY_LocationArray locaMic ; 

	/**
	\brief \~Japanese マイク位置の重心シフト(0 シフト ,1 固定) 
	\~English　The shift flag for the center of microphone position.(0 is shift ,1 is fix) 
	*/
	int iMicPosShift;

	/**
	\brief \~Japanese 伝達関数ファイルのフォーマットタイプ 
	\~English　The transfer function file format. 
	*/
	int m_iFormat_TF_CONJ ;

	/**
	\brief \~Japanese 分離行列初期値ファイルのフォーマットタイプ 
	\~English　The initial separate matrix file format. 
	*/
	int m_iFormat_InitW;	

	/**
	\brief \~Japanese 分離行列出力ファイルのフォーマットタイプ 
	\~English　The export separate matrix file format. 
	*/
	int m_iFormat_ExportW ;	
	
	/**
	\brief \~Japanese マイク位置座標ファイルのフォーマットタイプ 
	\~English　The microphone position file format. 
	*/
	int m_iFormat_MicPos ;
	
	/**
	\brief \~Japanese 固定雑音源位置座標ファイルのフォーマットタイプ 
	\~English　The fixed noise source position file format. 
	*/
	int m_iFormat_FixedNoise ;  

	
	// 2012/01/19 added
	/**
	\brief \~Japanese 伝達関数データテンポラリバッファ
	\~English Tmporary buffer of TF database
	*/
	MICARY_ComplexArray m_tmp_cmpaTFDB ;
	
	/**
	\brief \~Japanese 音源位置保存用テンポラリバッファ
	\~English Tmporary buffer of the source position
	*/
	MICARY_LocationArray m_plaSrcPosIni ;
	
		
};
// ---------------------------------------------------------------------
#endif // ENABLE_HARKIO_NH
#endif // Cexec_h_defined

