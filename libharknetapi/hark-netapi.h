/*******************************************************************************/
/**                                                                           **/
/**     Object name : libhark-netapi                                          **/
/**                                                                           **/
/**     Description                                                           **/
/**     -- -- -- -- -- -- -- --                                               **/
/**     ネットワーク経由で利用するためのAPIのヘッダファイル**/
/**                                                                           **/
/**     History**/
/**     -- -- -- -- -- -- -- --**/
/**     2011.07.07  新規作成**/
/**                                                                           **/
/**                                                                           **/
/**                                                                           **/
/*******************************************************************************/


/*=========================================================*/
/*               Special Macro Definition                  */
/*=========================================================*/

#ifndef _INCLUDE_LIB_KC_CLIENT_H_
#define _INCLUDE_LIB_KC_CLIENT_H_


#ifdef WIN32
	#ifdef NETWORK_EXPORTS
	#define _EXPORT __declspec(dllexport)
	#else
	#define _EXPORT __declspec(dllimport)
	#endif
	#define CONV_INET_ADDR_MODE
#else
	#define _EXPORT extern
	#undef  CONV_INET_ADDR_MODE
#endif



/*=========================================================*/
/*                   Include Files                         */
/*=========================================================*/

#ifdef WIN32
	#include <winsock.h>
#else

#include <sys/socket.h>
#include <netinet/in.h>

#endif



/*=========================================================*/
/*                   Macro Definition                      */
/*=========================================================*/
#define WS_BYTE_PERSAMPLE      4

/*
#define DATA_FORMAT_32BIT_PCM  32
*/

/* 通信で使用するポート番号 */
#define WS_CONTROL_PORT                     52920
#define WS_DATA_PORT                        52921


#define WS_CONECT_START_REQ  100 //接続要求
#define WS_CONECT_START_RES  101 //接続応答

#define WS_RECORD_START_REQ  200 //録音開始要求
#define WS_RECORD_START_RES  201 //録音開始応答

#define WS_RECORD_STOP_REQ   300 //録音停止要求
#define WS_RECORD_STOP_RES   301 //録音停止応答

#define WS_CONECT_END_REQ    900 //接続終了要求
#define WS_CONECT_END_RES    901 //接続終了応答


/* 通信で使用するバッファサイズ */
#define CMD_STRING_LENGTH       64
#define DATA_BUFFER_SIZE        (64 * 1024)


/* 通信対象 */
#define TARGET_TYPE_NETWORK          5 
#define NETWORK_MAX_CH_NUM           4


#ifdef WIN32
	#define write(a, b, c) send(a, b, c, 0)
	#define read(a, b, c)  recv(a, b, c, 0)
	#define close closesocket
	#define ioctl ioctlsocket
#endif



/*=========================================================*/
/*                 Structure Declaration                   */
/*=========================================================*/
#ifdef WIN32
typedef unsigned long in_addr_t;
#else
typedef int SOCKET;
#endif

typedef struct _KC_SOCKET_INFO_t_ 
{
    SOCKET             socket_fd;
    struct sockaddr_in socket_addr;
    int                socket_addr_size;
} KC_SOCKET_INFO_t;




typedef struct _KC_HANDLE_t_
{
    KC_SOCKET_INFO_t control_server_info;
    KC_SOCKET_INFO_t data_server_info;
    
#ifdef CONV_INET_ADDR_MODE
    in_addr_t        server_ip_addr;
#else
    struct in_addr   server_ip_addr;
#endif

    int              ch_mode;           /* [共通] チャネル数に関する動作モード */
    int              ch_num;            /* [共通] ソフトウェア上でのチャネル数 */
    int              max_ch_num;        /* [共通] 最大チャネル数（ボード単位） */
    int              header_flag;       /* [A/D]  ヘッダの有効、無効フラグ     */
    int              header_count;      /* [A/D]  A/D開始からのフレーム数      */
    int              header_size;       /* [A/D]  ヘッダのバイト数             */
    int              resolution_bit;    /* [共通] 個々のデータサイズ           */
    int              sample_per_frame;  /* [A/D]  フレーム中のサンプル数       */
    int              byte_per_sample;   /* [A/D]  サンプルあたりのバイト数     */
    int              frame_size;        /* [A/D]  フレームのバイト数           */
    int              target_type;       /* [共通] 通信相手の情報               */
    int              target_sub_type;   /* [共通] 通信相手の追加情報           */
    int              data_format;       /* [共通] データフォーマット           */
    float            gain;              /* [共通] floatモードでのスケール情報  */
    void             *work_buffer;      /* [共通] 作業用バッファ               */
    int              time_sec;          /* [A/D]  最終取得フレームの時刻(sec)  */
    int              time_usec;         /* [A/D]  最終取得フレームの時刻(usec) */

} KC_HANDLE_t;


/*=========================================================*/
/*                    Private Valiable                     */
/*=========================================================*/

/*=========================================================*/
/*                    Public Valiable                      */
/*=========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

    /* A/D関係 */
    _EXPORT KC_HANDLE_t *KcHandleOpen(char *ip_addr);
    _EXPORT int KcHandleClose(KC_HANDLE_t *h_ws);
    _EXPORT int KcADStart(KC_HANDLE_t *h_ws);
    _EXPORT int KcADStop(KC_HANDLE_t *h_ws);
    _EXPORT int KcDataRead(KC_HANDLE_t *h_ws, char *buffer, size_t read_req_size);
    _EXPORT int KcSampleDataRead(KC_HANDLE_t *h_ws, char *buffer, size_t read_req_size);
#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_LIB_KC_CLIENT_H_ */

/* EOF */
