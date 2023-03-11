///////////////////////////////////////////////////////////////////////////////////////////////////
// R9RenderGL.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "R9RenderGL.h"
#include "R9ImgLoader.h"

#ifndef R9_ENABLE_DLLGL
#pragma comment( lib, "opengl32.lib" )
#endif

#define R9_LOGERROR( prefix )	dlog( LOGERR, "RENDER: %s\n", prefix );

r9RenderGL::twglCreateContext		r9RenderGL::m_wglCreateContext		= NULL;
r9RenderGL::twglMakeCurrent			r9RenderGL::m_wglMakeCurrent		= NULL;		
r9RenderGL::twglDeleteContext		r9RenderGL::m_wglDeleteContext		= NULL;
r9RenderGL::tglGetString			r9RenderGL::m_glGetString			= NULL;
r9RenderGL::tglGenTextures			r9RenderGL::m_glGenTextures			= NULL;
r9RenderGL::tglBindTexture			r9RenderGL::m_glBindTexture			= NULL;
r9RenderGL::tglTexImage2D			r9RenderGL::m_glTexImage2D			= NULL;
r9RenderGL::tglDeleteTextures		r9RenderGL::m_glDeleteTextures		= NULL;
r9RenderGL::tglTexParameteri		r9RenderGL::m_glTexParameteri		= NULL;
r9RenderGL::tglBlendFunc			r9RenderGL::m_glBlendFunc			= NULL;
r9RenderGL::tglTexEnvi				r9RenderGL::m_glTexEnvi				= NULL;
r9RenderGL::tglEnable				r9RenderGL::m_glEnable				= NULL;
r9RenderGL::tglShadeModel			r9RenderGL::m_glShadeModel			= NULL;
r9RenderGL::tglViewport				r9RenderGL::m_glViewport			= NULL;		
r9RenderGL::tglMatrixMode			r9RenderGL::m_glMatrixMode			= NULL;
r9RenderGL::tglLoadIdentity			r9RenderGL::m_glLoadIdentity		= NULL;		
r9RenderGL::tglOrtho				r9RenderGL::m_glOrtho				= NULL;
r9RenderGL::tglClearColor			r9RenderGL::m_glClearColor			= NULL;
r9RenderGL::tglClear				r9RenderGL::m_glClear				= NULL;
r9RenderGL::tglEnableClientState	r9RenderGL::m_glEnableClientState	= NULL;
r9RenderGL::tglColorPointer			r9RenderGL::m_glColorPointer		= NULL;		
r9RenderGL::tglVertexPointer		r9RenderGL::m_glVertexPointer		= NULL;
r9RenderGL::tglTexCoordPointer		r9RenderGL::m_glTexCoordPointer		= NULL;
r9RenderGL::tglDrawArrays			r9RenderGL::m_glDrawArrays			= NULL;
r9RenderGL::tglDisableClientState	r9RenderGL::m_glDisableClientState	= NULL;
r9RenderGL::tglPixelStorei			r9RenderGL::m_glPixelStorei			= NULL;
r9RenderGL::tglReadPixels			r9RenderGL::m_glReadPixels			= NULL;
r9RenderGL::tglCopyTexImage2D		r9RenderGL::m_glCopyTexImage2D		= NULL;
r9RenderGL::tglGetTexImage			r9RenderGL::m_glGetTexImage			= NULL;

r9RenderGL::r9RenderGL()
{
	guard(r9RenderGL::r9RenderGL);
	m_api			= R9_API_OPENGL;
	m_hdc			= NULL;
	m_hrc			= NULL;
	m_batchcount	= 0;
	m_batchbuffer	= NULL;
	m_caps.m_texture_env_combine	= FALSE;
	m_textarget		= NULL;
	unguard();
}

r9RenderGL::~r9RenderGL()
{
	guard(r9RenderGL::~r9RenderGL);
	unguard();
}


#define _GETDLLPROC( name )	m_##name = (t##name)GetProcAddress(m_dll,#name); if(!m_##name) { R9_LOGERROR("bad dll version."); goto error; }
#define _SETLIBPROC( name ) m_##name = name;

BOOL r9RenderGL::LoadDll()
{
	guard(r9RenderGL::LoadDll);
#ifdef R9_ENABLE_DLLGL
	if(m_dll) return TRUE;
	m_dll = LoadLibrary("opengl32.dll");
	if(!m_dll) { R9_LOGERROR("can't load opengl32.dll"); return FALSE; }

	_GETDLLPROC(	wglCreateContext		);
	_GETDLLPROC(	wglMakeCurrent			);
	_GETDLLPROC(	wglDeleteContext		);
	_GETDLLPROC(	glGetString				);
	_GETDLLPROC(	glGenTextures			);
	_GETDLLPROC(	glBindTexture			);
	_GETDLLPROC(	glTexImage2D			);
	_GETDLLPROC(	glDeleteTextures		);
	_GETDLLPROC(	glTexParameteri			);
	_GETDLLPROC(	glBlendFunc				);
	_GETDLLPROC(	glTexEnvi				);
	_GETDLLPROC(	glEnable				);
	_GETDLLPROC(	glShadeModel			);
	_GETDLLPROC(	glViewport				);
	_GETDLLPROC(	glMatrixMode			);
	_GETDLLPROC(	glLoadIdentity			);
	_GETDLLPROC(	glOrtho					);
	_GETDLLPROC(	glClearColor			);
	_GETDLLPROC(	glClear					);
	_GETDLLPROC(	glEnableClientState		);
	_GETDLLPROC(	glColorPointer			);
	_GETDLLPROC(	glVertexPointer			);
	_GETDLLPROC(	glTexCoordPointer		);
	_GETDLLPROC(	glDrawArrays			);
	_GETDLLPROC(	glDisableClientState	);
	_GETDLLPROC(	glPixelStorei			);
	_GETDLLPROC(	glReadPixels			);
	_GETDLLPROC(	glCopyTexImage2D		);
	_GETDLLPROC(	glGetTexImage			);

	return TRUE;
	error:
	UnloadDll();
	return FALSE;

#else

	_SETLIBPROC(	wglCreateContext		);
	_SETLIBPROC(	wglMakeCurrent			);
	_SETLIBPROC(	wglDeleteContext		);
	_SETLIBPROC(	glGetString				);
	_SETLIBPROC(	glGenTextures			);
	_SETLIBPROC(	glBindTexture			);
	_SETLIBPROC(	glTexImage2D			);
	_SETLIBPROC(	glDeleteTextures		);
	_SETLIBPROC(	glTexParameteri			);
	_SETLIBPROC(	glBlendFunc				);
	_SETLIBPROC(	glTexEnvi				);
	_SETLIBPROC(	glEnable				);
	_SETLIBPROC(	glShadeModel			);
	_SETLIBPROC(	glViewport				);
	_SETLIBPROC(	glMatrixMode			);
	_SETLIBPROC(	glLoadIdentity			);
	_SETLIBPROC(	glOrtho					);
	_SETLIBPROC(	glClearColor			);
	_SETLIBPROC(	glClear					);
	_SETLIBPROC(	glEnableClientState		);
	_SETLIBPROC(	glColorPointer			);
	_SETLIBPROC(	glVertexPointer			);
	_SETLIBPROC(	glTexCoordPointer		);
	_SETLIBPROC(	glDrawArrays			);
	_SETLIBPROC(	glDisableClientState	);
	_SETLIBPROC(	glPixelStorei			);
	_SETLIBPROC(	glReadPixels			);
	_SETLIBPROC(	glCopyTexImage2D		);
	_SETLIBPROC(	glGetTexImage			);

	return TRUE;
#endif
	unguard();
}

void r9RenderGL::UnloadDll()
{
	guard(r9RenderGL::UnloadDll);
#ifdef R9_ENABLE_DLLGL
	if(m_dll==NULL) return;
	FreeLibrary(m_dll);	
	m_dll=NULL;
#endif

	m_wglCreateContext		= NULL;
	m_wglMakeCurrent		= NULL;
	m_wglDeleteContext		= NULL;
	m_glGetString			= NULL;
	m_glGenTextures			= NULL;
	m_glBindTexture			= NULL;
	m_glTexImage2D			= NULL;
	m_glDeleteTextures		= NULL;
	m_glTexParameteri		= NULL;
	m_glBlendFunc			= NULL;
	m_glTexEnvi				= NULL;
	m_glEnable				= NULL;
	m_glShadeModel			= NULL;
	m_glViewport			= NULL;
	m_glMatrixMode			= NULL;
	m_glLoadIdentity		= NULL;
	m_glOrtho				= NULL;
	m_glClearColor			= NULL;
	m_glClear				= NULL;
	m_glEnableClientState	= NULL;
	m_glColorPointer		= NULL;
	m_glVertexPointer		= NULL;
	m_glTexCoordPointer		= NULL;
	m_glDrawArrays			= NULL;
	m_glDisableClientState	= NULL;
	m_glPixelStorei			= NULL;
	m_glReadPixels			= NULL;
	m_glCopyTexImage2D		= NULL;
	m_glGetTexImage			= NULL;

	unguard();
}

void r9RenderGL::LogAdapterInfo()
{
	guard(r9RenderGL::LogAdapterInfo);
	DISPLAY_DEVICE  dd;
	memset(&dd,0,sizeof(dd));
	dd.cb = sizeof(DISPLAY_DEVICE);
	int i=0;
	while(TRUE)
	{
		if(!EnumDisplayDevices(NULL,i,&dd,0)) break;
		if(!(dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) continue; // ignore other devices

		dlog(LOGRND, "Video adapter #%i info:\n", i);
		dlog(LOGRND, "  device name = %s\n", dd.DeviceName);
		dlog(LOGRND, "  device string = %s\n", dd.DeviceString);
		dlog(LOGRND, "  device flags = %x\n", dd.StateFlags);
		return;
	}

	dlog(LOGRND, "Video adapter unknown.");
	unguard();
}

static int SortDisplayModes( const VOID* arg1, const VOID* arg2 )
{
    r9DisplayMode* p1 = (r9DisplayMode*)arg1;
    r9DisplayMode* p2 = (r9DisplayMode*)arg2;
	if( p1->m_windowed	< p2->m_windowed )	return +1;
    if( p1->m_windowed	> p2->m_windowed )	return -1;
    if( p1->m_bpp		< p2->m_bpp )		return -1;
    if( p1->m_bpp		> p2->m_bpp )		return +1;
    if( p1->m_width		< p2->m_width )		return -1;
    if( p1->m_width		> p2->m_width )		return +1;
    if( p1->m_height	< p2->m_height )	return -1;
    if( p1->m_height	> p2->m_height )	return +1;
    if( p1->m_refresh	< p2->m_refresh )	return -1;
    if( p1->m_refresh	> p2->m_refresh )	return +1;
    return 0;
}

int r9RenderGL::GatherDisplayModes( r9DisplayMode* displaymode )
{
	guard(r9RenderGL::GatherDisplayModes);
	BOOL ok;
	int count = 0;
	
	// adapter (only when counting)
	if(displaymode==NULL) LogAdapterInfo();

	// current display mode (windowed)
	int bpp;
	DEVMODE devmode;
	memset(&devmode,0,sizeof(devmode));
	devmode.dmSize = sizeof(DEVMODE);
	ok = EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&devmode);
    if(!ok) { R9_LOGERROR("failed to get current display mode."); goto next; }
	bpp = devmode.dmBitsPerPel;
	if(bpp!=16 && bpp!=32) { R9_LOGERROR("invalid current display mode format."); goto next; }
	if(displaymode)
	{
		displaymode[count].m_windowed	= 1;
		displaymode[count].m_bpp		= bpp;
		displaymode[count].m_width		= devmode.dmPelsWidth;
		displaymode[count].m_height		= devmode.dmPelsHeight;
		displaymode[count].m_refresh	= devmode.dmDisplayFrequency;		
		displaymode[count].m_reserved1	= 0;
	}
	count++;

	next:

	// supported display modes (fullscreen)
	int idx=0; // mode index
	while(TRUE)
	{
		memset(&devmode,0,sizeof(devmode));
		devmode.dmSize = sizeof(DEVMODE);
		ok = EnumDisplaySettings(NULL,idx,&devmode);
		if(!ok) break; // got them all
		idx++;
		bpp = devmode.dmBitsPerPel;
		if(bpp!=16 && bpp!=32) continue;
		if(displaymode)
		{
			displaymode[count].m_windowed	= 0;
			displaymode[count].m_bpp		= bpp;
			displaymode[count].m_width		= devmode.dmPelsWidth;
			displaymode[count].m_height		= devmode.dmPelsHeight;
			displaymode[count].m_refresh	= devmode.dmDisplayFrequency;		
			displaymode[count].m_reserved1	= 0;
		}
		count++;
	}

	// sort modes by windowed, bpp, width, height, refresh
	if(displaymode)
        qsort( displaymode, count, sizeof(r9DisplayMode), SortDisplayModes );

	// log
	if(displaymode)
	{
		dlog(LOGRND,"Display modes:\n");
		for(int i=0;i<count;i++)
			dlog(LOGRND,"   %i \t%ix%i \t%ibpp \t%iHz \t%s\n", i, displaymode[i].m_width, displaymode[i].m_height, displaymode[i].m_bpp, displaymode[i].m_refresh, displaymode[i].m_windowed?"windowed":"");
		dlog(LOGRND,"\n");
	}

	return count;
	unguard();
}

BOOL r9RenderGL::Init( HWND hwnd, r9Cfg* cfg )
{
	guard(r9RenderGL::Init);
	// m_hwnd = hwnd; OpenGL uses a child render window, because of the bpp problem
	
	// config
	if(cfg!=NULL) m_cfg = *cfg;
	int api = m_api;
	R9_FilterCfg(m_cfg,api);

	if(m_cfg.m_bpp!=16 && m_cfg.m_bpp!=32) return FALSE;

	// prepare window
	PrepareParentWindow();
	if(!CreateRenderWindow()) { DestroyRenderWindow(); return FALSE; }

	// create opengl device
	if(!GL_CreateDevice()) { DestroyRenderWindow(); return FALSE; }

	// batch
	GL_BatchCreate();

	// default render states
	SetDefaultStates();

	// clear doublebuffer
	if(BeginScene()) { Clear(0xff000000); EndScene(); Present(); }
	if(BeginScene()) { Clear(0xff000000); EndScene(); Present(); }

	// font
	CreateFont();

	return TRUE;
	unguard();
}

void r9RenderGL::Done()
{
	guard(r9RenderGL::Done);
	r9Render::Done();
	sfree(m_batchbuffer);
	if(!m_cfg.m_windowed) ChangeDisplaySettings(NULL,0);
	if(m_hrc) { m_wglMakeCurrent(NULL,NULL); m_wglDeleteContext(m_hrc); m_hrc=NULL; }
	if(m_hdc) { ReleaseDC(m_hwnd,m_hdc); m_hdc=NULL; }
	DestroyRenderWindow();
	unguard();
}

BOOL r9RenderGL::IsReady()
{
	guard(r9RenderGL::IsReady);
	return (m_hrc!=NULL);
	unguard();
}

R9TEXTURE r9RenderGL::TextureCreate( r9Img* img )
{
	guard(r9RenderGL::TextureCreate);
	// check image
	if(img==NULL) return NULL;
	if(!R9_ImgIsValid(img)) return NULL;
	int imgbpp = R9_PFBpp(img->m_pf);
	if(imgbpp!=24 && imgbpp!=32) return NULL;

	R9_ImgFlipRGB(img); // opengl is bgr

	// find accepted size, power of 2, etc
	int w = GetPow2HI(img->m_width);
	int h = GetPow2HI(img->m_height);
	if( w < 64 ) w = 64; // safe
	if( h < 64 ) h = 64; // safe

	void* imgdata = img->m_data;

	// resize canvas
	r9Img img2;
	if(w>img->m_width || h>img->m_height)
	{
		img2.m_pf = img->m_pf;
		img2.m_width = w;
		img2.m_height = h;
		R9_ImgCreate(&img2,TRUE); sassert(img2.m_data);
		R9_ImgBitBlt(img,0,0,img->m_width,img->m_height,&img2,0,0);
		imgdata = img2.m_data; // use this data
	}
	else
	if(w<img->m_width || h<img->m_height) return FALSE; // it can't be smaller

	// create GL texture
	GLuint gltex;
	m_glGenTextures(1, &gltex);
	m_glBindTexture(GL_TEXTURE_2D, gltex);
	if(imgbpp==24)
		m_glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, imgdata);
	else
		m_glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);

	if(R9_ImgIsValid(&img2)) R9_ImgDestroy(&img2); // destroy resized if needed

	// create R9 texture
	r9Texture* tex		= snew r9Texture;
	tex->m_width		= img->m_width;
	tex->m_height		= img->m_height;
	tex->m_realwidth	= w;
	tex->m_realheight	= h;
	tex->m_handler		= (void*)(intptr)gltex;
	tex->m_handlerex	= NULL;

	GL_BindTexture(); // for wrap and filter

	return tex;
	unguard();
}

R9TEXTURE r9RenderGL::TextureCreateTarget( int width, int height )
{
	guard(r9RenderGL::TextureCreateTarget);
	
	// find accepted size, power of 2, etc
	int w = GetPow2HI(width);
	int h = GetPow2HI(height);
	if( w < 64 ) w = 64; // safe
	if( h < 64 ) h = 64; // safe

	// data
	int spp = 4;
	byte* imgdata = (byte*)smalloc(w*h*spp); sassert(imgdata);
	memset(imgdata,0,w*h*spp);

	// create GL texture
	GLuint gltex;
	m_glGenTextures(1, &gltex);
	m_glBindTexture(GL_TEXTURE_2D, gltex);
	m_glTexImage2D(GL_TEXTURE_2D, 0, spp, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, imgdata);

	sfree(imgdata);

	// create R9 texture
	r9Texture* tex		= snew r9Texture;
	tex->m_width		= width;
	tex->m_height		= height;
	tex->m_realwidth	= w;
	tex->m_realheight	= h;
	tex->m_handler		= (void*)(intptr)gltex;
	tex->m_handlerex	= NULL;

	GL_BindTexture(); // for wrap and filter

	return tex;
	unguard();
}

void r9RenderGL::TextureDestroy( R9TEXTURE texture )
{
	guard(r9RenderGL::TextureDestroy);
	if(texture==NULL) return;
	GLuint gltex = (GLuint)(intptr)texture->m_handler;
	m_glDeleteTextures(1,&gltex);
	sdelete(texture);
	unguard();
}

void r9RenderGL::SetTexture( R9TEXTURE texture )
{
	guard(r9RenderGL::SetTexture);
	if(m_texture==texture) return;
	if(NeedFlush()) Flush();
	m_texture = texture;
	GL_BindTexture();
	unguard();
}

void r9RenderGL::SetState( int state, int value )
{
	guard(r9RenderGL::SetState);
	sassert(0<=state && state<R9_STATES);
	if(m_state[state]==value) return;
	if(NeedFlush()) Flush();
	m_state[state] = value;

	switch(state)
	{
		case R9_STATE_PRIMITIVE:
			break;
		case R9_STATE_BLEND:
		{
			switch(value)
			{
				case R9_BLEND_OPAQUE:
					m_glBlendFunc(GL_ONE, GL_ZERO);
					break;
				case R9_BLEND_ALPHA:
					m_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
				case R9_BLEND_ADD:
					m_glBlendFunc(GL_ONE, GL_ONE);
					break;
				case R9_BLEND_MOD:
					m_glBlendFunc(GL_DST_COLOR, GL_ZERO);
					break;
				case R9_BLEND_MOD2:
					m_glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
					break;
				case R9_BLEND_ALPHAREP:
					m_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					break;
			}
			GL_BindTexture(); // apply texture env states
			break;
		}
		case R9_STATE_TADDRESS:
		{
			GL_BindTexture();
			break;
		}
		case R9_STATE_FILTER:
		{
			GL_BindTexture();
			break;
		}
	}
	unguard();
}

void r9RenderGL::SetViewport( fRect& rect )
{
	guard(r9RenderGL::SetViewport);
	if(m_viewport==rect) return;
	m_viewport = rect;
	m_glViewport((GLint)rect.x1,(GLint)rect.y1,(GLint)rect.Width(),(GLint)rect.Height());
	m_glMatrixMode(GL_PROJECTION);
	m_glLoadIdentity();
	m_glOrtho(rect.x1,rect.x2,rect.y2,rect.y1,-1,1);
	m_glMatrixMode(GL_MODELVIEW);
	m_glLoadIdentity();
	unguard();
}

void r9RenderGL::SetView( int x, int y, dword flip )
{
	guard(r9RenderGL::SetView);
	m_viewx = x;
	m_viewy = y;
	m_viewflip = flip;

	fRect rect;
	rect.x1 = (flip & R9_FLIPX) ? m_viewport.x2 : m_viewport.x1;
	rect.y1 = (flip & R9_FLIPY) ? m_viewport.y2 : m_viewport.y1;
	rect.x2 = (flip & R9_FLIPX) ? m_viewport.x1 : m_viewport.x2;
	rect.y2 = (flip & R9_FLIPY) ? m_viewport.y1 : m_viewport.y2;
	rect.x1 += x;		
	rect.x2 += x;
	rect.y1 += y;
	rect.y2 += y;

	m_glMatrixMode(GL_PROJECTION);
	m_glLoadIdentity();
	m_glOrtho(rect.x1,rect.x2,rect.y2,rect.y1,-1,1);

	unguard();

}

void r9RenderGL::SetDefaultStates()
{
	guard(r9RenderGL::SetDefaultStates);
	// device default states
	m_glEnable(GL_BLEND);
	m_glEnable(GL_TEXTURE_2D);
	m_glShadeModel(GL_SMOOTH);

	m_glViewport(0,0,GetWidth(),GetHeight());						// Reset The Current Viewport
	m_glMatrixMode(GL_PROJECTION);
	m_glLoadIdentity();
	m_glOrtho(0,GetWidth(),GetHeight(),0,-1,1);
	m_glMatrixMode(GL_MODELVIEW);
	m_glLoadIdentity();

	// our render states
	r9Render::SetDefaultStates();
	unguard();
}

void r9RenderGL::Clear( dword color )
{
	guard(r9RenderGL::Clear);
	if( !m_beginendscene ) return;
	fColor fcolor(color);
	m_glClearColor(fcolor.r,fcolor.g,fcolor.b,fcolor.a);
	m_glClear(GL_COLOR_BUFFER_BIT);
	unguard();
}

BOOL r9RenderGL::BeginScene( R9TEXTURE target )
{
	guard(r9RenderGL::BeginScene);
	if( m_beginendscene ) return FALSE;
	m_primitivecount = 0;

	if(target) // use render target
	{
		// the upside down is a mess, so we will render upside down :(
		m_glMatrixMode(GL_PROJECTION);
		m_glLoadIdentity();
		m_glOrtho(0,GetWidth(),0,GetHeight(),-1,1);
		m_textarget = target; // store for later use
	}

	m_beginendscene = TRUE;
	return TRUE;
	unguard();
}

void r9RenderGL::EndScene()
{
	guard(r9RenderGL::EndScene);
	if( !m_beginendscene ) return;
	if(NeedFlush()) Flush();
	m_beginendscene = FALSE;
	
	// if render target, then copy from screen into it
	if(m_textarget!=NULL)
	{
		GLuint gltarget = (GLuint)(intptr)m_textarget->m_handler;
		m_glBindTexture(GL_TEXTURE_2D,gltarget);
		m_glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,m_textarget->m_realwidth,m_textarget->m_realheight,0);

		// restore projection for normap drawing
		m_glMatrixMode(GL_PROJECTION);
		m_glLoadIdentity();
		m_glOrtho(0,GetWidth(),GetHeight(),0,-1,1);
		m_textarget = NULL; // clear
	}

	unguard();
}

void r9RenderGL::Present()
{
	guard(r9RenderGL::Present);
	BOOL ok = SwapBuffers(m_hdc);
	unguard();
}

//@OBSOLETE
BOOL r9RenderGL::ToggleVideoMode()
{
	guard(r9RenderGL::ToggleVideoMode);
	dlog(LOGRND,"Toggle video mode: ");
	BOOL ok = FALSE;
	if(!m_cfg.m_windowed) // change to windowed
	{
		ok = (ChangeDisplaySettings(NULL,0)==DISP_CHANGE_SUCCESSFUL);
	}
	else // change to full screen
	{
		DEVMODE devmode;
		memset(&devmode,0,sizeof(DEVMODE));
		devmode.dmSize				= sizeof(DEVMODE);
		devmode.dmPelsWidth			= m_cfg.m_width;
		devmode.dmPelsHeight		= m_cfg.m_height;
		devmode.dmBitsPerPel		= m_cfg.m_bpp;
		devmode.dmDisplayFrequency	= m_cfg.m_refresh;
		devmode.dmFields			= DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
		ok = (ChangeDisplaySettings(&devmode,CDS_FULLSCREEN)==DISP_CHANGE_SUCCESSFUL);
	}

	if(ok) 
	{
		m_cfg.m_windowed = !m_cfg.m_windowed;
		PrepareParentWindow();
	}
	dlog(LOGRND,ok?"successful\n":"failed\n");
	return ok;
	unguard();
}

void r9RenderGL::Push( r9Vertex* vx, int vxs, int primitive )
{
	guard(r9RenderGL::Push);
	// set primitive
	if(GetState(R9_STATE_PRIMITIVE)!=primitive)
		SetState(R9_STATE_PRIMITIVE,primitive);

	// push
	int primitivevertexes = primitive ? 3 : 2;
	int batchsize = (R9_BATCHSIZE_GL / primitivevertexes) * primitivevertexes; // make multiple of primitive vertexes
	float ofs = (primitive==R9_PRIMITIVE_LINE) ? 0.5f : 0.0f; // pixel offset

	while(vxs>0)
	{
		// get count
		int count = vxs;
		if( m_batchcount + count > batchsize )
			count = batchsize - m_batchcount;

		// copy and adjust rgb>bgr
		memcpy(m_batchbuffer+m_batchcount,vx,count*sizeof(r9Vertex));
		r9Vertex* bvx = m_batchbuffer + m_batchcount;
		for(int i=0;i<count;i++)
		{
			// rgb to bgr
			byte* c = (byte*)&(bvx->color);
			byte t = c[0];
			c[0] = c[2];
			c[2] = t;
			bvx->y += ofs;
			bvx++;
		}

		vx += count;
		vxs -= count;
		m_batchcount += count;
		m_needflush = TRUE;

		// flush if full
		if( m_batchcount==batchsize ) Flush();
	}
	
	unguard();
}

void r9RenderGL::Flush()
{
	guard(r9RenderGL::Flush);
	m_needflush = FALSE;
	if(m_batchcount==0) return;

	// set arrays
	m_glEnableClientState( GL_VERTEX_ARRAY );
	m_glEnableClientState( GL_COLOR_ARRAY );
	m_glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	m_glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(r9Vertex),&(m_batchbuffer->color));
	m_glVertexPointer(2,GL_FLOAT,sizeof(r9Vertex),&(m_batchbuffer->x));
	m_glTexCoordPointer(2,GL_FLOAT,sizeof(r9Vertex),&(m_batchbuffer->u));

	// draw
	int primitive = GetState(R9_STATE_PRIMITIVE);
	m_glDrawArrays( primitive ? GL_TRIANGLES : GL_LINES, 0, m_batchcount );

	m_glDisableClientState( GL_VERTEX_ARRAY );
	m_glDisableClientState( GL_COLOR_ARRAY );
	m_glDisableClientState( GL_TEXTURE_COORD_ARRAY );


	m_batchcount = 0;
	unguard();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// SCREEN SHOT
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL r9RenderGL::SaveScreenShot( fRect* rect, BOOL full )
{
	guard(r9RenderGL::SaveScreenShot)

	r9Img img;
	if(!TakeScreenShot(&img, rect, full)) return FALSE;

	char file[64];
	char date[16];
	char time[16];
	
	SYSTEMTIME systime;
	GetSystemTime( &systime );
	GetDateFormat( NULL, 0, &systime, "yyMMdd", date, 16 );
	GetTimeFormat( NULL, 0, &systime, "_HHmm_ss", time, 16 );

	CreateDirectory("ScreenShots",NULL);
	strcpy( file, "ScreenShots\\" );
	strcat( file, date );
	strcat( file, time );
	strcat( file, ".png" ); // change this if you want (.tga)
	
	R9_ImgSaveFile(file,&img);
	R9_ImgDestroy(&img);
	
	dlog(LOGRND,"ScreenShot saved!\n");

	return TRUE;
	unguard()
}

BOOL r9RenderGL::TakeScreenShot( r9Img* img, fRect* rect, BOOL full )
{
	guard(r9RenderGL::TakeScreenShot)

	if(img==NULL) return FALSE;
	if( IsBeginEndScene() ) { dlog(LOGRND, "ScreenShot can not be taken inside Begin - End frame.\n"); return FALSE; }
	if( m_cfg.m_bpp!=32 ) 	{ dlog(LOGRND, "ScreenShot can be taken only in 32bit modes.\n"); return FALSE; }

	R9_ImgDestroy(img);

	if(full) // directly from screen; very slow
	{
		DEVMODE devmode;
		memset(&devmode,0,sizeof(devmode));
		devmode.dmSize = sizeof(DEVMODE);
		if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&devmode)) return FALSE;
		iRect irect(0,0,devmode.dmPelsWidth,devmode.dmPelsHeight);

		img->m_pf = R9_PF_RGB;
		img->m_width = irect.Width();
		img->m_height = irect.Height();
		if(!R9_ImgCreate(img)) return FALSE;

		HDC hdc = GetDC(NULL);
		if(!hdc) return FALSE;
		for(int y=0;y<irect.Height();y++)
		{
			for(int x=0;x<irect.Width();x++)
			{
				dword dw = GetPixel(hdc,irect.left+x,irect.top+y);
				R9_ImgSetColor(img,x,y,dw);
			}
		}
		ReleaseDC(NULL,hdc);
	}
	else // from backbuffer
	{
		iRect irect = (rect) ? *rect : iRect(0,0,GetWidth(),GetHeight());

		img->m_pf = R9_PF_RGB;
		img->m_width = irect.Width();
		img->m_height = irect.Height();
		if(!R9_ImgCreate(img)) return FALSE;

		m_glPixelStorei(GL_PACK_ALIGNMENT, 1);
		m_glReadPixels( irect.left, GetHeight()-irect.bottom, irect.Width(), irect.Height(), GL_RGB, GL_UNSIGNED_BYTE, img->m_data );
		R9_ImgFlipV(img);
	}

	R9_ImgFlipRGB(img);
	return TRUE;
	unguard()
}

BOOL r9RenderGL::CopyTargetToImage( R9TEXTURE target, r9Img* img, fRect* rect )
{
	guard(r9RenderGL::CopyTargetToImage);
	sassert(img); 
	sassert(rect);
	sassert(target);
	if(!R9_ImgIsValid(img)) return FALSE;

	int x = (int)rect->x1;
	int y = (int)rect->y1;
	int w = (int)rect->Width();
	int h = (int)rect->Height();
	if(w>target->m_realwidth) return FALSE;
	if(h>target->m_realheight) return FALSE;

	// temp image to read target data into
	r9Img imgtmp;
	imgtmp.m_pf = R9_PF_RGB;
	imgtmp.m_width = target->m_realwidth;
	imgtmp.m_height = target->m_realheight;
	if(!R9_ImgCreate(&imgtmp)) return FALSE;
	SetTexture(target);
	m_glPixelStorei(GL_PACK_ALIGNMENT, 1);
	m_glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_BYTE,imgtmp.m_data);
	R9_ImgFlipRGB(&imgtmp);
	//R9_ImgSaveFile(sprint("map_%02i_%02i.png",y/h,x/w),&imgtmp); // test

	// bitblt
	int wc = w; if(x+wc>img->m_width)	wc=img->m_width-x;
	int hc = h; if(y+hc>img->m_height)	hc=img->m_height-y;
	if(!R9_ImgBitBltSafe(&imgtmp,0,0,wc,hc,img,x,y)) return FALSE;

	R9_ImgDestroy(&imgtmp);
	return TRUE;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL r9RenderGL::CreateRenderWindow()
{
	guard(r9RenderGL::CreateRenderWindow);
	HWND hwnd_parent = E9_GetHWND();
	if(!hwnd_parent) return FALSE;
	RECT rect_parent;
	GetClientRect(hwnd_parent,&rect_parent);

	// register child window
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= E9_GetHINSTANCE();
	wcex.hIcon			= NULL;	// let the user set the icon later
	wcex.hCursor		= NULL; // LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= NULL;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= "E9_RNDCLASS";
	wcex.hIconSm		= NULL;	// use small icon from default icon
	BOOL ok = RegisterClassEx(&wcex);
	if(!ok) { dlog(LOGERR, "APP: failed to register render window class (%i).\n",GetLastError()); return FALSE; }

	// create child window
	int style = WS_CHILD | WS_VISIBLE;
	int width = rect_parent.right-rect_parent.left;
	int height= rect_parent.bottom-rect_parent.top;
	RECT rec = {0,0,width,height};
	AdjustWindowRectEx( &rec, style, FALSE, 0 );
	m_hwnd = CreateWindowEx(	0, 
								"E9_RNDCLASS", 
								"", 
								style,
								rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top, 
								hwnd_parent, NULL, E9_GetHINSTANCE(), 
								NULL );
	if(m_hwnd==NULL) { dlog(LOGERR, "RENDER: failed to create render window (%i).\n",GetLastError()); return FALSE; }

	return TRUE;

	unguard();
}

BOOL r9RenderGL::DestroyRenderWindow()
{
	guard(r9RenderGL::DestroyRenderWindow);
	if(m_hwnd) 
	{ 
		DestroyWindow(m_hwnd); 
		m_hwnd=NULL; 
	}
	if(!UnregisterClass("E9_RNDCLASS",E9_GetHINSTANCE())) { dlog(LOGERR,"RENDER: can't unregister window class."); }
	return TRUE;
	unguard();
}

LRESULT	CALLBACK r9RenderGL::WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	guard(r9RenderGL::WndProc);
	return DefWindowProc( hwnd, msg, wParam, lParam );
	unguard();
}

void r9RenderGL::PrepareParentWindow()
{
	guard(r9RenderGL::PrepareParentWindow);
	HWND hwnd = E9_GetHWND();
	if(!hwnd) return;
	if( m_cfg.m_windowed )
	{
		int scrw = sys_desktopwidth();
		int scrh = sys_desktopheight();
		BOOL fulldesktop = (m_cfg.m_width==scrw) || (m_cfg.m_height==scrh);
		int cx = (sys_desktopwidth()-m_cfg.m_width) / 2;
		int cy = (sys_desktopheight()-m_cfg.m_height) / 2;
		RECT rec = {cx,cy,cx+m_cfg.m_width,cy+m_cfg.m_height};
		long style = fulldesktop ? (WS_POPUP|WS_SYSMENU) : (WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX|WS_SIZEBOX));
		style |= CS_OWNDC;
		AdjustWindowRectEx( &rec, style, FALSE, 0 );
		MoveWindow( hwnd, rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top, TRUE );
		SetWindowLong( hwnd, GWL_STYLE, style );
		SetWindowPos( hwnd, HWND_NOTOPMOST,
                      rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top,
                      SWP_SHOWWINDOW|SWP_FRAMECHANGED );
	}
	else
	{
		RECT rec = {0,0,m_cfg.m_width,m_cfg.m_height};
		long style = WS_POPUP|WS_SYSMENU|WS_VISIBLE;
		style |= CS_OWNDC;
		MoveWindow( hwnd, rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top, TRUE );
		SetWindowLong( hwnd, GWL_STYLE, style );
		SetWindowPos( hwnd, HWND_TOPMOST,
                      rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top,
                      SWP_SHOWWINDOW|SWP_FRAMECHANGED );
	}
	RECT r;
	GetWindowRect(hwnd,&r);
	dlog(LOGRND,"window size %ix%i\n",r.right-r.left,r.bottom-r.top);
	unguard();
}

BOOL r9RenderGL::GL_CreateDevice()
{
	guard(r9RenderGL::GL_CreateDevice)
	sassert(!m_hrc);

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		m_cfg.m_bpp,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		0,											// Depth Buffer
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	// change mode to fullscreen if required
	if(!m_cfg.m_windowed)
	{
		DEVMODE devmode;
		memset(&devmode,0,sizeof(DEVMODE));
		devmode.dmSize				= sizeof(DEVMODE);
		devmode.dmPelsWidth			= m_cfg.m_width;
		devmode.dmPelsHeight		= m_cfg.m_height;
		devmode.dmBitsPerPel		= m_cfg.m_bpp;
		devmode.dmDisplayFrequency	= m_cfg.m_refresh;
		devmode.dmFields			= DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
		if( ChangeDisplaySettings(&devmode,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			R9_LOGERROR("fullscreen not supported for this mode."); 
			return FALSE; 
		}
	}

	GLuint pfgl;
	int glverhi=1,glverlo=1;
	char* glver;

	// get hdc
	m_hdc=GetDC(m_hwnd);
	if(!m_hdc) { R9_LOGERROR("can't create GL device context."); goto error; }

	// choose and set pixel format for hdc
	pfgl = ChoosePixelFormat(m_hdc,&pfd);
	if(!pfgl) {R9_LOGERROR("can't find a suitable pixel format."); goto error; }
	if(!DescribePixelFormat(m_hdc,pfgl,sizeof(PIXELFORMATDESCRIPTOR),&pfd)) { R9_LOGERROR("can't describe the pixel format."); goto error; }
	if(m_cfg.m_bpp != pfd.cColorBits) { R9_LOGERROR("choosed different pixel format."); goto error; }
	if(!SetPixelFormat(m_hdc,pfgl,&pfd)) { R9_LOGERROR("can't set the pixel format."); goto error; }

	// create and activate rendering context
	m_hrc = m_wglCreateContext(m_hdc);
	if(!m_hrc) { R9_LOGERROR("can't create a GL rendering context."); goto error; }
	if(!m_wglMakeCurrent(m_hdc,m_hrc)) { R9_LOGERROR("can't activate the GL rendering context."); goto error; }

	// get caps
	glver = (char*)m_glGetString(GL_VERSION);
	if(glver) sscanf(glver,"%i.%i",&glverhi,&glverlo);
	if(glverhi*10+glverlo>=13) m_caps.m_texture_env_combine = TRUE; // available from 1.3

	return TRUE;

	error:
	if(!m_cfg.m_windowed) ChangeDisplaySettings(NULL,0);
	if(m_hrc) { m_wglMakeCurrent(NULL,NULL); m_wglDeleteContext(m_hrc); m_hrc=NULL; }
	if(m_hdc) { ReleaseDC(m_hwnd,m_hdc); m_hdc=NULL; }
	return FALSE;
	unguard()
}

BOOL r9RenderGL::GL_BatchCreate()
{
	guard(r9RenderGL::GL_BatchCreate);
	m_batchbuffer = (r9Vertex*)smalloc(R9_BATCHSIZE_GL*sizeof(r9Vertex));
	memset(m_batchbuffer,0,R9_BATCHSIZE_GL*sizeof(r9Vertex)); // clear
	return TRUE;
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
