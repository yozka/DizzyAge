//////////////////////////////////////////////////////////////////////////////////////////////////
// EdiPaint.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "gs.h"
#include "EdiPaint.h"

cEdiPaint g_paint;

//////////////////////////////////////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////////////////////////////////////

cEdiPaint::cEdiPaint()
{
	guard(cEdiPaint::cEdiPaint)
	m_tilepath[0] = 0;
	m_shadersel = R9_BLEND_ALPHAREP;
	m_brushrect = 0;
	unguard()
}

cEdiPaint::~cEdiPaint()
{
	guard(cEdiPaint::~cEdiPaint)
	// nothing - use Done
	unguard()
}

BOOL cEdiPaint::Init()
{
	guard(cEdiPaint::Init)
	// tiles
	m_tile.Init(64);
	m_hash.Init(64,0);
	return TRUE;
	unguard()
}

void cEdiPaint::Done()
{
	guard(cEdiPaint::Done)
	// tiles
	TileUnload();
	m_hash.Done();
	m_tile.Done();
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// TILES
//////////////////////////////////////////////////////////////////////////////////////////////////
int		gstile_total;			// status report on total tiles declared (load+failed)
int		gstile_fail;			// status report on tiles failed to load
int		gstile_duplicates;		// status report on id duplicates

BOOL cEdiPaint::TileLoadFile( char* filepath )
{
	guard(cEdiPaint::TileLoadFile)
	
	// check file type (not counted if unaccepted); only TGA and PNG files accepted
	char* ext = file_path2ext(filepath); if(!ext) return FALSE;
	if( 0!=stricmp(ext,"tga") && 0!=strcmp(ext,"png") ) return FALSE;
	char* name = file_path2file(filepath); if(!name) return FALSE;
	
	gstile_total++;

	// check name format
	char szt[256];
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
	strcpy(szt,m_tilepath);strcat(szt,filepath);
	r9Img img;
	if(!R9_ImgLoadFile(szt,&img))
	{
		gstile_fail++;
		dlog(LOGSYS, "! %s (load failed)\n", filepath);
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
	tile->m_frames	= frames;
	tile->m_name	= sstrdup(filepath);

	R9_ImgDestroy(&img);
	
	dlog(LOGAPP, "  %s [%i]\n", filepath, frames );
	
	return TRUE;
	unguard()
}

void FFCallback_Tile( char* filepath, BOOL dir )
{
	guard(FFCallback_Tile)
	if(dir) return;
	int n = (int)strlen(g_paint.m_tilepath);
	BOOL ret = g_paint.TileLoadFile(filepath+n);
	unguard()
}

BOOL cEdiPaint::TileLoad( char* path )
{
	guard(cEdiPaint::TileLoad)
	if(!path || !path[0]) return FALSE; // invalid path
	strcpy(m_tilepath,path);
	int szlen = (int)strlen(m_tilepath);
	if( szlen>0 && m_tilepath[szlen-1]!='\\' ) strcat(m_tilepath,"\\");
	_strlwr(m_tilepath);
	dlog(LOGAPP,"Loading tiles from \"%s\"\n", m_tilepath);

	// init
	gstile_total		= 0;
	gstile_fail			= 0;
	gstile_duplicates	= 0;

	// find files on disk
	file_findfiles( m_tilepath, FFCallback_Tile, FILE_FINDREC );

	// report
	dlog(LOGAPP, "Tiles report: total=%i, failed=%i (duplicates=%i)\n", gstile_total, gstile_fail, gstile_duplicates );

	// sort by id
	int i;
	BOOL ok;
	do
	{
		ok=TRUE;
		for(i=1;i<TileCount();i++)
		{
			if(m_tile.Get(i-1)->m_id > m_tile.Get(i)->m_id)
			{
				ok = FALSE;
				cTile* temp = m_tile.Get(i-1);
				m_tile.m_list[i-1] = m_tile.Get(i);
				m_tile.m_list[i] = temp;
			}
		}
	}
	while(!ok);

	// rehash after reordering
	m_hash.Done();
	m_hash.Init(64,0);
	for(i=0;i<TileCount();i++)
	{
		cTile* tile = g_paint.TileGet(i); gsassert(tile!=NULL);
		m_hash.Add( tile->m_id, i );
	}

	return TRUE;
	unguard()
}

void cEdiPaint::TileUnload()
{
	guard(cEdiPaint::TileUnload)
	// done
	m_hash.Done();
	for(int i=0; i<m_tile.Size();i++) 
	{
		R9_TextureDestroy(m_tile.Get(i)->m_tex);
		if(m_tile.Get(i)->m_name) sfree(m_tile.Get(i)->m_name);
	}
	m_tile.Done();
	// init
	m_tile.Init(64);
	m_hash.Init(64,0);
	unguard()
}

int cEdiPaint::TileAdd( int id )
{
	guard(cEdiPaint::TileAdd)
	if(id<0) return -1; // negative ids not accepted

	// check duplicate id
	if(TileFind(id)!=-1) return -1; // duplicate id
	
	// add new tile to list
	cTile* tile = snew cTile();
	tile->m_id = id;
	int idx = m_tile.Add(tile); sassert(idx!=-1);

	// add tracker to hash
	m_hash.Add( tile->m_id, idx );
	return idx;
	unguard()
}

void cEdiPaint::TileDel( int idx )
{
	guard(cEdiPaint::TileDel)
	cTile* tile = TileGet(idx); 
	if(tile==NULL) return;
	m_hash.Del(tile->m_id);
	m_tile.Del(idx);
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Draw functions
//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiPaint::DrawTile( int idx, int x, int y, iRect& map, dword color, int flip, int frame, int blend, float scale )
{
	guard(cEdiPaint::DrawTile)
	cTile* tile = TileGet(idx); 
	if(tile==NULL) return;
	
	int w = tile->GetWidth();
	int h = tile->GetHeight();
	if(frame<0) frame=0;
	frame = frame % tile->m_frames;
	
	fRect src = map;
	src.x1 += frame * w;
	src.x2 += frame * w;
	fV2 pos( x, y );

	R9_SetState(R9_STATE_BLEND,blend);
	R9_DrawSprite( pos, src, tile->m_tex, color, flip, (float)scale );

	unguard()
}
	
void cEdiPaint::DrawTile( int idx, int x, int y, dword color, int flip, int frame, int blend, float scale )
{
	guard(cEdiPaint::DrawTile)
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
	fV2 pos( x, y );

	R9_SetState(R9_STATE_BLEND,blend);
	R9_DrawSprite( pos, src, tile->m_tex, color, flip, scale );

	unguard()
}

void cEdiPaint::DrawBrushAt( tBrush* brush, int x, int y, float zoom, BOOL anim )
{
	guard(cEdiPaint::DrawBrushAt)
	if(brush==NULL) return;
	int idx = TileFind(brush->m_data[BRUSH_TILE]);
	// if(idx==-1) return;

	iRect map;
	map.x1 = brush->m_data[BRUSH_MAP+0];
	map.y1 = brush->m_data[BRUSH_MAP+1];
	map.x2 = brush->m_data[BRUSH_MAP+2];
	map.y2 = brush->m_data[BRUSH_MAP+3];
	int mw = (int)GET_BRUSH_MAPWITH(*brush);
	int mh = (int)GET_BRUSH_MAPHEIGHT(*brush);
	float ms = GET_BRUSH_MAPSCALE(*brush);
	if( mw==0 || mh==0 ) return;

	fRect oldclip = R9_GetClipping();
	fRect newclip( (float)x, (float)y, (float)x+zoom*brush->m_data[BRUSH_W], (float)y+zoom*brush->m_data[BRUSH_H] );
	R9_AddClipping(newclip);
	if(!R9_IsClipping()) { R9_SetClipping(oldclip); return; } // fully clipped

	if(idx==-1) // no tile
	{
		dword color = brush->m_data[BRUSH_COLOR] & 0xffff40ff;
		R9_DrawLine( fV2(newclip.x1,newclip.y1),		fV2(newclip.x2-1,newclip.y1),	color );
		R9_DrawLine( fV2(newclip.x2-1,newclip.y1),		fV2(newclip.x2-1,newclip.y2-1),	color );
		R9_DrawLine( fV2(newclip.x2-1,newclip.y2-1),	fV2(newclip.x1,newclip.y2-1),	color );
		R9_DrawLine( fV2(newclip.x1,newclip.y2-1),		fV2(newclip.x1,newclip.y1),		color );
		R9_DrawLine( fV2(newclip.x1,newclip.y1),		fV2(newclip.x2-1,newclip.y2-1),	color );
		R9_DrawLine( fV2(newclip.x2-1,newclip.y1),		fV2(newclip.x1,newclip.y2-1),	color );
		R9_SetClipping(oldclip);
		return;
	}

	int cx = (brush->m_data[BRUSH_W]+mw-1) / mw;
	int cy = (brush->m_data[BRUSH_H]+mh-1) / mh;

	int shader = brush->m_data[BRUSH_SHADER];
	if(shader<0 || shader>=SHADER_MAX) shader = m_shadersel;

	int frame = brush->m_data[BRUSH_FRAME];
	//@ if(!anim) frame=0;
	
	int xt=x;
	for(int i=0;i<cy;i++)
	{
		x = xt;
		for(int j=0;j<cx;j++)
		{
			DrawTile( idx, x, y, map, brush->m_data[BRUSH_COLOR], brush->m_data[BRUSH_FLIP], frame, shader, zoom*ms );
			x+=(int)(zoom*mw);
		}
		y+=(int)(zoom*mh);
	}

	if(m_brushrect)
	{
		dword color = 0xa04040ff;
		R9_DrawLine( fV2(newclip.x1,newclip.y1),		fV2(newclip.x2-1,newclip.y1),	color );
		R9_DrawLine( fV2(newclip.x2-1,newclip.y1),		fV2(newclip.x2-1,newclip.y2-1),	color );
		R9_DrawLine( fV2(newclip.x2-1,newclip.y2-1),	fV2(newclip.x1,newclip.y2-1),	color );
		R9_DrawLine( fV2(newclip.x1,newclip.y2-1),		fV2(newclip.x1,newclip.y1),		color );
	}

	R9_SetClipping(oldclip);
	unguard()
}

void cEdiPaint::DrawBrushFlashAt( tBrush* brush, int x, int y, float zoom, BOOL anim )
{
	if(brush==NULL) return;
	int shd = brush->m_data[BRUSH_SHADER];
	int col = brush->m_data[BRUSH_COLOR];
	brush->m_data[BRUSH_SHADER] = -1; // will use the shadersel (with the ALPHAREP blend)
	brush->m_data[BRUSH_COLOR] = GetFlashingColorBW();
	g_paint.DrawBrushAt( brush, x, y, zoom, anim );
	brush->m_data[BRUSH_SHADER] = shd ;
	brush->m_data[BRUSH_COLOR] = col;
}

dword cEdiPaint::GetFlashingColorBW()
{
	guard(cEdiPaint::GetFlashingColorBW)
	int period = 500;
	float pow = (float)((sys_gettickcount())%period) / period;
	if(pow>0.5f) pow = 1.0f-pow;
	dword color = (dword)(pow * 255);
	color = 0xa0000000 | (color<<16) | (color<<8) | (color);
	return color;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
