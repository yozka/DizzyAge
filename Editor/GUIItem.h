//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIItem.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GUIITEM_H__
#define __GUIITEM_H__

#include "GUIUtil.h"

class cGUIDlg;

//////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////////////////////////////

#define IV_ID					0
#define IV_STYLE				1
#define IV_HIDDEN				2
#define IV_DISABLE				3
#define IV_RECT					4		// 4 
#define IV_POS					4		// 2
#define IV_X					4
#define IV_Y					5
#define IV_POS2					6		// 2
#define IV_X2					6
#define	IV_Y2					7
#define IV_TXT					8		// str
#define IV_TXTALIGN				9
#define IV_TXTCOLOR				10		
#define IV_TXTOFFSET			11
#define IV_COLOR				12		// 4
#define IV_IMG					16		// 2 img
#define IV_IMGCOLOR				18		
#define IV_IMGALIGN				19
#define IV_MODE					20

#define IV_CMDACTION			21		// str
#define IV_CMDACTIONPARAM		22

#define IV_VALUE				23
#define IV_GROUP				24
#define IV_TOOLTIP				25		// str

#define IV_USER					48

#define IV_MAX					64

struct tGuiVar
{
	tGuiVar()	{ m_int=0; m_str=NULL; }
	union
	{
		int		m_int;
		char*	m_str;
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUIItem
// Obs:
// 1. uses COLOR1=backgr, COLOR2=gradient, COLOR3=border
//////////////////////////////////////////////////////////////////////////////////////////////////

class cGUIItem
{
public:

						cGUIItem			();
virtual					~cGUIItem			();

virtual	void			Build				();					// build
virtual	void			Update				();					// update 
virtual	void			Draw				();					// draw 

		// access
virtual	void			SetInt				( int idx, int val );
virtual	int				GetInt				( int idx );
virtual	void			SetTxt				( int idx, char* text );
virtual	char*			GetTxt				( int idx );
virtual	void			SetPoint			( int idx, POINT point );
virtual	POINT			GetPoint			( int idx );
virtual	void			SetRect				( int idx, RECT rect );
virtual	RECT			GetRect				( int idx );

		// util
		void			GetScrRect			( RECT &rc );
inline	void			GetCliRect			( RECT &rc )		{ rc.left = 0; rc.top = 0; rc.right = GetInt(IV_X2) - GetInt(IV_X); rc.bottom = GetInt(IV_Y2) - GetInt(IV_Y); }
		int				SetParent			( cGUIDlg* dlg );	// calls dlg->ItemAdd

		void			Capture				(BOOL on);
		BOOL			IsCaptured			();
virtual	void			Action				();					// action
		void			Select				();					// select item & dlg as last

		// data
		tGuiVar			m_var[IV_MAX];							// variable zone
		cGUIDlg*		m_dlg;									// parent
		BOOL			m_mousein;
};


//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUITitle
// uses IV_VALUE for movable
//////////////////////////////////////////////////////////////////////////////////////////////////
class cGUITitle : public cGUIItem
{
public:

						cGUITitle			();
virtual					~cGUITitle			();

virtual	void			Update				();
		int				m_movex;
		int				m_movey;
};


#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
