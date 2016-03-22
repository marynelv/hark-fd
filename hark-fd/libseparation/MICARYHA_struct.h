// ---------------------------------------------------------------------
/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#ifndef MICARYHA_struct_h_defined
#define MICARYHA_struct_h_defined
#ifndef _WIN32
#include <sys/time.h>
#else
#include <winsock2.h>
#endif


/**
 * \ingroup HA_definitions 
 * \brief ID type for MICARY
 */
typedef int HA_Id;

/**
 * \ingroup HA_definitions 
 * \brief location flag type for MICARY
 */
typedef int HA_Location_Flag;

/**
 * \ingroup HA_definitions 
 * \brief Number type for MICARY
 */
typedef int HA_Num;

/**
 * \ingroup HA_definitions 
 * \brief Integer type for MICARY
 */
typedef int HA_Integer; 

/**
 * \ingroup HA_definitions 
 * \brief Float type for MICARY
 */
typedef float HA_Float; 

/**
 * \ingroup HA_definitions 
 * \brief Char type for MICARY
 */
typedef char HA_Char; 

/**
 * \ingroup HA_definitions 
 * \brief Complex float type for MICARY
 */
typedef struct 
{
    /// real part
    HA_Float re;
    /// imaginary part
    HA_Float im;
} HA_Complex;

/**
 * \ingroup HA_definitions 
 * \brief String type for MICARY
 */
typedef struct 
{
    /// text length
    HA_Num slen;
    /// text; 
    HA_Char *s;
} HA_String;


/** 
 * \ingroup HAF_definitions 
 * \brief Location Struct
 */
typedef struct 
{
    HA_Location_Flag flag;
    /// 位置
    HA_Float x, y, z;     
    /// 確率
    HA_Float px, py, pz;  
} HAF_Location;

/** 
 * \ingroup HAF_definitions 
 * \brief Direction Struct
 */
typedef struct 
{
    HA_Location_Flag flag;
    /// 方向（単位ベクトル）
    HA_Float dx, dy, dz;    
    /// 確率
    HA_Float pdx, pdy, pdz; 
} HAF_Direction;

/** 
 * \ingroup HAF_definitions 
 * \brief Sound Property
 */
typedef struct 
{
    /// pitch, velocity 
    HA_Float pitch, vel; 
    /// probability of pitch and velocity 
    HA_Float ppitch, pvel; 
} HAF_Sound_Property;

#endif//MICARYHA_struct_h_defined

