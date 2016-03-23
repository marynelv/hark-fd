//*******************************************************************************/
//**                                                                           **/
//**     Object name : hark-netapi                                             **/
//**                                                                           **/
//**     Public      : KcHandleOpen                                            **/
//**                   KcHandleClose                                           **/
//**                   KcADStart                                               **/
//**                   KcADStop                                                **/
//**                   KcDataRead                                              **/
//**                   KcSampleDataRead                                        **/
//**                                                                           **/
//**     Description                                                           **/
//**     -- -- -- -- -- -- -- -----------------------------------------------  **/
//**     各種Deviceをネットワーク経由で利用するためのAPI                       **/
//**                                                                           **/
//**     History                                                               **/
//**     -- -- -- -- -- -- -- -----------------------------------------------  **/
//**     2011.07.07  新規作成                                                  **/
//**     2013.03.01  改修                                                      **/
//**                                                                           **/
//**                                                                           **/
//*******************************************************************************/


/*=========================================================*/
/*               Special Macro Definition                  */
/*=========================================================*/

#ifdef WIN32

#pragma warning(disable:4996)

#endif

#undef DEBUG

//#define DEBUG_LOG 1
//#define DEBUG 1

/*=========================================================*/
/*                   Include Files                         */
/*=========================================================*/

#ifdef WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "WinSockVersion.h"

#include <time.h>


#ifdef _DEBUG
#define DEBUG
#endif

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/io.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/select.h>

#endif


#include "hark-netapi.h"


/*=========================================================*/
/*                   Macro Definition                      */
/*=========================================================*/
#define KINECT_SAMPLE_PER_FRAME        1024
#define KINECT_HEADER_SIZE  (sizeof(int) * 2)

#ifdef DEBUG

#define DEBUG_PRINT printf

#else

#define DEBUG_PRINT(...)  

#endif

#ifdef DEBUG_LOG

#define LOG_PRINT printf

#else

#define LOG_PRINT(...)  

#endif


int hasTwoColons(char *ip_addr){
    int i;
    int numColons = 0;
    for(i = 0; ip_addr[i]; i++){
        if(ip_addr[i] == ':') numColons++;
    }
    return numColons == 2;
}


/*******************************************************************************/
/**                                                                           **/
/**   Public  :                                                               **/
/**                                                                           **/
/**  History : 2010.10.18 新規作成                                            **/
/**                                                                           **/
/**  Description                                                              **/
/**  -----------------------------------------------------------------------  **/
/**  AD制御とデータ取得用のハンドルの作成                                     **/
/**                                                                           **/
/*******************************************************************************/
KC_HANDLE_t *KcHandleOpen(char *ip_addr)
{
    int status;
    int ctrlPort = WS_CONTROL_PORT;
    int dataPort = WS_DATA_PORT;
    KC_HANDLE_t *h_ws;

	DEBUG_PRINT("KcHandleOpen start\n");

    /////////////////////////////////////////////////////////////
    // 31.Jul.2014 Takeshi Mizumoto
    // Support manual IP address setting for
    //    WS_CONTROL_PORT (for control)
    //    WS_DATA_PORT (for data)
    // Rule:
    //    hostname:controlPort:dataPort
    //    (:controlPort:dataPort is optional)
    //
    // Example:
    //    127.0.0.1            -->  hostname=127.0.0.1, control port = WS_CONTROL_PORT, data port = WS_DATA_PORT (default)
    //    127.0.0.1:3000:4000  -->  hostname=127.0.0.1, control port = 3000,            data port = 4000
    //    127.0.0.1:4000       -->  not allowed (two ports are mandatory)
    /////////////////////////////////////////////////////////////

    if(hasTwoColons(ip_addr)){
        int i, j;
        char ip_addr_tmp[4192];
        char buf[100];
        int flag = 0;
        int last = 0;
        for(i=0; ip_addr[i]; i++){
            if(ip_addr[i] == ':' && flag == 0){
                strncpy(ip_addr_tmp, ip_addr, i);
                ip_addr_tmp[i] = '\0';
                last = i + 1;
                flag = 1;
            } else if(ip_addr[i] == ':' && flag == 1){
                strcpy(buf, ip_addr + last);
                for(j=0; j<buf[j]; j++) if(buf[j] == ':') buf[j] = '\0';
                ctrlPort = atoi(buf);
                last = i + 1;
                flag = 2;
            } else if(ip_addr[i] == '\0' && flag == 2){
            }
        }
        strncpy(buf, ip_addr + last, i);
        dataPort = atoi(buf);
        strcpy(ip_addr, ip_addr_tmp);

        fprintf(stderr, "Setting:\n");
        fprintf(stderr, "   Host        :%s\n", ip_addr);
        fprintf(stderr, "   Control port:%d\n", ctrlPort);
        fprintf(stderr, "   Data port   :%d\n", dataPort);
    }


#ifdef CONV_INET_ADDR_MODE
    in_addr_t server_ip_addr;
#else
    struct in_addr server_ip_addr;
#endif
    char buffer[CMD_STRING_LENGTH];
#ifdef WIN32
    {
        WSADATA wsa_data;
        
        /* 初期化 */
        status = WSAStartup(MAKEWORD(WIN_SOCK_MAJOR_VERSION, WIN_SOCK_MINOR_VERSION), &wsa_data);
        if(status != 0) 
        {
            printf("WSAStartup Error. (%d)\n", status);
            return NULL;
        }
    }
#endif

	/*-------------------------------------------------------------------------*/
    /* 制御サーバのIPアドレスを変換 */
	/*-------------------------------------------------------------------------*/
#ifdef CONV_INET_ADDR_MODE
    server_ip_addr = inet_addr(ip_addr);

    DEBUG_PRINT("  ip_addr->%s\n", ip_addr);

    if(server_ip_addr == -1)
    {
        printf("Server IP Address Error.\n");
        printf("  %s ??\n", ip_addr);
        goto HANDLE_OPEN_ERR1;
    }
#else
    status = inet_aton(ip_addr, &server_ip_addr);
    if(status == 0)
    {
        printf("Server IP Address Error.\n");
        printf("  %s ??\n", ip_addr);
        goto HANDLE_OPEN_ERR1;
    }
#endif

	/*-------------------------------------------------------------------------*/
    /* ハンドル用のメモリを確保 */
	/*-------------------------------------------------------------------------*/
    h_ws = (KC_HANDLE_t *)malloc(sizeof(KC_HANDLE_t));
    if(h_ws == NULL)
    {
        printf("Memory Allocate Error.\n");
        goto HANDLE_OPEN_ERR1;
    }
    memset(h_ws, 0, sizeof(KC_HANDLE_t));


	/*-------------------------------------------------------------------------*/
    /* 制御用ソケットの作成 */
	/*-------------------------------------------------------------------------*/
	DEBUG_PRINT("control socket start\n");

    h_ws->control_server_info.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(h_ws->control_server_info.socket_fd == -1)
    {
        printf("Control Socket Create Error.\n");
        goto HANDLE_OPEN_ERR2;
    }

	DEBUG_PRINT("control socket make end: socket[%d]\n",h_ws->control_server_info.socket_fd );

	/*-------------------------------------------------------------------------*/
    /* 制御用ソケットの構造体を初期化 */
	/*-------------------------------------------------------------------------*/
    h_ws->control_server_info.socket_addr_size = sizeof(struct sockaddr_in);

    h_ws->control_server_info.socket_addr.sin_family      = AF_INET;
    h_ws->control_server_info.socket_addr.sin_port        = htons(ctrlPort);

#ifdef CONV_INET_ADDR_MODE
    h_ws->control_server_info.socket_addr.sin_addr.s_addr = server_ip_addr;
#else
    h_ws->control_server_info.socket_addr.sin_addr.s_addr = server_ip_addr.s_addr;
#endif

    
	/*-------------------------------------------------------------------------*/
    /* 制御サーバに接続 */
	/*-------------------------------------------------------------------------*/
    status = connect(h_ws->control_server_info.socket_fd, 
                     (struct sockaddr *)&(h_ws->control_server_info.socket_addr),
                     h_ws->control_server_info.socket_addr_size);

    if(status == -1)
    {
        printf("Control Socket Connection Error.\n");
        goto HANDLE_OPEN_ERR3;
    }


	/*-------------------------------------------------------------------------*/
    /* 「接続要求」文字列（コマンド）を作成 */
	/*-------------------------------------------------------------------------*/
	memset(buffer,0,sizeof(buffer));
    strcpy(buffer, "100");

    /* 「接続要求」を送信 */
    status=write(h_ws->control_server_info.socket_fd, buffer, strlen(buffer)+1);


    /* 「接続応答」受信 */
	memset(buffer,0,sizeof(buffer));
    status=read(h_ws->control_server_info.socket_fd, buffer, CMD_STRING_LENGTH);
    
    
    status = atoi((char*)buffer);
    
    switch(status)
    {
	case 101:
		DEBUG_PRINT("■ SEND: connection response [101]\n");
		break;
	default:
		printf("error: Unknown Data Recv[%d]\n",status);
        goto HANDLE_OPEN_ERR3;
	}


	/*-------------------------------------------------------------------------*/
	/* データ用ソケットの作成 */
	/*-------------------------------------------------------------------------*/
	DEBUG_PRINT("data socket make start\n");
    h_ws->data_server_info.socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(h_ws->data_server_info.socket_fd == -1)
    {
        printf("Data Socket Create Error.\n");
        goto HANDLE_OPEN_ERR3;
    }

	DEBUG_PRINT("data socket make end: socket[%d]\n",h_ws->data_server_info.socket_fd);

	/*-------------------------------------------------------------------------*/
    /* データ用ソケットの構造体を初期化 */
	/*-------------------------------------------------------------------------*/
	DEBUG_PRINT("data socket init start\n");
    h_ws->data_server_info.socket_addr_size = sizeof(struct sockaddr_in);

    h_ws->data_server_info.socket_addr.sin_family      = AF_INET;
    h_ws->data_server_info.socket_addr.sin_port        = htons(dataPort);

#ifdef CONV_INET_ADDR_MODE
    h_ws->data_server_info.socket_addr.sin_addr.s_addr = server_ip_addr;
#else
    h_ws->data_server_info.socket_addr.sin_addr.s_addr = server_ip_addr.s_addr;
#endif

	DEBUG_PRINT("data socket init end\n");
	printf("port[%d]\n",dataPort);
	printf("ipaddr[%s]\n",ip_addr);

	/*-------------------------------------------------------------------------*/
    /* データサーバに接続 */
	/*-------------------------------------------------------------------------*/
	DEBUG_PRINT("data server connect start\n"); 
   status = connect(h_ws->data_server_info.socket_fd, 
                     (struct sockaddr *)&(h_ws->data_server_info.socket_addr),
                     h_ws->data_server_info.socket_addr_size);

    if(status == -1)
    {
        printf("Data Socket Connection Error.\n");
        goto HANDLE_OPEN_ERR4;
    }

    h_ws->server_ip_addr = server_ip_addr;
    h_ws->header_count   = 0;
    h_ws->max_ch_num     = 4;
    h_ws->data_format    = 32;
    h_ws->resolution_bit = 32;
    h_ws->work_buffer    = NULL;
    h_ws->header_flag    = 0;
    h_ws->byte_per_sample =4;
    
    
	DEBUG_PRINT("data server connect end\n"); 
    


	/*-------------------------------------------------------------------------*/
    /* 正常終了 */
	/*-------------------------------------------------------------------------*/
	DEBUG_PRINT("KcHandleOpen end\n");
	return h_ws;



	/*-----------------------------------------------------------------*/
	/* エラー終了                                                      */
	/*-----------------------------------------------------------------*/
 HANDLE_OPEN_ERR4:
    close(h_ws->data_server_info.socket_fd);

 HANDLE_OPEN_ERR3:
    close(h_ws->control_server_info.socket_fd);
 
 HANDLE_OPEN_ERR2:
    free(h_ws);
    
 HANDLE_OPEN_ERR1:
    
#ifdef WIN32
    /* 終了処理 */
    WSACleanup();
#endif

    return NULL;
}



/*******************************************************************************/
/**                                                                           **/
/**   Public  :                                                               **/
/**                                                                           **/
/**  History : 2011.07.07 新規作成                                            **/
/**                                                                           **/
/**  Description                                                              **/
/**  -----------------------------------------------------------------------  **/
/**  サーバとの接続を終了しAD制御とデータ取得用のハンドルを削除               **/
/**                                                                           **/
/*******************************************************************************/
int KcHandleClose(KC_HANDLE_t *h_ws)
{
	
    char buffer[CMD_STRING_LENGTH];

	DEBUG_PRINT("KcHandleClose START\n"); 	
	
    if(h_ws == NULL)
    {
        return 0;
    }
    
    /* サーバに終了を通知 */
    strcpy(buffer, "900");//終了通知
	DEBUG_PRINT("■RECV: connection end request [900]\n");

    /* 文字列（コマンド）を返信 */
    write(h_ws->control_server_info.socket_fd, buffer, strlen(buffer)+1);

    /* ステータス受信 */
    read(h_ws->control_server_info.socket_fd, buffer, CMD_STRING_LENGTH);
	DEBUG_PRINT("■RECV: connection end response [901]\n");

	DEBUG_PRINT("control socket close\n");    
    /* AD制御用ソケットを閉じる */
    close(h_ws->control_server_info.socket_fd);
    
	DEBUG_PRINT("data socket close\n");    
    /* データ取得用ソケットを閉じる */
    close(h_ws->data_server_info.socket_fd);
    
    /* 作業領域を開放 */
	DEBUG_PRINT("work_buffer free\n");    
//    free(h_ws->work_buffer);

    /* ハンドル用のメモリを開放 */
	DEBUG_PRINT("socket free\n");    
    free(h_ws);

#ifdef WIN32
    /* 終了処理 */
    WSACleanup();
#endif


DEBUG_PRINT("KcHandleClose END\n"); 	

    return 0;
}







/*******************************************************************************/
/**                                                                           **/
/**   Public  :                                                               **/
/**                                                                           **/
/**  History : 2011.07.07 新規作成                                            **/
/**                                                                           **/
/**  Description                                                              **/
/**  -----------------------------------------------------------------------  **/
/**  AD開始               **/
/**                                                                           **/
/*******************************************************************************/
int KcADStart(KC_HANDLE_t *h_ws)
{
    int status;
    char buffer[CMD_STRING_LENGTH];

    h_ws->header_count = 0;

	DEBUG_PRINT("KcADStart START\n"); 	


    /* サーバに録音開始要求 */
    strcpy(buffer, "200"); //録音開始
    DEBUG_PRINT("Send : %s\n", buffer);


    /* 文字列（コマンド）を送信 */
    DEBUG_PRINT("■SEND: recording start request [200]\n");
    status=write(h_ws->control_server_info.socket_fd, buffer, strlen(buffer)+1);
	if(status<0)
	{
		return -1;
	}
	
    /* ステータス受信 */

	DEBUG_PRINT("■WAITING: recording start response [201]\n\n");

    status=read(h_ws->control_server_info.socket_fd, buffer, CMD_STRING_LENGTH);

	if(status<=0)
	{
		return -1;
	}


	DEBUG_PRINT("KcADStart END\n"); 	

    return 0;
}





/*******************************************************************************/
/**                                                                           **/
/**   Public  :                                                               **/
/**                                                                           **/
/**  History : 2011.07.07 新規作成                                            **/
/**                                                                           **/
/**  Description                                                              **/
/**  -----------------------------------------------------------------------  **/
/**  AD停止                                                                   **/
/**                                                                           **/
/*******************************************************************************/
int KcADStop(KC_HANDLE_t *h_ws)


{
    int status;
//    int last_status;
    
    int  io_stat;
    char buffer[CMD_STRING_LENGTH];

#ifdef WIN32
    u_long rest_size;
#else
    int  rest_size;
#endif
    int  read_size;
    char dummy[1024];
    

DEBUG_PRINT("KcADStop START\n"); 	

    /* サーバに録音停止要求 */
    strcpy(buffer, "300"); //録音停止要求

    /* 文字列（コマンド）を送信 */
    DEBUG_PRINT("■SEND: recording stop request [300]\n");
    status=write(h_ws->control_server_info.socket_fd, buffer, strlen(buffer)+1);
	if(status<0)
	{
		return -1;
	}
	
    /* ステータス受信 */
	DEBUG_PRINT("■WAITING: recording stop response [301]\n");

    status=read(h_ws->control_server_info.socket_fd, buffer, CMD_STRING_LENGTH);

	if(status<=0)
	{
		return -1;
	}


    /* データ破棄 */
    io_stat = ioctl(h_ws->data_server_info.socket_fd, FIONREAD, &rest_size);
    while((io_stat != -1) && (rest_size > 0))
    {
        read_size = read(h_ws->data_server_info.socket_fd, dummy, 1024);
        
        if(read_size < 0)
        {
            break;
        }
        
        io_stat = ioctl(h_ws->data_server_info.socket_fd, FIONREAD, &rest_size);
    }

	DEBUG_PRINT("KcADStop END\n");	
    
    return 0;
}




/*----------------------------------------------------------*/
/*                     データ取得関係                       */
/*----------------------------------------------------------*/


/*******************************************************************************/
/**                                                                           **/
/**   Public  :                                                               **/
/**                                                                           **/
/**  History : 2011.07.07 新規作成                                            **/
/**                                                                           **/
/**  Description                                                              **/
/**  -----------------------------------------------------------------------  **/
/**  データ取得を実行するread()のラッパ関数               **/
/**                                                                           **/
/*******************************************************************************/
int KcDataRead(KC_HANDLE_t *h_ws, char *buffer, size_t read_req_size)
{
    int read_size;

    /* サーバーからデータを取得 */
    read_size = read(h_ws->data_server_info.socket_fd, buffer, read_req_size);
    
    return read_size;
}






/*******************************************************************************/
/**                                                                           **/
/**   Public  :                                                               **/
/**                                                                           **/
/**  History : 2011.07.07 新規作成                                            **/
/**                                                                           **/
/**  Description                                                              **/
/**  -----------------------------------------------------------------------  **/
/**  データ取得を実行するread()のラッパ関数                                     **/
/**    1サンプルデータの境界になるように調整する機能付き                         **/
/**                                                                           **/
/**  ソケットオプションのSO_RCVLOWATはselect/pollでは機能しないが                **/
/**  readでは機能する                                                          **/
/**  AD開始                                                                   **/
/**                                                                           **/
/*******************************************************************************/
int KcSampleDataRead(KC_HANDLE_t *h_ws, char *buffer, size_t read_req_size)
{
    unsigned int tmp;
    unsigned int read_size;
    unsigned int total_read_size;

#ifdef WIN32
    u_long readable_size;
#else
    unsigned int readable_size;
#endif

    unsigned int user_req_size;
    unsigned int byte_per_sample;

    int    io_stat;
    fd_set rfds;
    struct timeval timeout;



	DEBUG_PRINT("KcSampleDataRead start [%d]\n",clock());



    /* 1sampleのデータサイズ以下の要求はエラー */
    byte_per_sample = h_ws->byte_per_sample;
    if(read_req_size < byte_per_sample)
    {
        return -1;
    }


    /* 要求量を1sampleの倍数に調整 */
    user_req_size = read_req_size - (read_req_size % byte_per_sample);


    DEBUG_PRINT("byte_per_sample = %d, user_req_size = %d\n", byte_per_sample, user_req_size);


    total_read_size = 0;
    do
    {

usleep(30*1000);

        /* read可能か確認 */
		DEBUG_PRINT("read check[%d]\n",h_ws->data_server_info.socket_fd);
        do
        {
			timeout.tv_sec  = 1;
			timeout.tv_usec = 0;
		        
			FD_ZERO(&rfds);
			FD_SET(h_ws->data_server_info.socket_fd, &rfds);
			select(h_ws->data_server_info.socket_fd+1, &rfds, NULL, NULL, &timeout);

        }while(FD_ISSET(h_ws->data_server_info.socket_fd, &rfds) == 0);

        /* バッファリングされているデータ量を確認 */
        /* ioctlのFIONREADを使って、受信しているデータが有るか調べる */
        io_stat = ioctl(h_ws->data_server_info.socket_fd, FIONREAD, &readable_size);
        if(io_stat < 0)  
        {
            return io_stat;
        }


        tmp = total_read_size + readable_size;

        /* バッファから読み出すサイズを1sampleの倍数に調整 */
        if(tmp < byte_per_sample)
        {
            /* 1sampleに満たない場合はあるだけ読み出す */
           read_req_size = readable_size;

        }
        else
        {
            if(tmp >= user_req_size)
            {
                /* ユーザ要求より多く読み出し可能なら、要求量まで読み出す */
                read_req_size = user_req_size - total_read_size;
            }
            else
            {
                /* 1sampleの倍数になるように読み出し量を調整 */
                tmp = tmp - (tmp % byte_per_sample);
                read_req_size = tmp - total_read_size;
            }
        }


        do
        {
            /* サーバーからデータを取得 */
            read_size = read(h_ws->data_server_info.socket_fd, &buffer[total_read_size], read_req_size);

			DEBUG_PRINT("netapp: read_size[%d]\n",read_size);

            /* エラー判定 */
            if(read_size <= 0)
            {
                return read_size;
            }

            read_req_size   -= read_size;
            total_read_size += read_size;
            
        }while(read_req_size > 0);
        
    }while(total_read_size < byte_per_sample);

	DEBUG_PRINT("KcSampleDataRead end [%d]\n",clock());
    return total_read_size;
}



/* EOF */
