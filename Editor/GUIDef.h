//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIDef.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GUIDEF_H__
#define __GUIDEF_H__

//////////////////////////////////////////////////////////////////////////////////////////////////
// colors
//////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_GUIITEMS		1024			// some decent limit, nothing else

#define	GUICOLORSHIFT		32
#define	GUICOLOR_GUI		0xffaaaaaa
#define	GUICOLOR_TEXT		0xff000000
#define	GUICOLOR_YELLOW1	0xfffbbe55


// align bits 0,1=horizontal, 2,3=vertical; value 0=center, 1=top/left, 2=right/bottom
#define GUIALIGN_NONE			(0)			// default (top-left 0,0)
#define GUIALIGN_LEFT			(1<<0)
#define GUIALIGN_RIGHT			(2<<0)
#define GUIALIGN_CENTERX		(3)
#define GUIALIGN_TOP			(1<<2)
#define GUIALIGN_BOTTOM			(2<<2)
#define GUIALIGN_CENTERY		(3<<2)
#define GUIALIGN_CENTERXY		(GUIALIGN_CENTERX | GUIALIGN_CENTERY)

#define GUISTYLE_NONE			0
#define GUISTYLE_BACKGR			(1<<0)		// color bar
#define GUISTYLE_GRADIENT		(1<<1)		// color gradient
#define GUISTYLE_BORDER			(1<<2)		// simple border
#define GUISTYLE_BORDER3D		(1<<3)		// 3d border
#define GUISTYLE_PRESSED		(1<<4)		// 3d border status (pressed or released)

#define GUIKEYFLAG_NONE			0
#define GUIKEYFLAG_SHIFT		1
#define GUIKEYFLAG_CTRL			2
#define GUIKEYFLAG_ALT			4
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
