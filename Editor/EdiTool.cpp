//////////////////////////////////////////////////////////////////////////////////////////////////
// EdiTool.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EdiTool.h"
#include "EdiApp.h"
#include "EdiMap.h"
#include "GUI.h"

#define SNAP(x,grid)	( ((x)/(grid))*(grid) + ((x)%(grid)>=(grid)/2)*(grid) )

#define SNAP2GRID(x,y)						\
{											\
	if(EdiApp()->m_snap)					\
	{										\
		x=SNAP(x,EdiApp()->m_gridsize);		\
		y=SNAP(y,EdiApp()->m_gridsize);		\
	}										\
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// cEdiToolPaint
// mode: 0=none, 1=paint, 2=alt pick/del
//////////////////////////////////////////////////////////////////////////////////////////////////
cEdiToolPaint::cEdiToolPaint()
{
	guard(cEdiToolPaint::cEdiToolPaint)
	m_brushidx=-1;
	m_isbusy=FALSE;
	strcpy(m_name,"PAINT");
	unguard()
}

cEdiToolPaint::~cEdiToolPaint()
{
	guard(cEdiToolPaint::~cEdiToolPaint)
	unguard()
}

void cEdiToolPaint::Init()
{
	guard(cEdiToolPaint::Init)
	unguard()
}

void cEdiToolPaint::Done()
{
	guard(cEdiToolPaint::Done)
	unguard()
}

void cEdiToolPaint::Switch( BOOL on )
{
	guard(cEdiToolPaint::Switch)
	Reset();
	unguard()
}

void cEdiToolPaint::Reset()
{
	guard(cEdiToolPaint::Reset)
	if( m_mode==2 )	return;
	m_mode=-1; // draw trick
	unguard()
}

void cEdiToolPaint::Update( float dtime )
{
	guard(cEdiToolPaint::Update)

	int mx = EdiApp()->GetMouseX() - VIEWX;
	int my = EdiApp()->GetMouseY() - VIEWY;
	BOOL inview = INVIEW(mx,my);
	if(!inview && !m_isbusy) return;

	BOOL alt = (I9_GetKeyValue(I9K_LALT)) || (I9_GetKeyValue(I9K_RALT));
	BOOL ctrl	= (I9_GetKeyValue(I9K_LCONTROL)) || (I9_GetKeyValue(I9K_RCONTROL));

	m_brushidx = -1; // clear picked brush
	tBrush& brush = EdiApp()->m_brush;

	if( m_mode==-1 ) m_mode=0; // draw trick

	if( m_mode==0 )
	{
		int bw = (int)GET_BRUSH_MAPWITH(brush); // brush.m_data[BRUSH_MAP+2]-brush.m_data[BRUSH_MAP+0];
		int bh = (int)GET_BRUSH_MAPHEIGHT(brush); // brush.m_data[BRUSH_MAP+3]-brush.m_data[BRUSH_MAP+1];
		VIEW2CAM(mx,my);
		if(mx<CAMX1+bw)	mx=CAMX1+bw;
		if(my<CAMY1+bh)	my=CAMY1+bh;
		if(mx>CAMX2)	mx=CAMX2;
		if(my>CAMY2)	my=CAMY2;
		mx = mx-bw;
		my = my-bh;
		SNAP2GRID(mx,my); // grid snap

		brush.m_data[BRUSH_X] = mx;
		brush.m_data[BRUSH_Y] = my;
		brush.m_data[BRUSH_W] = bw;
		brush.m_data[BRUSH_H] = bh;

		if(I9_GetKeyDown(I9_MOUSE_B1)) m_mode=1;
		else
		if(I9_GetKeyValue(I9_MOUSE_B2)) m_mode=2;
		else
		if(I9_GetKeyValue(I9_MOUSE_B3) || alt) m_mode=3;
		else
		if(ctrl && I9_GetKeyDown(I9K_Z)) EdiApp()->Undo();

		// axes
		m_ax = mx;
		m_ay = my;
	}
	else
	if( m_mode==1 )
	{
		VIEW2CAM(mx,my);
		if(mx>CAMX2) mx=CAMX2;
		if(my>CAMY2) my=CAMY2;

		int bw = mx - brush.m_data[BRUSH_X];
		int bh = my - brush.m_data[BRUSH_Y];
		if(bw<0) bw=0;
		if(bh<0) bh=0;
		
		if(I9_GetKeyValue(I9K_LSHIFT))
		{
			int mw = (int)GET_BRUSH_MAPWITH(brush); // brush.m_data[BRUSH_MAP+2]-brush.m_data[BRUSH_MAP+0];
			int mh = (int)GET_BRUSH_MAPHEIGHT(brush); // brush.m_data[BRUSH_MAP+3]-brush.m_data[BRUSH_MAP+1];
			if(mw<1) bw=0; else	bw = (bw / mw) * mw;
			if(mh<1) bh=0; else	bh = (bh / mh) * mh;
		}
		else
			SNAP2GRID(bw,bh); // grid snap

		brush.m_data[BRUSH_W] = bw;
		brush.m_data[BRUSH_H] = bh;

		// axes
		m_ax = brush.m_data[BRUSH_X] + bw;
		m_ay = brush.m_data[BRUSH_Y] + bh;

		if(!I9_GetKeyValue(I9_MOUSE_B1)) 
		{
			// add brush !
			if( brush.m_data[BRUSH_W]>0 && brush.m_data[BRUSH_H]>0 && inview)
			{
				int idx = g_map.BrushNew();
				g_map.m_brush[idx] = brush;
				g_map.m_brush[idx].m_data[BRUSH_SELECT] = 0;
				g_map.m_brush[idx].m_data[BRUSH_LAYER] = EdiApp()->LayerActive();
				g_map.m_refresh = TRUE;
				g_map.PartitionAdd(idx);
				EdiApp()->UndoSet(UNDOOP_DEL,idx,NULL);
			}
			m_mode=0;
		}
	}
	else
	if( m_mode==2 || m_mode==3 ) // pick mode
	{
		VIEW2CAM(mx,my);
		if(mx<CAMX1) mx=CAMX1;
		if(my<CAMY1) my=CAMY1;
		if(mx>CAMX2) mx=CAMX2;
		if(my>CAMY2) my=CAMY2;
		SNAP2GRID(mx,my); // grid snap

		//axes
		m_ax = mx;
		m_ay = my;

		m_brushidx = g_map.BrushPick(mx,my);
	
		if(m_mode==2 && !I9_GetKeyValue(I9_MOUSE_B2))
		{
			if( m_brushidx!=-1 ) g_gui->ScriptDo( sprint("Tool_PickMenu(%i);",m_brushidx) );
			m_mode=-1; // draw trick
		}
		else
		if(m_mode==3 && !I9_GetKeyValue(I9_MOUSE_B3) && !alt)
		{
			if( m_brushidx!=-1 ) brush = g_map.m_brush[m_brushidx];
			m_mode=-1; // draw trick
		}
		else
		{
			// tooltip
			tBrush& brushpick = g_map.m_brush[m_brushidx];
			char sz[128];
			strcpy(sz, (m_mode==2)?"menu":"pick");
			if(m_brushidx!=-1)
			{
				char sz2[32];
				sprintf(sz2," #%i",m_brushidx);
				strcat(sz,sz2);
			}
			g_gui->SetTooltip(sz);
		}
	}

	m_isbusy = (m_mode==1 || m_mode==2 || m_mode==3);
	unguard()
}

void cEdiToolPaint::Draw()
{
	guard(cEdiToolPaint::Draw)
	int mx = EdiApp()->GetMouseX() - VIEWX;
	int my = EdiApp()->GetMouseY() - VIEWY;
	if(!INVIEW(mx,my)) return; // && !m_isbusy

	tBrush& brush = EdiApp()->m_brush;
	tBrush brushtemp = brush;

	// axes
	g_map.DrawAxes(m_ax,m_ay);

	if(m_mode==0||m_mode==1)
	{
		int fid = gs_findfn(g_gui->m_vm,"MOD_BrushToolDraw");
		if( fid!=-1 ) g_gui->ScriptCallback(fid);
		
		int x = CAMZ*(brush.m_data[BRUSH_X] - CAMX1) + VIEWX;
		int y = CAMZ*(brush.m_data[BRUSH_Y] - CAMY1) + VIEWY;
		
		g_paint.DrawBrushAt( &brush, x, y, (float)CAMZ, TRUE ); // animated
	}
	else
	if( (m_mode==2 || m_mode==3) && m_brushidx!=-1 )
	{
		tBrush& brushpick = g_map.m_brush[m_brushidx];
		int x = CAMZ*(brushpick.m_data[BRUSH_X] - CAMX1) + VIEWX;
		int y = CAMZ*(brushpick.m_data[BRUSH_Y] - CAMY1) + VIEWY;
		g_paint.DrawBrushFlashAt( &brushpick, x, y, (float)CAMZ ); // not animated
	}

	brush = brushtemp;

	unguard()
}


void cEdiToolPaint::Command( int cmd )
{
	guard(cEdiToolPaint::Command)
	if(m_brushidx<0 || m_brushidx>g_map.m_brushcount) return;
	
	if(cmd==TOOLCMD_PICKBRUSH)
	{
		EdiApp()->m_brush = g_map.m_brush[m_brushidx];
	}
	else
	if(cmd==TOOLCMD_PICKCOLOR)
	{
		EdiApp()->m_brush.m_data[BRUSH_COLOR] = g_map.m_brush[m_brushidx].m_data[BRUSH_COLOR];
	}
	else
	if(cmd==TOOLCMD_TOFRONT)
	{
		g_map.BrushToFront(m_brushidx);
		g_map.m_refresh = TRUE;
		EdiApp()->UndoReset();
	}
	else
	if(cmd==TOOLCMD_TOBACK)
	{
		g_map.BrushToBack(m_brushidx);
		g_map.m_refresh = TRUE;
		EdiApp()->UndoReset();
	}
	else
	if(cmd==TOOLCMD_DELETE)
	{
		EdiApp()->UndoSet(UNDOOP_ADD, m_brushidx, &g_map.m_brush[m_brushidx]);
		g_map.PartitionDel(m_brushidx);
		g_map.PartitionFix(m_brushidx+1, g_map.m_brushcount-1, -1); // fix indices before shifting
		g_map.BrushDel(m_brushidx);
		m_brushidx=-1;
		g_map.m_refresh = TRUE;
	}
	unguard()
}

void cEdiToolPaint::BeginUserUpdate()
{
	guard(cEdiToolPaint::BeginUserUpdate)
	if( (m_mode==2 || m_mode==3) && m_brushidx!=-1 )
	{
		m_brushtemp = EdiApp()->m_brush;
		EdiApp()->m_brush = g_map.m_brush[m_brushidx];
	}
	unguard()
}

void cEdiToolPaint::EndUserUpdate()
{
	guard(cEdiToolPaint::EndUserUpdate)
	if( (m_mode==2 || m_mode==3) && m_brushidx!=-1 )
	{
		EdiApp()->m_brush = m_brushtemp;
	}
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// cEdiToolEdit
// mode: 0=none, 1=select, 2=move
//////////////////////////////////////////////////////////////////////////////////////////////////
cEdiToolEdit::cEdiToolEdit()
{
	guard(cEdiToolEdit::cEdiToolEdit)
	m_selop = 0;
	m_rect = iRect(0,0,0,0);
	m_movex = m_movey = 0;
	m_movedx = m_movedy = 0;
	m_dragcount = 0;
	m_dragsize = 0;
	m_drag = NULL;
	m_clipcount = 0;
	m_clipsize = 0;		
	m_clip = NULL;
	strcpy(m_name,"EDIT");
	unguard()
}

cEdiToolEdit::~cEdiToolEdit()
{
	guard(cEdiToolEdit::~cEdiToolEdit)
	unguard()
}

void cEdiToolEdit::Init()
{
	guard(cEdiToolEdit::Init)
	unguard()
}

void cEdiToolEdit::Done()
{
	guard(cEdiToolEdit::Done)
	// draglist
	if(m_drag) sfree(m_drag);
	m_dragcount = 0;
	m_dragsize = 0;
	m_drag=NULL;
	// clipboard
	if(m_clip) sfree(m_clip);
	m_clipcount = 0;
	m_clipsize = 0;
	m_clip=NULL;
	unguard()
}

void cEdiToolEdit::Switch( BOOL on )
{
	guard(cEdiToolEdit::Switch)
	Reset();
	unguard()
}

void cEdiToolEdit::Reset()
{
	guard(cEdiToolEdit::Reset)
	if(m_mode==2)
	{
		m_dragcount = 0;
		g_map.m_hideselected = FALSE;
		g_map.m_refresh = TRUE;
		E9_AppSetCursor(E9_CURSOR_ARROW);
	}
	m_mode = 0;
	m_selop = 0;
	m_rect = iRect(0,0,0,0);
	unguard()
}

void cEdiToolEdit::Update( float dtime )
{
	guard(cEdiToolEdit::Update)

	tBrush& brush = EdiApp()->m_brush;

	int mx = EdiApp()->GetMouseX() - VIEWX;
	int my = EdiApp()->GetMouseY() - VIEWY;
	BOOL inview = INVIEW(mx,my);

	VIEW2CAM(mx,my);
	if(mx<CAMX1) mx=CAMX1;
	if(my<CAMY1) my=CAMY1;
	if(mx>CAMX2) mx=CAMX2;
	if(my>CAMY2) my=CAMY2;
	SNAP2GRID(mx,my); // grid snap

	// additional keys
	BOOL shift	= (I9_GetKeyValue(I9K_LSHIFT)) || (I9_GetKeyValue(I9K_RSHIFT));
	BOOL alt	= (I9_GetKeyValue(I9K_LALT)) || (I9_GetKeyValue(I9K_RALT)) || (I9_GetKeyValue(I9_MOUSE_B3));
	BOOL ctrl	= (I9_GetKeyValue(I9K_LCONTROL)) || (I9_GetKeyValue(I9K_RCONTROL));
	
	m_selop = 0;
	if( shift )	m_selop++;
	if( alt )	m_selop--;

	if( m_mode==0 && inview )
	{
		if(I9_GetKeyDown(I9_MOUSE_B1))
		{
			if(m_selop==0) BrushDeselect();
			m_rect.x1 = mx;
			m_rect.y1 = my;
			m_rect.x2 = mx;
			m_rect.y2 = my;
			m_mode=1;
		}
		else
		if(I9_GetKeyDown(I9_MOUSE_B2))
		{
			m_movex = mx;
			m_movey = my;
			m_movedx = 0;
			m_movedy = 0;
			BrushMoveStart();
			m_mode=2;
			E9_AppSetCursor(E9_CURSOR_HAND);
		}
		else
		if(I9_GetKeyDown(I9K_DELETE))	BrushDelete();
		else
		if(ctrl && I9_GetKeyDown(I9K_C)) BrushCopy();
		else
		if(ctrl && I9_GetKeyDown(I9K_V)) { BrushPaste(); g_map.SelectionGoto(); }
		else
		if(ctrl && I9_GetKeyDown(I9K_X)) { BrushCopy(); BrushDelete(); }
	}	
	else
	if( m_mode==1 )
	{
		m_rect.x2 = mx;
		m_rect.y2 = my;
		if(m_rect.x2<m_rect.x1+1) m_rect.x2 = m_rect.x1+1;
		if(m_rect.y2<m_rect.y1+1) m_rect.y2 = m_rect.y1+1;

		if(!I9_GetKeyValue(I9_MOUSE_B1))
		{
			BrushSelect();
			m_mode=0;
		}
	}
	else
	if( m_mode==2 )
	{
		m_movedx = mx - m_movex;
		m_movedy = my - m_movey;

		if(!I9_GetKeyValue(I9_MOUSE_B2))
		{
			BrushMove();
			m_mode=0;
			E9_AppSetCursor(E9_CURSOR_ARROW);
		}
	}

	// axes
	m_ax = mx;
	m_ay = my;

	// tooltip
	char sz[64];
	sz[0]=0;
	if(m_mode!=2)
	{
		if( m_selop==-1 ) strcat(sz,"sub\n");
		if( m_selop==1 ) strcat(sz,"add\n");
		strcat(sz, sprint("%i,%i", mx, my) );
		if( m_mode==1 ) strcat(sz, sprint("\n%i x %i", m_rect.x2-m_rect.x1, m_rect.y2-m_rect.y1) );
	}
	else
	{
		strcat(sz, sprint("mov %i,%i\n", m_movedx, m_movedy) );
		strcat(sz, sprint("%i,%i", mx, my) );
	}
	g_gui->SetTooltip(inview?sz:"");

	m_isbusy = (m_mode!=0);
	unguard()
}

void cEdiToolEdit::Draw()
{
	guard(cEdiToolEdit::Draw)
	int mx = EdiApp()->GetMouseX() - VIEWX;
	int my = EdiApp()->GetMouseY() - VIEWY;
	BOOL inview = INVIEW(mx,my);
	
	g_map.DrawAxes(m_ax,m_ay);

	// offsets
	int dx = 0;
	int dy = 0;
	if(m_mode==2)
	{
		dx = m_movedx;
		dy = m_movedy;
	}

	// draw selected brushes ( from visible or from dragging )
	int count = (m_mode!=2) ? g_map.m_brushviscount : m_dragcount;
	for(int i=0;i<count;i++)
	{
		int idx = (m_mode!=2) ? g_map.m_brushvis[i] : m_drag[i];
		
		tBrush& brush = g_map.m_brush[ idx ];
		if(!brush.m_data[BRUSH_SELECT]) continue;
		int x = VIEWX + CAMZ * (brush.m_data[BRUSH_X]-CAMX1+dx);
		int y = VIEWY + CAMZ * (brush.m_data[BRUSH_Y]-CAMY1+dy);

		int shd = brush.m_data[BRUSH_SHADER];
		int col = brush.m_data[BRUSH_COLOR];
		brush.m_data[BRUSH_SHADER] = -1;
		brush.m_data[BRUSH_COLOR] = g_paint.GetFlashingColorBW();
		g_paint.DrawBrushAt( &brush, x, y, (float)CAMZ );
		brush.m_data[BRUSH_SHADER] = shd;
		brush.m_data[BRUSH_COLOR] = col;
	}

	if( m_mode==1 )	
	{
		dword color = 0xffffffff;
		iRect rect = m_rect;
		CAM2VIEW(rect.x1,rect.y1);
		CAM2VIEW(rect.x2,rect.y2);
		rect.x1 += VIEWX;
		rect.y1 += VIEWY;
		rect.x2 += VIEWX;
		rect.y2 += VIEWY;
		
		GUIDrawRectDot( rect.x1,rect.y1,rect.x2,rect.y2,color);
	}

	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiToolEdit::BrushSelect()
{
	guard(cEdiToolEdit::BrushSelect)
	for( int i=0;i<g_map.m_brushviscount;i++ )
	{
		int idx = g_map.m_brushvis[i];
		tBrush& brush = g_map.m_brush[idx];
		int layer = brush.m_data[BRUSH_LAYER];
		if(layer<0 || layer>=LAYER_MAX) continue;
		if(EdiApp()->LayerGet(layer)==0) continue; // hidden

		iRect bb;
		bb.x1 = brush.m_data[BRUSH_X];
		bb.y1 = brush.m_data[BRUSH_Y];
		bb.x2 = brush.m_data[BRUSH_X] + brush.m_data[BRUSH_W];
		bb.y2 = brush.m_data[BRUSH_Y] + brush.m_data[BRUSH_H];
		
		if( RECT2RECT(m_rect,bb) )
		{
			if(m_selop==-1 && (brush.m_data[BRUSH_SELECT]!=0) )
			{
				brush.m_data[BRUSH_SELECT] = 0;
				g_map.m_selectcount--;
			}
			else
			if((m_selop==0 || m_selop==1) && (brush.m_data[BRUSH_SELECT]==0) )
			{
				brush.m_data[BRUSH_SELECT] = 1;
				g_map.m_selectcount++;
			}
		}
	}
	g_map.m_refresh=TRUE;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiToolEdit::BrushDeselect()
{
	guard(cEdiToolEdit::BrushDeselect)
	for(int idx=0;idx<g_map.m_brushcount;idx++)
	{
		g_map.m_brush[idx].m_data[BRUSH_SELECT] = 0;
	}
	g_map.m_selectcount=0;
	g_map.m_refresh=TRUE;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiToolEdit::BrushMoveStart()
{
	guard(void cEdiToolEdit::BrushMoveStart)
	// create drag list with those visible and selected
	m_dragcount = 0;
	for(int i=0;i<g_map.m_brushviscount;i++)
	{
		int idx = g_map.m_brushvis[i];
		if(!g_map.m_brush[idx].m_data[BRUSH_SELECT]) continue;
		if(m_dragcount==m_dragsize) // resize buffer
		{
			m_dragsize += 256;
			m_drag = (int*)srealloc(m_drag,m_dragsize*sizeof(int));
		}
		m_drag[m_dragcount]=idx;
		m_dragcount++;
	}
	g_map.m_hideselected = TRUE;
	g_map.m_refresh=TRUE;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiToolEdit::BrushMove()
{
	guard(cEdiToolEdit::BrushMove)
	for(int idx=0;idx<g_map.m_brushcount;idx++)
	{
		if(!g_map.m_brush[idx].m_data[BRUSH_SELECT]) continue;
		g_map.PartitionDel(idx); // delete before changing
		g_map.m_brush[idx].m_data[BRUSH_X] += m_movedx;
		g_map.m_brush[idx].m_data[BRUSH_Y] += m_movedy;
		g_map.PartitionAdd(idx); // readd after changed

	}
	m_dragcount = 0;
	g_map.m_hideselected = FALSE;
	g_map.m_refresh=TRUE;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiToolEdit::BrushDelete()
{
	guard(cEdiToolEdit::BrushDelete)
	BEEP_OK;
	EdiApp()->UndoReset();
	for(int idx=0;idx<g_map.m_brushcount;idx++)
	{
		if(!g_map.m_brush[idx].m_data[BRUSH_SELECT]) continue;
		g_map.PartitionDel(idx);
		g_map.PartitionFix(idx+1, g_map.m_brushcount-1, -1); // fix indices before shifting
		g_map.BrushDel(idx); // this also shift and reduce m_brushcount
		idx--; // redo this new one
	}
	g_map.m_selectcount=0;
	g_map.m_refresh=TRUE;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiToolEdit::BrushCopy()
{
	guard(cEdiToolEdit::BrushCopy)
	m_clipcount=0;
	for(int idx=0;idx<g_map.m_brushcount;idx++)
	{
		if(!g_map.m_brush[idx].m_data[BRUSH_SELECT]) continue;
		if(m_clipcount==m_clipsize) // resize buffer
		{
			m_clipsize += 512;
			m_clip = (tBrush*)srealloc(m_clip,m_clipsize*sizeof(tBrush));
		}
		m_clip[m_clipcount] = g_map.m_brush[idx];
		m_clipcount++;
	}
	if(m_clipcount==0) return;

	int size = m_clipcount * sizeof(tBrush);
	UINT reg = RegisterClipboardFormat("DizzyAGE_Brushes");
	if(reg==NULL) return;
	if(!OpenClipboard(NULL)) return;
	if(EmptyClipboard())
	{
		HGLOBAL handler = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,size+4); sassert(handler!=NULL);
		byte* data = (byte*)GlobalLock(handler); sassert(data!=NULL);
		*(int*)data = size;
		memcpy(data+4,m_clip,size);
		GlobalUnlock(handler);
		SetClipboardData(reg,handler);
	}
	CloseClipboard();

	BEEP_OK;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void cEdiToolEdit::BrushPaste()
{
	guard(cEdiToolEdit::BrushPaste)
	UINT reg = RegisterClipboardFormat("DizzyAGE_Brushes");
	if(!IsClipboardFormatAvailable(reg)) return;
	if(!OpenClipboard(NULL)) return;
	HGLOBAL handler = GetClipboardData(reg);
	if(handler)
	{
		byte* data = (byte*)GlobalLock(handler);
		if(data)
		{
			int size = *(int*)data;
			int count = size / sizeof(tBrush);
			if(count>0)
			{
				EdiApp()->UndoReset();
				BrushDeselect();
				for(int i=0;i<count;i++)
				{
					int idx = g_map.BrushNew();
					g_map.m_brush[idx] = *(tBrush*)(data+4+i*sizeof(tBrush));
					g_map.m_brush[idx].m_data[BRUSH_LAYER] = EdiApp()->LayerActive(); // use current layer !
					g_map.m_refresh = TRUE;
					g_map.PartitionAdd(idx);	
				}
				g_map.m_selectcount=count;
				g_map.m_refresh=TRUE;
				BEEP_OK;
			}
			
			GlobalUnlock(handler); 
		}
	}
	CloseClipboard();
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
