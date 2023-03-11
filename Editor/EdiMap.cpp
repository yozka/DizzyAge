//////////////////////////////////////////////////////////////////////////////////////////////////
// EdiMap.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "I9Input.h"
#include "gs.h"

#include "EdiMap.h"
#include "EdiApp.h"
#include "GUI.h"

#include "R9ImgLoader.h"

cEdiMap g_map;

//////////////////////////////////////////////////////////////////////////////////////////////////
// PARTITION
//////////////////////////////////////////////////////////////////////////////////////////////////
cPartitionCel::cPartitionCel()
{
	guard(cPartitionCel::cPartitionCel)
	m_count=0;
	m_size=0;
	m_data=NULL;
	unguard()
}

cPartitionCel::~cPartitionCel()
{
	guard(cPartitionCel::~cPartitionCel)
	if(m_data!=NULL) sfree(m_data);
	unguard()
}

void cPartitionCel::Add( int val )
{
	guard(cPartitionCel::cPartitionCel)
	if(m_count==m_size)
	{
		m_size+=256;
		m_data = (int*)srealloc(m_data,m_size*sizeof(int));
		sassert(m_data!=NULL);
	}
	m_data[m_count]=val;
	m_count++;
	unguard()
}

void cPartitionCel::Sub( int val )
{
	guard(cPartitionCel::cPartitionCel)
	for(int i=0;i<m_count;i++)
	{
		if(m_data[i]==val) 
		{
			if( i<m_count-1 ) 
				memcpy( &m_data[i], &m_data[i+1], (m_count-1-i)*sizeof(int) );
			m_count--;
			return;
		}
	}
	unguard()
}

int cPartitionCel::Find( int val )
{
	guard(cPartitionCel::Find)
	for(int i=0;i<m_count;i++)
		if(m_data[i]==val) return i;
	return -1;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// INIT
//////////////////////////////////////////////////////////////////////////////////////////////////

cEdiMap::cEdiMap()
{
	guard(cEdiMap::cEdiMap)
	
	// map
	m_mapw			= 0;
	m_maph			= 0;
	m_roomw			= 0;
	m_roomh			= 0;
	m_roomgrid		= 0;

	// view
	m_viewx			= 0;
	m_viewy			= 0;
	m_vieww			= 0;
	m_viewh			= 0;

	// camera
	m_camx			= 0;
	m_camy			= 0;
	m_camz			= 1;
	
	// refresh
	m_hideselected	= FALSE;
	m_refresh		= TRUE;
	m_target		= NULL;

	// brushes
	m_brushcount	= 0;
	m_brushsize		= 0;
	m_brush			= NULL;
	m_brushviscount	= 0;
	m_brushvis		= NULL;

	// selection
	m_selectcount	= 0;
	m_selectgoto	= 0;

	// others
	m_count_brushdraw = 0;
	m_count_brushcheck = 0;

	unguard()
}

cEdiMap::~cEdiMap()
{
	guard(cEdiMap::~cEdiMap)
	unguard()
}

BOOL cEdiMap::Init()
{
	guard(cEdiMap::Init)

	// create render target shader
	int width = R9_GetWidth();
	int height = R9_GetHeight();
	m_target = R9_TextureCreateTarget(width,height);
	if(!m_target) {	dlog(LOGAPP,"can't create render target."); return FALSE; }

	m_mapw = MAP_SIZEDEFAULT;
	m_maph = MAP_SIZEDEFAULT;
	CheckMapView();

	m_camx = m_mapw/2;
	m_camy = m_maph/2;

	// partitioning
	PartitionInit();

	// clear the target
	if(R9_BeginScene(m_target))
	{
		R9_Clear(EdiApp()->GetColor(EDI_COLORMAP));
		R9_EndScene();
	}

	return TRUE;
	unguard()
}

void cEdiMap::Done()
{
	guard(cEdiMap::Done)

	MarkerClear();
	PartitionDone();

	// refresh
	if(m_target) { R9_TextureDestroy(m_target); m_target=NULL; }
	
	// brushes
	if(m_brush) sfree(m_brush);
	m_brush=NULL;
	m_brushcount = 0;
	m_brushsize	= 0;
	if(m_brushvis) sfree(m_brushvis);
	m_brushviscount=NULL;
	m_brushviscount=0;
	m_brushvissize=0;

	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// UPDATE
//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiMap::Update( float dtime )
{
	guard(cEdiMap::Update)

	int stepx = EdiApp()->m_gridsize;
	int stepy = EdiApp()->m_gridsize;
	int mx = EdiApp()->GetMouseX() - VIEWX;
	int my = EdiApp()->GetMouseY() - VIEWY;
	int mz = I9_GetAxeDelta(I9_MOUSE_Z);
	BOOL shift	= (I9_GetKeyValue(I9K_LSHIFT))	 || (I9_GetKeyValue(I9K_RSHIFT));
	BOOL alt	= (I9_GetKeyValue(I9K_LALT))	 || (I9_GetKeyValue(I9K_RALT)) || (I9_GetKeyValue(I9_MOUSE_B3));
	BOOL ctrl	= (I9_GetKeyValue(I9K_LCONTROL)) || (I9_GetKeyValue(I9K_RCONTROL));

	// navigation
	int dx = 0;
	int dy = 0;
	
	// key
	if(ctrl) 
	{
		stepx=m_roomw;
		stepy=m_roomh;
	}
	if( I9_GetKeyValue(I9K_RIGHT) )	dx = stepx;
	if( I9_GetKeyValue(I9K_LEFT) )	dx =-stepx;
	if( I9_GetKeyValue(I9K_DOWN) )	dy = stepy;
	if( I9_GetKeyValue(I9K_UP) )	dy =-stepy;

	// smart key hit delaying system
	static int keycnt=0; // key delay counter
	if( dx!=0 || dy!=0 )
	{
		if( keycnt>0 ) // allow first hit
		{
			if(keycnt<400 ) 
			{
				dx = dy = 0; // big wait when first pressed
			}
			else
			{
				keycnt=350; // allow this hit and request another small wait
			}
		}
		keycnt += 1+(int)(dtime*1000); // grow keycounter (make sure it grows)
	}
	else
	{
		keycnt=0; // reset keycounter
	}

	// vertical scroll
	if( mz!=0 && INRECT( mx, my, iRect(VIEWW, 0, VIEWW+VIEWB, VIEWH) ) )
	{
		if(mz<0) dy = stepy;
		if(mz>0) dy =-stepy;
		EdiApp()->m_mscrolly = 0;
		EdiApp()->m_mscrollx = 0;
	}
	
	// horizontal scroll
	if( mz!=0 && INRECT( mx, my, iRect(0, VIEWH, VIEWW, VIEWH+VIEWB) ) )
	{
		if(mz<0) dx = stepx;
		if(mz>0) dx =-stepx;
		EdiApp()->m_mscrolly = 0;
		EdiApp()->m_mscrollx = 0;
	}

	// scroll with the Scroll function (from WM_MOUSEWHEEL)
	if(EdiApp()->m_mscrolly!=0) { dy = EdiApp()->m_mscrolly * stepy; EdiApp()->m_mscrolly = 0; }
	if(EdiApp()->m_mscrollx!=0) { dx = EdiApp()->m_mscrollx * stepx; EdiApp()->m_mscrollx = 0; }

	// scrolling
	iRect rc;
	mx = EdiApp()->GetMouseX();
	my = EdiApp()->GetMouseY();
	if(!m_scrolling && I9_GetKeyDown(I9_MOUSE_B1))
	{
		rc = GetHScrollRect();
		if(rc.IsInside(fV2(mx,my))) 
		{
			m_scrolling = 1;
			m_scrollofs = mx-rc.left;
			E9_AppSetCursor(E9_CURSOR_HAND);
		}
		else
		{
			rc = GetVScrollRect();
			if(rc.IsInside(fV2(mx,my)))
			{
				m_scrolling = 2;
				m_scrollofs = my-rc.top;
				E9_AppSetCursor(E9_CURSOR_HAND);
			}
		}
	}
	else
	if(m_scrolling && !I9_GetKeyValue(I9_MOUSE_B1))
	{
		m_scrolling = 0;
		E9_AppSetCursor(E9_CURSOR_ARROW);
	}

	if(m_scrolling==1) // scroll horizontal
	{
		rc = GetHScrollRect();
		dx = (mx-m_scrollofs)-rc.left;
		dx = (dx/EdiApp()->m_gridsize)*EdiApp()->m_gridsize;
	}
	else
	if(m_scrolling==2) // scroll vertical
	{
		rc = GetVScrollRect();
		dy = (my-m_scrollofs)-rc.top;
		dy = (dy/EdiApp()->m_gridsize)*EdiApp()->m_gridsize;
	}


	// apply
	if( dx!=0 || dy!=0 )
	{
		m_camx += dx;
		m_camy += dy;
		m_refresh=TRUE;
	}

	// others
	if(!alt && !shift && !ctrl)
	{
		if(I9_GetKeyDown(I9K_A))			{ EdiApp()->m_axes = !EdiApp()->m_axes; }
		if(I9_GetKeyDown(I9K_S))			{ EdiApp()->m_snap = !EdiApp()->m_snap; }
		if(I9_GetKeyDown(I9K_G))			{ EdiApp()->m_grid = !EdiApp()->m_grid; m_refresh=TRUE; }
	}

	// bounds
	if(m_camz<1) m_camz=1;
	if(m_camz>4) m_camz=4;

	int camw = m_vieww/m_camz;
	int camh = m_viewh/m_camz;
	if(m_camx<camw/2) m_camx=camw/2;
	if(m_camy<camh/2) m_camy=camh/2;
	if(m_camx>m_mapw-camw/2) m_camx=m_mapw-camw/2;
	if(m_camy>m_maph-camh/2) m_camy=m_maph-camh/2;

	if(m_refresh) Refresh(); // @HM is it safe for draw (needs to be after bounds checks) !
	m_refresh = FALSE;

	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// DRAW
//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiMap::Draw()
{
	guard(cEdiMap::Draw)

	// draw pre-rendered map
	if(!m_target) return; // render target is not supported
	R9_SetState(R9_STATE_BLEND,R9_BLEND_OPAQUE);
	R9_DrawSprite( fV2(m_viewx,m_viewy), fRect(0,0,m_vieww,m_viewh), m_target );
	R9_Flush();
	R9_SetState(R9_STATE_BLEND,R9_BLEND_ALPHA);

	// draw scrollers
	DrawScrollers();

	// draw marker
	int dist = 0;
	int mark = MarkerClosest(m_camx,m_camy,dist);
	if(mark!=-1 && dist==0) // on marker
	{
		R9_DrawBar( fRect(m_viewx-VIEWB+3,m_viewy-VIEWB+3,m_viewx-3, m_viewy-3), 0xffffffff );
	}

	unguard()
}

void cEdiMap::Refresh()
{
	guard(cEdiMap::Refresh)
	// draw in render target
	if(R9_BeginScene(m_target))
	{
		R9_Clear(EdiApp()->GetColor(EDI_COLORMAP));

		iRect view; // camera view in map
		view.x1 = m_camx - (m_vieww/m_camz)/2;
		view.y1 = m_camy - (m_viewh/m_camz)/2;
		view.x2 = m_camx + (m_vieww/m_camz)/2;
		view.y2 = m_camy + (m_viewh/m_camz)/2;

		BrushDrawExtra( view );

		DrawGrid( view );

		R9_EndScene();
	}
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// IO
//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiMap::Reset()
{
	guard(cEdiMap::Reset)

	MarkerClear();
	PartitionDone();
	BrushClear();

	m_mapw = MAP_SIZEDEFAULT;
	m_maph = MAP_SIZEDEFAULT;
	CheckMapView();

	// partitioning
	PartitionInit();

	m_camx = m_mapw/2;
	m_camy = m_maph/2;

	m_refresh = TRUE;
	SelectionRefresh();

	unguard()
}

int cEdiMap::Resize( int width, int height )
{
	if(width<MAP_SIZEMIN)	width = MAP_SIZEMIN;	// too small
	if(height<MAP_SIZEMIN)	height = MAP_SIZEMIN;	// too small
	if(width>MAP_SIZEMAX)	width = MAP_SIZEMAX;	// too big
	if(height>MAP_SIZEMAX)	height = MAP_SIZEMAX;	// too big

	PartitionDone();
	
	m_mapw = width;
	m_maph = height;
	CheckMapView();
	
	PartitionInit();
	BOOL ok = PartitionRepartition();

	SelectionRefresh();
	MarkerResize();

	m_camx = m_mapw/2;
	m_camy = m_maph/2;
	m_refresh = TRUE;
	return ok;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// BRUSHES
//////////////////////////////////////////////////////////////////////////////////////////////////
int	cEdiMap::BrushNew()
{
	guard(cEdiMap::BrushNew)
	if(m_brushcount==m_brushsize)
	{
		m_brushsize += 1024; // grow
		m_brush = (tBrush*)srealloc(m_brush, m_brushsize*sizeof(tBrush));
		sassert(m_brush!=NULL);
	}
	
	// init
	tBrush& brush = m_brush[m_brushcount];
	memset(&brush,0,sizeof(brush));
	brush.m_data[BRUSH_TILE] = -1;
	brush.m_data[BRUSH_COLOR] = 0xffffffff;

	m_brushcount++;
	return m_brushcount-1;
	unguard()
}

void cEdiMap::BrushIns( int idx, tBrush& brush )
{
	guard(cEdiMap::BrushIns)
	sassert(0<=idx && idx<=m_brushcount);
	BrushNew();
	for(int i=m_brushcount-1;i>idx;i--) m_brush[i] = m_brush[i-1];
	m_brush[idx]=brush;
	if(m_brush[idx].m_data[BRUSH_SELECT]) m_selectcount++;
	unguard()
}

void cEdiMap::BrushDel( int idx )
{
	guard(cEdiMap::BrushDel)
	sassert(0<=idx && idx<m_brushcount);
	if(m_brush[idx].m_data[BRUSH_SELECT]) m_selectcount--;
	if( idx<m_brushcount-1 )
		memcpy( &m_brush[idx], &m_brush[idx+1], sizeof(tBrush)*(m_brushcount-1-idx) );
	m_brushcount--;
	unguard()
}


/*
void cEdiMap::BrushDrawOld( iRect& view )
{
	guard(cEdiMap::BrushDrawOld)
	return; // update brushview to be usable !!!???

	m_count_brushdraw = 0;
	m_count_brushcheck = 0;

	tBrush brushtemp = EdiApp()->m_brush;
	int fid = gs_findfn(g_gui->m_vm,"MOD_BrushDraw");

	for( int idx=0; idx<m_brushcount; idx++ )
	{
		m_count_brushcheck++;
		tBrush& brush = m_brush[idx];
		int layer = brush.m_data[BRUSH_LAYER];
		if(layer<0 || layer>=LAYER_MAX) continue;
		if(EdiApp()->LayerGet(layer)==0) continue; // hidden

		iRect bb;
		bb.x1 = brush.m_data[BRUSH_X];
		bb.y1 = brush.m_data[BRUSH_Y];
		bb.x2 = brush.m_data[BRUSH_X] + brush.m_data[BRUSH_W];
		bb.y2 = brush.m_data[BRUSH_Y] + brush.m_data[BRUSH_H];
		if(!RECT2RECT(view,bb)) continue;

		// user callback
		EdiApp()->m_brush = brush;
		if(fid!=-1)
		{
			int ret=g_gui->ScriptCallback(fid);
			if(!ret) continue;
		}

		int x = m_camz * (EdiApp()->m_brush.m_data[BRUSH_X]-view.x1);
		int y = m_camz * (EdiApp()->m_brush.m_data[BRUSH_Y]-view.y1);
		g_paint.DrawBrushAt( &EdiApp()->m_brush, x, y, m_camz );

		m_count_brushdraw++;
	}

	EdiApp()->m_brush = brushtemp;

	unguard()
}
*/

void cEdiMap::BrushDrawExtra( iRect& view )
{
	guard(cEdiMap::BrushDrawExtra)
	int i;
	m_count_brushdraw = 0;
	m_count_brushcheck = 0;

	tBrush brushtemp = EdiApp()->m_brush;
	int fid = gs_findfn(g_gui->m_vm,"MOD_BrushDraw");

	int partition[32];
	int partitioncount = PartitionGet(view, partition,32);
	if( partitioncount==0 ) return;

	// brushvis is a draw buffer that holds indexes to brushes accepted for draw; will be order before draw
	m_brushviscount = 0;

	// check partitions for draw
	for( int p=0; p<partitioncount; p++ )
	{
		int pidx = partition[p];
		int brushcount = m_partition.Get(pidx)->m_count;
		for( i=0; i<brushcount; i++ )
		{
			int idx = m_partition.Get(pidx)->m_data[i];
			sassert(0<=idx && idx<m_brushcount);
			m_count_brushcheck++;

			tBrush& brush = m_brush[idx];
			int layer = brush.m_data[BRUSH_LAYER];
			if(layer<0 || layer>=LAYER_MAX) continue;
			if(EdiApp()->LayerGet(layer)==0) continue; // hidden

			iRect bb;
			bb.x1 = brush.m_data[BRUSH_X];
			bb.y1 = brush.m_data[BRUSH_Y];
			bb.x2 = brush.m_data[BRUSH_X] + brush.m_data[BRUSH_W];
			bb.y2 = brush.m_data[BRUSH_Y] + brush.m_data[BRUSH_H];
			if(!RECT2RECT(view,bb)) continue;

			// store in drawbuffer
			if(m_brushviscount==m_brushvissize)
			{
				m_brushvissize+=1024;
				m_brushvis = (int*)srealloc(m_brushvis, m_brushvissize*sizeof(int));
			}
			m_brushvis[m_brushviscount] = idx;
			m_brushviscount++;
		}
	}

	// order drawbuffer by index // @TODO optimize
	BOOL ord;
	while(TRUE)
	{
		ord=TRUE;
		for(i=0;i<m_brushviscount-1;i++)
		{
			int l1 = m_brush[m_brushvis[i]].m_data[BRUSH_LAYER];
			int l2 = m_brush[m_brushvis[i+1]].m_data[BRUSH_LAYER];
			if( (l1>l2) || 
				((l1==l2) && (m_brushvis[i]>m_brushvis[i+1]))
				)
			{
				int t=m_brushvis[i+1];
				m_brushvis[i+1]=m_brushvis[i];
				m_brushvis[i]=t;
				ord=FALSE;
			}
		}
		if(ord) break;
	}

	// remove duplicates // @TODO optimize
	for(i=0;i<m_brushviscount-1;i++)
	{
		if(m_brushvis[i]==m_brushvis[i+1])
		{
			memcpy( &m_brushvis[i], &m_brushvis[i+1], (m_brushviscount-1-i)*sizeof(int) );
			m_brushviscount--;
			i--; // redo this new one
		}
	}

	// draw drawbuffer
	for( i=0; i<m_brushviscount; i++ )
	{
		int idx = m_brushvis[i];
		tBrush& brush = m_brush[idx];

		// user callback
		EdiApp()->m_brush = brush;
		if(fid!=-1)
		{
			int ret=g_gui->ScriptCallback(fid);
			if(!ret) continue;
		}

		if( m_hideselected && brush.m_data[BRUSH_SELECT] ) continue;
		int x = m_camz * (EdiApp()->m_brush.m_data[BRUSH_X]-view.x1);
		int y = m_camz * (EdiApp()->m_brush.m_data[BRUSH_Y]-view.y1);
		g_paint.DrawBrushAt( &EdiApp()->m_brush, x, y, (float)m_camz );

		m_count_brushdraw++;
	}

	EdiApp()->m_brush = brushtemp;

	unguard()
}

int	cEdiMap::BrushPick( int x, int y )
{
	guard(cEdiMap::BrushPick)
	for( int idx=m_brushcount-1; idx>=0; idx-- ) // top to bottom
	{
		tBrush& brush = m_brush[idx];
		int layer = brush.m_data[BRUSH_LAYER];
		if(layer<0 || layer>=LAYER_MAX) continue;
		if(EdiApp()->LayerGet(layer)==0) continue; // hidden

		iRect bb;
		bb.x1 = brush.m_data[BRUSH_X];
		bb.y1 = brush.m_data[BRUSH_Y];
		bb.x2 = brush.m_data[BRUSH_X] + brush.m_data[BRUSH_W];
		bb.y2 = brush.m_data[BRUSH_Y] + brush.m_data[BRUSH_H];
		if(INRECT(x,y,bb)) return idx;
	}
	return -1;
	unguard()
}

void cEdiMap::BrushToFront( int idx )
{
	guard(cEdiMap::BrushToFront)
	sassert(0<=idx && idx<m_brushcount);
	// search	
	int i;
	int idx0=idx;
	int layer = m_brush[idx].m_data[BRUSH_LAYER];
	for(i=idx;i<m_brushcount;i++)
	{
		if(m_brush[i].m_data[BRUSH_LAYER]==layer) idx0=i;
	}
	if(idx0==idx) return; // already in front

	PartitionDel(idx); // delete before messing it

	// shift and replace
	tBrush brush = m_brush[idx];
	for(i=idx;i<idx0;i++)
	{
		m_brush[i] = m_brush[i+1];
	}
	m_brush[idx0] = brush;

	PartitionFix(idx+1,idx0,-1); // fix indices after shifting
	PartitionAdd(idx0); // add new one

	unguard()
}

void cEdiMap::BrushToBack( int idx )
{
	guard(cEdiMap::BrushToBack)
	sassert(0<=idx && idx<m_brushcount);
	// search	
	int i;
	int idx0=idx;
	int layer = m_brush[idx].m_data[BRUSH_LAYER];
	for(i=idx;i>=0;i--)
	{
		if(m_brush[i].m_data[BRUSH_LAYER]==layer) idx0=i;
	}
	if(idx0==idx) return; // already in back

	PartitionDel(idx); // delete before messing it

	// shift and replace
	tBrush brush = m_brush[idx];
	for(i=idx;i>idx0;i--)
	{
		m_brush[i] = m_brush[i-1];
	}
	m_brush[idx0] = brush;

	PartitionFix(idx0, idx-1, 1); // fix indices after shifting
	PartitionAdd(idx0); // add new one
	

	unguard()
}

void cEdiMap::BrushClear()
{
	guard(cEdiMap::BrushClear)
	if(m_brush) sfree(m_brush);
	m_brush=NULL;
	m_brushcount = 0;
	m_brushsize	= 0;
	if(m_brushvis) sfree(m_brushvis);
	m_brushviscount=NULL;
	m_brushviscount=0;
	m_brushvissize=0;
	m_selectcount=0;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// PARTITIONING
//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiMap::PartitionInit()
{
	guard(cEdiMap::PartitionInit)
	m_partition.Done(); // safety
	int pcountw = PartitionCountW();
	int pcounth = PartitionCountH();
	for(int i=0;i<pcountw*pcounth;i++)
	{
		cPartitionCel* partition = snew cPartitionCel();
		m_partition.Add(partition);
	}
	unguard()
}

void cEdiMap::PartitionDone()
{
	guard(cEdiMap::PartitionDone)
	m_partition.Done();
	unguard()
}

BOOL cEdiMap::PartitionAdd( int brushidx )
{
	guard(cEdiMap::PartitionAdd)
	int pcountw = PartitionCountW();
	iRect br;
	br.x1 = m_brush[brushidx].m_data[BRUSH_X];
	br.y1 = m_brush[brushidx].m_data[BRUSH_Y];
	br.x2 = br.x1 + m_brush[brushidx].m_data[BRUSH_W];
	br.y2 = br.y1 + m_brush[brushidx].m_data[BRUSH_H];
	BOOL ok=FALSE;
	for(int i=0; i<m_partition.Size(); i++)
	{
		iRect pr;
		pr.x1 = (i%pcountw) * PARTITION_CELSIZE;
		pr.y1 = (i/pcountw) * PARTITION_CELSIZE;
		pr.x2 = pr.x1 + PARTITION_CELSIZE;
		pr.y2 = pr.y1 + PARTITION_CELSIZE;
		if( RECT2RECT(br,pr) )
		{	
			m_partition.Get(i)->Add(brushidx);
			ok = TRUE;
		}
	}
	return ok;
	unguard()
}

void cEdiMap::PartitionDel( int brushidx )
{
	guard(cEdiMap::PartitionDel)
	int pcountw = PartitionCountW();
	iRect br;
	br.x1 = m_brush[brushidx].m_data[BRUSH_X];
	br.y1 = m_brush[brushidx].m_data[BRUSH_Y];
	br.x2 = br.x1 + m_brush[brushidx].m_data[BRUSH_W];
	br.y2 = br.y1 + m_brush[brushidx].m_data[BRUSH_H];
	for(int i=0; i<m_partition.Size(); i++)
	{
		iRect pr;
		pr.x1 = (i%pcountw) * PARTITION_CELSIZE;
		pr.y1 = (i/pcountw) * PARTITION_CELSIZE;
		pr.x2 = pr.x1 + PARTITION_CELSIZE;
		pr.y2 = pr.y1 + PARTITION_CELSIZE;
		if( RECT2RECT(br,pr) )
			m_partition.Get(i)->Sub(brushidx);
	}
	unguard()
}

int	cEdiMap::PartitionGet( iRect& rect, int* buffer, int buffersize )
{
	guard(cEdiMap::PartitionGet)
	sassert(buffer!=NULL);
	sassert(buffersize>0);
	int pcountw = PartitionCountW();
	int count = 0;
	for(int i=0; i<m_partition.Size(); i++)
	{
		iRect pr;
		pr.x1 = (i%pcountw) * PARTITION_CELSIZE;
		pr.y1 = (i/pcountw) * PARTITION_CELSIZE;
		pr.x2 = pr.x1 + PARTITION_CELSIZE;
		pr.y2 = pr.y1 + PARTITION_CELSIZE;
		if( RECT2RECT(rect,pr) )
		{
			buffer[count] = i;
			count++;
			if(count==buffersize) break;
		}
	}	
	return count;
	unguard()
}

void cEdiMap::PartitionFix( int brushidx1, int brushidx2, int delta )
{
	guard(cEdiMap::PartitionFix)
	for(int i=0; i<m_partition.Size(); i++)
	{
		cPartitionCel* pcel = m_partition.Get(i);
		for(int j=0; j<pcel->m_count; j++)
		{
			if( pcel->m_data[j]>=brushidx1 && pcel->m_data[j]<=brushidx2 )
				pcel->m_data[j] += delta;
		}
	}
	unguard()
}

BOOL cEdiMap::PartitionRepartition()
{
	guard(cEdiMap::PartitionRepartition)
	int i;
	// force all clean
	for(i=0; i<m_partition.Size(); i++)
		m_partition.Get(i)->m_count = 0;
	// repartition all brushes
	BOOL ok = TRUE;
	for(i=0; i<m_brushcount; i++)
		ok &= PartitionAdd(i);
	return ok;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// MARKERS
//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiMap::MarkerToggle( int x, int y )
{
	guard(cEdiMap::MarkerToggle)
	int dist = -1;
	int mark = MarkerClosest( m_camx, m_camy, dist );
	if(mark!=-1 && dist==0) // remove existing mark
	{
		m_marker.Del(mark);
	}
	else // add mark
	{
		m_marker.Add( tMarker(x,y,m_camz) );
	}
	unguard()
}
void cEdiMap::MarkerGoto( int dir )				
{
	guard(cEdiMap::MarkerGoto)
	int dist = -1;
	int mark = MarkerClosest( m_camx, m_camy, dist );
	if(mark==-1) return; // no markers
	if(dist==0) // select next
	{
		mark = mark+dir;
		if(mark<0) mark = m_marker.Size()-1;
		if(mark>m_marker.Size()-1) mark = 0;
	}
	m_camx = m_marker.Get(mark).x;
	m_camy = m_marker.Get(mark).y;
	m_camz = m_marker.Get(mark).z;
	m_refresh = TRUE;
	unguard()
}

int	cEdiMap::MarkerClosest( int x, int y, int &dist )
{
	guard(cEdiMap::MarkerClosest)
	int mark=-1;
	int mind = -1;
	for(int i=0;i<m_marker.Size();i++)
	{
		int mx = m_marker.Get(i).x;
		int my = m_marker.Get(i).y;
		double d = (double)(x-mx)*(double)(x-mx)+(double)(y-my)*(double)(y-my);
		d=sqrt(d);
		if(mind==-1 || (int)d<mind)
		{
			mark=i;
			mind=(int)d;
		}
	}
	dist = mind;
	return mark;
	unguard()
}

void cEdiMap::MarkerClear()
{
	guard(cEdiMap::MarkerClear)
	m_marker.Done();
	unguard()
}

void cEdiMap::MarkerResize()
{
	guard(cEdiMap::MarkerResize)
	// remove markers out of the new map size
	for(int i=0;i<m_marker.Size();i++)
	{
		if(!MarkerTest(i))	
		{
			m_marker.Del(i);
			i--;
		}
	}
	unguard()
}

BOOL cEdiMap::MarkerTest( int idx )
{
	guard(cEdiMap::MarkerTest);
	if(idx<0 || idx>=m_marker.Size()) return FALSE;
	int camx = m_marker[idx].x;
	int camy = m_marker[idx].y;
	int camz = m_marker[idx].z;
	int camw = m_vieww/camz;
	int camh = m_viewh/camz;
	if(camx<camw/2) return FALSE;
	if(camy<camh/2) return FALSE;
	if(camx>m_mapw-camw/2) return FALSE;
	if(camy>m_maph-camh/2) return FALSE;
	return TRUE;
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// SELECTION GOTO
//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiMap::SelectionRefresh()
{
	guard(cEdiMap::SelectionRefresh)
	m_selectcount=0;
	for(int i=0;i<m_brushcount;i++)
	{
		if(m_brush[i].m_data[BRUSH_SELECT]) 
			m_selectcount++;
	}
	unguard()
}

void cEdiMap::SelectionGoto( int dir )
{
	guard(cEdiMap::SelectionGoto)
	if(m_brushcount==0) return;
	sassert(dir==-1 || dir==1);
	if(m_selectgoto<=-1) m_selectgoto = m_brushcount-1;
	if(m_selectgoto>=m_brushcount) m_selectgoto = 0;
	int i = m_selectgoto;
	while(TRUE)
	{
		i+=dir;
		if(i<=-1) i=m_brushcount-1;
		if(i>=m_brushcount) i=0;
		if(m_brush[i].m_data[BRUSH_SELECT]) 
		{
			m_camx = m_brush[i].m_data[BRUSH_X];
			m_camy = m_brush[i].m_data[BRUSH_Y];
			m_refresh = TRUE;
			m_selectgoto = i;
			return;
		}
		if(i==m_selectgoto) return;
	}
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// OTHERS
//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiMap::DrawGrid( iRect &view )
{
	guard(cEdiMap::DrawGrid)
	int i;
	iRect view2;
	R9_SetState(R9_STATE_BLEND,R9_BLEND_ALPHA);

	// TILE GRID
	int grid = EdiApp()->m_gridsize;
	if( EdiApp()->m_grid && grid!=0 ) 
	{
		// snap view
		view2 = view; 
		view2.x1 = view2.x1 / grid * grid;
		view2.y1 = view2.y1 / grid * grid;
		view2.x2 = view2.x2 / grid * grid;
		view2.y2 = view2.y2 / grid * grid;
		
		// vertical
		for( i=view2.x1; i<=view2.x2; i+=grid )
		{
			int x = m_camz * (i - (m_camx - (m_vieww/m_camz)/2));
			R9_DrawLine( fV2(x,0), fV2(x,m_viewh), EdiApp()->GetColor(EDI_COLORGRID1) );
		}

		// horizontal
		for( i=view2.y1; i<=view2.y2; i+=grid )
		{
			int y = m_camz * (i - (m_camy - (m_viewh/m_camz)/2));
			R9_DrawLine( fV2(0,y), fV2(m_vieww,y), EdiApp()->GetColor(EDI_COLORGRID1) );
		}
	}

	// ROOM GRID
	int gridx = m_roomw;
	int gridy = m_roomh;
	if( m_roomgrid && gridx!=0 && gridy!=0 ) 
	{
		// snap view
		view2 = view; 
		view2.x1 = view2.x1 / gridx * gridx;
		view2.y1 = view2.y1 / gridy * gridy;
		view2.x2 = view2.x2 / gridx * gridx;
		view2.y2 = view2.y2 / gridy * gridy;
		
		// vertical
		for( i=view2.x1; i<=view2.x2; i+=gridx )
		{
			int x = m_camz * (i - (m_camx - (m_vieww/m_camz)/2));
			R9_DrawLine( fV2(x,0), fV2(x,m_viewh), EdiApp()->GetColor(EDI_COLORGRID2) );
		}

		// horizontal
		for( i=view2.y1; i<=view2.y2; i+=gridy )
		{
			int y = m_camz * (i - (m_camy - (m_viewh/m_camz)/2));
			R9_DrawLine( fV2(0,y), fV2(m_vieww,y), EdiApp()->GetColor(EDI_COLORGRID2));
		}
	}

	unguard()
}

void cEdiMap::DrawAxes( int x, int y )
{
	guard(cEdiMap::DrawAxes)
	if(!EdiApp()->m_axes) return;

	CAM2VIEW(x,y);
	x += VIEWX;
	y += VIEWY;
	R9_DrawLine( fV2(x,VIEWY), fV2(x,VIEWY+VIEWH), EdiApp()->GetColor(EDI_COLORGRID3) );
	R9_DrawLine( fV2(VIEWX,y), fV2(VIEWX+VIEWW,y), EdiApp()->GetColor(EDI_COLORGRID3) );
	unguard()
}

void cEdiMap::DrawScrollers()
{
	guard(cEdiMap::DrawScrollers)
	fRect rc;
	rc = GetHScrollRect();
	R9_DrawBar(rc, (EdiApp()->GetColor(EDI_COLORBACK2) & 0x00ffffff) | 0x60000000 );
	rc = GetVScrollRect();
	R9_DrawBar(rc, (EdiApp()->GetColor(EDI_COLORBACK2) & 0x00ffffff) | 0x60000000);
	unguard()
}

iRect cEdiMap::GetHScrollRect()
{
	guard(cEdiMap::GetHScrollRect);
	float x1 = (float)(m_camx-(m_vieww/m_camz)/2) / m_mapw * m_vieww;
	float x2 = (float)(m_camx+(m_vieww/m_camz)/2) / m_mapw * m_vieww;
	iRect rc( (float)m_viewx+x1, (float)m_viewy+m_viewh+2, (float)m_viewx+x2, (float)m_viewy+m_viewh+VIEWB );	
	return rc;
	unguard();
}

iRect cEdiMap::GetVScrollRect()
{
	guard(cEdiMap::GetVScrollRect);
	float y1 = (float)(m_camy-(m_viewh/m_camz)/2) / m_maph * m_viewh;
	float y2 = (float)(m_camy+(m_viewh/m_camz)/2) / m_maph * m_viewh;
	iRect rc( (float)m_viewx+m_vieww+2, (float)m_viewy+y1, (float)m_viewx+m_vieww+VIEWB, (float)m_viewy+y2 );
	return rc;
	unguard();
}

void cEdiMap::CheckMapView()
{
	guard(cEdiMap::CheckMapView)
	m_viewx = VIEWB;
	m_viewy = VIEWB+32;
	m_vieww = R9_GetWidth() - 2*VIEWB;
	m_viewh = R9_GetHeight() - (32+16+2*VIEWB);

	if(m_vieww>m_mapw) m_vieww=m_mapw;
	if(m_viewh>m_maph) m_viewh=m_maph;
	m_viewx = VIEWB + (R9_GetWidth() - m_vieww - (2*VIEWB) ) / 2;
	m_viewy = VIEWB + 32 + (R9_GetHeight() - m_viewh - (32+16+2*VIEWB) ) / 2;
	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// SAVE MAP IMAGE
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cEdiMap::SaveMapImage( char* filename )
{
	guard(cEdiMap::SaveMapImage)
	if(!m_target) return FALSE;

	// CREATE IMGHUGE
	r9Img imghuge;
	imghuge.m_pf = R9_PF_RGB;
	imghuge.m_width = m_mapw;
	imghuge.m_height = m_maph;
	if(!R9_ImgCreate(&imghuge)) return FALSE;

	// LOOP
	int w=256;
	int h=256;
	int y=0;
	while(y<m_maph)
	{
		int x=0;
		while(x<m_mapw)
		{
			// draw in render target
			if(R9_BeginScene(m_target))
			{
				R9_Clear(EdiApp()->GetColor(EDI_COLORMAP));

				// DRAW
				int camz = m_camz;
				int camx = m_camx;
				int camy = m_camy;
				int vieww = m_vieww;
				int viewh = m_viewh;
				m_camz = 1;
				m_camx = x + 128;
				m_camy = y + 128;
				m_vieww = w;
				m_viewh = h;
				iRect view; // camera view in map
				view.x1 = x;
				view.y1 = y;
				view.x2 = x+w;
				view.y2 = y+h;
				BrushDrawExtra( view );
				DrawGrid( view );
				m_camz = camz;
				m_camx = camx;
				m_camy = camy;
				m_vieww = vieww;
				m_viewh = viewh;

				// END DRAW
				R9_Flush();
				R9_EndScene();
			}

			fRect rect(x,y,x+w,y+h);
			BOOL ok = R9_CopyTargetToImage(m_target,&imghuge,&rect);

			x+=w;
		}

		y+=h;
	}

	BOOL ok = R9_ImgSaveFile(filename,&imghuge);
	R9_ImgDestroy(&imghuge);
	return ok;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
