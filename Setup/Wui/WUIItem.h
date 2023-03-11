//////////////////////////////////////////////////////////////////////////////////////////////////
// WUIItem.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WUIITEM_H__
#define __WUIITEM_H__

#include "D9Debug.h"
#include "E9System.h"
#include "WUIResource.h"

class cWndDlg;

//////////////////////////////////////////////////////////////////////////////////////////////////
// defines
//////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
#define IITYPE					0
#define	IIINVISIBLE				1
#define IIINTERACTIVE			2
#define	IIRECT					3		// 4
#define	IIPOS					3		// 2
#define	IIX						3
#define	IIY						4
#define IIPOS2					5
#define IIX2					5
#define	IIY2					6
#define IITEXT					7
#define IICOLOR					8		// 4
#define IITOOLTIP				12

#define IITXTRES				13
#define IITXTALIGN				14
#define IITXTSINGLELINE			15

#define IIIMGRES				16
#define IIIMG9					17
#define IIIMGALIGN				18
#define IIIMGBLEND				19
#define IIIMGMAP				20		// 4
#define IIIMGWRAP				24		
#define IIIMGWRAPLEFT			25		
#define IIIMGWRAPTOP			26		
#define IIIMGWRAPRIGHT			27		
#define IIIMGWRAPBOTTOM			28		
#define IIIMGBORDER				29		// 4
#define IIIMGBORDERLEFT			29
#define IIIMGBORDERTOP			30
#define IIIMGBORDERRIGHT		31
#define IIIMGBORDERBOTTOM		32
#define IIIMGSTRETCHMODE		33

#define IIBUTCHECK				34
#define IIBUTPRESSED			35
#define IIBUTCMDTEXT			36

#define IIMAX					64

// Values
#define	IITYPE_RECT				0
#define	IITYPE_COLOR			1
#define	IITYPE_GRADIENT			2
#define	IITYPE_IMG				3
#define	IITYPE_TEXT				4
#define	IITYPE_BUTTON			5

#define IIWRAP_NORMAL			0			// normal paint (with crop) - use align
#define IIWRAP_STRETCH			1			// stretch to fit destination rect
#define IIWRAP_REPEAT			2			// repeat to fit destination rect (crop last)

// align bits 0,1=horizontal, 2,3=vertical; value 0=center, 1=top/left, 2=right/bottom
#define IIALIGN_NONE			(0)			// default (top-left 0,0)
#define IIALIGN_LEFT			(1<<0)
#define IIALIGN_RIGHT			(2<<0)
#define IIALIGN_CENTERX			(3)
#define IIALIGN_TOP				(1<<2)
#define IIALIGN_BOTTOM			(2<<2)
#define IIALIGN_CENTERY			(3<<2)

#define IIBLEND_NORMAL			0			// normal paint (no blend)
#define	IIBLEND_COLORKEY		1			// use colorkey
#define IIBLEND_ALPHA			2			// use alpha channel


//////////////////////////////////////////////////////////////////////////////////////////////////
// class cWUIItem - item base class
//////////////////////////////////////////////////////////////////////////////////////////////////
class cWUIItem
{
public:
						cWUIItem		();
	virtual				~cWUIItem		();

	// access
	virtual	void		SetInt			( int idx, int val );
	virtual	int			GetInt			( int idx );
	virtual	void		SetText			( int idx, char* text );
	virtual	char*		GetText			( int idx );
	virtual	void		SetPoint		( int idx, POINT point );
	virtual	POINT		GetPoint		( int idx );
	virtual	void		SetRect			( int idx, RECT rect );
	virtual	RECT		GetRect			( int idx );

	virtual	BOOL		Build			();
	virtual void		Update			( float dtime );
	virtual void		Draw			();
	virtual void		Notify			( int msg, int p1=0, int p2=0 );

			void		DrawRect		();
			void		DrawColor		();
			void		DrawGradient	();
			void		DrawImg			();
			void		DrawImg9		();
			void		DrawText		();
			void		DrawButton		();
	
			int			m_idx;			// index in dialog
			cWndDlg*	m_dlg;			// parent dialog
			int			m_var[IIMAX];	// variable zone
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////////////////////////////////
cWUIItem*	ItemNew						();									// create an item instance 
cWUIItem*	ItemFind					( HWND hwnd );						// search for an item with a specified HWND

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
