//////////////////////////////////////////////////////////////////////////////////////////////////
// GUI.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __GUI_H__
#define __GUI_H__

#include "E9System.h"
#include "E9Math.h"
#include "E9List.h"
#include "gs.h"
#include "gslib_file.h"
#include "gslib_win.h"
#include "GUIUtil.h"
#include "GUIDlg.h"


#define	GUIKEY_MB1			0
#define	GUIKEY_MB2			1
#define	GUIKEY_MB3			2
#define	GUIKEY_CTRL			3
#define	GUIKEY_SHIFT		4
#define	GUIKEY_ALT			5
#define	GUIKEY_MAX			16

#define	GV_USER				16
#define	GV_MAX				128

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUI
//////////////////////////////////////////////////////////////////////////////////////////////////
class cGUI
{
public:
						cGUI();
virtual					~cGUI();

		BOOL			Init();							// init
		void			Done();							// done
		void			Update();						// update 
		void			Draw();							// draw 
		
		// access
		void			SetInt( int idx, int val );
		int				GetInt( int idx );
		void			SetTxt( int idx, char* text );
		char*			GetTxt( int idx );
		tGuiVar			m_var[GV_MAX];					// variable zone

		// status bar
inline	char*			GetTooltip()					{ return m_tooltip; }
inline	void			SetTooltip( char* text )		{ if(text) strncpy(m_tooltip,text,255); else m_tooltip[0]=0; m_tooltip[255]=0; }
		char			m_tooltip[256];					// tooltip text

		// input
		int				m_mousex;						// mouse x pos
		int				m_mousey;						// mouse y pos
		int				m_key[GUIKEY_MAX];		
		void			ReadInput();					// read input ( CALL IT BEFORE UPDATE ! )

		// images
		int				ImgLoad( char* image );
		int				ImgFind( char* image );
		r9TexturePool	m_texturepool;					// textures
				
		// inheritance
inline	int				DlgCount()						{ return m_dlg.Size(); }
inline	cGUIDlg*		DlgGet( int idx )				{ if(0<=idx && idx<DlgCount()) return m_dlg.Get(idx); else return NULL; }
inline	int				DlgAdd( cGUIDlg* dlg )			{ if(dlg) return m_dlg.Add(dlg); else return -1; }
inline	void			DlgDel( int idx )				{ if(0<=idx && idx<DlgCount()) m_dlg.Del(idx); }
virtual	int				DlgFind( int id );				// return idx
virtual	int				DlgFind( cGUIDlg* dlg );		// return idx

		// data
		cPList<cGUIDlg>	m_dlg;							// cGUIDlg list
		r9Font*			m_font;

		cGUIItem*		m_capture;						// pointer to capturing item
		BOOL			m_isbusy;						// if gi is busy (there is at least one modal dialog or is in a capture)
				
		// script
		gsVM*			m_vm;
		int				m_lastdlg;						// last (selected) dlg index
		int				m_lastitem;						// last (selected) item index
	
		BOOL			ScriptInit();					// init script
		void			ScriptDone();					// finish script
		void			ScriptDo( char* szcmd );		// do command
		int				ScriptCallback( int fid );		// call a script function by function id
		BOOL			ScriptCompile( char* file );	// compile a file
		void			ScriptRegister();				// register script functions
};

// gui replicators (cGUIItem, cGUIDlg)
void* GUICreateClass( char* classname );

extern	cGUI*			g_gui;
inline	BOOL			GUIInit			()						{ g_gui = snew cGUI(); return g_gui->Init(); }
inline	void			GUIDone			()						{ if(g_gui)	{ g_gui->Done(); sdelete(g_gui); }}	

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
