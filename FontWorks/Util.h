//////////////////////////////////////////////////////////////////////////////////////////////////
// Util.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __UTIL_H__
#define __UTIL_H__

//////////////////////////////////////////////////////////////////////////////////////////////////
// defines
//////////////////////////////////////////////////////////////////////////////////////////////////

typedef unsigned char		byte;		// 8  bit unsigned integer
typedef unsigned short		word;		// 16 bit unsigned integer
typedef unsigned int		dword;		// 32 bit unsigned integer
typedef int					BOOL;		// boolean data type

#ifndef	FALSE
#define FALSE 0
#endif
#ifndef	TRUE
#define TRUE 1
#endif


char*	file_path2ext( char* path );
BOOL	file_exists( char* path );

//////////////////////////////////////////////////////////////////////////////////////////////////
// LOG and PROGRESS
//////////////////////////////////////////////////////////////////////////////////////////////////
#define LOG( text )				Log(g_hwndlog, text)
#define LOGLN( text )			{ Log(g_hwndlog, text); Log(g_hwndlog, "\r\n"); }
#define PROGRESSINIT( range )	ProgressInit(g_hwndprogress, range)
#define PROGRESSSTEP()			ProgressStep(g_hwndprogress);

void Log( HWND hwnd, char* text );
void CenterDialog( HWND hwnd );
void ProgressInit( HWND hwnd, int range );
void ProgressStep( HWND hwnd );

//////////////////////////////////////////////////////////////////////////////////////////////////
// INPUTBOX
// read input text (max 255 chars); ret FALSE if closed
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL InputBox( HINSTANCE hinstance, HWND hwnd, char* title, char* msg, char* content );

//////////////////////////////////////////////////////////////////////////////////////////////////
// FONT
//////////////////////////////////////////////////////////////////////////////////////////////////
struct tFontChr
{
	word x,y;
	word w;
};
struct tFont
{
	word		m_chrw;			// character max width
	word		m_chrh;			// character max height (font size)
	word		m_ofsx;			// character offset on x (added between characters) (0)
	word		m_ofsy;			// character offset on y (added between rows) (0)
	word		m_texw;			// texture width
	word		m_texh;			// texture height
	word		m_scale;		// scale percentage % (100)
	word		m_aspect;		// aspect percentage % (widths are multiplied by this) (100)
	word		m_italic;		// italic shift width (0)
	dword		m_color;		// color (0xffffffff)

	dword		m_charscount;	// chars count (<=256)
	tFontChr	m_chars[256];	// chars mapping
};

BOOL SaveFont( char* filename, tFont& font, int char_count, byte* char_list );

//////////////////////////////////////////////////////////////////////////////////////////////////
// TGA
//////////////////////////////////////////////////////////////////////////////////////////////////
struct tTGA
{
	char IDLength;
	char ColormapType;
	char ImageType;
	char ColormapSpecification[5];
	unsigned short XOrigin;
	unsigned short YOrigin;
	unsigned short width;
	unsigned short height;
	char PixelDepth;
	char ImageDescriptor;
};

BOOL SaveTGA(char* filename, byte* buffer, int w, int h, int bpp);
BOOL LoadTGA(char* filename, byte* &buffer, int &w, int &h, int &bpp);

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
