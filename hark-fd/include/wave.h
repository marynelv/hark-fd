/*
 *  wave.h
 */

#ifndef WAVE_H
#define WAVE_H

// 2006/10/25 added by N.Tanaka
#include <stdio.h>

#define RIFF 0
#define WAVE 1
#define WAVE_DATA 2
#define WAVE_FMT  3
#define ERR  3
#define TAGLENGTH 4


#define FMT_PCM  1
#define FMT_MONO  1
#define FMT_STEREO  2

#define BIT16 16
#define BIT8 8

#define MONO 1
#define STEREO 2
#define SYSTEM64CH (64) /* 2005/10/19 Added by N.Tanaka 64chSystem*/

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

struct waveheader{
  int fs;
  int ch;
  int bytes;
  int alien;
  int bits;
  unsigned int length;
};

struct divwave
{
    struct waveheader wh;
    int buflen;         /* read buffer size */
    unsigned char *buf; /* read buffer */
    int **data;         /* arranged data */
};


struct windowread
{
    int win;    /* window size   */ 
    int winofs; /* window offset */
    int len;    /* data length   */
};


// 2007/07/05 added by N.Tanaka C++‚©‚çC‚ğŒÄ‚Ño‚·
#ifdef __cplusplus
extern "C" {
#endif

/* prototype declaration */
unsigned int readDataInt(FILE *);
unsigned int readDataShort(FILE *);
unsigned int transInt(unsigned char *);
unsigned int transShort(unsigned char *);
int readData( FILE *, struct waveheader *, int **, unsigned char *, int);
int readWaveFromSpecifiedFile( char *fname, struct waveheader *wh, 
			       unsigned char **buf );
int readDataInc( FILE *fp, struct waveheader *wh, unsigned char *buf,
                 int bufsize, int offset );
int readWaveHeader(FILE *fp, struct waveheader *wh);
unsigned int readWaveData( FILE *fp,
                           struct divwave *dw,
                           struct windowread *wr);

int writeWaveSpecifiedFile( char *fname, struct waveheader *wh, 
			    unsigned char *data );
int writeWaveFile( FILE *fp, struct waveheader *wh, int **data );
int writeShortWaveFile( FILE *fp, struct waveheader *wh, short *data );
int writeDoubleWaveFile( FILE *fp, struct waveheader *wh, double *data );
int writeWaveHeader( FILE *fp, struct waveheader *wh );

void printWaveHeader( struct waveheader *wh );
int mallocBuf(unsigned char **buf, int buflen);
int mallocData(int ***data, struct waveheader *wh, int len );
void freeData( int **odata, int ch );

/* data -> wave format */
void transIntData( int in, unsigned char *out );
void transUIntData( unsigned int in, unsigned char *out );
void transShortData( int in, unsigned char *out );
void transShort2SaveData( short in, unsigned char *out );
int intdata2rawdata( struct waveheader *wh, int **data, int index1, int index2, unsigned char **bufo, struct waveheader *who );

/* wave format -> data */
unsigned int transInt(unsigned char *data);
unsigned int transShort(unsigned char *data);
int transSignedShort(unsigned char *data);
int rawdata2intdata( struct waveheader *wh, unsigned char *buf, int ***data);

#ifdef __cplusplus
}
#endif

#endif

