#ifndef mfcnet_h
#define mfcnet_h

//! julius hark plugin mfcnetプロトコル用構造体
struct SourceInfo {
  int     source_id;    //!< 音源ID
  float   azimuth;      //!< 水平方向
  float   elevation;    //!< 垂直方向
  int64_t tv_sec;       //!< 紀元(1700年1月1日0:00)からの経過時間(秒)
  int64_t tv_usec;      //!< 紀元(1700年1月1日0:00)からの経過時間(マイクロ秒)
} __attribute__ ((packed));

#endif

