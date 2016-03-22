/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#ifndef __HARK_DATA_HEADER__
#define __HARK_DATA_HEADER__

#include "sys/time.h"

// bit flags for data transfer
#define PACKET_MIC_WAVE        0x00000001
#define PACKET_MIC_FFT         0x00000002
#define PACKET_SRC_INFO        0x00000004
#define PACKET_SRC_WAVE        0x00000008
#define PACKET_SRC_FFT         0x00000010
#define PACKET_SRC_FEATURE     0x00000020
#define PACKET_SRC_RELIABILITY 0x00000040
#define PACKET_TEXT            0x00000080
#define PACKET_MATRIX          0x00000100
#define PACKET_VECTOR          0x00000200

// Header for one cycle data
typedef struct tag_HD_Header {
    int type;    // variable for bit flag
    int advance; // shift length
    int count;   // frameID of HARK
    int64_t tv_sec;  // timestamp of HARK in seconds
    int64_t tv_usec; // timestamp of HARK in micro-seconds
} HD_Header;

// Header for microphone signal information
typedef struct tag_HDH_MicData {
    int nch;        // number of microphones
    int length;     // size of STFT window
    int data_bytes; // byte size of wave signal
} HDH_MicData;

// Header for source information
typedef struct tag_HDH_SrcInfo {
    int src_id;    // sound source id
    float x[3];    // position of sound source
    float power;   // power of sound source
} HDH_SrcInfo;

// Header for any signal data 
typedef struct tag_HDH_SrcData {
    int length;     // number of element of this body
    int data_bytes; // byte size of this body
} HDH_SrcData;

#endif
