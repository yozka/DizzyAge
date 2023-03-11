///////////////////////////////////////////////////////////////////////////////////////////////////
// R9Font.h
//
// file format: FNT0 - 040723
//
// size		member		value / meening
//
// HEADER	(pos 0, 24 bytes)
// 4		ID			FNT0
// 2		chrw		character max width
// 2		chrh		character max height (font size)
// 2		ofsx		character offset on x (added between characters) (0)
// 2		ofsy		character offset on y (added between rows) (0)
// 2		texw		texture width (NOT NEEDED)
// 2		texh		texture height (NOT NEEDED)
// 2		scale		scale percentage % (100)
// 2		aspect		aspect percentage % (widths are multiplied by this) (100)
// 2		italic		italic shift width (0)
// 2		empty		---
//
// BODY		( pos 24, size to the end of file, each entry is 6 bytes )
// 1		char		character [0..255]
// 2		x			cel x position on texture
// 2		y			cel y position on texture
// 1		w			character real width
//
// Obs: characters above 128 may have problems because of the char sign if used >> should work!
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __R9FONT_H__
#define __R9FONT_H__

#include "E9System.h"
#include "R9Render.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
struct r9FontChar
{
	word x,y;		// cel pos
	word w;			// cel width
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class r9Font  
{
public:
					r9Font();
virtual				~r9Font();

// load
virtual	BOOL		Create( int chrw, int chrh, int cols=16, int start=32, int count=96 ); // create from a table with given cel size and columns (fixed char size)
virtual	BOOL		Create( char* fontfile );							// create from a .fnt file
virtual void		Destroy();											// destroy font (not the texture!)

// config
inline	void		SetTexture( R9TEXTURE texture )						{ m_tex = texture; } // set a texture for the font
inline	R9TEXTURE	GetTexture()										{ return m_tex; }
inline	void		SetBlend( int blend )								{ m_blend = blend; }
inline	int			GetBlend()											{ return m_blend; }
inline	void		SetColor( dword color )								{ m_color = color; }
inline	void		SetSize( float size )								{ m_scale = size / m_chrh; }
inline	void		SetSpace( int width )								{ m_char[32].w = width; }
inline  BOOL		IsValid( char c )									{ return ( m_char[(byte)c].w>0 ); }

inline	dword		GetColor()											{ return m_color; }
inline	float		GetSize()											{ return m_scale * m_chrh; }
inline	float		GetOfsX()											{ return m_scale * m_aspect * m_ofsx; }
inline	float		GetOfsY()											{ return m_scale * m_ofsy; }
inline	float		GetItalic()											{ return m_scale * m_aspect * m_italic; }

// sizes
inline	float		GetCharWidth()										{ return m_scale * m_aspect * m_chrw; }
		float		GetCharWidth( char c );								// gets the current width of a char (in pixels) - italic not included
		float		GetTextWidth( char* text );							// gets the current width of a string (in pixels) - italic included, newlines ignored
		float		GetTextWidth( char* text, int size );				// gets the current width of a string (in pixels) - italic included, newlines ignored
		void		GetTextBox( char* text, float &w, float& h );		// gets the box sizes the text fits in; italic and newlines included

// draw
		void		Char( float x, float y, char c );					// draw a single char at (x,y)
		void		Print( float x, float y, char* text );				// draw a text at (x,y)
		void		Printn( float x, float y, char* text, int size );	// draw a text at (x,y) (size characters)
		void		Printf( float x, float y, char* format, ... );		// draw a formated text at (x,y)

// utils
		BOOL		Check( char* text, float size, int& len );			// if FALSE len is size of str that fits in size pixels area (space break if possible)

public:

		word		m_chrw;			// character max width
		word		m_chrh;			// character max height (font size)
		short int	m_ofsx;			// character offset on x (added between characters) (0)
		short int	m_ofsy;			// character offset on y (added between rows) (0)
		word		m_texw;			// texture width
		word		m_texh;			// texture height
		float		m_scale;		// general scale [0..1]
		float		m_aspect;		// aspect (widths are multiplied by this) [0..1]
		word		m_italic;		// italic shift width (0)
		dword		m_color;		// color (0xffffffff)
		int			m_blend;		// blend state
		R9TEXTURE	m_tex;			// texture

public:
		r9FontChar	m_char[256];	// chars mapping
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
