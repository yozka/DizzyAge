/////////////////////////////////////////////////////////////////////////////////////////////////
// WUIResource.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WUIResource.h"
#include "WUIApp.h"
#include "F9Files.h"
#include "R9ImgLoader.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// class cResBitmap - Image
//////////////////////////////////////////////////////////////////////////////////////////////////
cResBitmap::cResBitmap()	
{
	guard(cResBitmap::cResBitmap)
	m_handle	= NULL; 
	m_hdc		= NULL; 
	m_width		= 0;
	m_height	= 0;
	m_handleold	= NULL;
	unguard()
}

cResBitmap::~cResBitmap()
{
	guard(cResBitmap::~cResBitmap)
	if( m_hdc && m_handleold )	SelectObject( m_hdc, m_handleold );
	if( m_hdc )					DeleteDC(m_hdc);
	if( m_handle )				DeleteObject(m_handle);
	unguard()
}

BOOL cResBitmap::Create( char* name )
{
	guard(cResBitmap::Create)
	if(!name) return FALSE;
	SetName( name );
	SetRef(1);

/*	// find resource
	HRSRC hrsrc;
	hrsrc = FindResourceEx(NULL, "IMAGES", name,  MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL) ); 
	if(hrsrc==NULL) return FALSE;
	dword size = SizeofResource(NULL,hrsrc); sassert(size>0);
	HGLOBAL hglobal = LoadResource(NULL,hrsrc); sassert(hglobal!=NULL);
	char* buffer = (char*)LockResource(hglobal);
*/
	// img
	r9Img img;
	int ret = R9_ImgLoadFile( name, &img ); //file_makename(name,buffer,size), &img );
	if( !ret ) return FALSE;
	if(img.m_width<=0 || img.m_height<=0 || R9_PFBpp(img.m_pf)!=24) { R9_ImgDestroy	(&img); return FALSE; }
	R9_ImgFlipRGB(&img);

	// hdc and hbitmap
	m_hdc = CreateCompatibleDC(NULL);
	if(!m_hdc) return FALSE;
	m_handle = CreateCompatibleBitmap(GetDC(NULL), img.m_width, img.m_height);
	if(!m_handle) { DeleteDC(m_hdc); m_hdc=NULL; return FALSE; }
	m_handleold = SelectObject( m_hdc, m_handle );
	if(m_handleold==NULL) { DeleteDC(m_hdc); m_hdc=NULL; DeleteObject(m_handle); m_handle=NULL; return FALSE; }

	// fill
	for(int i=0;i<img.m_height;i++)
		for(int j=0;j<img.m_width;j++)
			::SetPixel(m_hdc,j,i,0x00ffffff & R9_ImgGetColor(&img,j,i));

	// clear
	R9_ImgDestroy(&img);
	return TRUE;
	unguard()
}

dword cResBitmap::GetPixel( int x, int y )
{
	guard(cResBitmap::GetPixel)
	if( !m_hdc ) return 0;
	if( x<0 || x>m_width || y<0 || y>m_height ) return 0;
	return ::GetPixel( m_hdc, x, y );
	unguard()
}

dword cResBitmap::SetPixel( int x, int y, dword color )
{
	guard(cResBitmap::SetPixel)
	if( !m_hdc ) return 0;
	if( x<0 || x>m_width || y<0 || y>m_height ) return 0;
	return ::SetPixel( m_hdc, x, y, color );
	unguard()
}

HBITMAP cResBitmap::LoadPng( char* name )
{
	guard(cResBitmap::LoadPng)
	
	/*
	int intres = 0;
	if( 1==sscanf( name,"#%d", &intres ) )
		m_handle = LoadImage( App()->GetHINSTANCE(), MAKEINTRESOURCE(intres), IMAGE_BITMAP, 0, 0, 0 );
	else
		m_handle = LoadImage( App()->GetHINSTANCE(), name, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	*/

//	int intres = 0;
//	if( 1==sscanf( name,"#%d", &intres ) )



	return 0;//hbitmap;
	unguard()
}
//////////////////////////////////////////////////////////////////////////////////////////////////
// class cResFont - Font
//////////////////////////////////////////////////////////////////////////////////////////////////
cResFont::cResFont()
{
	guard(cResFont::cResFont)
	m_hfont=NULL;
	m_width=0;
	m_height=0;
	m_flags=0;
	unguard()
}

cResFont::~cResFont()
{
	guard(cResFont::~cResFont)
	if(m_hfont) DeleteObject(m_hfont);
	unguard()
}

BOOL cResFont::Create( char* face, int width, int height, int flags )
{
	guard(cResFont::Create)
	m_width = width;
	m_height = height;
	m_flags = flags;
	m_hfont = CreateFont(	height, width,
							0, 0, 
							(flags & FONTFLAG_BOLD) ? 700 : 0,
							(flags & FONTFLAG_ITALIC),
							(flags & FONTFLAG_UNDERLINE),
							(flags & FONTFLAG_STRIKEOUT),
							DEFAULT_CHARSET,
							0,
							0,
							(flags & FONTFLAG_QUALITY) ? PROOF_QUALITY : DEFAULT_QUALITY,
							0,
							face );
	return (m_hfont!=NULL);
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// WUIResource
//////////////////////////////////////////////////////////////////////////////////////////////////
cWUIResource::cWUIResource()
{
	guard(cWUIResource::cWUIResource)
	sassert(m_wuiresource==NULL);
	m_wuiresource = this;

	for(int i=0;i<MAX_CURSORS;i++) m_cursor[i]=NULL;

	unguard()
}

cWUIResource::~cWUIResource()
{
	guard(cWUIResource::~cWUIResource)
	sassert(m_wuiresource==this);
	m_wuiresource = NULL;
	unguard()
}

BOOL cWUIResource::Init()
{
	guard(cWUIResource::Init)
	m_array.Init(256);
	m_hash.Init(256,1);
	return TRUE;
	unguard()
}

void cWUIResource::Done()
{
	guard(cWUIResource::Done)
	DelAll();
	m_hash.Done();
	m_array.Done();
	unguard()
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Management
///////////////////////////////////////////////////////////////////////////////////////////////////
cRes* cWUIResource::New( char* classname )
{
	guard(cWUIResource::New)
	sassert( classname!=NULL && classname[0]!=0 ); // check

	cRes* res; 
	if(strcmp(classname,"cResBitmap")==0)
	{
		res = (cRes*) snew cResBitmap();
		if(!res) return NULL; 
	}
	if(strcmp(classname,"cResFont")==0)
	{
		res = (cRes*) snew cResFont();
		if(!res) return NULL; 
	}
	// set a default unique name (in case the user do not set it's own name)
	static int rescount = 0; // static counter for name unicity
	char sz[16];
	sprintf(sz,"_RES%04i", rescount);
	res->SetName(sz);
	rescount++;

	return res;
	unguard()
}

int	cWUIResource::Find( char* name )
{
	guard(cWUIResource::Find)
	int idx = -1;
	m_hash.Find(name,idx);
	return idx;
	unguard()
}

int cWUIResource::Add( cRes* res )
{
	guard(cWUIResource::Add)
	sassert(res!=NULL); // check
	sassert(res->GetName()!=NULL && res->GetName()[0]!=0); // check

	int idx = Find(res->GetName());
	if(idx!=-1)
	{
		dlog(LOGAPP, "RESOURCES: trying to add a duplicate resource.\n");
		return -1;
	}

	// add resource
	idx = m_array.Add(res);
	if(idx==-1)
	{
		dlog(LOGAPP, "RESOURCES: unable to add resource.\n");
		return -1;
	}

	// add to hash
	m_hash.Add(res->GetName(),idx);

	return idx;
	unguard()
}

void cWUIResource::Del( int idx )
{
	guard(cWUIResource::Del)
	cRes* res = Get(idx);
	if(res==NULL) return;

	m_hash.Del(res->GetName()); // remove tracker from hash
	m_array.Del(idx); // remove from array (and destroy from destructor)
	unguard()
}

void cWUIResource::DelGroup( int group )
{
	guard(cWUIResource::DelGroup)
	for(int i=0; i<Size(); i++)
		if( Get(i)!=NULL && Get(i)->GetGroup()==group )
			Del( i );
	unguard()
}

void cWUIResource::DelAll()
{
	guard(cWUIResource::DelAll)
	for(int i=0; i<Size(); i++)	Del(i);
	unguard()
}

BOOL cWUIResource::Unload( int idx )
{
	guard(cWUIResource::Unload)
	cRes* res = Get(idx);
	if(res==NULL) return FALSE;
	res->DecRef();
	if(res->GetRef()>0) return FALSE;
	Del(idx);
	return TRUE;
	unguard()
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int cWUIResource::LoadBitmap( char* name )
{
	guard(cWUIResource::LoadBitmap)
	int idx;
	sassert(name);

	// check if already loaded
	if(name!=NULL)
	{
		idx = Find(name);
		if(idx!=-1) 
		{
			Get(idx)->IncRef();
			return idx;
		}
	}	
	
	cResBitmap* res = (cResBitmap*)New("cResBitmap");
	if(!res) return -1;
	if(!res->Create(name)) { sdelete(res); return -1; }
	idx = Add(res);
	if(idx==-1) { sdelete(res);	return -1; }

	return idx;	
	unguard()
}

int	cWUIResource::CreateFont( char* face, int width, int height, int flags )
{
	guard(cWUIResource::CreateFont)
	cResFont* res = (cResFont*)New("cResFont");
	if(!res) return -1;
	if(!res->Create(face, width, height, flags)) { sdelete(res); return -1; }
	int idx = Add(res);
	if(idx==-1) { sdelete(res);	return -1; }
	return idx;	
	unguard()
}



cResBitmap* cWUIResource::GetBitmap( int idx )	
{
	guard(cWUIResource::GetBitmap)
	cRes* res = Get(idx); 
	if(res==NULL) return NULL;
	return (cResBitmap*)res;
	unguard()
}

cResFont* cWUIResource::GetFont( int idx )
{
	guard(cWUIResource::GetFont)
	cRes* res = Get(idx); 
	if(res==NULL) return NULL;
	return (cResFont*)res;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////////////////////////////////
cWUIResource* cWUIResource::m_wuiresource = NULL;

BOOL InitWUIResource()
{
	guard(InitWUIResource)
	dlog(LOGAPP, "INIT WUIResource.\n");
	if(WUIResource()) return TRUE;
	cWUIResource* wuiresource = snew cWUIResource();
	return WUIResource()->Init();
	unguard()
}

void DoneWUIResource()
{
	guard(DoneWUIResource)
	if(WUIResource()) 
	{
		WUIResource()->Done();
		cWUIResource* wuiresource = WUIResource();
		sdelete(wuiresource);
	}
	dlog(LOGAPP, "DONE WUIResource.\n");
	unguard()
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
