///////////////////////////////////////////////////////////////////////////////////////////////////
// R9Img.h
// Image library
// Interface:
// R9_PF_A, R9_PF_RGB, R9_PF_BGR, R9_PF_ARGB, R9_PF_ABGR
// r9PFInfo, 
// R9_PFBpp, R9_PFSpp, ...
// r9Img
// R9_ImgIsValid, R9_ImgClear, R9_ImgLineSize, R9_ImgCreate, R9_ImgDestroy
// R9_ImgDuplicate, R9_ImgFlipV, ...
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __R9IMG_H__
#define __R9IMG_H__

#include "E9System.h"
#include "D9Debug.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// PIXEL FORMAT
///////////////////////////////////////////////////////////////////////////////////////////////////

// pixel formats
#define R9_PF_UNKNOWN		0	// invalid
#define R9_PF_A				1	// alpha (greyscale) 8 bits
#define R9_PF_RGB			2	// R8G8B8
#define R9_PF_BGR			3	// B8G8R8
#define R9_PF_ARGB			4	// A8R8G8B8
#define R9_PF_ABGR			5	// A8B8G8R8
#define R9_PF_COUNT			6	// dummy

struct r9PFInfo
{
	int		m_bpp;				// bits per pixel
	int		m_spp;				// size per pixel (in bytes)
	int		m_channelbpp[4];	// bits per channel (0=b,1=g,2=r,3=a)
	dword	m_channelmask[4];	// dword channel mask (0=b,1=g,2=r,3=a)
	int		m_channelshift[4];	// right shift to obtain channel (0=b,1=g,2=r,3=a)
	char	m_name[16];			// printable name
};

extern r9PFInfo r9_pfinfo[R9_PF_COUNT];

inline	int		R9_PFBpp( int pf )											{ return r9_pfinfo[pf].m_bpp; }
inline	int		R9_PFSpp( int pf )											{ return r9_pfinfo[pf].m_spp; }
inline	char*	R9_PFName( int pf )											{ return r9_pfinfo[pf].m_name; }
inline	dword	R9_PFGetDWORD( void* src, int spp );						// gets a dword packed color from a pixel address with given pixel size; obs: the dword is not necessarily 0xaarrggbb
inline	void	R9_PFSetDWORD( void* src, int spp, dword color );			// sets a dword packed color in a pixel address with given pixel size; obs: the dword is not necessarily 0xaarrggbb
inline	void	R9_PFGetARGB( void* src, r9PFInfo* pfinfo, float* argb );	// reads a pixel in float argb values [0,1]
inline	void	R9_PFSetARGB( void* dst, r9PFInfo* pfinfo, float* argb );	// writes a pixel from float argb values [0,1]

///////////////////////////////////////////////////////////////////////////////////////////////////
// IMAGE
///////////////////////////////////////////////////////////////////////////////////////////////////
// images types
#define	R9_IMG_UNKNOWN			0
#define	R9_IMG_TGA				1
#define	R9_IMG_JPG				2
#define	R9_IMG_BMP				3
#define	R9_IMG_PNG				4

struct r9Img
{
					r9Img();			// initialize with 0
	int				m_width;		// w
	int				m_height;		// h
	int				m_pf;			// pixel format
	dword			m_size;			// size of the data buffer 
	byte*			m_data;			// pixel data
};

inline	BOOL	R9_ImgIsValid		( r9Img* img )									{ return (img->m_width>0 && img->m_height>0 && img->m_pf!=R9_PF_UNKNOWN && img->m_size>0 && img->m_data!=NULL ); }
inline	void	R9_ImgClear			( r9Img* img )									{ memset( img, 0, sizeof(img) ); }
inline	dword	R9_ImgLineSize		( r9Img* img )									{ return img->m_size / img->m_height; }

		BOOL	R9_ImgCreate		( r9Img* img, BOOL clear=FALSE );				// from m_width, m_height and m_pf
		void	R9_ImgDestroy		( r9Img* img );									// free m_data and clear
		BOOL	R9_ImgDuplicate		( r9Img* src, r9Img* dst );						// make a duplicate image
		BOOL	R9_ImgFlipV			( r9Img* img );									// vertical flip 
		BOOL	R9_ImgScale			( r9Img* src, r9Img* dst );						// scale src into dst; dst.width, dst.height required
		BOOL	R9_ImgScale			( r9Img* img, int w, int h );					// scale img
		BOOL	R9_ImgFlipRGB		( r9Img* img );									// switch red and blue channels (rgb to bgr)
		BOOL	R9_ImgCrop			( r9Img* src, int x, int y, r9Img* dst );		// crop from source to dest
		BOOL	R9_ImgCrop			( r9Img* src, int x, int y, int w, int h );		// crop image

		BOOL	R9_ImgConvertPF		( r9Img* src, r9Img* dst, int pf );				// create dst image from src image, by changing the pf

		BOOL	R9_ImgCopy2Mem		( r9Img* img, void* buffer, int pitch );		// copy the image into buffer
		BOOL	R9_ImgCopy			( r9Img* src, r9Img* dst, int x, int y );		// copy the source into the destination; must not exceed dst img

		BOOL	R9_ImgBitBlt		( r9Img* src, int sx, int sy, int sw, int sh, r9Img* dst, int dx, int dy ); // warning: no bound checking! (bpp must match)
		BOOL	R9_ImgBitBltSafe	( r9Img* src, int sx, int sy, int sw, int sh, r9Img* dst, int dx, int dy ); // slow but safe: bound checking! (color (bpp) convert performed too)

		BOOL	R9_ImgWriteBuffer	( r9Img* img, void* buffer, r9PFInfo* pfinfo, int pitch );	// write image into buffer with user pf
		BOOL	R9_ImgReadBuffer	( r9Img* img, void* buffer, r9PFInfo* pfinfo, int pitch );	// read image from buffer with user pf

//		...

inline	dword	R9_ImgGetColor		( r9Img* img, int x, int y );
inline	void	R9_ImgSetColor		( r9Img* img, int x, int y, dword color );
inline	dword	R9_ImgGetColorSafe	( r9Img* img, int x, int y );
inline	void	R9_ImgSetColorSafe	( r9Img* img, int x, int y, dword color );
inline	dword	R9_ImgGetColorRect	( r9Img* img, int x1, int y1, int x2, int y2 );	// used by img_scale; w=x2-x1; bpp must be 3 or 4; img must be valid


///////////////////////////////////////////////////////////////////////////////////////////////////
// PIXEL FORMAT INLINES
///////////////////////////////////////////////////////////////////////////////////////////////////
inline dword R9_PFGetDWORD( void* src, int spp )
{
	guardfast(R9_PFGetDWORD);
	switch(spp)
	{
		case 4:	return *(dword*)(src);
		case 3: return (dword)(*((byte*)src+0)) | (dword)(*((byte*)src+1))<<8 | (dword)(*((byte*)src+2))<<16 | 0xff000000; // use alpha opaque!
		case 2: return (dword)( *(word*)(src) );
		case 1: return (dword)( *(byte*)(src) )<<24; // as alpha
	}
	return 0;
	unguardfast();
}

inline void R9_PFSetDWORD( void* src, int spp, dword color )
{
	guardfast(R9_PFSetDWORD);
	switch(spp)
	{
		case 4:	*(dword*)(src) = color; break;
		case 3: *((byte*)src+0) = *( (byte*)&color+0 ); *((byte*)src+1) = *( (byte*)&color+1 ); *((byte*)src+2) = *( (byte*)&color+2 ); break;
		case 2: *(word*)(src) = *( (word*)&color ); break;
		case 1: *(byte*)(src) = *( (byte*)&color+3 ); break; // as alpha
	}
	unguardfast();
}

inline void R9_PFGetARGB( void* src, r9PFInfo* pfinfo, float* argb )
{
	guardfast(R9_PFGetARGB);
	dword color = R9_PFGetDWORD(src,pfinfo->m_spp);
	argb[0] = (pfinfo->m_channelbpp[0]) ? (float)((color & pfinfo->m_channelmask[0]) >> pfinfo->m_channelshift[0]) / (float)((2<<(pfinfo->m_channelbpp[0]-1))-1) : 0.0f;
	argb[1] = (pfinfo->m_channelbpp[1]) ? (float)((color & pfinfo->m_channelmask[1]) >> pfinfo->m_channelshift[1]) / (float)((2<<(pfinfo->m_channelbpp[1]-1))-1) : 0.0f;
	argb[2] = (pfinfo->m_channelbpp[2]) ? (float)((color & pfinfo->m_channelmask[2]) >> pfinfo->m_channelshift[2]) / (float)((2<<(pfinfo->m_channelbpp[2]-1))-1) : 0.0f;
	argb[3] = (pfinfo->m_channelbpp[3]) ? (float)((color & pfinfo->m_channelmask[3]) >> pfinfo->m_channelshift[3]) / (float)((2<<(pfinfo->m_channelbpp[3]-1))-1) : 0.0f;
	unguardfast();
}

inline void R9_PFSetARGB( void* dst, r9PFInfo* pfinfo, float* argb )
{
	guardfast(R9_PFSetARGB);
	dword color = 0;
	if(pfinfo->m_channelbpp[0])	color |= (dword)( argb[0] * (float)((2<<(pfinfo->m_channelbpp[0]-1))-1) ) << pfinfo->m_channelshift[0];
	if(pfinfo->m_channelbpp[1])	color |= (dword)( argb[1] * (float)((2<<(pfinfo->m_channelbpp[1]-1))-1) ) << pfinfo->m_channelshift[1];
	if(pfinfo->m_channelbpp[2])	color |= (dword)( argb[2] * (float)((2<<(pfinfo->m_channelbpp[2]-1))-1) ) << pfinfo->m_channelshift[2];
	if(pfinfo->m_channelbpp[3])	color |= (dword)( argb[3] * (float)((2<<(pfinfo->m_channelbpp[3]-1))-1) ) << pfinfo->m_channelshift[3];
	R9_PFSetDWORD(dst,pfinfo->m_spp,color);
	unguardfast();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// IMAGE INLINES
///////////////////////////////////////////////////////////////////////////////////////////////////
inline dword R9_ImgGetColor( r9Img* img, int x, int y )
{
	guardfast(R9_ImgGetColor);
	int spp = R9_PFSpp(img->m_pf);
	return R9_PFGetDWORD( img->m_data+(y*img->m_width+x)*spp, spp );
	unguardfast();
}

inline void R9_ImgSetColor( r9Img* img, int x, int y, dword color )
{
	guardfast(R9_ImgSetColor);
	int spp = R9_PFSpp(img->m_pf);
	R9_PFSetDWORD( img->m_data+(y*img->m_width+x)*spp, spp, color );
	unguardfast();
}

inline dword R9_ImgGetColorSafe( r9Img* img, int x, int y )
{
	guardfast(R9_ImgGetColorSafe)
	if( x<0 || x>=(int)img->m_width || y<0 || y>=(int)img->m_height ) return 0;
	int spp = R9_PFSpp(img->m_pf);
	return R9_PFGetDWORD( img->m_data+(y*img->m_width+x)*spp, spp );
	unguardfast()
}

inline void R9_ImgSetColorSafe( r9Img* img, int x, int y, dword color )
{
	guardfast(R9_ImgSetColorSafe)
	if( x<0 || x>=(int)img->m_width || y<0 || y>=(int)img->m_height ) return;
	int spp = R9_PFSpp(img->m_pf);
	R9_PFSetDWORD( img->m_data+(y*img->m_width+x)*spp, spp, color );
	unguardfast()
}

inline dword R9_ImgGetColorRect( r9Img* img, int x1, int y1, int x2, int y2 )
{
	guardfast(R9_ImgGetColorRect)

	int a, r, g, b;
	int spp = R9_PFSpp(img->m_pf);
	
	a = r = g = b = 0;
	int width  = x2-x1;
	int height = y2-y1;
	int adr0 = (y1*img->m_width+x1)*spp;

	for(int y=0; y<height; y++)
	{
		int adr = adr0+y*img->m_width*spp;
		for(int x=0; x<width; x++)
		{
			b += img->m_data[adr+0];
			g += img->m_data[adr+1];
			r += img->m_data[adr+2];
			if(spp==4) a += img->m_data[adr+3];
			adr+=spp;
		}
	}

	int size = width * height;
	a /= size;
	r /= size;
	g /= size;
	b /= size;

	a <<= 24;
	r <<= 16;
	g <<= 8;

	return (a | r | g | b);

	unguardfast();
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
