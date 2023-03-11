///////////////////////////////////////////////////////////////////////////////////////////////////
// R9Render.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "R9Render.h"
#include "R9RenderDX.h"
#include "R9RenderGL.h"
#include "R9ImgLoader.h"
#include "R9Font.h"
#include "R9Resources.h"

r9Cfg::r9Cfg()
{ 
	guard(r9Cfg::r9Cfg);
	m_width		= R9_CFG_WIDTH;
	m_height	= R9_CFG_HEIGHT;
	m_bpp		= R9_CFG_BPP;
	m_windowed	= 1;
	m_refresh	= 0;
	m_vsync		= 0;
	unguard();
}

r9Render::r9Render()
{
	guard(r9Render::r9Render);
	m_api				= -1;
	m_dll				= NULL;
	m_hwnd				= NULL;
	m_beginendscene 	= FALSE;
	m_texture			= NULL;
	m_viewx				= 0;
	m_viewy				= 0;
	m_viewflip			= 0;
	m_needflush			= FALSE;
	m_primitivecount	= 0;
	m_font				= NULL;
	m_handlereset		= NULL;
	for(int i=0;i<R9_STATES;i++) m_state[i]=-1; // clear states with invalid values
	unguard();
}

r9Render::~r9Render()
{
	guard(r9Render::~r9Render);
	unguard();
}

BOOL		r9Render::LoadDll()												{ return TRUE; }
void		r9Render::UnloadDll()											{}
int			r9Render::GatherDisplayModes( r9DisplayMode* displaymode )		{ return 0; }
BOOL 		r9Render::Init( HWND hwnd, r9Cfg* cfg )							{ return TRUE; }
void 		r9Render::Done()												{ if(m_font) { TextureDestroy(m_font->GetTexture()); m_font->Destroy(); sdelete(m_font); } }
BOOL		r9Render::IsReady()												{ return TRUE; }
R9TEXTURE	r9Render::TextureCreate( r9Img* img )							{ return NULL; }
R9TEXTURE	r9Render::TextureCreateTarget( int width, int height )			{ return NULL; }

R9TEXTURE r9Render::TextureLoad( char* filename )
{
	guard(r9Render::TextureLoad);
	r9Img img;
	if(!R9_ImgLoadFile(filename, &img)) return NULL;
	R9TEXTURE tex = TextureCreate(&img);
	R9_ImgDestroy(&img);
	return tex;
	unguard();
}

void		r9Render::TextureDestroy( R9TEXTURE texture )					{}

void r9Render::SetTexture( R9TEXTURE texture )				{ m_texture = texture; }
void r9Render::SetState( int state, int value )				{ m_state[state] = value; }
void r9Render::SetViewport( fRect& rect )					{ m_viewport = rect; }
void r9Render::SetView( int x, int y, dword flip )			{ m_viewx = x; m_viewy = y; m_viewflip = flip; }
void r9Render::SetDefaultStates()
{
	guard(r9Render::SetDefaultStates);
	m_texture = NULL;
	for(int i=0;i<R9_STATES;i++) m_state[i]=-1; // clear states with invalid values
	SetState(R9_STATE_PRIMITIVE,R9_PRIMITIVE_TRIANGLE);
	SetState(R9_STATE_BLEND,R9_BLEND_ALPHA);
	SetState(R9_STATE_TADDRESS,R9_TADDRESS_WRAP);
	SetState(R9_STATE_FILTER,TRUE);
	SetViewport(fRect(0,0,GetWidth(),GetHeight()));
	SetView( 0, 0, 0 );
	unguard();
}

void r9Render::Clear( dword color )							{}				
BOOL r9Render::BeginScene( R9TEXTURE target )				{ return TRUE; }
void r9Render::EndScene()									{}
void r9Render::Present()									{}
BOOL r9Render::CheckDevice()								{ return TRUE; }
BOOL r9Render::ToggleVideoMode()							{ return FALSE; }
void r9Render::Push( r9Vertex* vx, int vxs, int primitive ) {}
void r9Render::Flush()										{}

BOOL r9Render::SaveScreenShot( fRect* rect, BOOL full)							{ return TRUE; }
BOOL r9Render::TakeScreenShot( r9Img* img, fRect* rect, BOOL full )				{ return TRUE; }
BOOL r9Render::CopyTargetToImage( R9TEXTURE target, r9Img* img, fRect* rect )	{ return FALSE; }

///////////////////////////////////////////////////////////////////////////////////////////////////
void r9Render::DrawQuadRot( fV2& pos, fV2& size, fV2& center, float angle, fRect& src, R9TEXTURE tex, dword color )
{
	guardfast(r9Render::DrawQuadRot);
	if(GetTexture()!=tex) SetTexture(tex);

	fV2 dst[4];
	dst[0].x = -center.x - size.x/2.0f;
	dst[0].y = -center.y - size.y/2.0f;
	dst[1].x = -center.x + size.x/2.0f;
	dst[1].y = -center.y - size.y/2.0f;
	dst[2].x = -center.x + size.x/2.0f;
	dst[2].y = -center.y + size.y/2.0f;
	dst[3].x = -center.x - size.x/2.0f;
	dst[3].y = -center.y + size.y/2.0f;

	float angsin = sinf(DEG2RAD(angle));
	float angcos = cosf(DEG2RAD(angle));
	dst[0] = Rotate(dst[0],angsin,angcos) + pos;
	dst[1] = Rotate(dst[1],angsin,angcos) + pos;
	dst[2] = Rotate(dst[2],angsin,angcos) + pos;
	dst[3] = Rotate(dst[3],angsin,angcos) + pos;

	r9Vertex vx[6];

	vx[0].x = dst[0].x;
	vx[0].y = dst[0].y;
	vx[0].u = src.x1;
	vx[0].v = src.y1;
	vx[0].color = color;

	vx[1].x = dst[1].x;
	vx[1].y = dst[1].y;
	vx[1].u = src.x2;
	vx[1].v = src.y1;
	vx[1].color = color;

	vx[2].x = dst[3].x;
	vx[2].y = dst[3].y;
	vx[2].u = src.x1;
	vx[2].v = src.y2;
	vx[2].color = color;

	vx[3].x = dst[1].x;
	vx[3].y = dst[1].y;
	vx[3].u = src.x2;
	vx[3].v = src.y1;
	vx[3].color = color;

	vx[4].x = dst[2].x;
	vx[4].y = dst[2].y;
	vx[4].u = src.x2;
	vx[4].v = src.y2;
	vx[4].color = color;

	vx[5].x = dst[3].x;
	vx[5].y = dst[3].y;
	vx[5].u = src.x1;
	vx[5].v = src.y2;
	vx[5].color = color;

	Push(vx,6,R9_PRIMITIVE_TRIANGLE);
	unguardfast();
}

void r9Render::DrawSprite( fV2& pos, fRect& src, R9TEXTURE tex, dword color, dword flip, float scale )
{
	guardfast(r9Render::DrawSprite);

	BOOL rotated = flip & R9_FLIPR;

	fRect dst;
	dst.x1 = pos.x;
	dst.y1 = pos.y;
	dst.x2 = pos.x + (rotated ? src.Height() : src.Width())*scale;
	dst.y2 = pos.y + (rotated ? src.Width() : src.Height())*scale;
	fRect src0 = src;
	if(flip & 1)	{ src0.x1=src.x2; src0.x2=src.x1; }
	if(flip & 2)	{ src0.y1=src.y2; src0.y2=src.y1; }
	if(rotated)		{ fRect src1 = src0; src0.x1=src1.y2; src0.y1=src1.x1; src0.x2=src1.y1; src0.y2=src1.x2; }

	// src: normal={x1y1,x2y2}; rotated={y2x1,y1x2};

	ClipQuad(dst,src0);
	if(dst.x2<=dst.x1 || dst.y2<=dst.y1) return;

	if(GetTexture()!=tex) SetTexture(tex);

	if(tex)
	{
		if(rotated)
		{
			src0.x1 /= tex->m_realheight;
			src0.x2 /= tex->m_realheight;
			src0.y1 /= tex->m_realwidth;
			src0.y2 /= tex->m_realwidth;
		}
		else
		{
			src0.x1 /= tex->m_realwidth;
			src0.x2 /= tex->m_realwidth;
			src0.y1 /= tex->m_realheight;
			src0.y2 /= tex->m_realheight;
		}
	}

	r9Vertex vx[6];

	if(rotated)
	{
		vx[0].x = dst.x1;
		vx[0].y = dst.y1;
		vx[0].u = src0.y1;
		vx[0].v = src0.x1;
		vx[0].color = color;

		vx[1].x = dst.x2;
		vx[1].y = dst.y1;
		vx[1].u = src0.y1;
		vx[1].v = src0.x2;
		vx[1].color = color;

		vx[2].x = dst.x1;
		vx[2].y = dst.y2;
		vx[2].u = src0.y2;
		vx[2].v = src0.x1;
		vx[2].color = color;

		vx[3].x = dst.x2;
		vx[3].y = dst.y1;
		vx[3].u = src0.y1;
		vx[3].v = src0.x2;
		vx[3].color = color;

		vx[4].x = dst.x2;
		vx[4].y = dst.y2;
		vx[4].u = src0.y2;
		vx[4].v = src0.x2;
		vx[4].color = color;

		vx[5].x = dst.x1;
		vx[5].y = dst.y2;
		vx[5].u = src0.y2;
		vx[5].v = src0.x1;
		vx[5].color = color;
	}
	else
	{
		vx[0].x = dst.x1;
		vx[0].y = dst.y1;
		vx[0].u = src0.x1;
		vx[0].v = src0.y1;
		vx[0].color = color;

		vx[1].x = dst.x2;
		vx[1].y = dst.y1;
		vx[1].u = src0.x2;
		vx[1].v = src0.y1;
		vx[1].color = color;

		vx[2].x = dst.x1;
		vx[2].y = dst.y2;
		vx[2].u = src0.x1;
		vx[2].v = src0.y2;
		vx[2].color = color;

		vx[3].x = dst.x2;
		vx[3].y = dst.y1;
		vx[3].u = src0.x2;
		vx[3].v = src0.y1;
		vx[3].color = color;

		vx[4].x = dst.x2;
		vx[4].y = dst.y2;
		vx[4].u = src0.x2;
		vx[4].v = src0.y2;
		vx[4].color = color;

		vx[5].x = dst.x1;
		vx[5].y = dst.y2;
		vx[5].u = src0.x1;
		vx[5].v = src0.y2;
		vx[5].color = color;
	}

	Push(vx,6,R9_PRIMITIVE_TRIANGLE);
	unguardfast();
}

BOOL r9Render::CreateFont()
{
	guard(r9Render::CreateFont);
	sassert(m_font==NULL);
	char* filename; // name for the memory file
	dword memsize; // memory buffer (file) size
	byte* memfile; // memory buffer

	// create fixed font (courier new)
	m_font = snew r9Font();
	m_font->Create(R9_CHRW,R9_CHRH-1);

	// create texture from memory
	R9TEXTURE tex;
	memsize = r9_fonttga_buffer[0];
	memfile = (byte*)smalloc(memsize);
	if(!decompress_data((byte*)(r9_fonttga_buffer+2),r9_fonttga_buffer[1],memfile,memsize)) goto error;
	filename = F9_MakeFileName("font.tga",memfile,memsize);
	if(!filename) goto error;
	tex = TextureLoad(filename);
	if(!tex) goto error;
	m_font->SetTexture(tex);
	sfree(memfile);
	return TRUE;

	error:
	sdelete(m_font); m_font=NULL;
	if(memfile) sfree(memfile);
	return FALSE;
	unguard();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// INTERFACE
///////////////////////////////////////////////////////////////////////////////////////////////////
r9Render* r9_render = NULL;

r9Render* R9_CreateRender( int api )
{
	guard(R9_CreateRender);
	r9Render* render=NULL;
	if(api==R9_API_DIRECTX)	render = snew r9RenderDX();
	if(api==R9_API_OPENGL)	render = snew r9RenderGL();
	if(!render) return NULL;
	if(!render->LoadDll()) { sdelete(render); return NULL; }
	return render;
	unguard();
}

// interface
int				r9DisplayModesCount;		// displaymodes count
r9DisplayMode*	r9DisplayModes;				// displaymodes list

BOOL R9_InitInterface( int api )
{
	guard(R9_GatherDisplayModes);
	dlog(LOGRND, "Render init interface (api=%i).\n",api);
	r9Render* render = R9_CreateRender(api);
	if(!render) return FALSE;

	// gather display modes
	r9DisplayModesCount = render->GatherDisplayModes(NULL);
	if(r9DisplayModesCount==0) return FALSE;
	r9DisplayModes = (r9DisplayMode*)smalloc(r9DisplayModesCount*sizeof(r9DisplayMode));
	r9DisplayModesCount = render->GatherDisplayModes(r9DisplayModes);

	render->UnloadDll();
	sdelete(render);
	return TRUE;
	unguard();
}

BOOL R9_FilterCfg( r9Cfg& cfg, int &api )
{
	guard(R9_FilterCfg);
	int count = r9DisplayModesCount;
	r9DisplayMode* displaymode = r9DisplayModes;
	if(!displaymode || !count) return FALSE;

	dlog(LOGRND, "Filter config:\n");
	dlog(LOGRND, "  Requested: "); R9_LogCfg(cfg,api);

	BOOL ok = FALSE;
	r9Cfg cfgout = cfg;
	cfgout.m_refresh = 0;

	// search modes	
	for(int i=0;i<count;i++)
	{
		if( cfgout.m_windowed != displaymode[i].m_windowed ) continue;
		
		if( cfgout.m_windowed ) // in windowed
		{
			// overwrite bpp
			cfgout.m_bpp = displaymode[i].m_bpp;
			// clamp resolution in windowed
			if( cfgout.m_width > displaymode[i].m_width || cfgout.m_height > displaymode[i].m_height )
			{
				cfgout.m_width = displaymode[i].m_width;
				cfgout.m_height = displaymode[i].m_height;
			}
			// overwrite refresh
			cfgout.m_refresh = 0;
			ok = TRUE;
			// got it
			break;
		}
		else // in fullscreen
		{
			// match bpp
			if( cfgout.m_bpp != displaymode[i].m_bpp ) continue;
			// match resolution
			if( cfgout.m_width != displaymode[i].m_width ) continue;
			if( cfgout.m_height != displaymode[i].m_height ) continue;
			// select highest refresh found, but not higher then requested
			if( displaymode[i].m_refresh > cfgout.m_refresh &&
				displaymode[i].m_refresh <= cfg.m_refresh )
				cfgout.m_refresh = displaymode[i].m_refresh; 
			ok = TRUE;
			// continue search for better refresh
		}
	}
	
	cfg = cfgout;
	if(ok)
	{ dlog(LOGRND, "  Received:  "); R9_LogCfg(cfg,api); }
	else
	{ dlog(LOGRND, "  Received:  FAILED\n"); return FALSE; }
	return ok;
	unguard();
}

void R9_LogCfg( r9Cfg& cfg, int api )
{
	guard(R9_LogCfg);
	dlog(LOGRND,"%s %s %ix%i %ibpp %iHz%s\n", 
		api?"OpenGL":"DirectX",
		cfg.m_windowed?"windowed":"full-screen",
		cfg.m_width, cfg.m_height,
		cfg.m_bpp,
		cfg.m_refresh,
		cfg.m_vsync?" vsync":""
		);
	unguard();
}

void R9_DoneInterface()
{
	guard(R9_DoneInterface);
	if(!r9DisplayModes) return;
	sfree(r9DisplayModes);
	r9DisplayModes = NULL;
	r9DisplayModesCount = 0;
	dlog(LOGRND, "Render done interface.\n");
	unguard();
}

BOOL R9_Init( HWND hwnd, r9Cfg* cfg, int api )
{
	guard(R9_Init);
	if(r9_render) return TRUE;
	dlog(LOGRND, "Render init (api=%i).\n",api);
	r9_render = R9_CreateRender(api);
	if(!r9_render) return FALSE;
	if(!r9_render->Init(hwnd, cfg))
	{
		r9_render->UnloadDll();
		sdelete(r9_render);
		r9_render = NULL;
		return FALSE;
	}
	return TRUE;
	unguard();
}

void R9_Done()
{
	guard(R9_Done);
	if(!r9_render) return;
	r9_render->Done();
	r9_render->UnloadDll();
	sdelete(r9_render);
	r9_render = NULL;
	dlog(LOGRND, "Render done.\n");
	unguard();
}

void R9_DrawText( fV2& pos, char* text, dword color, float scale )
{ 
	guard(R9_DrawText); 
	sassert(r9_render); 
	if(!r9_render->m_font) return; 
	r9_render->m_font->SetColor(color); 
	r9_render->m_font->m_scale=scale; 
	r9_render->m_font->Print(pos.x,pos.y,text); 
	unguard(); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
