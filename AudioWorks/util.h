//////////////////////////////////////////////////////////////////////////////////////////////////
// Util.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __UTIL_H__
#define __UTIL_H__

//#include "E9System.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// defines
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef sassert
#define sassert(e)		
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
// Log
//////////////////////////////////////////////////////////////////////////////////////////////////
void	Log( HWND hwnd, char* text );

//////////////////////////////////////////////////////////////////////////////////////////////////
// Browse
//////////////////////////////////////////////////////////////////////////////////////////////////
#define YM_FILTER		0
#define YMFILTER		"ym files\0*.ym\0\0"

int		OnBrowseFile( char *title, int filter, char *path );
int		OnBrowseDir( char *title, char *path );
BOOL	OnBrowseColor( dword* color, BOOL extended );

//////////////////////////////////////////////////////////////////////////////////////////////////
// YM file
//////////////////////////////////////////////////////////////////////////////////////////////////
inline dword	read_dword( byte* p ){ return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3]; }
inline word		read_word( byte* p ) { return (p[0]<<8)|p[1]; }
inline void		write_dword( byte* p, dword	val )	{ p[0] = (val>>24)&0xff; p[1]=(val>>16)&0xff; p[2]=(val>>8)&0xff; p[3]=val&0xff; }
inline void		write_word( byte* p, word val )		{ p[2]=(val>>8)&0xff; p[3]=val&0xff;  }

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#pragma pack(push)
#pragma pack(1)
#else
warning: be sure to the following struct is 1 byte
aligned and exactly 22 bytes long. ( FILE header )
#endif	// ifdef _WIN32

typedef struct
{
	byte	size;
	byte	sum;
	char	id[5];
	dword	packed;
	dword	original;
	byte	reserved[5];
	byte	level;
	byte	name_lenght;
} lzhHeader_t;

#ifdef _WIN32
#pragma pack(pop)
#endif	// ifdef _WIN32

#ifdef __cplusplus
}
#endif

int		LZHUnpack( byte* src, int srcsize, byte* &dst, int &dstsize );
BOOL	LoadYM( char* file, byte* &data, int &datasize );

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
