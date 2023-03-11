//////////////////////////////////////////////////////////////////////////////////////////////////
// DizPaint.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "gs.h"
#include "DizPaint.h"
#include "DizGame.h"
#include "DizDebug.h"

cDizPaint g_paint;

//////////////////////////////////////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////////////////////////////////////

cDizPaint::cDizPaint()
{
	guard(cDizPaint::cDizPaint)
	m_scrx = 0;
	m_scry = 0;
	m_scale = 1;
	m_drawtilesoft = FALSE;
	m_drawtilemat = 0;
	m_hudfont = 0;
	m_hudshader = SHADER_BLEND;
	m_hudcolor = 0xffffffff;
	m_huddraw = FALSE;
	unguard()
}

cDizPaint::~cDizPaint()
{
	guard(cDizPaint::~cDizPaint)
	// nothing - use Done
	unguard()
}

BOOL cDizPaint::Init()
{
	guard(cDizPaint::Init)
	BOOL ok = TRUE;

	// screen
	Layout();

	// tiles
	m_tile.Init(64);
	m_hash.Init(64,0);

	// fonts
	m_font.Init(8);

	return ok;
	unguard()
}

void cDizPaint::Done()
{
	guard(cDizPaint::Done)

	// tiles
	m_hash.Done();
	m_tile.Done();

	// fonts
	m_font.Done();

	unguard()
}

BOOL cDizPaint::Reacquire()
{
	guard(cDizPaint::Reacquire);
	dlog(LOGAPP,"Paint reaquire.\n");
	BOOL ok=TRUE;
	for(int i=0; i<m_tile.Size();i++) 
	{
		m_tile.Get(i)->m_tex = R9_TextureLoad(m_tile[i]->m_name);
		if(m_tile.Get(i)->m_tex==NULL)
		{
			dlog(LOGSYS,"error reacquireing tile %s.\n",m_tile[i]->m_name);
			ok = FALSE;
		}
	}
	return ok;
	unguard();
}

void cDizPaint::Unacquire()
{
	guard(cDizPaint::Unacquire);
	dlog(LOGAPP,"Paint unaquire.\n");
	int i;
	for(i=0; i<m_tile.Size(); i++) 
	{
		R9_TextureDestroy(m_tile.Get(i)->m_tex);
		m_tile.Get(i)->m_tex = NULL;
	}
	for(i=0; i<m_font.Size(); i++)
	{
		if(m_font.Get(i)->m_font)
			m_font.Get(i)->m_font->SetTexture(NULL); // safe
	}
	unguard();
}

void cDizPaint::Layout()
{
	guard(cDizPaint::Layout);
	m_scale = g_cfg.m_scale;
	if(m_scale==0)	m_scale = MAX( 0, MIN( R9_GetWidth()/g_game.m_screen_bw, R9_GetHeight()/g_game.m_screen_bh ) );
	if(!g_dizdebug.m_console)
	{
		m_scrx = (R9_GetWidth() - g_game.m_screen_w*m_scale )/2;
		m_scry = (R9_GetHeight() - g_game.m_screen_h*m_scale )/2;
	}
	else
	{
		m_scrx = 0;
		m_scry = 0;
	}
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// TILES
//////////////////////////////////////////////////////////////////////////////////////////////////

int		gstile_total;			// status report on total tiles declared (load+failed)
int		gstile_fail;			// status report on tiles failed to load
int		gstile_duplicates;		// status report on id duplicates
int		gstile_group;			// current loading group

BOOL cDizPaint::TileLoadFile( char* filepath, int group )
{
	guard(cDizPaint::TileLoadFile)
	
	// check file type (not counted if unaccepted); only TGA and PNG files accepted
	char* ext = file_path2ext(filepath); if(!ext) return FALSE;
	if( 0!=stricmp(ext,"tga") && 0!=strcmp(ext,"png") ) return FALSE;
	char* name = file_path2file(filepath); if(!name) return FALSE;
	
	gstile_total++;

	// check name format
	char szt[128];
	int id		= -1;
	int frames	= 1;
	int ret = sscanf(name,"%i %s %i",&id,szt,&frames);
	if(ret==0 || id==-1) 
	{ 
		gstile_fail++; 
		dlog(LOGSYS, "! %s (bad name)\n", filepath); 
		return FALSE; 
	}
	if(frames<1) frames=1;
	
	// check unique id
	int idx = TileFind(id);
	if( idx!=-1 )
	{
		gstile_fail++;
		gstile_duplicates++;
		dlog(LOGSYS, "! %s (duplicate id)\n", filepath, id);
		return FALSE;
	}

	// load image rgba
	r9Img img;
	if(!R9_ImgLoadFile(filepath,&img))
	{
		gstile_fail++;
		dlog(LOGSYS, "! %s (load failed)\n", filepath);
		return FALSE;
	}

	// create img alpha
	r9Img imga;
	imga.m_pf = R9_PF_A;
	imga.m_width = img.m_width;
	imga.m_height = img.m_height;
	if( !R9_ImgCreate(&imga) ||
		!R9_ImgBitBltSafe(&img,0,0,img.m_width,img.m_height,&imga,0,0) ) 
	{
		gstile_fail++;
		dlog(LOGSYS, "! %s (alpha failed)\n", filepath);
		R9_ImgDestroy(&img);
		return FALSE;
	}

	// create new tile
	idx = TileAdd(id);
	cTile* tile = TileGet(idx); gsassert(tile!=NULL);
	tile->m_tex = R9_TextureCreate(&img);
	if(tile->m_tex==NULL)
	{
		TileDel(idx);
		gstile_fail++;
		dlog(LOGSYS, "! %s (texture failed)\n", filepath);
		return FALSE;
	}

	// set current options
	tile->m_group	= group;
	tile->m_frames	= frames;
	tile->m_img		= imga;
	tile->m_name	= sstrdup(filepath);

	R9_ImgDestroy(&img);

	if(IS_DEVELOPER()) // log for developers
		dlog(LOGAPP, "  %s [%i]\n", filepath, frames );
	
	return TRUE;
	unguard()
}

void FFCallback_Tile( char* filepath, BOOL dir )
{
	guard(FFCallback_Tile)
	if(dir) return;
	BOOL ret = g_paint.TileLoadFile(filepath,gstile_group);
	unguard()
}

BOOL cDizPaint::TileLoad( char* path, int group )
{
	guard(cDizPaint::TileLoad)
	if(!path || !path[0]) return FALSE; // invalid path
	int szlen = (int)strlen(path);
	if(path[szlen-1]!='\\') strcat(path,"\\");
	_strlwr(path);
	dlog(LOGAPP,"Loading tiles from \"%s\" (group=%i)\n", path, group);

	// init
	gstile_total		= 0;
	gstile_fail			= 0;
	gstile_duplicates	= 0;
	gstile_group		= group;

	// find files on disk
	int archivefiles = F9_ArchiveGetFileCount(0);
	if(archivefiles==0) // if no archive found then open from disk
	{
		file_findfiles( path, FFCallback_Tile, FILE_FINDREC );
	}
	else // if archive opened, load from it
	{
		for(int i=0;i<archivefiles;i++)
		{
			char* filename = F9_ArchiveGetFileName(0,i);
			if(strstr(filename,path)==filename)
			{
				FFCallback_Tile(filename,FALSE);
			}
		}
	}

	// report
	dlog(LOGAPP, "Tiles report: total=%i, failed=%i (duplicates=%i)\n\n", gstile_total, gstile_fail, gstile_duplicates );

	return TRUE;
	unguard()
}

void cDizPaint::TileUnload( int group )
{
	guard(cDizPaint::TileUnload)
	int i;
	for(i=0;i<m_tile.Size();i++)
	{
		if(m_tile[i]->m_group==group)
		{
			TileDel(i);
			i--;
		}
	}
	unguard()
}

int cDizPaint::TileAdd( int id )
{
	guard(cDizPaint::TileAdd)
	if(id<0) return -1; // negative ids not accepted

	// check duplicate id
	if(TileFind(id)!=-1) return -1; // duplicate id
	
	// add new tile to list
	cTile* tile = snew cTile();
	tile->m_id = id;
	int idx = m_tile.Add(tile); sassert(idx!=-1);

	// add to hash
	m_hash.Add( tile->m_id, idx );
	return idx;
	unguard()
}

void cDizPaint::TileDel( int idx )
{
	guard(cDizPaint::TileDel)
	cTile* tile = TileGet(idx); 
	if(tile==NULL) return;
	m_hash.Del(tile->m_id);
	m_tile.Del(idx);
	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// Draw functions
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizPaint::DrawTile( int idx, int x, int y, iRect& map, dword color, int flip, int frame, int blend, float scale )
{
	guard(cDizPaint::DrawTile)
	cTile* tile = TileGet(idx); 
	if(tile==NULL) return;

	int w = tile->GetWidth();
	int h = tile->GetHeight();
	if(frame<0) frame=0;
	frame = frame % tile->m_frames;

	fRect src = map;
	src.x1 += frame * w;
	src.x2 += frame * w;
	fV2 pos( m_scrx+x*m_scale, m_scry+y*m_scale );

	R9_SetState(R9_STATE_BLEND,blend);
	R9_DrawSprite( pos, src, tile->m_tex, color, flip, (float)m_scale*scale );

	unguard()
}
	
void cDizPaint::DrawTile( int idx, int x, int y, dword color, int flip, int frame, int blend, float scale )
{
	guard(cDizPaint::DrawTile)
	cTile* tile = TileGet(idx); 
	if(tile==NULL) return;

	int w = tile->GetWidth();
	int h = tile->GetHeight();
	if(frame<0) frame=0;
	frame = frame % tile->m_frames;

	fRect src;
	src.x1 = (float)(frame * w);
	src.x2 = (float)((frame+1) * w);
	src.y1 = 0.0f;
	src.y2 = (float)h;
	fV2 pos( m_scrx+x*m_scale, m_scry+y*m_scale );

	R9_SetState(R9_STATE_BLEND,blend);
	R9_DrawSprite( pos, src, tile->m_tex, color, flip, (float)m_scale*scale );

	unguard()
}

void cDizPaint::DrawChar( int fontidx, int x, int y, char c, dword color )
{
	guard(cDizPaint::DrawChar);
	if(!FontGet(fontidx)) return;
	r9Font* font = FontGet(fontidx)->m_font;
	if(!font) return;
	float tsize = font->GetSize();
	font->SetSize( tsize*m_scale );
	font->SetColor( color );
	font->Char( (float)(m_scrx+x*m_scale), (float)(m_scry+y*m_scale), c );
	font->SetSize(tsize);
	unguard();
}

void cDizPaint::DrawBrush( tBrush* brush, int x, int y, int frame )
{
	guard(cDizPaint::DrawBrush)
	if(brush==NULL) return;
	int idx = TileFind(brush->Get(BRUSH_TILE));
	if(idx==-1) return;

	iRect map;
	map.x1 = brush->Get(BRUSH_MAP+0);
	map.y1 = brush->Get(BRUSH_MAP+1);
	map.x2 = brush->Get(BRUSH_MAP+2);
	map.y2 = brush->Get(BRUSH_MAP+3);
	int mw = (int)GET_BRUSH_MAPWITH(*brush);
	int mh = (int)GET_BRUSH_MAPHEIGHT(*brush);
	float ms = GET_BRUSH_MAPSCALE(*brush);
	if( mw==0 || mh==0 ) return;

	fRect oldclip = R9_GetClipping();
	fRect newclip = fRect(	m_scrx+x*m_scale, 
							m_scry+y*m_scale, 
							m_scrx+x*m_scale+m_scale*brush->Get(BRUSH_W), 
							m_scry+y*m_scale+m_scale*brush->Get(BRUSH_H) );
	R9_AddClipping(newclip);
	if(R9_IsClipping())
	{
		g_game.m_visible_brushes++;

		int cx = (brush->Get(BRUSH_W)+mw-1) / mw;
		int cy = (brush->Get(BRUSH_H)+mh-1) / mh;

		int blend = brush->Get(BRUSH_SHADER);
		BOOL soft2 = (brush->Get(BRUSH_FLIP) & R9_FLIPR) || (brush->Get(BRUSH_SCALE)!=0 && brush->Get(BRUSH_SCALE)!=100);
		
		int xt=x;
		for(int i=0;i<cy;i++)
		{
			x = xt;
			for(int j=0;j<cx;j++)
			{
				if(m_drawtilesoft)
				{
					if(soft2)
						DrawTileSoft2( idx, x, y, map, brush->Get(BRUSH_COLOR), brush->Get(BRUSH_FLIP), frame, blend, ms );
					else
						DrawTileSoft( idx, x, y, map, brush->Get(BRUSH_COLOR), brush->Get(BRUSH_FLIP), frame, blend, ms );
				}
				else
				{
					DrawTile( idx, x, y, map, brush->Get(BRUSH_COLOR), brush->Get(BRUSH_FLIP), frame, blend, ms );
				}
				x+=mw;
			}
			y+=mh;
		}
	}

	R9_SetClipping(oldclip);
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// TILE MATERIAL DRAW (SOFTWARE)
// color and shader ignored
// use imgtarget, drawtilemat and correct clipping
// rotated and scalled sprites go through DrawTileSoft2
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizPaint::DrawTileSoft( int idx, int x, int y, iRect& map, dword color, int flip, int frame, int blend, float scale )
{
	guard(cDizPaint::DrawTileSoft)
	cTile* tile = TileGet(idx); 
	if(tile==NULL) return;
	int w = tile->GetWidth();
	int h = tile->GetHeight();
	// sassert(frame==0);
	if( frame<0 ) frame=0;
	frame = frame % tile->m_frames;
	BOOL rotated = flip & R9_FLIPR;

	// source rectangle safe
	iRect rsrc = map;
	rsrc.x1 += frame * w;
	rsrc.x2 += frame * w;
	if(rsrc.x1<0) rsrc.x1=0;
	if(rsrc.y1<0) rsrc.y1=0;
	if(rsrc.x2>w*tile->m_frames) rsrc.x2=w*tile->m_frames;
	if(rsrc.y2>h) rsrc.y2=h;

	// destination rectangle
	iRect rdst;
	rdst.x1	= x;
	rdst.y1	= y;
	rdst.x2	= x + rsrc.x2 - rsrc.x1;
	rdst.y2	= y + rsrc.y2 - rsrc.y1;

	// clip by clipping (clipping must be already clipped by imgtarget)
	fRect frdst = rdst;
	fRect frsrc = rsrc;
//	if(rotated)	{ fRect frsrc1 = frsrc; frsrc.x1=frsrc1.y2; frsrc.y1=frsrc1.x1; frsrc.x2=frsrc1.y1; frsrc.y2=frsrc1.x2; } // rotate for clipping
	R9_ClipSprite(frdst,frsrc,flip); // must always clip !!!
//	if(rotated)	{ fRect frsrc1 = frsrc; frsrc.y2=frsrc1.x1; frsrc.x1=frsrc1.y1; frsrc.y1=frsrc1.x2; frsrc.x2=frsrc1.y2; } // rotate back
	rdst = frdst;
	rsrc = frsrc;

	if(!(rdst.x1<rdst.x2 && rdst.y1<rdst.y2)) return; // out-clipped

	// prepare blit
	int dstw = m_imgtarget.m_width;
	int dsth = m_imgtarget.m_height;
	byte* dst = m_imgtarget.m_data; sassert(dst!=NULL);
	dst += rdst.x1 + rdst.y1 * dstw; // start

	int rw = rdst.x2-rdst.x1;
	int rh = rdst.y2-rdst.y1;
	int srcw = tile->m_img.m_width;
	int srch = tile->m_img.m_height;
	byte* src = tile->m_img.m_data; sassert(src!=NULL);
	src += rsrc.x1 + rsrc.y1 * srcw; // start
	int srcsx = 1; // step x;
	if(flip & 1)
	{
		srcsx = -1;
		src += rw-1;
	}
	int srcsy = 1; // step y;
	if(flip & 2)
	{
		srcsy = -1;
		src += (rh-1) * srcw;
	}

	// draw loops
	byte mat = m_drawtilemat;
	for(int iy=0;iy<rh;iy++)
	{
		byte* src0 = src;
		for(int ix=0;ix<rw;ix++)
		{
			if(*src>128) *dst=mat;
			dst++; src+=srcsx;
		}
		dst += dstw - rw;
		src = src0 + srcw*srcsy;
	}

	unguard()
}

void cDizPaint::DrawTileSoft2( int idx, int x, int y, iRect& map, dword color, int flip, int frame, int blend, float scale )
{
	guard(cDizPaint::DrawTileSoft2)
	cTile* tile = TileGet(idx); 
	if(tile==NULL) return;
	int tw = tile->GetWidth();
	int th = tile->GetHeight();
	// sassert(frame==0);
	if( frame<0 ) frame=0;
	frame = frame % tile->m_frames;

	// source rectangle safe
	iRect rsrc = map;
	rsrc.x1 += frame * tw;
	rsrc.x2 += frame * tw;
	if(rsrc.x1<0) rsrc.x1=0;
	if(rsrc.y1<0) rsrc.y1=0;
	if(rsrc.x2>tw*tile->m_frames) rsrc.x2=tw*tile->m_frames;
	if(rsrc.y2>th) rsrc.y2=th;

	// DRAW SPRITE SOFTWARE
	BOOL rotated = flip & R9_FLIPR;

	fRect dst;
	dst.x1 = (float)x;
	dst.y1 = (float)y;
	dst.x2 = (float)x + (rotated ? (float)rsrc.Height() : (float)rsrc.Width())*scale;
	dst.y2 = (float)y + (rotated ? (float)rsrc.Width() : (float)rsrc.Height())*scale;
	
	fRect src(rsrc);
	if(flip & 1)	{ src.x1=(float)rsrc.x2; src.x2=(float)rsrc.x1; }
	if(flip & 2)	{ src.y1=(float)rsrc.y2; src.y2=(float)rsrc.y1; }
	if(rotated)		{ fRect src1 = src; src.x1=src1.y2; src.y1=src1.x1; src.x2=src1.y1; src.y2=src1.x2; }

	// src: normal={x1y1,x2y2}; rotated={y2x1,y1x2};

	R9_ClipQuad(dst,src);
	if(dst.x2<=dst.x1 || dst.y2<=dst.y1) return;

	int dstw = m_imgtarget.m_width;
	int dsth = m_imgtarget.m_height;
	int dw = (int)dst.Width();
	int dh = (int)dst.Height();
	byte* dstdata = m_imgtarget.m_data; sassert(dst!=NULL);
	dstdata += (int)dst.x1 + (int)dst.y1 * dstw; // start
	float sw = src.Width();
	float sh = src.Height();
	int srcw = tile->m_img.m_width;
	int srch = tile->m_img.m_height;
	byte* srcdata = tile->m_img.m_data; sassert(src!=NULL);
	byte mat = m_drawtilemat;

	// draw
	for(int iy=0;iy<dh;iy++)
	{
		float ty = (float)iy / (float)dh + 0.001f;
		int my = (int)(src.y1 + sh*ty);
		for(int ix=0;ix<dw;ix++)
		{
			float tx = (float)ix / (float)dw + 0.001f;
			int mx = (int)(src.x1 + sw*tx);
			if(rotated)
			{
				if(srcdata[mx*srcw+my]>128) *dstdata=mat;
			}
			else
			{
				if(srcdata[my*srcw+mx]>128) *dstdata=mat;
			}
			dstdata++;
		}
		dstdata += dstw - dw;
	}

	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// HUD draw functions
//////////////////////////////////////////////////////////////////////////////////////////////////
#define DLGCMD_NONE		0
#define DLGCMD_ALIGN	1
#define DLGCMD_COLOR	2
#define DLGCMD_FOCUS	3
#define DLGCMD_TILE		4

// command format can be: {a:l} or {a:left}, {a:c}, {a:r}, {c:ff0080}, {f:0}, {f:1}, {t:10 16 18}
int	cDizPaint::HUDScanText( char* text, int start, int& end, int* data )
{
	guard(cDizPaint::HUDScanText)
	end = start;
	data[0]=0;

	// search for close sign
	while(text[end]!=0 && text[end]!='}') end++;
	if(end<start+4 || text[end]!='}' || text[start+2]!=':') return DLGCMD_NONE; // invalid command

	// copy data string
	char szdata[64];
	int szdatalen = MIN(63,end-start-3);
	strncpy( szdata, text+start+3, szdatalen );
	szdata[szdatalen]=0;
	
	switch(text[start+1])
	{
		case 'a': // align
		case 'A': // align
		{
			data[0] = (szdata[0]=='l') ? -1 : (szdata[0]=='r') ? 1 : 0;
			return DLGCMD_ALIGN;
		}
		case 'c': // color
		case 'C': // color
		{
			if(1==sscanf(szdata,"%x",&data[0]))
				return DLGCMD_COLOR;
		}
		case 'f': // focus
		case 'F': // focus
		{
			data[0] = (szdata[0]=='1') ? 1 : 0;
			return DLGCMD_FOCUS;
		}
		case 't': // tile
		case 'T': // tile
		{
			if(3==sscanf(szdata,"%i %i %i",&data[0],&data[1],&data[2])) // id, x, y (in client)
				return DLGCMD_TILE;
		}
	}

	return DLGCMD_NONE;
	unguard()
}

void cDizPaint::HUDGetTextSize( char* text, int& w, int& h, int& c, int& r )
{
	guard(cDizPaint::HUDGetTextSize)
	w = h = c = r = 0;
	if(text==NULL) return; // invalid text
	int fontidx = FontFind(m_hudfont); // find font
	cFont* font = FontGet(fontidx);
	if(!font) return; // no font

	int m=0;
	int data[4];
	h = 0;
	int chrcount = 0;
	int rowcount = 0;
	int linesize = 0;
	BOOL newline=TRUE;
	while(text[m]!=0)
	{
		// escape command
		int m2;
		int cmd=DLGCMD_NONE;
		if( text[m]=='{')	cmd = HUDScanText(text,m,m2,data); // read command
		if(cmd!=DLGCMD_NONE) m=m2+1; // step over it
		else 
		{
			if(text[m]=='\n')
			{
				if(chrcount) linesize -= font->GetOfsX();
				h += font->GetSize() + font->GetOfsY();
				if(linesize>w) w=linesize;
				if(chrcount>c) c=chrcount;
				chrcount = 0;
				linesize = 0;
				rowcount++;
				newline = TRUE;
			}
			else
			{
				newline = FALSE;
				linesize += font->GetCharWidth(text[m]) + font->GetOfsX();
				chrcount++;
			}
			m++;
		}
	}
	// last row, if not empty, or after a \n
	if(chrcount || newline) 
	{
		if(chrcount) linesize -= font->GetOfsX();
		h += font->GetSize();
		if(linesize>w) w=linesize;
		if(chrcount>c) c=chrcount;
		rowcount++;
	}

	r = rowcount;
	unguard()
}

void cDizPaint::HUDDrawText( int tileid, iRect& dst, char* text, int m_align )
{
	guard(cDizPaint::HUDDrawText)
	if( m_huddraw==0 ) return; // not in draw
	if( m_hudshader<0 || m_hudshader>=SHADER_MAX ) return; // invalid shader
	if( text==NULL ) return; // invalid text
	int tileidx = TileFind(tileid);
	cTile* tile = TileGet(tileidx); 
	if(tile==NULL) return; // invalid tile
	int fontidx = FontFind(m_hudfont); // find font
	cFont* font = FontGet(fontidx);
	if(!font) return; // no font

	// overwrite font's texture and shader
	font->m_font->SetTexture(tile->m_tex);
	font->m_font->SetBlend(m_hudshader);

	// draw process
	int x = dst.x1;
	int y = dst.y1;
	int w = dst.x2-dst.x1;
	int h = dst.y2-dst.y1;

	int linecount = 0;				// line count
	int lnstart=0;					// start of current line (chr scan pos)
	int lnend;						// end of current line (chr scan pos)
	int size = (int)strlen(text);	// text size
	int align = m_align;			// lign align mode
	int focus = 0;					// focus mode 1/0
	dword color = m_hudcolor;		// color
	dword colorfocus;				// color when focus=1
	dword focuscolors[8] = { 0xffc80000, 0xffc800c8, 0xff00c800, 0xff00c8c8, 0xffc8c800,  0xff00c8c8, 0xff00c800, 0xffc800c8 };
//	dword focuscolors[6] = { 0xffff8000, 0xffffa000, 0xffffc000, 0xffffe000, 0xffffc000, 0xffffa000 }; // some orange version
	colorfocus = focuscolors[ (GetTickCount()/30) % 8 ];

	// scan text
	while(lnstart<size)
	{
		// Current Line Processing
		int x = dst.x1;

		int cmd;			// escape command
		int data[4];		// escape command data

		// PASS1: scan current line for size
		int	m=lnstart;		// scan cursor
		int chrcount=0;		// printable characters to scan cursor
		int linesize=0;		// line size in pixels
		
		while(m<size && text[m]!='\n')
		{
			// escape command
			int m2;
			cmd=DLGCMD_NONE;
			if( text[m]=='{') cmd = HUDScanText(text,m,m2,data); // read command
			if(cmd!=DLGCMD_NONE) // only if command found and valid
			{
				if(cmd==DLGCMD_ALIGN) align=data[0];
				m=m2+1; // step over it
			}
			else
			{
				linesize+=font->GetCharWidth(text[m])+font->GetOfsX();
				m++;
				chrcount++;
			}
		}
		if(chrcount>0) linesize-=font->GetOfsX();

		lnend = m;

		// compute aligniament
		if(align==0) x+=((w-linesize)/2); else
		if(align==1) x+=(w-linesize);

		// PASS2: print characters to the end of the line
		m = lnstart;
		while(m<lnend)
		{
			if(text[m]==0) break; // safety+

			// escape command
			int m2;
			cmd=DLGCMD_NONE;
			if( text[m]=='{')	cmd = HUDScanText(text,m,m2,data); // read command
			if(cmd!=DLGCMD_NONE) // only if command found and valid
			{
				if(cmd==DLGCMD_COLOR) 	color=0xff000000 | data[0];
				else
				if(cmd==DLGCMD_FOCUS)	focus=data[0];
				else
				if(cmd==DLGCMD_TILE)	DrawTile( TileFind(data[0]), x+data[1], y+data[2], focus?colorfocus:color, 0, 0 );
				m=m2+1; // step over it
			}
			else
			{
				// print character
				DrawChar( fontidx, x, y, text[m], focus?colorfocus:color );
				x+=font->GetCharWidth(text[m])+font->GetOfsX();
				chrcount--;
				m++;
			}
		}

		if(text[m]=='\n') m++; // step over new line character

		// new Line
		y += font->GetSize() + font->GetOfsY();
		lnstart = m;
		linecount++;
	}

	unguard()
}


void cDizPaint::HUDDrawTile( int tileid, iRect& dst, iRect& src, dword flags, int frame )
{
	guard(cDizPaint::HUDDrawTile)
	if( m_huddraw==0 ) return; // not in draw
	int tileidx = TileFind(tileid);
	if(tileidx==-1) return;
	int w = src.x2-src.x1;
	int h = src.y2-src.y1;
	if( w==0 || h==0 ) return;
	if(m_hudshader<0 || m_hudshader>=SHADER_MAX) return;

	fRect oldclip = R9_GetClipping();
	fRect newclip = fRect(	m_scrx+dst.x1*m_scale, 
							m_scry+dst.y1*m_scale, 
							m_scrx+dst.x2*m_scale, 
							m_scry+dst.y2*m_scale );
	R9_AddClipping(newclip);
	if(R9_IsClipping())
	{
		int cx = (dst.x2-dst.x1+w-1) / w;
		int cy = (dst.y2-dst.y1+h-1) / h;

		int blend = m_hudshader;

		int y = dst.y1;
		for(int i=0;i<cy;i++)
		{
			int x = dst.x1;
			for(int j=0;j<cx;j++)
			{
				DrawTile( tileidx, x, y, src, m_hudcolor, flags, frame, blend );
				x+=w;
			}
			y+=h;
		}
	}

	R9_SetClipping(oldclip);	
	unguard()
}

void cDizPaint::HudClipping( iRect& dst )
{
	guard(cDizPaint::HudClipping)
	if( m_huddraw==0 ) return; // not in draw
	if(dst.x2<dst.x1 || dst.y2<dst.y1)
	{
		R9_ResetClipping();
		return;
	}
	fRect rect;
	rect.x1 = (float)(m_scrx + dst.x1*m_scale);
	rect.y1 = (float)(m_scry + dst.y1*m_scale);
	rect.x2 = (float)(m_scrx + dst.x2*m_scale);
	rect.y2 = (float)(m_scry + dst.y2*m_scale);
	R9_SetClipping(rect);	
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Fonts
//////////////////////////////////////////////////////////////////////////////////////////////////
void cDizPaint::FontDel( int idx )
{
	guard(cDizPaint::FontDel);
	cFont* font = FontGet(idx); 
	if(font==NULL) return;
	m_font.Del(idx);
	unguard();
}

int		gsfont_total;			// status report on total fonts declared (load+failed)
int		gsfont_fail;			// status report on fonts failed to load
int		gsfont_duplicates;		// status report on id duplicates
int		gsfont_group;			// current font group

BOOL cDizPaint::FontLoadFile( char* filepath, int group )
{
	guard(cDizPaint::FontLoadFile)

	// check file type (not counted if unaccepted);
	char* name = file_path2file(filepath); if(!name) return FALSE;
	char* ext = file_path2ext(filepath); if(!ext) return FALSE;
	if(0!=stricmp(ext,"fnt")) return FALSE;
	
	gsfont_total++;

	// check name format
	char szt[128];
	int id = -1;
	int ret = sscanf(name,"%i %s",&id,szt);
	if(ret==0 || id==-1) 
	{ 
		gsfont_fail++; 
		dlog(LOGSYS, "! %s (bad name)\n", filepath); 
		return FALSE; 
	}
	
	// check unique id
	int idx = FontFind(id);
	if( idx!=-1 )
	{
		gsfont_fail++;
		gsfont_duplicates++;
		dlog(LOGSYS, "! %s (duplicate id)\n", filepath, id);
		return FALSE;
	}

	// add to list
	cFont* font = snew cFont();
	font->m_id = id;
	font->m_group = group;
	font->m_font = snew r9Font();
	font->m_font->Create(8,8,8,32,128);
	if(!font->m_font->Create(filepath))
	{
		gsfont_fail++;
		dlog(LOGSYS, "! %s (failed to load)\n", filepath);
		sdelete(font);
		return FALSE;
	}
	m_font.Add(font);

	if(IS_DEVELOPER()) // log for developers
		dlog(LOGAPP, "  %s\n", filepath );
	
	return TRUE;
	unguard()
}

void FFCallback_Font( char* filepath, BOOL dir )
{
	guard(FFCallback_Font)
	if(dir) return;
	BOOL ret = g_paint.FontLoadFile(filepath,gsfont_group);
	unguard()
}

BOOL cDizPaint::FontLoad( char* path, int group )
{
	guard(cDizPaint::FontLoad)
	if(!path || !path[0]) return FALSE; // invalid path
	int szlen = (int)strlen(path);
	if(path[szlen-1]!='\\') strcat(path,"\\");
	_strlwr(path);
	dlog(LOGAPP,"Loading fonts from \"%s\" (group=%i)\n", path, group);

	// init
	gsfont_total		= 0;
	gsfont_fail			= 0;
	gsfont_duplicates	= 0;
	gsfont_group		= group;

	// find files on disk
	int archivefiles = F9_ArchiveGetFileCount(0);
	if(archivefiles==0) // if no archive found then open from disk
	{
		file_findfiles( path, FFCallback_Font, FILE_FINDREC );
	}
	else // if archive opened, load from it
	{
		for(int i=0;i<archivefiles;i++)
		{
			char* filename = F9_ArchiveGetFileName(0,i);
			if(strstr(filename,path)==filename)
			{
				FFCallback_Font(filename,FALSE);
			}
		}
	}

	// report
	dlog(LOGAPP, "Fonts report: total=%i, failed=%i (duplicates=%i)\n\n", gsfont_total, gsfont_fail, gsfont_duplicates );

	return TRUE;
	unguard()
}

void cDizPaint::FontUnload( int group )
{
	guard(cDizPaint::FontUnload)
	int i;
	for(i=0;i<m_font.Size();i++)
	{
		if(m_font[i]->m_group==group)
		{
			FontDel(i);
			i--;
		}
	}
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
