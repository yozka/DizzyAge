///////////////////////////////////////////////////////////////////////////////////////////////////
// E9Math.h
// math structures and functions
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __E9MATH_H__
#define __E9MATH_H__

#include "E9System.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// defines
///////////////////////////////////////////////////////////////////////////////////////////////////
#define PI							(3.1415926535897932385f)	// pi
#define DBLPI						(6.283185307179586477f)		// pi*2
#define RAD2DEG(r)					((r) * 180.0f / PI)			// convert from radians to degrees
#define DEG2RAD(d)					((d) * PI / 180.0f)			// convert from degrees to radians
#define	SQR(x)						((x)*(x))

#define MIN(a,b)					( ((a)<(b)) ? (a) : (b) )
#define MAX(a,b)					( ((a)>(b)) ? (a) : (b) )
#define BOUND(a,min,max)			( ((a)>(min)) ? ( ((a)<(max)) ? (a) : (max) ) : (min) )
#define	ISZERO(x)					( fabsf(x) < 0.001f )

#define INRECT( x,y,r )				( r.left<=x && x<r.right && r.top<=y && y<r.bottom )
#define INLPRECT( x,y,r )			( r->left<=x && x<r->right && r->top<=y && y<r->bottom )
#define RECT2RECT( r1,r2 )			( !( (r1.x2 <= r2.x1) || (r1.x1 >= r2.x2) || (r1.y2 <= r2.y1) || (r1.y1 >= r2.y2) ) )
#define LRECT2LRECT( r1,r2 )		( !( (r1->x2 <= r2->x1) || (r1->x1 >= r2->x2) || (r1->y2 <= r2->y1) || (r1->y1 >= r2->y2) ) )

#define FCOLOR2DWORD(color)			( ((int)(color.a * 255) << 24) | ((int)(color.r * 255) << 16) | ((int)(color.g * 255) << 8) | (int)(color.b * 255) )
#define DWORD2FCOLOR(color)			( fColor((float)((color >> 16) & 255) / 255.0f, (float)((color >> 8) & 255) / 255.0f, (float)((color) & 255) / 255.0f, (float)((color >> 24) & 255) / 255.0f) )
#define ARGB2DWORD( a, r, g, b )	( (dword)( a<<24 | r<<16 | g<<8 | b ) )
#define RGB2BGR( argb )				( (argb & 0xff00ff00) | ((argb & 0x00ff0000)>>16) | ((argb & 0x000000ff)<<16) )

#define	FLOAT2INT( f )				( ((f)-floor(f)>=0.5f) ? (int)ceil(f) : (int)floor(f); )

inline int GetPow2LO( int value );	// get the nearst power of 2 (but lower than value)
inline int GetPow2HI( int value );	// get the nearst power of 2 (but higher than value)

struct iV2;
struct iRect;

///////////////////////////////////////////////////////////////////////////////////////////////////
// fV2 
// float vector 2
///////////////////////////////////////////////////////////////////////////////////////////////////
struct fV2
{
		union
		{
			struct { float x, y; };
			float v[2];
		};

inline 	fV2()							: x(0.0f), y(0.0f) {}
inline 	fV2( float _x, float _y )		: x(_x), y(_y) {}
inline 	fV2( int _x, int _y )			: x((float)_x), y((float)_y) {}
inline 	fV2( iV2 _v );
inline 	fV2( POINT& point )				: x((float)point.x), y((float)point.y) {}

inline	operator fV2 &()				{ return *this; }
inline	operator float *()				{ return (float*)this; }
inline	operator POINT ()				{ POINT pt={(int)x,(int)y}; return pt; }

};

inline 	BOOL	__fastcall operator==	( const fV2& v1, const fV2& v2 )		{ return v1.x==v2.x && v1.y==v2.y; }
inline 	BOOL	__fastcall operator!=	( const fV2& v1, const fV2& v2 )		{ return v1.x!=v2.x || v1.y!=v2.y; }
inline 	fV2		__fastcall operator*	( float scale, const fV2& v1 )			{ return fV2( v1.x * scale, v1.y * scale ); }
inline 	fV2		__fastcall operator*	( const fV2& v1, float scale )			{ return fV2( v1.x * scale, v1.y * scale ); }
inline 	fV2		__fastcall operator/	( const fV2& v1, float scale )			{ return fV2( v1.x / scale, v1.y / scale ); }
inline 	fV2		__fastcall operator+	( const fV2& v1, const fV2& v2 )		{ return fV2( v1.x + v2.x, v1.y + v2.y ); }
inline 	fV2		__fastcall operator-	( const fV2& v1, const fV2& v2 )		{ return fV2( v1.x - v2.x, v1.y - v2.y ); }
inline 	fV2		__fastcall operator-	( fV2& v1 )								{ return fV2( -v1.x, -v1.y ); }
inline 	fV2		__fastcall operator*=	( fV2& v1, float scale )				{ v1.x *= scale; v1.y *= scale; return v1; }
inline 	fV2		__fastcall operator/=	( fV2& v1, float scale )				{ v1.x /= scale; v1.y /= scale; return v1; }
inline 	fV2		__fastcall operator+=	( fV2& v1, const fV2& v2 )				{ v1.x += v2.x; v1.y += v2.y; return v1; }
inline 	fV2		__fastcall operator-=	( fV2& v1, const fV2& v2 )				{ v1.x -= v2.x; v1.y -= v2.y; return v1; }
inline 	float	__fastcall Dot			( const fV2& v1, const fV2& v2 )		{ return v1.x*v2.x + v1.y*v2.y; }
inline 	float	__fastcall Len			( const fV2 &v1 )						{ return (float)sqrt( v1.x*v1.x + v1.y*v1.y ); }
inline 	float	__fastcall LenSqr		( const fV2 &v1 )						{ return v1.x*v1.x + v1.y*v1.y; }
inline 	BOOL	__fastcall Nor			( fV2 &v1 )								{ float t = Len(v1); if( t==0.0f ) return FALSE; else { v1 *= (1.0f/t); return TRUE; } }
inline 	fV2		__fastcall Unit			( fV2 &v1 )								{ fV2 v = v1; Nor(v); return v; }
inline 	fV2		__fastcall Lerp			( fV2 &v1, fV2 &v2, float s )			{ return v1 + s * (v2 - v1); }
inline 	fV2		__fastcall Rotate		( fV2 &v1, float angsin, float angcos )	{ return  fV2( v1.x * angcos + v1.y * -angsin, v1.x * angsin + v1.y * angcos ); }
inline 	fV2		__fastcall Rotate		( fV2 &v1, float angle )				{ float rad = DEG2RAD(angle); float angsin = sinf(rad); float angcos=cosf(rad); return Rotate(v1,angsin,angcos); }

extern 	fV2		fv2_0;
extern 	fV2		fv2_1;


///////////////////////////////////////////////////////////////////////////////////////////////////
// iV2 
// int vector 2
///////////////////////////////////////////////////////////////////////////////////////////////////
struct iV2
{
		union
		{
			struct { int x, y; };
			int v[2];
		};

inline 	iV2()							: x(0), y(0) {}
inline 	iV2( int _x, int _y )			: x(_x), y(_y) {}
inline 	iV2( float _x, float _y )		: x((int)_x), y((int)_y) {}
inline 	iV2( fV2 _v )					: x((int)_v.x), y((int)_v.y) {}
inline 	iV2( POINT& point )				: x(point.x), y(point.y) {}

inline	operator iV2 &()				{ return *this; }
inline	operator int *()				{ return (int*)this; }
inline	operator POINT ()				{ POINT pt={x,y}; return pt; }

};

inline 	BOOL	__fastcall operator==	( const iV2& v1, const iV2& v2 )		{ return v1.x==v2.x && v1.y==v2.y; }
inline 	BOOL	__fastcall operator!=	( const iV2& v1, const iV2& v2 )		{ return v1.x!=v2.x || v1.y!=v2.y; }
inline 	iV2		__fastcall operator*	( int scale, const iV2& v1 )			{ return iV2( v1.x * scale, v1.y * scale ); }
inline 	iV2		__fastcall operator*	( const iV2& v1, int scale )			{ return iV2( v1.x * scale, v1.y * scale ); }
inline 	iV2		__fastcall operator/	( const iV2& v1, int scale )			{ return iV2( v1.x / scale, v1.y / scale ); }
inline 	iV2		__fastcall operator+	( const iV2& v1, const iV2& v2 )		{ return iV2( v1.x + v2.x, v1.y + v2.y ); }
inline 	iV2		__fastcall operator-	( const iV2& v1, const iV2& v2 )		{ return iV2( v1.x - v2.x, v1.y - v2.y ); }
inline 	iV2		__fastcall operator-	( iV2& v1 )								{ return iV2( -v1.x, -v1.y ); }
inline 	iV2		__fastcall operator*=	( iV2& v1, int scale )					{ v1.x *= scale; v1.y *= scale; return v1; }
inline 	iV2		__fastcall operator/=	( iV2& v1, int scale )					{ v1.x /= scale; v1.y /= scale; return v1; }
inline 	iV2		__fastcall operator+=	( iV2& v1, const iV2& v2 )				{ v1.x += v2.x; v1.y += v2.y; return v1; }
inline 	iV2		__fastcall operator-=	( iV2& v1, const iV2& v2 )				{ v1.x -= v2.x; v1.y -= v2.y; return v1; }

extern 	iV2		iv2_0;
extern 	iV2		iv2_1;


///////////////////////////////////////////////////////////////////////////////////////////////////
// fRect
// float rectangle
///////////////////////////////////////////////////////////////////////////////////////////////////
struct fRect
{
		union
		{
			struct { float x1, y1, x2, y2; };
			struct { float left, top, right, bottom; };
			float v[4];
		};

inline 	fRect()												: x1(0.0f), y1(0.0f), x2(0.0f), y2(0.0f) {}
inline 	fRect( float _x1, float _y1, float _x2, float _y2 )	: x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
inline 	fRect( int _x1, int _y1, int _x2, int _y2 )			: x1((float)_x1), y1((float)_y1), x2((float)_x2), y2((float)_y2) {}
inline 	fRect( iRect& r );
inline 	fRect( RECT& rect )									: x1((float)rect.left), y1((float)rect.top), x2((float)rect.right), y2((float)rect.bottom) {}

inline 	operator fRect &()									{ return *this; }
inline 	operator float *()									{ return (float*)this; }
inline 	operator RECT ()									{ RECT rect={(int)x1,(int)y1,(int)x2,(int)y2}; return rect; }

inline 	float 	Width()										{ return x2-x1; }
inline 	float 	Height()									{ return y2-y1; }
inline 	fV2		Center()									{ return fV2((x1+x2)/2.0f,(y1+y2)/2.0f); }
inline 	void	Inflate( fV2 _v )							{ x1+=_v.x; y1+=_v.y; x2-=_v.x; y2-=_v.y; }
inline 	void	Deflate( fV2 _v )							{ x1-=_v.x; y1-=_v.y; x2+=_v.x; y2+=_v.y; }
inline 	void	Offset( fV2 _v )							{ x1+=_v.x; y1+=_v.y; x2+=_v.x; y2+=_v.y; }
inline 	BOOL	IsInside( fV2 _v )							{ return (x1<=_v.x && _v.x<x2 && y1<=_v.y && _v.y<y2); }

};

inline	BOOL	__fastcall operator==	( const fRect& r1, const fRect& r2 )	{ return (r1.x1==r2.x1 && r1.y1==r2.y1 && r1.x2==r2.x2 && r1.y2==r2.y2); }
inline	BOOL	__fastcall operator!=	( const fRect& r1, const fRect& r2 )	{ return (r1.x1!=r2.x1 || r1.y1!=r2.y1 || r1.x2!=r2.x2 || r1.y2!=r2.y2); }
inline fRect	__fastcall operator+	( const fRect& r1, const fRect& r2 )	{ return fRect( (r1.x1<r2.x1)?r1.x1:r2.x1, (r1.y1<r2.y1)?r1.y1:r2.y1, (r1.x2>r2.x2)?r1.x2:r2.x2, (r1.y2>r2.y2)?r1.y2:r2.y2 ); }
inline fRect	__fastcall operator+=	( fRect& r1, const fRect& r2 )			{ if(r2.x1<r1.x1) r1.x1=r2.x1; if(r2.y1<r1.y1) r1.y1=r2.y1; if(r2.x2>r1.x2) r1.x2=r2.x2; if(r2.y2>r1.y2) r1.y2=r2.y2; return r1; }
inline fRect	__fastcall operator*	( const fRect& r1, const fRect& r2 )	{ return fRect( (r1.x1>r2.x1)?r1.x1:r2.x1, (r1.y1>r2.y1)?r1.y1:r2.y1, (r1.x2<r2.x2)?r1.x2:r2.x2, (r1.y2<r2.y2)?r1.y2:r2.y2 ); }
inline fRect	__fastcall operator*=	( fRect& r1, const fRect& r2 )			{ if(r2.x1>r1.x1) r1.x1=r2.x1; if(r2.y1>r1.y1) r1.y1=r2.y1; if(r2.x2<r1.x2) r1.x2=r2.x2; if(r2.y2<r1.y2) r1.y2=r2.y2; return r1; }

extern fRect	frect_0;
extern fRect	frect_1;
extern fRect	frect_01;


///////////////////////////////////////////////////////////////////////////////////////////////////
// iRect
// int rectangle
///////////////////////////////////////////////////////////////////////////////////////////////////
struct iRect
{
		union
		{
			struct { int x1, y1, x2, y2; };
			struct { int left, top, right, bottom; };
			int v[4];
		};

inline 	iRect()												: x1(0), y1(0), x2(0), y2(0) {}
inline 	iRect( int _x1, int _y1, int _x2, int _y2 )			: x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
inline 	iRect( float _x1, float _y1, float _x2, float _y2 )	: x1((int)_x1), y1((int)_y1), x2((int)_x2), y2((int)_y2) {}
inline 	iRect( fRect& r )									: x1((int)r.x1), y1((int)r.y1), x2((int)r.x2), y2((int)r.y2) {}
inline 	iRect( RECT& rect )									: x1(rect.left), y1(rect.top), x2(rect.right), y2(rect.bottom) {}

inline 	operator iRect &()									{ return *this; }
inline 	operator int *()									{ return (int*)this; }
inline 	operator RECT ()									{ RECT rect={x1,y1,x2,y2}; return rect; }

inline 	int 	Width()										{ return x2-x1; }
inline 	int 	Height()									{ return y2-y1; }
inline 	iV2		Center()									{ return iV2((x1+x2)/2.0f,(y1+y2)/2.0f); }
inline 	void	Inflate( iV2 _v )							{ x1+=_v.x; y1+=_v.y; x2-=_v.x; y2-=_v.y; }
inline 	void	Deflate( iV2 _v )							{ x1-=_v.x; y1-=_v.y; x2+=_v.x; y2+=_v.y; }
inline 	void	Offset( iV2 _v )							{ x1+=_v.x; y1+=_v.y; x2+=_v.x; y2+=_v.y; }
inline 	BOOL	IsInside( iV2 _v )							{ return (x1<=_v.x && _v.x<x2 && y1<=_v.y && _v.y<y2); }

};

inline	BOOL	__fastcall operator==	( const iRect& r1, const iRect& r2 )	{ return (r1.x1==r2.x1 && r1.y1==r2.y1 && r1.x2==r2.x2 && r1.y2==r2.y2); }
inline	BOOL	__fastcall operator!=	( const iRect& r1, const iRect& r2 )	{ return (r1.x1!=r2.x1 || r1.y1!=r2.y1 || r1.x2!=r2.x2 || r1.y2!=r2.y2); }
inline iRect	__fastcall operator+	( const iRect& r1, const iRect& r2 )	{ return iRect( (r1.x1<r2.x1)?r1.x1:r2.x1, (r1.y1<r2.y1)?r1.y1:r2.y1, (r1.x2>r2.x2)?r1.x2:r2.x2, (r1.y2>r2.y2)?r1.y2:r2.y2 ); }
inline iRect	__fastcall operator+=	( iRect& r1, const iRect& r2 )			{ if(r2.x1<r1.x1) r1.x1=r2.x1; if(r2.y1<r1.y1) r1.y1=r2.y1; if(r2.x2>r1.x2) r1.x2=r2.x2; if(r2.y2>r1.y2) r1.y2=r2.y2; return r1; }
inline iRect	__fastcall operator*	( const iRect& r1, const iRect& r2 )	{ return iRect( (r1.x1>r2.x1)?r1.x1:r2.x1, (r1.y1>r2.y1)?r1.y1:r2.y1, (r1.x2<r2.x2)?r1.x2:r2.x2, (r1.y2<r2.y2)?r1.y2:r2.y2 ); }
inline iRect	__fastcall operator*=	( iRect& r1, const iRect& r2 )			{ if(r2.x1>r1.x1) r1.x1=r2.x1; if(r2.y1>r1.y1) r1.y1=r2.y1; if(r2.x2<r1.x2) r1.x2=r2.x2; if(r2.y2<r1.y2) r1.y2=r2.y2; return r1; }

extern iRect	irect_0;
extern iRect	irect_1;
extern iRect	irect_01;


///////////////////////////////////////////////////////////////////////////////////////////////////
// fColor
// float color
///////////////////////////////////////////////////////////////////////////////////////////////////
struct fColor
{
	union
	{
		struct { float b,g,r,a; };
		float v[4];
	};
	
	fColor() : b(0.0f), g(0.0f), r(0.0f), a(0.0f) { }
	fColor( float _a, float _r, float _g, float _b )	: a(_a), r(_r), g(_g), b(_b) { }
	fColor( dword color ) { b=(float)((color & 0x000000ff)) / 255.0f; g=(float)((color & 0x0000ff00)>>8) / 255.0f; r=(float)((color & 0x00ff0000)>>16) / 255.0f; a=(float)((color & 0xff000000)) / 255.0f; }
	
	inline	operator float *()				{ return (float*)this; }
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// inlines
///////////////////////////////////////////////////////////////////////////////////////////////////
inline fV2::fV2( iV2 _v )		: x((float)_v.x), y((float)_v.y) {}
inline fRect::fRect( iRect& r )	: x1((float)r.x1), y1((float)r.y1), x2((float)r.x2), y2((float)r.y2) {}

inline int GetPow2LO( int value )	
{
	for(int i=16; i>=1; i--)
		if( (1<<i) <= value ) return (1<<i);
	return 2;
}

inline int GetPow2HI( int value )
{
	for(int i=1; i<16; i++)
		if( (1<<i) >= value ) return (1<<i);
	return 1<<16; //!
}

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
