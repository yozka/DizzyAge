//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIUtil.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GUIUTIL_H__
#define __GUIUTIL_H__

#include "E9System.h"
#include "E9Math.h"
#include "I9Input.h"
#include "R9Font.h"
#include "R9TexturePool.h"
#include "GUIDef.h"

#define BEEP_OK		MessageBeep(MB_OK);
#define BEEP_ERROR	MessageBeep(MB_ICONHAND);

		void		GUIInitResources	();	// init static gui resources (like the dot texture)
		void		GUIDoneResources	();	// done static gui resources

		dword		GUIColorShift		( dword color, int delta );
inline	void		GUIDrawLine			( int x1, int y1, int x2, int y2, dword color )					{ R9_DrawBar(fRect(x1,y1,x2+(x1==x2),y2+(y1==y2)),color); } // R9_DrawLine( fV2(x1,y1), fV2(x2,y2), color); // line primitive is slower
inline  void		GUIDrawRect			( int x1, int y1, int x2, int y2, dword color )					{ if(x1==x2 || y1==y2) return; GUIDrawLine(x1,y1,x2,y1,color); GUIDrawLine(x1,y2-1,x2,y2-1,color); GUIDrawLine(x1,y1+1,x1,y2-1,color); GUIDrawLine(x2-1,y1+1,x2-1,y2-1,color); }
inline	void		GUIDrawRect3D		( int x1, int y1, int x2, int y2, dword color, BOOL pressed )	{ dword c1 = GUIColorShift(color,pressed?-GUICOLORSHIFT:GUICOLORSHIFT); dword c2 = GUIColorShift(color,pressed?GUICOLORSHIFT:-GUICOLORSHIFT); GUIDrawLine(x1,y1,x2,y1,c1); GUIDrawLine(x1,y2-1,x2,y2-1,c2); GUIDrawLine(x1,y1+1,x1,y2-1,c1); GUIDrawLine(x2-1,y1+1,x2-1,y2-1,c2); }
inline	void		GUIDrawBar			( int x1, int y1, int x2, int y2, dword color )					{ R9_DrawBar( fRect(x1,y1,x2,y2), color); }
		void		GUIDrawGradient		( int x1, int y1, int x2, int y2, dword color1, dword color2 );
		void		GUIDrawBarDot		( int x1, int y1, int x2, int y2, dword color );
		void		GUIDrawRectDot		( int x1, int y1, int x2, int y2, dword color );
		void		GUIDrawText			( int x1, int y1, int x2, int y2, char* text, dword color, int align = GUIALIGN_CENTERX | GUIALIGN_CENTERY, int offset = 0 );
		void		GUIDrawImg			( int x1, int y1, int x2, int y2, int img, dword color = 0xffffffff, int align = GUIALIGN_CENTERX | GUIALIGN_CENTERY );

		BOOL		WinDlgOpenFile		( char* filename, char* ext, int mode ); // filenname must have at least 256 chars; mode 0=open 1=save
		BOOL		WinDlgOpenFolder	( char* foldername ); // foldername muse have at least 256 chars
		BOOL		WinDlgOpenColor		( dword* color, BOOL extended=FALSE );

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
