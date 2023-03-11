///////////////////////////////////////////////////////////////////////////////////////////////////
// R9Render.h
// Render
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __R9RENDER_H__
#define __R9RENDER_H__

#include "E9System.h"
#include "E9Engine.h"
#include "D9Debug.h"
#include "R9Img.h"
#include "E9Math.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
#define R9_API_DIRECTX			0
#define R9_API_OPENGL			1
#define R9_API_DEFAULT			R9_API_DIRECTX

#define R9_STATE_PRIMITIVE		0		// primitive type
#define R9_STATE_BLEND			1		// blending mode
#define R9_STATE_TADDRESS		2		// texture address mode
#define R9_STATE_FILTER			3		// filter on/off
#define	R9_STATES				5		// dummy

#define	R9_PRIMITIVE_LINE		0		// line primitive with 2 vertexes
#define	R9_PRIMITIVE_TRIANGLE	1		// triangle primitive with 3 vertexes

#define	R9_BLEND_OPAQUE			0		// no alpha
#define	R9_BLEND_ALPHA			1		// alpha blend
#define	R9_BLEND_ADD			2		// additive
#define	R9_BLEND_MOD			3		// modulative
#define	R9_BLEND_MOD2			4		// decal
#define	R9_BLEND_ALPHAREP		5		// alpha replicate

#define	R9_TADDRESS_WRAP		0		// repeat texture mapping
#define	R9_TADDRESS_CLAMP		1		// clamp texture mapping

#define	R9_CFG_WIDTH			640		// default width
#define	R9_CFG_HEIGHT			480		// default height
#define	R9_CFG_BPP				32		// default bpp

#define	R9_CHRW					6		// system font chr width
#define	R9_CHRH					9		// system font chr height

#define	R9_FLIPX				(1<<0)	// flip horizontal
#define	R9_FLIPY				(1<<1)	// flip vertical
#define	R9_FLIPR				(1<<2)	// flip rotation
#define	R9_FLIPXY				(R9_FLIPX|R9_FLIPY)
#define	R9_FLIPXR				(R9_FLIPX|R9_FLIPR)
#define	R9_FLIPYR				(R9_FLIPY|R9_FLIPR)
#define	R9_FLIPXYR				(R9_FLIPXY|R9_FLIPR)

///////////////////////////////////////////////////////////////////////////////////////////////////
struct r9Vertex
{
	float x,y;
	float u,v;
	dword color;
};

struct r9Cfg
{
	int		m_windowed;		// windowed 1/0
	int		m_bpp;			// bpp 16/32 (ignored in windowed)
	int		m_width;		// resolution width
	int		m_height;		// resolution height
	int		m_refresh;		// refresh rate (0=default);
	int		m_vsync;		// vsync (0=off)
	r9Cfg();				// set defauld config values
};

struct r9DisplayMode
{
	int		m_windowed;		// windowed 1/0 (1 for current display mode)
	int		m_bpp;			// bpp 16/32
	int		m_width;		// resolution width
	int		m_height;		// resolution height
	int		m_refresh;		// refresh rate (0 for current display mode)
	dword	m_reserved1;	// reserved for platforms
};

struct r9Texture
{
	int		m_width;		// requested width
	int		m_height;		// requested height
	int		m_realwidth;	// real created width (may be greater than requested, pow2 etc)
	int		m_realheight;	// real created height (may be greater than requested, pow2 etc)
	void*	m_handler;		// platform handler
	void*	m_handlerex;	// platform handler ex
};
typedef r9Texture* R9TEXTURE;

typedef void (*r9HandleReset)();	// HandleReset is called after the device was successfully restored, so user can repaint content of render targets (targets and states are managed by render)

class r9Font;

///////////////////////////////////////////////////////////////////////////////////////////////////
class r9Render
{
public:
					r9Render();
virtual				~r9Render();

// dll
virtual	BOOL		LoadDll();
virtual	void		UnloadDll();

// preinit
virtual	int			GatherDisplayModes( r9DisplayMode* displaymode );				// fill list with valid displaymodes and return count; use NULL just for the count; first entry is the current mode (windowed) if available; @WARNING: only safe to call in windowed mode, at start

// init
virtual	BOOL		Init( HWND hwnd, r9Cfg* cfg );					// init render; if cfg is NULL, default cfg is used
virtual	void		Done();											// done render
virtual	BOOL		IsReady();										// if render is ready; avoid using render in window messages, before the device is ready
inline	int			GetWidth()										{ return m_cfg.m_width; }
inline	int			GetHeight()										{ return m_cfg.m_height; }
inline	r9Cfg&		GetCfg()										{ return m_cfg; }
inline	int			GetApi()										{ return m_api; }

// texture
virtual	R9TEXTURE	TextureCreate( r9Img* img );					// create texture from image
virtual	R9TEXTURE	TextureLoad( char* filename );					// load a texture from file
virtual	R9TEXTURE	TextureCreateTarget( int width, int height );	// create a texture with render target support
virtual	void		TextureDestroy( R9TEXTURE tex );				// destroy texture

// states
virtual	void		SetTexture( R9TEXTURE tex );					// set current texture (if different); flushes if needed
inline	R9TEXTURE	GetTexture()									{ return m_texture; }
virtual void		SetState( int state, int value );				// set a render state (if different); flushes if needed
inline	int			GetState( int state )							{ return m_state[state]; }
virtual	void		SetViewport( fRect& rect );						// set viewport rect (used as scissor)
inline	fRect&		GetViewport()									{ return m_viewport; }
virtual	void		SetView( int x, int y, dword flip );			// set view options
virtual	void		SetDefaultStates();								// set states to default values

// flow
virtual	void		Clear( dword color );							// clear backbuffer
virtual	BOOL		BeginScene( R9TEXTURE target=NULL );			// begine scene drawing; if target is valid then render in texture target
virtual	void		EndScene();										// end scene drawing
virtual	void		Present();										// present scene (flip buffers)
inline	BOOL		IsBeginEndScene()								{ return m_beginendscene; }
virtual	BOOL		CheckDevice();									// check if device is lost and if so, try to reset it
virtual	BOOL		ToggleVideoMode();								// @OBSOLETE toggle between windowed and full screen

// batch primitives
virtual	void		Push( r9Vertex* vx, int vxs, int primitive );	// push vertices in the batch buffer
virtual	void		Flush();										// flush the batch buffer
inline	BOOL		NeedFlush()										{ return m_needflush; }

// draw functions
inline	void		DrawLine( fV2& a, fV2& b, dword color=0xffffffff );
inline	void		DrawTriangle( fV2& a, fV2& b, fV2& c, fV2& ta, fV2& tb, fV2& tc, R9TEXTURE tex, dword color=0xffffffff );
inline	void		DrawBar( fRect& dst, dword color=0xffffffff );
inline	void		DrawQuad( fRect& dst, fRect& src, R9TEXTURE tex, dword color=0xffffffff );
		void		DrawQuadRot( fV2& pos, fV2& size, fV2& center, float angle, fRect& src, R9TEXTURE tex, dword color=0xffffffff ); // center is relative to the middle of the rectangle
		void		DrawSprite( fV2& pos, fRect& src, R9TEXTURE tex, dword color=0xffffffff, dword flip=0, float scale=1.0f ); // does clipping

// clipping
inline	BOOL		IsClipping()										{ return ( (m_cliprect.x1<m_cliprect.x2) && (m_cliprect.y1<m_cliprect.y2) ); }
inline	void		SetClipping( fRect& rect )							{ m_cliprect = rect; }
inline	fRect&		GetClipping()										{ return m_cliprect; }
inline 	void		ClipBar( fRect& dst );								// clip destination rect (dst must be ordered)
inline 	void		ClipQuad( fRect& dst, fRect& src );					// clip destination rect and source mapping rect (dst must be ordered, src coordinates can be flipped)
inline 	void		ClipSprite( fRect& dst, fRect& src, int flip=0 );	// clip destination rect and source mapping rect (dst and src must have the same sizes; src coordinates can't be flipped); rotation not supported

// screen shot
virtual	BOOL		SaveScreenShot( fRect* rect=NULL, BOOL full=TRUE);					// auto save screenshot
virtual BOOL		TakeScreenShot( r9Img* img, fRect* rect=NULL, BOOL full=TRUE );		// shoots full screen or backbuffer (create img)
virtual BOOL		CopyTargetToImage( R9TEXTURE target, r9Img* img, fRect* rect );		// copy the content of a texture target from (0,0) into an image at a specified rect

// font
		BOOL		CreateFont();									// creates render debug font, from source resources; call at the end of platform Init, since it requires a texture to be created

// members
		HMODULE			m_dll;				// platform dll
		HWND			m_hwnd;				// associated window
		int				m_api;				// render api (fixed per render class)
		r9Cfg			m_cfg;				// render config
		BOOL			m_beginendscene;	// if inside begin-end scene cycle
		R9TEXTURE		m_texture;			// current texture
		int				m_state[R9_STATES];	// render states
		fRect			m_viewport;			// viewport rect
		int				m_viewx;			// view x offset
		int				m_viewy;			// view y offset
		dword			m_viewflip;			// view flip option
		fRect			m_cliprect;			// clipping rect (clipping is performed if rect is valid)
		BOOL			m_needflush;		// need flush, batch buffer not empty
		int				m_primitivecount;	// rendered primitives from the scene
		r9Font*			m_font;				// render font (created from source resource)
		r9HandleReset	m_handlereset;		// on reset user callback
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// INLINES
///////////////////////////////////////////////////////////////////////////////////////////////////
inline void r9Render::DrawLine( fV2& a, fV2& b, dword color )
{
	guardfast(r9Render::DrawLine);
	if(GetTexture()!=NULL) SetTexture(NULL);

	r9Vertex vx[2];

	vx[0].x = a.x;
	vx[0].y = a.y;
	vx[0].u = 0.0f;
	vx[0].v = 0.0f;
	vx[0].color = color;

	vx[1].x = b.x;
	vx[1].y = b.y;
	vx[1].u = 1.0f;
	vx[1].v = 1.0f;
	vx[1].color = color;

	Push(vx,2,R9_PRIMITIVE_LINE);
	unguardfast();
}

inline void r9Render::DrawTriangle( fV2& a, fV2& b, fV2& c, fV2& ta, fV2& tb, fV2& tc, R9TEXTURE tex, dword color )
{
	guardfast(r9Render::DrawTriangle);
	if(GetTexture()!=tex) SetTexture(tex);

	r9Vertex vx[3];

	vx[0].x = a.x;
	vx[0].y = a.y;
	vx[0].u = ta.x;
	vx[0].v = ta.y;
	vx[0].color = color;

	vx[1].x = b.x;
	vx[1].y = b.y;
	vx[1].u = tb.x;
	vx[1].v = tb.y;
	vx[1].color = color;

	vx[2].x = c.x;
	vx[2].y = c.y;
	vx[2].u = tc.x;
	vx[2].v = tc.y;
	vx[2].color = color;

	Push(vx,3,R9_PRIMITIVE_TRIANGLE);
	unguardfast();
}

inline void r9Render::DrawQuad( fRect& dst, fRect& src, R9TEXTURE tex, dword color )
{
	guardfast(r9Render::DrawQuad);
	if(GetTexture()!=tex) SetTexture(tex);

	r9Vertex vx[6];

	vx[0].x = dst.x1;
	vx[0].y = dst.y1;
	vx[0].u = src.x1;
	vx[0].v = src.y1;
	vx[0].color = color;

	vx[1].x = dst.x2;
	vx[1].y = dst.y1;
	vx[1].u = src.x2;
	vx[1].v = src.y1;
	vx[1].color = color;

	vx[2].x = dst.x1;
	vx[2].y = dst.y2;
	vx[2].u = src.x1;
	vx[2].v = src.y2;
	vx[2].color = color;

	vx[3].x = dst.x2;
	vx[3].y = dst.y1;
	vx[3].u = src.x2;
	vx[3].v = src.y1;
	vx[3].color = color;

	vx[4].x = dst.x2;
	vx[4].y = dst.y2;
	vx[4].u = src.x2;
	vx[4].v = src.y2;
	vx[4].color = color;

	vx[5].x = dst.x1;
	vx[5].y = dst.y2;
	vx[5].u = src.x1;
	vx[5].v = src.y2;
	vx[5].color = color;

	Push(vx,6,R9_PRIMITIVE_TRIANGLE);
	unguardfast();
}

inline void r9Render::DrawBar( fRect& dst, dword color )
{
	guardfast(r9Render::DrawBar);
	DrawQuad(dst,fRect(0.0f,0.0f,1.0f,1.0f),NULL,color);
	unguardfast();
}

inline void r9Render::ClipBar( fRect& dst )
{
	guardfast(r9Render::ClipBar);
	if(!IsClipping()) return;
	
	if(dst.x1<m_cliprect.x1) dst.x1=m_cliprect.x1;	else
	if(dst.x1>m_cliprect.x2) dst.x1=m_cliprect.x2;
	
	if(dst.y1<m_cliprect.y1) dst.y1=m_cliprect.y1;	else
	if(dst.y1>m_cliprect.y2) dst.y1=m_cliprect.y2;
	
	if(dst.x2>m_cliprect.x2) dst.x2=m_cliprect.x2;	else
	if(dst.x2<m_cliprect.x1) dst.x2=m_cliprect.x1;
	
	if(dst.y2>m_cliprect.y2) dst.y2=m_cliprect.y2;	else
	if(dst.y2<m_cliprect.y1) dst.y2=m_cliprect.y1;

	unguardfast();
}

inline void r9Render::ClipQuad( fRect& dst, fRect& src )
{
	guardfast(r9Render::ClipQuad);
	if(!IsClipping()) return;
	
	float f;
	fRect dst0 = dst;
	float srcw = src.x2-src.x1;
	float srch = src.y2-src.y1;
	float dstw = dst.x2-dst.x1;
	float dsth = dst.y2-dst.y1;

	if(dst.x1<m_cliprect.x1) dst.x1=m_cliprect.x1;
	if(dst.y1<m_cliprect.y1) dst.y1=m_cliprect.y1;
	if(dst.x2>m_cliprect.x2) dst.x2=m_cliprect.x2;
	if(dst.y2>m_cliprect.y2) dst.y2=m_cliprect.y2;

	if(dstw>0.0f) f=(dst.x1 - dst0.x1)/dstw; else f=0.0f;
	src.x1 += srcw*f;

	if(dstw>0.0f) f=(dst0.x2 - dst.x2)/dstw; else f=0.0f;
	src.x2 -= srcw*f;

	if(dsth>0.0f) f=(dst.y1 - dst0.y1)/dsth; else f=0.0f;
	src.y1 += srch*f;

	if(dsth>0.0f) f=(dst0.y2 - dst.y2)/dsth; else f=0.0f;
	src.y2 -= srch*f;

	unguardfast();
}

inline void r9Render::ClipSprite( fRect& dst, fRect& src, int flip )
{
	guardfast(r9Render::ClipSprite);
	if(!IsClipping()) return;
	if(flip & 1)
	{
		if(dst.x1<m_cliprect.x1) { src.x2-=(m_cliprect.x1-dst.x1); dst.x1=m_cliprect.x1; }
		if(dst.x2>m_cliprect.x2) { src.x1+=(dst.x2-m_cliprect.x2); dst.x2=m_cliprect.x2; }
	}
	else
	{
		if(dst.x1<m_cliprect.x1) { src.x1+=(m_cliprect.x1-dst.x1); dst.x1=m_cliprect.x1; }
		if(dst.x2>m_cliprect.x2) { src.x2-=(dst.x2-m_cliprect.x2); dst.x2=m_cliprect.x2; }
	}
	if(flip & 2)
	{
		if(dst.y1<m_cliprect.y1) { src.y2-=(m_cliprect.y1-dst.y1); dst.y1=m_cliprect.y1; }
		if(dst.y2>m_cliprect.y2) { src.y1+=(dst.y2-m_cliprect.y2); dst.y2=m_cliprect.y2; }
	}
	else
	{
		if(dst.y1<m_cliprect.y1) { src.y1+=(m_cliprect.y1-dst.y1); dst.y1=m_cliprect.y1; }
		if(dst.y2>m_cliprect.y2) { src.y2-=(dst.y2-m_cliprect.y2); dst.y2=m_cliprect.y2; }
	}
	unguardfast();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
extern r9Render* r9_render;
		
		BOOL		R9_InitInterface( int api=R9_API_DEFAULT );
		void		R9_GetDisplayModes( r9DisplayMode* &displaymode, int& displaymodecount );		// available after init interface
		BOOL		R9_FilterCfg( r9Cfg& cfg, int& api );														// available after init interface
		void		R9_LogCfg( r9Cfg& cfg, int api );
		void		R9_DoneInterface();

		BOOL		R9_Init( HWND hwnd, r9Cfg* cfg, int api=R9_API_DEFAULT );
		void		R9_Done();
inline	BOOL		R9_IsReady()											{ guardfast(R9_IsReady);			return (r9_render && r9_render->IsReady()); unguardfast(); }
inline	int			R9_GetWidth()											{ guardfast(R9_GetWidth);			sassert(r9_render); return r9_render->GetWidth(); unguardfast(); }
inline	int			R9_GetHeight()											{ guardfast(R9_GetHeight);			sassert(r9_render); return r9_render->GetHeight(); unguardfast(); }
inline	r9Cfg&		R9_GetCfg()												{ guardfast(R9_GetCfg);				sassert(r9_render); return r9_render->GetCfg(); unguardfast(); }
inline	int			R9_GetApi()												{ guardfast(R9_GetApi);				sassert(r9_render); return r9_render->GetApi(); unguardfast(); }
inline	void		R9_SetHandleReset( r9HandleReset callback )				{ guardfast(R9_OnReset);			sassert(r9_render); r9_render->m_handlereset = callback; unguardfast(); }

inline	R9TEXTURE	R9_TextureCreate( r9Img* img )							{ guardfast(R9_TextureCreate);		sassert(r9_render); return r9_render->TextureCreate(img); unguardfast(); }
inline	R9TEXTURE	R9_TextureLoad( char* filename )						{ guardfast(R9_TextureLoad);		sassert(r9_render); return r9_render->TextureLoad(filename); unguardfast(); }
inline	R9TEXTURE	R9_TextureCreateTarget( int width, int height )			{ guardfast(R9_TextureCreateTarget);sassert(r9_render); return r9_render->TextureCreateTarget(width,height); unguardfast(); }
inline	void		R9_TextureDestroy( R9TEXTURE tex )						{ guardfast(R9_TextureDestroy); 	sassert(r9_render); r9_render->TextureDestroy(tex); unguardfast(); }
inline	BOOL		R9_TextureIsValid( R9TEXTURE tex )						{ return tex->m_handler!=NULL; }
inline	int			R9_TextureGetWidth( R9TEXTURE tex )						{ return tex->m_width; }
inline	int			R9_TextureGetHeight( R9TEXTURE tex )					{ return tex->m_height; }
inline	int			R9_TextureGetRealWidth( R9TEXTURE tex )					{ return tex->m_realwidth; }
inline	int			R9_TextureGetRealHeight( R9TEXTURE tex )				{ return tex->m_realheight; }

inline	void		R9_SetTexture( R9TEXTURE tex )							{ guardfast(R9_SetTexture);			sassert(r9_render); r9_render->SetTexture(tex); unguardfast(); }
inline	R9TEXTURE	R9_GetTexture()											{ guardfast(R9_GetTexture);			sassert(r9_render); return r9_render->GetTexture(); unguardfast(); }
inline	void		R9_SetState( int state, int value )						{ guardfast(R9_SetState);			sassert(r9_render); r9_render->SetState(state,value); unguardfast(); }
inline	int			R9_GetState( int state )								{ guardfast(R9_GetState);			sassert(r9_render); return r9_render->GetState(state); unguardfast(); }
inline	void		R9_SetViewport( fRect& rect )							{ guardfast(R9_SetViewport);		sassert(r9_render); r9_render->SetViewport(rect); unguardfast(); }
inline	fRect&		R9_GetViewport()										{ guardfast(R9_GetViewport);		sassert(r9_render); return r9_render->GetViewport(); unguardfast(); }
inline	void		R9_SetView( int x, int y, dword flip )					{ guardfast(R9_SetView);			sassert(r9_render); r9_render->SetView(x,y,flip); unguardfast(); }

inline	void		R9_Clear( dword color )									{ guardfast(R9_Clear);				sassert(r9_render); r9_render->Clear(color); unguardfast(); }
inline	BOOL		R9_BeginScene( R9TEXTURE target=NULL )					{ guardfast(R9_BeginScene);			sassert(r9_render); return r9_render->BeginScene( target ); unguardfast(); }
inline	void		R9_EndScene()											{ guardfast(R9_EndScene);			sassert(r9_render); r9_render->EndScene(); unguardfast(); }
inline	void		R9_Present()											{ guardfast(R9_Present);			sassert(r9_render); r9_render->Present(); unguardfast(); }
inline	BOOL		R9_IsBeginEndScene()									{ guardfast(R9_IsBeginEndScene);	sassert(r9_render); return r9_render->IsBeginEndScene(); unguardfast(); }
inline	BOOL		R9_CheckDevice()										{ guardfast(R9_CheckDevice);		sassert(r9_render); return r9_render->CheckDevice(); unguardfast(); }
inline	BOOL		R9_ToggleVideoMode()									{ guardfast(R9_ToggleVideoMode);	sassert(r9_render); return r9_render->ToggleVideoMode(); unguardfast(); }

inline	void		R9_Push( r9Vertex* vx, int vxs, int primitive )			{ guardfast(R9_Push);		sassert(r9_render); r9_render->Push(vx,vxs,primitive); unguardfast(); }
inline	void		R9_Flush()												{ guardfast(R9_Flush);		sassert(r9_render); r9_render->Flush(); unguardfast(); }
inline	BOOL		R9_NeedFlush()											{ guardfast(R9_NeedFlush);	sassert(r9_render); return r9_render->NeedFlush(); unguardfast(); }

inline	void		R9_DrawLine( fV2& a, fV2& b, dword color=0xffffffff )	{ guardfast(R9_DrawLine); sassert(r9_render); r9_render->DrawLine(a,b,color); unguardfast(); }
inline	void		R9_DrawTriangle( fV2& a, fV2& b,fV2& c, fV2& ta, fV2& tb, fV2& tc, R9TEXTURE tex, dword color=0xffffffff ) { guardfast(R9_DrawTriangle); sassert(r9_render); r9_render->DrawTriangle(a,b,c,ta,tb,tc,tex,color); unguardfast(); }
inline	void		R9_DrawBar( fRect& dst, dword color=0xffffffff ) { guardfast(R9_DrawBar); sassert(r9_render); r9_render->DrawBar(dst,color); unguardfast(); }
inline	void		R9_DrawQuad( fRect& dst, fRect& src, R9TEXTURE tex, dword color=0xffffffff ) { guardfast(R9_DrawQuad); sassert(r9_render); r9_render->DrawQuad(dst,src,tex,color); unguardfast(); }
inline	void		R9_DrawQuadRot( fV2& pos, fV2& size, fV2& center, float angle, fRect& src, R9TEXTURE tex, dword color=0xffffffff ) { guardfast(R9_DrawQuadRot); sassert(r9_render); r9_render->DrawQuadRot(pos,size,center,angle,src,tex,color); unguardfast(); }
inline	void		R9_DrawSprite( fV2& pos, fRect& src, R9TEXTURE tex, dword color=0xffffffff, dword flip=0, float scale=1.0f ) { guardfast(R9_DrawSprite); sassert(r9_render); r9_render->DrawSprite(pos,src,tex,color,flip,scale); unguardfast(); }
		void		R9_DrawText( fV2& pos, char* text, dword color=0xffffffff, float scale=1.0f );

inline	BOOL		R9_IsClipping()											{ guardfast(R9_GetClipRect);	sassert(r9_render); return r9_render->IsClipping(); unguardfast(); }
inline	void		R9_SetClipping( fRect& rect )							{ guardfast(R9_SetClipRect);	sassert(r9_render); r9_render->SetClipping(rect); unguardfast(); }
inline	fRect&		R9_GetClipping()										{ guardfast(R9_GetClipRect);	sassert(r9_render); return r9_render->GetClipping(); unguardfast(); }
inline	void		R9_ResetClipping()										{ guardfast(R9_SetClipRect);	sassert(r9_render); r9_render->SetClipping(frect_0); unguardfast(); }
inline	void		R9_AddClipping( fRect& rect )							{ guardfast(R9_SetClipRect);	sassert(r9_render); if(!r9_render->IsClipping()) r9_render->SetClipping(rect); else r9_render->SetClipping(r9_render->GetClipping() * rect); unguardfast(); }
inline 	void		R9_ClipBar( fRect& dst )								{ guardfast(R9_ClipBar);		sassert(r9_render); r9_render->ClipBar(dst); unguardfast(); }
inline 	void		R9_ClipQuad( fRect& dst, fRect& src )					{ guardfast(R9_ClipQuad);		sassert(r9_render); r9_render->ClipQuad(dst,src); unguardfast(); }
inline 	void		R9_ClipSprite( fRect& dst, fRect& src, int flip=0 )		{ guardfast(R9_ClipSprite);		sassert(r9_render); r9_render->ClipSprite(dst,src,flip); unguardfast(); }

inline	BOOL		R9_SaveScreenShot( fRect* rect=NULL, BOOL full=TRUE)				{ guardfast(R9_SaveScreenShot);	sassert(r9_render); return r9_render->SaveScreenShot(rect,full); unguardfast(); }
inline	BOOL		R9_TakeScreenShot( r9Img* img, fRect* rect=NULL, BOOL full=TRUE )	{ guardfast(R9_TakeScreenShot);	sassert(r9_render); return r9_render->TakeScreenShot(img,rect,full); unguardfast(); }
inline	BOOL		R9_CopyTargetToImage( R9TEXTURE target, r9Img* img, fRect* rect )	{ guardfast(R9_CopyTargetToImage);	sassert(r9_render); return r9_render->CopyTargetToImage(target,img,rect); unguardfast(); }

#endif
///////////////////////////////////////////////////////////////////////////////////////////////////
