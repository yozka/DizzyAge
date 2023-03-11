//////////////////////////////////////////////////////////////////////////////////////////////////
// WUIResource.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __WUIRESOURCE_H__
#define __WUIRESOURCE_H__

#include "E9Array.h"
#include "E9Hash.h"
#include "E9String.h"
///////////////////////////////////////////////////////////////////////////////////////////////////
// cRes - Base resource class
// Use destructor to destroy content
///////////////////////////////////////////////////////////////////////////////////////////////////
class cRes
{
public:
						cRes			()				{ m_name = NULL; m_refcnt = 0; m_group = 0; m_flags = 0; }
	virtual			   ~cRes			()				{ if(m_name) sfree(m_name); }

	inline	char*		GetName			()				{ return m_name; }
	inline	void		SetName			( char* name )	{ if(m_name) sfree(m_name); if(name) m_name=sstrdup(name); }

	inline	void		IncRef			()				{ m_refcnt++; }
	inline	void		DecRef			()				{ m_refcnt--; }
	inline	int			GetRef()						{ return m_refcnt; }
	inline	void		SetRef			( int refcnt )	{ m_refcnt = refcnt; }

	inline	int			GetGroup()						{ return m_group; }
	inline	void		SetGroup		( int group )	{ m_group = group; }

	inline	dword		GetFlags		()				{ return m_flags; }
	inline	void		SetFlags		( dword flags )	{ m_flags = flags; }
	inline	BOOL		TestFlags		( dword flags )	{ return GetFlags() & flags; }
	inline	void		AddFlags		( dword flags )	{ SetFlags( GetFlags() | flags ); }
	inline	void		SubFlags		( dword flags )	{ SetFlags( GetFlags() & (~flags) ); }

protected:
	char*				m_name;							// resource name
	int					m_refcnt;						// reference counter
	int					m_group;						// resource group
	dword				m_flags;						// general flags
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// Defines
//////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_CURSORS					4
#define	CURSOR_ARROW				0
#define	CURSOR_HAND					1
#define	CURSOR_FINGER				2
#define	CURSOR_WAIT					3
#define CURSOR_CROSS				4 // not used
#define	CURSOR_HELP					5 // not used
#define	CURSOR_NO					6 // not used
#define	CURSOR_APPSTARTING			7 // not used

//////////////////////////////////////////////////////////////////////////////////////////////////
// class cResBitmap - hbitmap - "file" or "#id"
//////////////////////////////////////////////////////////////////////////////////////////////////
class cResBitmap : public cRes
{
public:
						cResBitmap();
	virtual				~cResBitmap();
	virtual BOOL		Create( char* name );

	virtual	dword		GetPixel( int x, int y );
	virtual	dword		SetPixel( int x, int y, dword color );

			HBITMAP		LoadPng( char* name );

	HDC		m_hdc;
	HANDLE	m_handle;
	int		m_width;
	int		m_height;

protected:
	HANDLE	m_handleold;
};


//////////////////////////////////////////////////////////////////////////////////////////////////
// class cResFont - hfont - "face"
//////////////////////////////////////////////////////////////////////////////////////////////////
#define FONTFLAG_BOLD		(1<<0)
#define FONTFLAG_ITALIC		(1<<1)
#define FONTFLAG_UNDERLINE	(1<<2)
#define FONTFLAG_STRIKEOUT	(1<<3)
#define FONTFLAG_QUALITY	(1<<4)

class cResFont : public cRes
{
public:
						cResFont();
	virtual				~cResFont();
	virtual BOOL		Create( char* face, int width, int height, int flags );

	HFONT	m_hfont;
	int		m_width;
	int		m_height;
	int		m_flags;
};

//////////////////////////////////////////////////////////////////////////////////////////////////
// cWUIResource - Interface to resources
//////////////////////////////////////////////////////////////////////////////////////////////////
class cWUIResource
{
public:
						cWUIResource	();
virtual				   ~cWUIResource	();

// init done
virtual	BOOL		Init			();									// Init IResource
virtual	void		Done			();									// Done IResource
// management
virtual	cRes*		New				( char* classname );				// create a new resource by class
virtual	int			Size			()									{ return m_array.Size(); }
virtual	cRes*		Get				( int idx )							{ return m_array.Get(idx); }
virtual int			Find			( char* name );						// find resource index by name
virtual	int			Add				( cRes* res );						// add a new resource
virtual void		Del				( int idx );						// delete resource
virtual void		DelGroup		( int group );						// delete all resources from a group
virtual void		DelAll			();									// delete all resources
virtual BOOL		Unload			( int idx );						// unload resource (dec refcnt and delete if <=0); return TRUE if deleted

// specific
		int				LoadBitmap		( char* name );							// load bitmap with refcounter
		int				CreateFont		( char* face, int width, int height, int flags );	// create font (no use of refcounter)
inline	void			SetCursor		( int idx, HCURSOR hcursor )			{ if(0<=idx && idx<MAX_CURSORS) m_cursor[idx] = hcursor; }

		cResBitmap*		GetBitmap		( int idx );							// get bitmap by index (and check kind)
		cResFont*		GetFont			( int idx );							// get font by index
inline	HCURSOR			GetCursor		( int idx )								{ if(0<=idx && idx<MAX_CURSORS) return m_cursor[idx]; else return NULL; }

protected:
		cPArray<cRes>				m_array;							// resource list
		cHash						m_hash;								// hash for resources (name,idx)

private:
		HCURSOR			m_cursor[MAX_CURSORS];									// available cursors

static	cWUIResource*	m_wuiresource;											// global access pointer
friend	inline			cWUIResource*	WUIResource();

};
	
//////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////////////////////////////////
inline	cWUIResource*	WUIResource()	{ return cWUIResource::m_wuiresource; }
		BOOL		InitWUIResource();
		void		DoneWUIResource();

#endif
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
