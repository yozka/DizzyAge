//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIDlg.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GUIDLG_H__
#define __GUIDLG_H__

#include "GUIItem.h"
#include "GUI.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////////////////////////////
#define DV_ID					0
#define DV_HIDDEN				1
#define DV_DISABLE				2
#define DV_RECT					3		// 4
#define DV_POS					3		// 2
#define DV_X					3
#define DV_Y					4
#define DV_POS2					5		// 2
#define DV_X2					5
#define	DV_Y2					6
#define DV_MODAL				7
#define DV_TESTKEY				8		// values: 0=none, 1=always, 2=mousein	
#define DV_CLOSEOUT				9		// close when click outside dlg (ret=-1)
#define DV_CLOSERET				10		// close returned value
#define DV_CLOSECMD				11		// close command str

#define DV_USER					16

#define DV_MAX					24

struct  tDlgKey
{
	tDlgKey()	{};
	~tDlgKey()	{ if(m_cmd) sfree(m_cmd); }
	int		m_key;
	byte	m_flags;			// bit 0 = shift, bit 1 = ctrl, bit 2 = alt etc
	char*	m_cmd;				// action to be done on key
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUIDlg
//////////////////////////////////////////////////////////////////////////////////////////////////

class cGUIDlg
{
public:

						cGUIDlg				();
virtual					~cGUIDlg			();

virtual	void			Update				();					// update 
virtual	void			Draw				();					// draw 
virtual	void			Close				( int ret );

		// access
virtual	void			SetInt				( int idx, int val );
virtual	int				GetInt				( int idx );
virtual	void			SetTxt				( int idx, char* text );
virtual	char*			GetTxt				( int idx );
virtual	void			SetPoint			( int idx, POINT point );
virtual	POINT			GetPoint			( int idx );
virtual	void			SetRect				( int idx, RECT rect );
virtual	RECT			GetRect				( int idx );

		// inheritance
inline	int				ItemCount			()					{ return m_item.Size(); }
inline	cGUIItem*		ItemGet				(int idx)			{ if(0<=idx && idx<ItemCount()) return m_item.Get(idx); else return NULL; }
inline	int				ItemAdd				( cGUIItem* item )	{ if(item) { item->m_dlg=this; return m_item.Add(item); } else return -1; }
inline	void			ItemDel				( int idx )			{ if(0<=idx && idx<ItemCount()) m_item.Del(idx); }
virtual	int				ItemFind			( int id );		
virtual	int				ItemFind			( cGUIItem* item );

		// data
		tGuiVar				m_var[DV_MAX];						// variable zone
		cPList<cGUIItem>	m_item;								// cGUIItem list
		BOOL				m_mustclose;						// closing marker
		BOOL				m_mousein;

		cPList<tDlgKey>		m_keys;										// key list
		void				AddKey				( int key, byte flags, char* cmd );
		void				TestKey				();
					
};

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
