///////////////////////////////////////////////////////////////////////////////////////////////////
// R9RenderDX.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "R9RenderDX.h"
#include "R9ImgLoader.h"

#ifndef R9_ENABLE_DLLDX
#ifdef _DEBUG
#pragma comment( lib, "d3d9.lib" ) 
// or use d3d9d.lib
#else
#pragma comment( lib, "d3d9.lib" )
#endif
#endif

#define R9_LOGERROR( prefix, hr )	dlog( LOGERR, "RENDER: %s (%x)\n", prefix, hr );

r9RenderDX::tDirect3DCreate9 r9RenderDX::m_Direct3DCreate9 = NULL;

r9RenderDX::r9RenderDX()
{
	guard(r9RenderDX::r9RenderDX);
	m_api = R9_API_DIRECTX;

	m_d3d		= NULL;
	m_d3dd		= NULL;
	m_pfdisplay	= D3DFMT_X8R8G8B8;
	m_pfopaque	= D3DFMT_R8G8B8;
	m_pfalpha	= D3DFMT_A8R8G8B8;
	
	m_caps.m_texsquareonly	= 0;
	m_caps.m_texaspectratio	= 0;
	m_caps.m_texwidth		= 0;
	m_caps.m_texheight		= 0;

	m_batchcount	= 0;
	m_batchbuffer	= NULL;
	m_batchd3d		= NULL;

	m_d3dtarget		= NULL;
	m_targetwidth	= 0;
	m_targetheight	= 0;

	m_targetlist.Init(8,4);

	unguard();
}

r9RenderDX::~r9RenderDX()
{
	guard(r9RenderDX::~r9RenderDX);
	m_targetlist.Done();
	unguard();
}

BOOL r9RenderDX::LoadDll()
{
	guard(r9RenderDX::LoadDll);
#ifdef R9_ENABLE_DLLDX
	if(m_dll) return TRUE;
	m_dll = LoadLibrary("d3d9.dll");
	if(!m_dll) { R9_LOGERROR("can't load d3d9.dll",0); return FALSE; }
	m_Direct3DCreate9 = (tDirect3DCreate9)GetProcAddress(m_dll,"Direct3DCreate9");
	if(!m_Direct3DCreate9) { R9_LOGERROR("bad dll version.",0); goto error; }
	return TRUE;
	error:
	UnloadDll();
	return FALSE;
#else
	m_Direct3DCreate9 = &Direct3DCreate9;
	return TRUE;
#endif
	unguard();
}

void r9RenderDX::UnloadDll()
{
	guard(r9RenderDX::UnloadDll);
#ifdef R9_ENABLE_DLLDX
	if(m_dll==NULL) return;
	FreeLibrary(m_dll);	
	m_dll=NULL;
#endif
	m_Direct3DCreate9 = NULL;
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

int r9RenderDX::GatherDisplayModes( r9DisplayMode* displaymode )
{
	guard(r9RenderDX::GatherDisplayModes);
	int p;
	int count = 0;

	HRESULT hr;
//	LPDIRECT3D9 d3d = Direct3DCreate9(D3D_SDK_VERSION);
	LPDIRECT3D9 d3d = m_Direct3DCreate9(D3D_SDK_VERSION);
	if( d3d==NULL ) { R9_LOGERROR("failed to create Direct3D.",0); return 0; }
	if(d3d->GetAdapterCount()==0) { R9_LOGERROR("no available adapters.",0); d3d->Release(); return 0; }

	// adapter
	int adapter = D3DADAPTER_DEFAULT;
	D3DADAPTER_IDENTIFIER9 d3dadapter;
    hr = d3d->GetAdapterIdentifier( adapter, 0, &d3dadapter );
	if(FAILED(hr)) { R9_LOGERROR("failed to get adapter.",hr); d3d->Release(); return 0; }

	int dvProduct		= HIWORD(d3dadapter.DriverVersion.HighPart);
	int dvVersion		= LOWORD(d3dadapter.DriverVersion.HighPart);
	int dvSubVersion	= HIWORD(d3dadapter.DriverVersion.LowPart);
	int dvBuild			= LOWORD(d3dadapter.DriverVersion.LowPart);

	if(displaymode==NULL)
	{
		dlog(LOGRND, "Video adapter info:\n");
		dlog(LOGRND, "  driver      = %s\n", (d3dadapter.Driver)?(d3dadapter.Driver):"NONE");
		dlog(LOGRND, "  description = %s\n", (d3dadapter.Description)?(d3dadapter.Description):"NONE");
		dlog(LOGRND, "  version     = p%i v%i.%i b%i\n", dvProduct, dvVersion, dvSubVersion, dvBuild );
	}

	// caps
	D3DCAPS9 d3dcaps;
	D3DDEVTYPE d3ddevtype = D3DDEVTYPE_HAL;
	hr = d3d->GetDeviceCaps( adapter, d3ddevtype, &d3dcaps );
	if(FAILED(hr)) { R9_LOGERROR("failed to get device caps.",hr); d3d->Release(); return 0; }

	// current display mode (windowed)
	D3DDISPLAYMODE d3dmode;
	r9PFInfo* pfinfo;
    hr = d3d->GetAdapterDisplayMode( adapter, &d3dmode );
	if(FAILED(hr)) { R9_LOGERROR("failed to get current display mode.",hr); goto next; }
	pfinfo = D3D_PFInfo(d3dmode.Format);
	if(!pfinfo) { R9_LOGERROR("invalid current display mode format.",hr); goto next; }
	if(displaymode)
	{
		displaymode[count].m_windowed	= 1;
		displaymode[count].m_bpp		= pfinfo->m_bpp;
		displaymode[count].m_width		= d3dmode.Width;
		displaymode[count].m_height		= d3dmode.Height;
		displaymode[count].m_refresh	= 0;		
		displaymode[count].m_reserved1	= (dword)d3dmode.Format;
	}
	count++;

	next:
	// supported display modes (fullscreen)
	D3DFORMAT pfdx[2][2] = { {D3DFMT_R5G6B5,D3DFMT_A4R4G4B4}, {D3DFMT_X8R8G8B8,D3DFMT_A8R8G8B8} };
	for(p=0; p<2; p++)
	{
		int modecount = d3d->GetAdapterModeCount( adapter, pfdx[p][0] );	
		for(int i=0; i<modecount; i++)
		{
			hr = d3d->EnumAdapterModes( adapter, pfdx[p][0], i, &d3dmode );
			if(FAILED(hr)) { R9_LOGERROR("failed to enum display mode.",hr); continue; }
			pfinfo = D3D_PFInfo(d3dmode.Format);
			if(!pfinfo) continue; // refuse format
			// check texture formats (opaque and alpha)
			hr = d3d->CheckDeviceFormat( adapter, d3ddevtype, d3dmode.Format, 0, D3DRTYPE_TEXTURE, pfdx[p][0] );
			if(FAILED(hr)) continue; // refuse
			hr = d3d->CheckDeviceFormat( adapter, d3ddevtype, d3dmode.Format, 0, D3DRTYPE_TEXTURE, pfdx[p][1] );
			if(FAILED(hr)) continue; // refuse

			if(displaymode)
			{
				displaymode[count].m_windowed	= 0;
				displaymode[count].m_bpp		= pfinfo->m_bpp;
				displaymode[count].m_width		= d3dmode.Width;
				displaymode[count].m_height		= d3dmode.Height;
				displaymode[count].m_refresh	= d3dmode.RefreshRate;		
				displaymode[count].m_reserved1	= (dword)d3dmode.Format;
			}
			count++;
		}
	}

	if(d3d) d3d->Release();

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

BOOL r9RenderDX::Init( HWND hwnd, r9Cfg* cfg )
{
	guard(r9RenderDX::Init);
	m_hwnd = hwnd;

	// config
	if(cfg!=NULL) m_cfg = *cfg;
	int api = m_api;
	R9_FilterCfg(m_cfg,api);

	if(m_cfg.m_bpp!=16 && m_cfg.m_bpp!=32) return FALSE;
	m_pfdisplay	= (m_cfg.m_bpp==32) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
	m_pfopaque	= (m_cfg.m_bpp==32) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5;
	m_pfalpha	= (m_cfg.m_bpp==32) ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;

	// create d3d
//	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	m_d3d = m_Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_d3d) { R9_LOGERROR("failed to create Direct3D.",0); return FALSE; }

	// prepare window
	PrepareWindow();

	// create d3d device
	if(!D3D_CreateDevice())	{ srelease(m_d3d); return FALSE; }

	// batch
	if(!D3D_BatchCreate()) { srelease(m_d3dd); srelease(m_d3d); return FALSE; }

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

void r9RenderDX::Done()
{
	guard(r9RenderDX::Done);
	r9Render::Done();
	if(m_batchd3d) D3D_BatchUnlock();
	srelease(m_batchd3d);
	srelease(m_d3dtarget);
	srelease(m_d3dd);
	srelease(m_d3d);
	unguard();
}

BOOL r9RenderDX::IsReady()
{
	guard(r9RenderDX::IsReady);
	return (m_d3d!=NULL && m_d3dd!=NULL);
	unguard();
}

R9TEXTURE r9RenderDX::TextureCreate( r9Img* img )
{
	guard(r9RenderDX::TextureCreate);
	sassert(m_d3dd);
	HRESULT hr;
	// check image
	if(img==NULL) return NULL;
	if(!R9_ImgIsValid(img)) return NULL;
	int imgbpp = R9_PFBpp(img->m_pf);
	if(imgbpp!=24 && imgbpp!=32) return NULL;

	// find accepted size, power of 2, etc
	int w = GetPow2HI(img->m_width);
	int h = GetPow2HI(img->m_height);
	if( w<8 ) w=8; // safe
	if( h<8 ) h=8; // safe
	if( m_caps.m_texsquareonly )
	{
		if( w>h ) h = w; else w = h;
	}
	if( m_caps.m_texaspectratio != 0 )
	{
		if( w/h > m_caps.m_texaspectratio )	h = w / m_caps.m_texaspectratio;
		if( h/w > m_caps.m_texaspectratio )	w = h / m_caps.m_texaspectratio;
	}
	if( w > m_caps.m_texwidth )	return NULL;
	if( h > m_caps.m_texheight ) return NULL;

	// create DX texture
	int d3dmips = 1;
	dword d3dusage = 0;
	D3DFORMAT d3dpf = (R9_PFBpp(img->m_pf)==24) ? m_pfopaque : m_pfalpha;
	D3DPOOL d3dpool = D3DPOOL_MANAGED;
	LPDIRECT3DTEXTURE9 d3dtex = NULL;
	hr = m_d3dd->CreateTexture( w, h, d3dmips, d3dusage, d3dpf, d3dpool, &d3dtex, NULL );
	if( FAILED(hr) || d3dtex==NULL) return NULL;
	
	// lock
	D3DLOCKED_RECT d3dlockedrect;
	hr = d3dtex->LockRect( 0, &d3dlockedrect, NULL, D3DLOCK_NOSYSLOCK );
	if(FAILED(hr)) { srelease(d3dtex); return NULL; }
	int pitch = d3dlockedrect.Pitch;
	byte* data = (byte*)d3dlockedrect.pBits;

	// fill
	r9PFInfo* pfinfo = D3D_PFInfo(d3dpf);
	BOOL ok = R9_ImgWriteBuffer(img, data, pfinfo, pitch);
	if(!ok) memset(data,0,img->m_height*pitch); // clear

	// unlock
	hr = d3dtex->UnlockRect(0);

	// create R9 texture
	r9Texture* tex = snew r9Texture;
	tex->m_width = img->m_width;
	tex->m_height = img->m_height;
	tex->m_realwidth = w;
	tex->m_realheight = h;
	tex->m_handler = d3dtex;
	tex->m_handlerex = NULL;

	return tex;
	unguard();
}

R9TEXTURE r9RenderDX::TextureCreateTarget( int width, int height )
{
	guard(r9RenderDX::TextureCreateTarget);
	sassert(m_d3dd);
	HRESULT hr;

	// find accepted size, power of 2, etc
	int w = GetPow2HI(width);
	int h = GetPow2HI(height);
	if( w<64 ) w=64; // safe
	if( h<64 ) h=64; // safe
	if( m_caps.m_texsquareonly )
	{
		if( w>h ) h = w; else w = h;
	}
	if( m_caps.m_texaspectratio != 0 )
	{
		if( w/h > m_caps.m_texaspectratio )	h = w / m_caps.m_texaspectratio;
		if( h/w > m_caps.m_texaspectratio )	w = h / m_caps.m_texaspectratio;
	}
	if( w > m_caps.m_texwidth )	return NULL;
	if( h > m_caps.m_texheight ) return NULL;

	// create DX texture
	int d3dmips = 1;
	dword d3dusage = D3DUSAGE_RENDERTARGET;
	D3DFORMAT d3dpf = m_pfdisplay;
	D3DPOOL d3dpool = D3DPOOL_DEFAULT;
	LPDIRECT3DTEXTURE9 d3dtex = NULL;
	hr = m_d3dd->CreateTexture( w, h, d3dmips, d3dusage, d3dpf, d3dpool, &d3dtex, NULL );
	if( FAILED(hr) || d3dtex==NULL) return NULL;
	
	// no locks on the render target!

	// create R9 texture
	r9Texture* tex = snew r9Texture;
	tex->m_width = width;
	tex->m_height = height;
	tex->m_realwidth = w;
	tex->m_realheight = h;
	tex->m_handler = d3dtex;
	LPDIRECT3DSURFACE9 d3dsrf = NULL;
	d3dtex->GetSurfaceLevel(0,&d3dsrf);
	tex->m_handlerex = d3dsrf;

	// add render target texture to manager
	TT_Add(tex);

	return tex;
	unguard();
}

void r9RenderDX::TextureDestroy( R9TEXTURE texture )
{
	guard(r9RenderDX::TextureDestroy);
	if(texture==NULL) return;
	LPDIRECT3DTEXTURE9 d3dtex = (LPDIRECT3DTEXTURE9)(texture->m_handler);
	LPDIRECT3DSURFACE9 d3dsrf = (LPDIRECT3DSURFACE9)(texture->m_handlerex);
	if(d3dsrf) TT_Del(texture); // delete render target texture from manager
	if(d3dsrf) srelease(d3dsrf);
	if(d3dtex) srelease(d3dtex);
	sdelete(texture);
	unguard();
}

void r9RenderDX::SetTexture( R9TEXTURE texture )
{
	guard(r9RenderDX::SetTexture);
	if(m_texture==texture) return;
	if(NeedFlush()) Flush();
	m_texture = texture;
	LPDIRECT3DTEXTURE9 d3dtex = NULL;
	if(m_texture) d3dtex = (LPDIRECT3DTEXTURE9)(m_texture->m_handler);
	HRESULT hr = m_d3dd->SetTexture(0,d3dtex);
	unguard();
}

void r9RenderDX::SetState( int state, int value )
{
	guard(r9RenderDX::SetState);
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
					m_d3dd->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
					m_d3dd->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
					break;
				case R9_BLEND_ALPHA:
					m_d3dd->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
					m_d3dd->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
					break;
				case R9_BLEND_ADD:
					m_d3dd->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
					m_d3dd->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
					break;
				case R9_BLEND_MOD:
					m_d3dd->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_DESTCOLOR);
					m_d3dd->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ZERO);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
					break;
				case R9_BLEND_MOD2:
					m_d3dd->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_DESTCOLOR);
					m_d3dd->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_SRCCOLOR);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
					break;
				case R9_BLEND_ALPHAREP:
					m_d3dd->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
					m_d3dd->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
					m_d3dd->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
					break;
			}
			break;
		}
		case R9_STATE_TADDRESS:
		{
			dword mode = (value==R9_TADDRESS_WRAP) ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP;
			m_d3dd->SetSamplerState(0,D3DSAMP_ADDRESSU,mode);
			m_d3dd->SetSamplerState(0,D3DSAMP_ADDRESSV,mode);
			break;
		}
		case R9_STATE_FILTER:
		{
			dword mode = value ? D3DTEXF_LINEAR : D3DTEXF_POINT;
			m_d3dd->SetSamplerState(0,D3DSAMP_MINFILTER,mode);
			m_d3dd->SetSamplerState(0,D3DSAMP_MAGFILTER,mode);
			break;
		}
	}
	unguard();
}

void r9RenderDX::SetViewport( fRect& rect )
{
	guard(r9RenderDX::SetViewport);
	if(m_viewport==rect) return;
	m_viewport = rect;
	D3DVIEWPORT9 vp;
	vp.X = (DWORD)rect.x1;
	vp.Y = (DWORD)rect.y1;
	vp.Width = (DWORD)rect.Width();
	vp.Height = (DWORD)rect.Height();
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	m_d3dd->SetViewport(&vp);
	unguard();
}

void r9RenderDX::SetView( int x, int y, dword flip )
{
	guard(r9RenderDX::SetView);
	m_viewx = x;
	m_viewy = y;
	m_viewflip = flip;
	D3DMATRIX mat;
	memset( &mat, 0, sizeof(mat) );
	mat._11 = (flip & R9_FLIPX) ? -1.0f : 1.0f; 
	mat._22 = (flip & R9_FLIPY) ? -1.0f : 1.0f; 
	mat._33 = 1.0f; 
	mat._44 = 1.0f;
	m_d3dd->SetTransform( D3DTS_VIEW, &mat );	
	// I could also have it software at Push, like I do with x and y
	unguard();
}

void r9RenderDX::SetDefaultStates()
{
	guard(r9RenderDX::SetDefaultStates);
	// device default states
	m_d3dd->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	m_d3dd->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	m_d3dd->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
	m_d3dd->SetRenderState(D3DRS_DITHERENABLE,FALSE);
	m_d3dd->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	m_d3dd->SetRenderState(D3DRS_FOGENABLE,FALSE);
	m_d3dd->SetRenderState(D3DRS_COLORVERTEX,TRUE);
	m_d3dd->SetRenderState(D3DRS_LIGHTING,FALSE);
	m_d3dd->SetRenderState(D3DRS_LASTPIXEL,FALSE); // this doesn't draw the last pixel of line primitives (default was true)
	
	m_d3dd->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
//	m_d3dd->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);

	m_d3dd->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
	m_d3dd->SetTexture(0,NULL);

	// our render states
	r9Render::SetDefaultStates();

	// hide cursor in fullscreen
	m_d3dd->ShowCursor(m_cfg.m_windowed);
	unguard();
}

void r9RenderDX::Clear( dword color )
{
	guard(r9RenderDX::Clear);
	if( !m_beginendscene ) return;
	m_d3dd->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
	unguard();
}

BOOL r9RenderDX::BeginScene( R9TEXTURE target )
{
	guard(r9RenderDX::BeginScene);
	if( m_beginendscene ) return FALSE;
	m_primitivecount = 0;
	if(m_d3dd->TestCooperativeLevel()!=D3D_OK) return FALSE; // lost or something, wait for CheckDevice to aquire it back

	if( target ) // use render target
	{
		if( !target->m_handler || !target->m_handlerex ) return FALSE; // invalid target
		// remember default target
		HRESULT hr = m_d3dd->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&m_d3dtarget);
		if(FAILED(hr)) { R9_LOGERROR("can't get default render target.",hr); return FALSE; }
		// set new target
		LPDIRECT3DSURFACE9 d3dsrf = (LPDIRECT3DSURFACE9)(target->m_handlerex);
		hr = m_d3dd->SetRenderTarget(0,d3dsrf);
		if(FAILED(hr)) { R9_LOGERROR("can't set render target.",hr); srelease(m_d3dtarget); return FALSE; }

		m_targetwidth = target->m_realwidth;
		m_targetheight = target->m_realheight;
	}
	else
	{
		m_targetwidth = m_cfg.m_width;
		m_targetheight = m_cfg.m_height;
	}

	HRESULT hr = m_d3dd->BeginScene();
	if(FAILED(hr)) 
	{ 
		R9_LOGERROR("can't begin scene.",hr);
		//set back old render target
		m_d3dd->SetRenderTarget(0,m_d3dtarget);
		srelease(m_d3dtarget);
		return FALSE;
	}
	m_beginendscene = TRUE;
	return TRUE;
	unguard();
}

void r9RenderDX::EndScene()
{
	guard(r9RenderDX::EndScene);
	if( !m_beginendscene ) return;
	if(NeedFlush()) Flush();
	m_beginendscene = FALSE;
	m_d3dd->EndScene();

	if(m_d3dtarget) // restore old render target
	{
		m_d3dd->SetRenderTarget(0,m_d3dtarget);
		srelease(m_d3dtarget);
	}
	unguard();
}

void r9RenderDX::Present()
{
	guard(r9RenderDX::Present);
	if(m_d3dd->TestCooperativeLevel()!=D3D_OK) return; // lost or something
	HRESULT hr = m_d3dd->Present( NULL, NULL, m_hwnd, NULL );	
	unguard();
}

BOOL r9RenderDX::CheckDevice()
{
	guard(r9RenderDX::CheckDevice);
	
	HRESULT hr = m_d3dd->TestCooperativeLevel();
	if( hr==D3D_OK ) return TRUE; // everything is fine
	if( hr!=D3DERR_DEVICENOTRESET ) { sys_sleep(100); return FALSE; } // can't reset now

	// store and release all unmanaged video resources
	D3D_HandleReset(0); 

	// reset device
	while(!D3D_Reset()) sys_sleep(100); // if any weird error, try again

	// restore video resources
	D3D_HandleReset(1);	

	return TRUE; // success
	unguard();
}

//@OBSOLETE
BOOL r9RenderDX::ToggleVideoMode()
{
	guard(r9RenderDX::ToggleVideoMode);
	if(m_d3dd->TestCooperativeLevel()!=D3D_OK ) return FALSE;
	dlog(LOGRND, "Toggle video mode.\n");

	m_cfg.m_windowed = !m_cfg.m_windowed;
	PrepareWindow(); // prepare for setting style

	D3D_HandleReset(0); // store
	if(!D3D_Reset()) // reset
	{
		m_cfg.m_windowed = !m_cfg.m_windowed;
		// leave it to the check device to reset after failure (can't even restore resources)
		return FALSE;
	}
	D3D_HandleReset(1); // restore

	//@REM sys_sleep(100); // wait a little bit
	PrepareWindow(); // prepare again for correct resize
	return TRUE;

	unguard();
}

void r9RenderDX::Push( r9Vertex* vx, int vxs, int primitive )
{
	guard(r9RenderDX::Push);
	// set primitive
	if(GetState(R9_STATE_PRIMITIVE)!=primitive)
		SetState(R9_STATE_PRIMITIVE,primitive);

	// push
	int primitivevertexes = primitive ? 3 : 2;
	int batchsize = (R9_BATCHSIZE_DX / primitivevertexes) * primitivevertexes; // make multiple of primitive vertexes
	float ofs = (primitive==R9_PRIMITIVE_LINE) ? 0.0f : 0.5f; // pixel offset
	float scrw2 = 2.0f/(float)m_targetwidth;
	float scrh2 = 2.0f/(float)m_targetheight;
	while(vxs>0)
	{
		// get count
		int count = vxs;
		if( m_batchcount + count > batchsize )
			count = batchsize - m_batchcount;

		// copy
		r9VertexDX* vxdx = m_batchbuffer + m_batchcount;
		for(int i=0;i<count;i++)
		{
			vxdx->x =  (vx->x+m_viewx-ofs)*scrw2-1.0f;
			vxdx->y = -(vx->y+m_viewy-ofs)*scrh2+1.0f;
			vxdx->z = 0.0f;
			vxdx->u = vx->u;
			vxdx->v = vx->v;
			vxdx->color = vx->color;
			vxdx++;
			vx++;
		}
		vxs -= count;
		m_batchcount += count;
		m_needflush = TRUE;

		// flush if full
		if( m_batchcount==batchsize ) Flush();
	}
	
	unguard();
}

void r9RenderDX::Flush()
{
	guard(r9RenderDX::Flush);
	HRESULT hr;
	m_needflush = FALSE;
	if(m_batchcount==0) return;

	// unlock
	if(!D3D_BatchUnlock()) { m_batchcount=0; return; }

	// set stream
	hr = m_d3dd->SetStreamSource(0,m_batchd3d,0,sizeof(r9VertexDX));
	if(FAILED(hr)) { m_batchcount=0; D3D_BatchLock(); return; }

	// draw
	int primitive = GetState(R9_STATE_PRIMITIVE);
	int primitivecount = m_batchcount / (primitive ? 3 : 2);
	hr = m_d3dd->DrawPrimitive(	primitive ? D3DPT_TRIANGLELIST : D3DPT_LINELIST, 0, primitivecount  );
	if(!FAILED(hr)) m_primitivecount += primitivecount;

	// lock
	D3D_BatchLock();
	m_batchcount = 0;
	unguard();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// SCREEN SHOT
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL r9RenderDX::SaveScreenShot( fRect* rect, BOOL full )
{
	guard(r9RenderDX::SaveScreenShot)
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

BOOL r9RenderDX::TakeScreenShot( r9Img* img, fRect* rect, BOOL full )
{
	guard(r9RenderDX::TakeScreenShot)
	HRESULT	hr;

	if(img==NULL) return FALSE;
	if( IsBeginEndScene() ) { dlog(LOGRND, "ScreenShot can not be taken inside Begin - End frame.\n"); return FALSE; }
	if( m_cfg.m_bpp!=32 ) 	{ dlog(LOGRND, "ScreenShot can be taken only in 32bit modes.\n"); return FALSE; }

	int		srfw,srfh;	// surface size
	RECT	r;			// rectangle in surface

	R9_ImgDestroy(img);

	if(full)
	{
		D3DDISPLAYMODE d3dmode;
		hr = m_d3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3dmode );
		if(FAILED(hr)) { R9_LOGERROR("failed to get current display mode.",hr); return FALSE; }
		r.left = 0; 
		r.top = 0;
		img->m_width = r.right = srfw = (word)d3dmode.Width;
		img->m_height = r.bottom = srfh = (word)d3dmode.Height;
	}
	else
	{
		if(rect)
		{
			r.left = (int)rect->left;
			r.top = (int)rect->top;
			r.right = (int)rect->right;
			r.bottom = (int)rect->bottom;
			srfw =	GetWidth();
			srfh = GetHeight();
			img->m_width = (word)(rect->right-rect->left);
			img->m_height = (word)(rect->bottom-rect->top);
		}
		else
		{
			r.left = 0;
			r.top = 0;
			img->m_width = r.right = srfw = GetWidth();
			img->m_height = r.bottom = srfh = GetHeight();
		}
	}

	img->m_pf = R9_PF_RGB;

	
	LPDIRECT3DSURFACE9	dxsrf;
	D3DLOCKED_RECT		dxlockdata;

	if(full) // directly from screen; whole full screen is taken
	{
		hr = m_d3dd->CreateOffscreenPlainSurface( srfw, srfh, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &dxsrf, NULL ); 
		if( FAILED(hr) ) return FALSE;

		hr = m_d3dd->GetFrontBufferData(0,dxsrf);
		if( FAILED(hr) ) { srelease(dxsrf); return FALSE; }
	}
	else // from backbuffer; copy in other surf because bkbuffer may not be lockable
	{
		
		hr = m_d3dd->CreateOffscreenPlainSurface( srfw, srfh, m_pfdisplay, D3DPOOL_SYSTEMMEM, &dxsrf, NULL ); 
		if( FAILED(hr) ) return FALSE;

		LPDIRECT3DSURFACE9 bksrf;
		hr = m_d3dd->GetRenderTarget(0,&bksrf);
		if(FAILED(hr)) { srelease(dxsrf); return FALSE; }
		hr = m_d3dd->GetRenderTargetData(bksrf,dxsrf);
		if(FAILED(hr)) { srelease(bksrf); srelease(dxsrf); return FALSE; }
		srelease(bksrf);
	}

	// lock dx surf
	memset( &dxlockdata, 0, sizeof(dxlockdata) );
	hr = dxsrf->LockRect( &dxlockdata, &r, D3DLOCK_READONLY );
	if( FAILED(hr) ) { srelease(dxsrf); return FALSE; }

	// copy pixels 32bit > 24bit
	if(R9_ImgCreate(img))
	{
		int m = 0;
		for( int i=r.top; i<r.bottom; i++ )
		{
			for( int j=r.left; j<r.right; j++ )
			{
				img->m_data[m+0] = *( (byte*)dxlockdata.pBits + i*dxlockdata.Pitch + j*4 + 0 ); //B
				img->m_data[m+1] = *( (byte*)dxlockdata.pBits + i*dxlockdata.Pitch + j*4 + 1 ); //G
				img->m_data[m+2] = *( (byte*)dxlockdata.pBits + i*dxlockdata.Pitch + j*4 + 2 ); //R
				m+=3;
			}
		}
	}

	// unlock and release
	dxsrf->UnlockRect();
	srelease(dxsrf);

	return TRUE;
	unguard()
}

BOOL r9RenderDX::CopyTargetToImage( R9TEXTURE target, r9Img* img, fRect* rect )
{
	guard(r9RenderDX::CopyTargetToImage);
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
	
	// target surface
	LPDIRECT3DSURFACE9 dsback = (LPDIRECT3DSURFACE9)(target->m_handlerex);
	if(dsback==NULL) return FALSE;
	D3DSURFACE_DESC desc;
	dsback->GetDesc(&desc);
	if(desc.Format!=D3DFMT_X8R8G8B8) return FALSE; // only 32 bit format
		
	// temp surface (lockable)
	LPDIRECT3DSURFACE9 dstemp;
	D3DFORMAT pf = desc.Format;
	HRESULT hr = m_d3dd->CreateOffscreenPlainSurface( target->m_realwidth, target->m_realheight, pf, D3DPOOL_SYSTEMMEM, &dstemp, NULL );
	if(FAILED(hr)) return FALSE;
	
	// copy rect into temp srf	
	hr = m_d3dd->GetRenderTargetData( dsback, dstemp );
	if(FAILED(hr)) { dstemp->Release(); return FALSE; }

	// lock tmp srf
	D3DLOCKED_RECT lockrect;
	hr = dstemp->LockRect( &lockrect, NULL, D3DLOCK_READONLY  );
	if(FAILED(hr)) { dstemp->Release(); return FALSE; }

	// bitblt from tmp srf to img (assume width*spp = pitch)
	r9Img imgtmp;
	imgtmp.m_pf		= R9_PF_ARGB;
	imgtmp.m_width	= target->m_realwidth;
	imgtmp.m_height	= target->m_realheight;
	imgtmp.m_size	= lockrect.Pitch * target->m_realheight;
	imgtmp.m_data	= (byte*)lockrect.pBits;
	// R9_ImgSaveFile(sprint("map_%02i_%02i.png",y/h,x/w),&imgtmp); // test
	int wc = w; if(x+wc>img->m_width)	wc=img->m_width-x;
	int hc = h; if(y+hc>img->m_height)	hc=img->m_height-y;
	if(!R9_ImgBitBltSafe(&imgtmp,0,0,wc,hc,img,x,y)) { dstemp->Release(); return FALSE; }

	// unlock tmp srf
	hr = dstemp->UnlockRect();
	dstemp->Release();

	return TRUE;
	unguard();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
///////////////////////////////////////////////////////////////////////////////////////////////////
void r9RenderDX::PrepareWindow()
{
	guard(r9RenderDX::PrepareWindow);
	if( m_cfg.m_windowed )
	{
		int scrw = sys_desktopwidth();
		int scrh = sys_desktopheight();
		BOOL fulldesktop = (m_cfg.m_width==scrw) || (m_cfg.m_height==scrh);
		int cx = (scrw-m_cfg.m_width) / 2;
		int cy = (scrh-m_cfg.m_height) / 2;
		RECT rec = {cx,cy,cx+m_cfg.m_width,cy+m_cfg.m_height};
		long style = fulldesktop ? (WS_POPUP|WS_SYSMENU) : (WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX|WS_SIZEBOX));
		AdjustWindowRectEx( &rec, style, FALSE, 0 );
		MoveWindow( m_hwnd, rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top, TRUE );
		SetWindowLong( m_hwnd, GWL_STYLE, style );
		SetWindowPos( m_hwnd, HWND_NOTOPMOST,
                      rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top,
                      SWP_SHOWWINDOW|SWP_FRAMECHANGED );
	}
	else
	{
		RECT rec = {0,0,m_cfg.m_width,m_cfg.m_height};
		long style = WS_POPUP|WS_SYSMENU|WS_VISIBLE;
		MoveWindow( m_hwnd, rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top, TRUE );
		SetWindowLong( m_hwnd, GWL_STYLE, style );
		SetWindowPos( m_hwnd, HWND_TOPMOST,
                      rec.left, rec.top, rec.right-rec.left, rec.bottom-rec.top,
                      SWP_SHOWWINDOW|SWP_FRAMECHANGED );
	}
	RECT r;
	GetWindowRect(m_hwnd,&r);
	dlog(LOGRND,"window size %ix%i\n",r.right-r.left,r.bottom-r.top);
	unguard();
}

void r9RenderDX::D3D_GetPresentParams( D3DPRESENT_PARAMETERS* d3dparam )
{
	guard(r9RenderDX::D3D_GetPresentParams)
	sassert(d3dparam!=NULL);
	memset(d3dparam,0,sizeof(D3DPRESENT_PARAMETERS));
	d3dparam->BackBufferWidth					= m_cfg.m_width;
	d3dparam->BackBufferHeight					= m_cfg.m_height;
	d3dparam->BackBufferFormat					= m_pfdisplay;
	d3dparam->BackBufferCount					= 1;
	d3dparam->MultiSampleType					= D3DMULTISAMPLE_NONE;
	d3dparam->SwapEffect						= m_cfg.m_windowed ? D3DSWAPEFFECT_COPY : D3DSWAPEFFECT_FLIP;
	d3dparam->hDeviceWindow						= m_hwnd;
	d3dparam->Windowed							= m_cfg.m_windowed;
	d3dparam->EnableAutoDepthStencil			= FALSE;
	d3dparam->AutoDepthStencilFormat			= D3DFMT_UNKNOWN;
	d3dparam->Flags								= 0;
	d3dparam->FullScreen_RefreshRateInHz		= m_cfg.m_windowed ? 0 : m_cfg.m_refresh;
	d3dparam->PresentationInterval				= m_cfg.m_vsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE;
	unguard()
}

BOOL r9RenderDX::D3D_CreateDevice()
{
	guard(r9RenderDX::D3D_CreateDevice)
	sassert(m_d3d);
	sassert(!m_d3dd);
	HRESULT hr;

	// caps
	D3DCAPS9 d3dcaps;
	hr = m_d3d->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps );
	if(FAILED(hr)) { R9_LOGERROR("GetDeviceCaps failed.",hr); return FALSE; }
	m_caps.m_tnl			= d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT;
	m_caps.m_texsquareonly	= (d3dcaps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)!=0;
	m_caps.m_texaspectratio	= d3dcaps.MaxTextureAspectRatio;
	m_caps.m_texwidth		= d3dcaps.MaxTextureWidth;
	m_caps.m_texheight		= d3dcaps.MaxTextureHeight;

	// create
	D3DPRESENT_PARAMETERS d3dparam;
	D3D_GetPresentParams(&d3dparam);
	dword flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	if( m_caps.m_tnl )	flags = D3DCREATE_MIXED_VERTEXPROCESSING; // or D3DCREATE_HARDWARE_VERTEXPROCESSING;
	hr = m_d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd, flags, &d3dparam, &m_d3dd );
	if(FAILED(hr)) { R9_LOGERROR("CreateDevice failed.",hr); return FALSE; }

	return TRUE;
	unguard()
}

BOOL r9RenderDX::D3D_Reset()
{
	guard(r9RenderDX::D3D_Reset);
	dlog(LOGRND,"RENDER: reset device.\n");
	D3DPRESENT_PARAMETERS d3dparam;
	D3D_GetPresentParams(&d3dparam);
	HRESULT hr = m_d3dd->Reset(&d3dparam);
	if(FAILED(hr)) { R9_LOGERROR("reset device failure.",hr); return FALSE; }
	return TRUE;
	unguard();
}

void r9RenderDX::D3D_HandleReset( int mode )
{
	guard(r9RenderDX::D3D_HandleReset);
	if(mode==0) // store and release
	{
		if(m_batchd3d) { D3D_BatchUnlock();	srelease(m_batchd3d); } // vertex buffer
		TT_Release();
	}
	else // restore
	{
		if(!D3D_BatchCreate()) { errorexit("Can't recover buffer from lost device."); } // vertex buffer
		TT_Recreate();

		// restore render states
		int i;
		int state[R9_STATES];
		for(i=0;i<R9_STATES;i++) state[i] = m_state[i];
		SetDefaultStates();
		for(i=0;i<R9_STATES;i++) SetState(i,state[i]);

		// notify user, to repaint content of render targets (the rest is handeled here)
		if(m_handlereset) m_handlereset();
	}
	unguard();
}

BOOL r9RenderDX::D3D_BatchCreate()
{
	guard(r9RenderDX::D3D_BatchCreate);
	dword fvf = D3DFVF_XYZ | D3DFVF_DIFFUSE |D3DFVF_TEX1;
	HRESULT hr = m_d3dd->CreateVertexBuffer( R9_BATCHSIZE_DX*sizeof(r9VertexDX), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, fvf, D3DPOOL_DEFAULT, &m_batchd3d, NULL );
	if(FAILED(hr)) { R9_LOGERROR("create vertex buffer failed.", hr); return FALSE; }
	if(!D3D_BatchLock()) { R9_LOGERROR("vertex buffer lock failed.", hr); srelease(m_batchd3d); return FALSE; }
	memset(m_batchbuffer,0,R9_BATCHSIZE_DX*sizeof(r9VertexDX)); // clear
	return TRUE;
	unguard();
}

BOOL r9RenderDX::D3D_BatchLock()
{
	guard(r9RenderDX::D3D_BatchLock);
	if(m_batchbuffer!=NULL) return FALSE;
	m_batchcount = 0;
	void* data = NULL;
	HRESULT hr = m_batchd3d->Lock(0,0,&data,D3DLOCK_DISCARD);
	if(FAILED(hr)) return FALSE;
	m_batchbuffer = (r9VertexDX*)data;
	return TRUE;
	unguard();
}

BOOL r9RenderDX::D3D_BatchUnlock()
{
	guard(r9RenderDX::D3D_BatchUnlock);
	if(m_batchbuffer==NULL) return FALSE;
	HRESULT hr = m_batchd3d->Unlock();
	if(FAILED(hr)) return FALSE;
	m_batchbuffer = NULL;
	return TRUE;
	unguard();
}

r9PFInfo* r9RenderDX::D3D_PFInfo( D3DFORMAT d3dpf )
{
	guard(r9RenderDX::D3D_PFInfo);
	static r9PFInfo d3dpfinfo[] =
	{
		// D3DFMT_R5G6B5
		{ 16, 2, {5,6,5,0}, {0x0000001f,0x000007e0,0x0000f800,0x00000000}, { 0, 5,11, 0 }, "R5G6B5" },
		// D3DFMT_A4R4G4B4
		{ 16, 2, {4,4,4,4}, {0x0000000f,0x000000f0,0x00000f00,0x0000f000}, { 0, 4, 8,12 }, "A4R4G4B4" },
		// D3DFMT_X8R8G8B8
		{ 32, 4, {8,8,8,0}, {0x000000ff,0x0000ff00,0x00ff0000,0x00000000}, { 0, 8,16, 0 }, "X8R8G8B8" },
		// D3DFMT_A8R8G8B8
		{ 32, 4, {8,8,8,8}, {0x000000ff,0x0000ff00,0x00ff0000,0xff000000}, { 0, 8,16,24 }, "A8R8G8B8" }
	};
	switch(d3dpf)
	{
		case D3DFMT_R5G6B5:		return &d3dpfinfo[0];
		case D3DFMT_A4R4G4B4:	return &d3dpfinfo[1];
		case D3DFMT_X8R8G8B8:	return &d3dpfinfo[2];
		case D3DFMT_A8R8G8B8:	return &d3dpfinfo[3];
	}
	return NULL;
	unguard();
}

void r9RenderDX::TT_Add(R9TEXTURE texture)
{
	guardfast(r9RenderDX::TT_Add);
	m_targetlist.Add(texture);
	unguardfast();
}
void r9RenderDX::TT_Del(R9TEXTURE texture)
{
	guardfast(r9RenderDX::TT_Del);
	for(int i=0;i<m_targetlist.Size();i++)
	{
		if(m_targetlist[i]==texture)
		{
			m_targetlist.Del(i);
			return;
		}
	}
	unguardfast();
}

void r9RenderDX::TT_Release()
{
	guard(r9RenderDX::TT_Release);
	for(int i=0;i<m_targetlist.Size();i++)
	{
		LPDIRECT3DTEXTURE9 d3dtex = (LPDIRECT3DTEXTURE9)(m_targetlist[i]->m_handler);
		LPDIRECT3DSURFACE9 d3dsrf = (LPDIRECT3DSURFACE9)(m_targetlist[i]->m_handlerex);
		if(d3dsrf) srelease(d3dsrf);
		if(d3dtex) srelease(d3dtex);
		m_targetlist[i]->m_handler = NULL;
		m_targetlist[i]->m_handlerex = NULL;
	}
	unguard();
}

void r9RenderDX::TT_Recreate()
{
	guard(r9RenderDX::TT_Recreate);
	for(int i=0;i<m_targetlist.Size();i++)
	{
		// manareli...
		// create new temporary tex (will add it in targetlist too!)
		R9TEXTURE ttex = TextureCreateTarget(m_targetlist[i]->m_width,m_targetlist[i]->m_height);
		if(ttex==NULL) { errorexit("Can't recover render target, from lost device."); }
		// force content into the old tex pointer (that was cleared before reset)
		*m_targetlist[i] = *ttex;
		// remove last entry in targetlist (the temporary new tex)
		sverify(m_targetlist[m_targetlist.Size()-1]==ttex);
		m_targetlist.Del(m_targetlist.Size()-1);
		// delete temporary new tex pointer
		sdelete(ttex);
	}
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
