/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A9Def.h
// obs: sound positons and sizes are in samples unless otherwise noted
// obs: the whole lib mignt fail huge sounds (>1h), because of the integer sizes
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __A9DEF_H__
#define __A9DEF_H__

#include "E9System.h"
#include "E9Engine.h"
#include "D9Debug.h"		// use better debug system
#include "F9Files.h"		// use files system
//#define A9_DEBUG			// no better debug system

#ifdef A9_DEBUG
// data types
typedef unsigned char	byte;		// 8  bit unsigned integer
typedef unsigned short	word;		// 16 bit unsigned integer
typedef unsigned int	dword;		// 32 bit
typedef	int				BOOL;		// 32 bit int

#ifndef TRUE
#define TRUE			1
#endif
#ifndef FALSE
#define FALSE			0
#endif

// debug macros
#define smalloc			malloc
#define srealloc		realloc
#define sfree			free
#define	snew			new
#define	sdelete			delete
#define	srelease(a)		{ if(a!=NULL){ a->Release(); a=NULL; } }
#define guard(fn)
#define unguard()
#define guardfast(fn)
#define unguardfast()
#define sassert(exp)	assert(exp)

#define LOGSND 0
void alog( int ch, const char *fmt, ... );
void alogv( int ch, const char *fmt, va_list args );

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
// ERROR CODES
///////////////////////////////////////////////////////////////////////////////////////////////////
#define A9_OK				0
#define A9_FAIL				-1
#define A9_ERROR			-1
#define A9_LOST				-2
#define A9_UNSUPORTED		-3

///////////////////////////////////////////////////////////////////////////////////////////////////
// SOUND INFO STRUCTURE
///////////////////////////////////////////////////////////////////////////////////////////////////
struct a9Info
{
		int		m_depth;		// depth in bits (8 or 16)
		int		m_signed;		// signed (1 or 0)
		int		m_channels;		// channels (1 or 2)
		int		m_frequency;	// sample rate (like 22050, 41000)
		int		m_size;			// size in samples

inline	int		SampleSize()	{ return m_channels * (m_depth/8); } // in bytes
inline	int		DataSize()		{ return SampleSize() * m_size; }  // in bytes
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
