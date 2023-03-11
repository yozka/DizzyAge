//////////////////////////////////////////////////////////////////////////////////////////////////
// GUITile.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GUITile.h"
#include "GUI.h"
#include "EdiPaint.h"
#include "EdiApp.h"
#include "EdiMap.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUITile
//////////////////////////////////////////////////////////////////////////////////////////////////
cGUITile::cGUITile()
{
	guard(cGUITile::cGUITile)
	SetInt(IV_IMGALIGN,0);
	unguard()
}


cGUITile::~cGUITile()
{
	guard(cGUITile::~cGUITile)
	unguard()
}

void cGUITile::Draw()
{
	guard(cGUITile::Draw)
	RECT rc;
	GetScrRect(rc);

	// tile
	int idx = g_paint.TileFind(GetInt(IV_VALUE));
	cTile* tile = g_paint.TileGet(idx); 
	if(tile==NULL) return;
	int x = rc.left;
	int y = rc.top;
	int w = tile->GetWidth();
	int h = tile->GetHeight();

	// sprite
	fRect src(0,0,w,h);

	// shrink or clip
	float scale = (float)GetInt(IV_GUITILE_SCALE);
	int shrink = GetInt(IV_GUITILE_SHRINK);
	if( w*scale > rc.right-rc.left )
	{
		if(shrink)
			scale = (float)(rc.right-rc.left) / w;
		else
			src.x2 = (rc.right-rc.left) / scale;
	}
	if( h*scale > rc.bottom-rc.top )
	{
		if(shrink)
			scale = (float)(rc.bottom-rc.top) / h;
		else
			src.y2 = (rc.bottom-rc.top) / scale;
	}

	// frame anim (1 game frame = 25ms); don't know brush delay !
	int frame = GetTickCount() / (25*3);
	frame = frame % tile->m_frames;

	src.x1 += frame*w;
	src.x2 += frame*w;

	// align
	int align = GetInt(IV_IMGALIGN);
	if((align & GUIALIGN_CENTERX) == GUIALIGN_CENTERX)	x = (rc.left+rc.right-w)/2;	else	
	if(align & GUIALIGN_LEFT)		x = rc.left;				else
	if(align & GUIALIGN_RIGHT)		x = rc.right-w;
	if((align & GUIALIGN_CENTERY) == GUIALIGN_CENTERY)	y = (rc.top+rc.bottom-h)/2;	else	
	if(align & GUIALIGN_TOP)		y = rc.top;				else
	if(align & GUIALIGN_BOTTOM)		y = rc.bottom-h;

	// clipping on
	fRect oldclip = R9_GetClipping();
	R9_SetClipping( fRect(rc.left,rc.top,rc.right,rc.bottom) );

	// background
	iRect rect;
	rect.x1 = x;
	rect.y1 = y;
	rect.x2 = (int)MIN(x+w*scale, rc.right);
	rect.y2 = (int)MIN(y+h*scale, rc.bottom);
	GUIDrawBar(rect.x1, rect.y1, rect.x2, rect.y2, GetInt(IV_COLOR+1)); 
		
	// sprite
	R9_DrawSprite( fV2(x,y), src, tile->m_tex, 0xffffffff, 0, scale );
	
	// clipping off
	R9_SetClipping(oldclip);

	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUITileMap
//////////////////////////////////////////////////////////////////////////////////////////////////
cGUITileMap::cGUITileMap()
{
	guard(cGUITileMap::cGUITileMap)
	m_mode = 0;
	m_movex = 0;
	m_movey = 0;
	unguard()
}

cGUITileMap::~cGUITileMap()
{
	guard(cGUITileMap::~cGUITileMap)
	unguard()
}

void cGUITileMap::Update()
{
	guard(cGUITileMap::Update)

	int selx = GetInt(IV_GUITILEMAP_MAP+0);
	int sely = GetInt(IV_GUITILEMAP_MAP+1);
	int selw = GetInt(IV_GUITILEMAP_MAP+2)-selx;
	int selh = GetInt(IV_GUITILEMAP_MAP+3)-sely;
	if(selw<0) selw=0;
	if(selh<0) selh=0;

	// tile info
	int tileid = GetInt(IV_VALUE);
	int idx = g_paint.TileFind(tileid);
	cTile* tile = g_paint.TileGet(idx); 
	if(tile==NULL) return;
	int tilew = tile->GetWidth();
	int tileh = tile->GetHeight();

	RECT rc;
	GetScrRect(rc); // control rect in screen
	m_mousein = INRECT( g_gui->m_mousex, g_gui->m_mousey, rc);
	int mx = g_gui->m_mousex - rc.left;	// mousex relative to client
	int my = g_gui->m_mousey - rc.top;	// mousey relative to client
	mx = (int)((float)mx / GetInt(IV_GUITILEMAP_SCALE)); // to tile space
	my = (int)((float)my / GetInt(IV_GUITILEMAP_SCALE)); // to tile space

	iRect rctile; // tile rect
	rctile.x1 = 0;
	rctile.y1 = 0;
	rctile.x2 = tilew;
	rctile.y2 = tileh;
	BOOL mouseintile = INRECT( mx, my, rctile);

	iRect rcsel;	// selection rect
	rcsel.x1 = selx;
	rcsel.y1 = sely;
	rcsel.x2 = selx+selw;
	rcsel.y2 = sely+selh;
	BOOL mouseinsel = INRECT( mx, my, rcsel);

	// additional keys for snap and grid
	BOOL shift	= (I9_GetKeyValue(I9K_LSHIFT)) || (I9_GetKeyValue(I9K_RSHIFT));
	if(I9_GetKeyDown(I9K_S))	SetInt( IV_GUITILEMAP_SNAP, !GetInt(IV_GUITILEMAP_SNAP) );
	if(I9_GetKeyDown(I9K_G))	SetInt( IV_GUITILEMAP_GRID, !GetInt(IV_GUITILEMAP_GRID) );
	if(I9_GetKeyDown(I9K_A))	SetInt( IV_GUITILEMAP_AXES, !GetInt(IV_GUITILEMAP_AXES) );
	if(I9_GetKeyDown(I9K_LEFT))	if(shift) selw--; else selx--;
	if(I9_GetKeyDown(I9K_RIGHT))	if(shift) selw++; else selx++;
	if(I9_GetKeyDown(I9K_UP))		if(shift) selh--; else sely--;
	if(I9_GetKeyDown(I9K_DOWN))	if(shift) selh++; else sely++;

	// mouse down
	if(m_mode==0)
	{
		if(m_mousein && I9_GetKeyDown(I9_MOUSE_B1))
		{ 
			// start new selection
			m_mode = 1;
			selx = Snap(mx);
			sely = Snap(my);
			selw = 0;
			selh = 0;
			Capture(TRUE);
		}
		if(mouseinsel && I9_GetKeyDown(I9_MOUSE_B2))
		{ 
			// start moving selection
			m_mode = 2;
			m_movex = mx-selx;
			m_movey = my-sely;
			Capture(TRUE);
		}
	}
	else
	if(m_mode==1)	// selecting
	{
		selw = Snap(mx) - selx;
		selh = Snap(my) - sely;

		if(selw<0) selw=0;
		if(selw>tilew-selx)	selw=tilew-selx;
		if(selh<0) selh=0;
		if(selh>tileh-sely)	selh=tileh-sely;
	}
	else
	if(m_mode==2)  // move selection
	{
		selx =  mx - m_movex;
		sely =  my - m_movey;
		selx = Snap(selx);
		sely = Snap(sely);

		if(selx<0) selx=0;
		if(sely<0) sely=0;
		if(selx+selw>tilew) selx=tilew-selw;
		if(sely+selh>tileh) sely=tileh-selh;
	}

	// loosing captures
	if(m_mode==1 && !I9_GetKeyValue(I9_MOUSE_B1))
	{
		m_mode = 0;
		Capture(FALSE);
	}
	if(m_mode==2 && !I9_GetKeyValue(I9_MOUSE_B2))
	{
		m_mode = 0;
		Capture(FALSE);
	}

	
	// bound
	if(selx<0) selx=0;
	if(sely<0) sely=0;
	if(selx>tilew-1) selx=tilew-1;
	if(sely>tileh-1) sely=tileh-1;
	if(selx+selw>tilew) selw=tilew-selx;
	if(sely+selh>tileh) selh=tileh-sely;

	// set back
	SetInt(IV_GUITILEMAP_MAP+0, selx);
	SetInt(IV_GUITILEMAP_MAP+1, sely);
	SetInt(IV_GUITILEMAP_MAP+2, selx+selw);
	SetInt(IV_GUITILEMAP_MAP+3, sely+selh);

	unguard()
}

void cGUITileMap::Draw()
{
	guard(cGUITileMap::Draw)

	RECT rect;
	GetScrRect(rect);

	int selx = GetInt(IV_GUITILEMAP_MAP+0);
	int sely = GetInt(IV_GUITILEMAP_MAP+1);
	int selw = GetInt(IV_GUITILEMAP_MAP+2)-selx;
	int selh = GetInt(IV_GUITILEMAP_MAP+3)-sely;
	if(selw<0) selw=0;
	if(selh<0) selh=0;
	int scale = GetInt(IV_GUITILEMAP_SCALE);

	int mx = g_gui->m_mousex - rect.left; // mousex relative to client
	int my = g_gui->m_mousey - rect.top; // mousey relative to client
	mx = (int)((float)mx / scale); // to tile space
	my = (int)((float)my / scale); // to tile space

	// draw grid
	if(GetInt(IV_GUITILEMAP_GRID))
	{
		int i;
		RECT rc; 
		GetScrRect(rc);
		int step=8;
		dword color = 0xA0ffffff;

		for(i=rc.top; i<rc.bottom; i+=step*scale)
			R9_DrawLine( fV2(rc.left,i), fV2(rc.right,i), color );

		for(i=rc.left; i<rc.right; i+=step*scale)
			R9_DrawLine( fV2(i,rc.top), fV2(i,rc.bottom), color );
	}

	// draw selection
	if( selw!=0 && selh!=0 )
	{
		int x1,x2,y1,y2;
		x1 = rect.left + selx * scale;
		y1 = rect.top + sely * scale;
		x2 = rect.left + (selx+selw) * scale;
		y2 = rect.top + (sely+selh) * scale;
		if( x1!=x2 && y1!=y2 ) 
			GUIDrawRectDot(x1,y1,x2,y2,0xffffffff);
	}

	// draw axes
	if(GetInt(IV_GUITILEMAP_AXES))
	{
		RECT rc; 
		GetScrRect(rc);
		dword color = 0xff00ff00;
		
		int x = rect.left+Snap(mx)*scale;
		int y = rect.top+Snap(my)*scale;
		if(m_mousein)
		{
			R9_DrawLine( fV2(rc.left,y), fV2(rc.right,y), color );
			R9_DrawLine( fV2(x,rc.top), fV2(x,rc.bottom), color );
		}
	}

	// tooltip bar info
	static char sz[64]; sz[0]=0;
	mx=Snap(mx); 
	my=Snap(my);

	if(m_mousein)
	{
		if(m_mode==0)	sprintf( sz,"%i %i", mx, my ); else
		if(m_mode==1)	sprintf( sz,"%i %i\n%i x %i", mx, my, selw, selh ); else
		if(m_mode==2)	sprintf( sz,"%i %i", mx, my );
		g_gui->SetTooltip(sz);
	}

	unguard()
}

int cGUITileMap::Snap( int x )
{ 
	guard(cGUITileMap::Snap)
	if(!GetInt(IV_GUITILEMAP_SNAP)) return x;
	return (x/8)*8 + (x%8>=4)*8; // snap closer
	// return x = (x/8)*8; // snap under
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
