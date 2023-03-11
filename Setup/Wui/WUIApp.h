//////////////////////////////////////////////////////////////////////////////////////////////////
// WUIApp.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WUIAPP_H__
#define __WUIAPP_H__

#include "WndDlg.h"
#include "WUIResource.h"
#include "WUIScript.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// cWUIApp
//////////////////////////////////////////////////////////////////////////////////////////////////
class cWUIApp
{
public:
					cWUIApp();
virtual			   ~cWUIApp();

// init and main loop
virtual BOOL		Init			( HINSTANCE	hinst, HINSTANCE hinstPrev, LPSTR cmdline, int cmdshow ); // do init the application (managers and stuff)
virtual void		Done			();														// destroy the application
virtual void		Run				();														// app loop that calls mainloop
virtual BOOL		MainLoop		()								{ return TRUE; }			// return FALSE to exit the Run cycle
virtual BOOL		IsActive		()								{ return m_active; }
virtual void		Activate		( bool active, bool minimized )	{ m_active = active; m_minimized = minimized; }

// get
static	char*		GetName			()							{ return m_name;		}
static	char*		GetVersion		()							{ return m_version;		}
static	char*		GetPath			()							{ return m_path;		}
static	HINSTANCE	GetHINSTANCE	()							{ return m_hinst;		}
static	char*		GetCmdLine		()							{ return m_cmdline;		}
static	int			GetCmdShow		()							{ return m_cmdshow;		}
static	HWND		GetHWND			()							{ return m_hwnd;		}
static	HICON		GetIcon			()							{ return m_hicon;		}

// set
static	void		SetName			( char* name )				{ if(name) strcpy( m_name, name ); }
static	void		SetVersion		( char* version )			{ if(version) strcpy( m_version, version ); }
static	void		SetHWND			( HWND hwnd )				{ m_hwnd = hwnd; }
static	void		SetIcon			( HICON hicon )				{ m_hicon = hicon; }
static	void		SetRIB			( BOOL rib )				{ m_rib = rib; }

friend	inline		cWUIApp*		WUIApp();

// register specific script fn
void		RegisterFunc	();

protected:
// members
static	cWUIApp*	m_app;							// global access pointer
static	char		m_name[MAX_PATH];				// app name
static	char		m_version[32];					// app version
static	char		m_path[MAX_PATH];				// path to exe
static	HINSTANCE	m_hinst;						// instance handle
static	HINSTANCE	m_hinst2;						// previous instance handle
static	char*		m_cmdline;						// pointer to command line string
static	int			m_cmdshow;						// window show style
static	HWND		m_hwnd;							// main window handle
static	HICON		m_hicon;						// app icon
static	BOOL		m_active;						// active state
static	BOOL		m_minimized;					// minimized state
static	BOOL		m_inmenuloop;					// menu loop is running (aplication loop is not)
static	BOOL		m_rib;							// application runs in background (when window not active)
static  BOOL		m_processcommand;				// request of ProcessCommand

};


//////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////////////////////////////////
inline	cWUIApp*	WUIApp()	{ return cWUIApp::m_app; }


#endif
//////////////////////////////////////////////////////////////////////////////////////////////////

