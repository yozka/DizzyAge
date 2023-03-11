///////////////////////////////////////////////////////////////////////////////////////////////////
// R9RenderDX.h
// Render platform DirectX implementation
// Control:
// R9_ENABLE_DLLDX
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __R9RENDERDX_H__
#define __R9RENDERDX_H__

#include <d3d9.h>
#include "R9Render.h"
#include "E9List.h"

#define R9_BATCHSIZE_DX		(3*64)	// number of vertices in the batch buffer

struct r9VertexDX
{
	float x,y,z;
	dword color;
	float u,v;
};

struct r9CapsDX
{
	BOOL	m_tnl;					// hardware vertex processing
	BOOL	m_texsquareonly;		// only square textures supported
	int		m_texaspectratio;		// min aspect ration for textures
	int		m_texwidth;				// max texture width
	int		m_texheight;			// max texture height
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class r9RenderDX : public r9Render
{
public:
					r9RenderDX();
virtual				~r9RenderDX();

virtual	BOOL		LoadDll();
virtual	void		UnloadDll();
virtual	int			GatherDisplayModes( r9DisplayMode* displaymode );

virtual	BOOL		Init( HWND hwnd, r9Cfg* cfg );
virtual	void		Done();
virtual	BOOL		IsReady();

virtual	R9TEXTURE	TextureCreate( r9Img* img );
virtual	R9TEXTURE	TextureCreateTarget( int width, int height );
virtual	void		TextureDestroy( R9TEXTURE tex );

virtual	void		SetTexture( R9TEXTURE tex );
virtual void		SetState( int state, int value );
virtual	void		SetViewport( fRect& rect );
virtual	void		SetView( int x, int y, dword flip );
virtual	void		SetDefaultStates();

virtual	void		Clear( dword color );
virtual	BOOL		BeginScene( R9TEXTURE target=NULL );
virtual	void		EndScene();
virtual	void		Present();
virtual	BOOL		CheckDevice();
virtual	BOOL		ToggleVideoMode();

virtual	void		Push( r9Vertex* vx, int vxs, int primitive );
virtual	void		Flush();

virtual	BOOL		SaveScreenShot( fRect* rect=NULL, BOOL full=TRUE);								// auto save screenshot
virtual BOOL		TakeScreenShot( r9Img* img, fRect* rect=NULL, BOOL full=TRUE );					// shoots full screen or backbuffer (create img)
virtual BOOL		CopyTargetToImage( R9TEXTURE target, r9Img* img, fRect* rect );

protected:

		void				PrepareWindow();										// set window style
		void				D3D_GetPresentParams( D3DPRESENT_PARAMETERS* d3dparam );// 
		BOOL				D3D_CreateDevice();										// create a direct3d device
		BOOL				D3D_Reset();											// reset device; before call, clear all device dependent and unmanaged data; return true if device reseted so restore data
		void				D3D_HandleReset( int mode=0 );							// mode: 0=before, 1=after
		BOOL				D3D_BatchCreate();										// create buffer for batching
		BOOL				D3D_BatchLock();										// lock 
		BOOL				D3D_BatchUnlock();										// unlock
static	r9PFInfo*			D3D_PFInfo( D3DFORMAT d3dpf );							// return a r9PFInfo from a dx pf, or NULL if not supported
		
		LPDIRECT3D9			m_d3d;							// direct3d
		LPDIRECT3DDEVICE9	m_d3dd;							// direct3d device
		D3DFORMAT			m_pfdisplay;					// pixel format for display (backbuffer)
		D3DFORMAT			m_pfopaque;						// pixel format for opaque textures
		D3DFORMAT			m_pfalpha;						// pixel format for alpha textures
		r9CapsDX			m_caps;							// render caps
		
		int						m_batchcount;				// no of vertexes in the batch buffer
		r9VertexDX*				m_batchbuffer;				// batch buffer
		LPDIRECT3DVERTEXBUFFER9 m_batchd3d;					// d3d dynamic batch buffer

		LPDIRECT3DSURFACE9	m_d3dtarget;					// temporary render target (kept during begin-end scene)
		int					m_targetwidth;					// render target width
		int					m_targetheight;					// render target height
		cList<R9TEXTURE>	m_targetlist;					// management list with textures created with render target support (these are not managed by DX, when device is lost)
		void				TT_Add(R9TEXTURE texture);		// add a render target texture
		void				TT_Del(R9TEXTURE texture);		// del a render target texture
		void				TT_Release();
		void				TT_Recreate();

// DLL functions
typedef IDirect3D9* (__stdcall * tDirect3DCreate9)(UINT SDKVersion);
static	tDirect3DCreate9	m_Direct3DCreate9;
};

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
