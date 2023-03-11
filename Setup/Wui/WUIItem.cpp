/////////////////////////////////////////////////////////////////////////////////////////////////
// WUIItem .cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "WUIItem.h"
#include "WndDlg.h"
#include "E9Math.h"

#define CRT_HDC		(cWndDlg::m_crthdc)

cWUIItem::cWUIItem()
{
	guard(cWUIItem::cWUIItem)
	m_idx = -1;
	m_dlg = NULL;
	memset(&m_var,0,sizeof(m_var));
	// defaults
	SetInt(IITYPE, IITYPE_COLOR);
	SetInt(IITXTRES, -1);
	SetInt(IIIMGRES, -1);
	SetInt(IITXTSINGLELINE, 1);
	unguard()
}

cWUIItem::~cWUIItem()
{
	guard(cWUIItem::~cWUIItem)
	SetText(IITEXT,NULL); // free text
	SetText(IITOOLTIP,NULL); // free text
	SetText(IIBUTCMDTEXT,NULL); // free text
	// items no longer deal with resources unloading !
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////////////////////////////
void cWUIItem::SetInt( int idx, int val )
{
	guard(cWUIItem::SetInt)
	sassert(0<=idx && idx<IIMAX);
	m_var[idx]=val;
	unguard()
}

int cWUIItem::GetInt( int idx )
{
	guard(cWUIItem::GetInt)
	sassert(0<=idx && idx<IIMAX);
	return m_var[idx];	
	unguard()
}

void cWUIItem::SetText( int idx, char* text )
{
	guard(cWUIItem::SetText)
	sassert(0<=idx && idx<IIMAX);
	char* sz = (char*)(m_var[idx]);
	if(sz) sfree(sz);
	m_var[idx]=(int)(sstrdup(text));
	if(idx==IITOOLTIP)
		m_dlg->TooltipAdd(m_dlg->m_hwndtip,m_dlg->GetHWND(),GetRect(IIRECT),text);
	unguard()
}

char* cWUIItem::GetText( int idx )
{
	guard(cWUIItem::GetText)
	sassert(0<=idx && idx<IIMAX);
	return (char*)(m_var[idx]);
	unguard()
}

void cWUIItem::SetPoint( int idx, POINT point )
{
	guard(cWUIItem::SetPoint)
	sassert(0<=idx && idx<IIMAX-1);
	m_var[idx+0]=point.x;
	m_var[idx+1]=point.y;
	unguard()
}

POINT cWUIItem::GetPoint( int idx )
{
	guard(cWUIItem::GetPoint)
	sassert(0<=idx && idx<IIMAX-1);
	POINT point;
	point.x = m_var[idx+0];
	point.y = m_var[idx+1];
	return point;
	unguard()
}

void cWUIItem::SetRect( int idx, RECT rect )
{
	guard(cWUIItem::SetRect)
	sassert(0<=idx && idx<IIMAX-3);
	m_var[idx+0]=rect.left;
	m_var[idx+1]=rect.top;
	m_var[idx+2]=rect.right;
	m_var[idx+3]=rect.bottom;
	unguard()
}

RECT cWUIItem::GetRect( int idx )
{
	guard(cWUIItem::GetRect)
	sassert(0<=idx && idx<IIMAX-3);
	RECT rect;
	rect.left = m_var[idx+0];
	rect.top = m_var[idx+1];
	rect.right = m_var[idx+2];
	rect.bottom = m_var[idx+3];
	return rect;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Update and Draw
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL cWUIItem::Build()
{
	guard(cWUIItem::Build)
	return TRUE;
	unguard()
}

void cWUIItem::Update( float dtime )
{
	guard(cWUIItem::Update)
	unguard()
}

void cWUIItem::Draw()
{
	guard(cWUIItem::Draw)
	switch(GetInt(IITYPE))
	{
		case IITYPE_RECT:		DrawRect(); break;
		case IITYPE_COLOR:		DrawColor(); break;
		case IITYPE_GRADIENT:	DrawGradient(); break;
		case IITYPE_IMG:		if(GetInt(IIIMG9)) DrawImg9(); else	DrawImg(); break;
		case IITYPE_TEXT:		DrawText(); break;
		case IITYPE_BUTTON:		DrawButton(); break;
	}
	unguard()
}

void cWUIItem::DrawRect()
{
	guard(cWUIItem::DrawRect)
	HPEN hpen = CreatePen( PS_SOLID, 0, GetInt(IICOLOR) );
	HPEN oldpen = (HPEN)SelectObject( CRT_HDC, hpen );
	if(!hpen) return;
	RECT rect = GetRect(IIRECT);
	MoveToEx(CRT_HDC, rect.left, rect.top,NULL);
	LineTo(CRT_HDC, rect.right-1, rect.top);
	LineTo(CRT_HDC, rect.right-1, rect.bottom);
	MoveToEx(CRT_HDC, rect.left, rect.top,NULL);
	LineTo(CRT_HDC, rect.left, rect.bottom-1);
	LineTo(CRT_HDC, rect.right-1, rect.bottom-1);
	SelectObject( CRT_HDC, oldpen );
	DeleteObject( hpen );
	unguard()
}

void cWUIItem::DrawColor()
{
	guard(cWUIItem::DrawColor)
	HBRUSH hbr = CreateSolidBrush( GetInt(IICOLOR) );
	if(!hbr) return;
	RECT rect = GetRect(IIRECT);
	FillRect( CRT_HDC, &rect, hbr );
	DeleteObject( hbr );
	unguard()
}

void cWUIItem::DrawGradient()
{
	guard(cWUIItem::DrawGradient)
	fColor rgba;
	TRIVERTEX verts[4];
	RECT rect = GetRect(IIRECT);

	rgba = DWORD2FCOLOR( GetInt(IICOLOR+0) );
	verts[0].x		= rect.left;
	verts[0].y		= rect.top;
	verts[0].Red	= (dword)((float)rgba.b * 0xff00);
	verts[0].Green	= (dword)((float)rgba.g * 0xff00);
	verts[0].Blue	= (dword)((float)rgba.r * 0xff00);
	verts[0].Alpha	= (dword)((float)rgba.a * 0xff00);

	rgba = DWORD2FCOLOR( GetInt(IICOLOR+1) );
	verts[1].x		= rect.right;
	verts[1].y		= rect.top;
	verts[1].Red	= (dword)((float)rgba.b * 0xff00);
	verts[1].Green	= (dword)((float)rgba.g * 0xff00);
	verts[1].Blue	= (dword)((float)rgba.r * 0xff00);
	verts[1].Alpha	= (dword)((float)rgba.a * 0xff00);

	rgba = DWORD2FCOLOR( GetInt(IICOLOR+2) );
	verts[2].x		= rect.right;
	verts[2].y		= rect.bottom;
	verts[2].Red	= (dword)((float)rgba.b * 0xff00);
	verts[2].Green	= (dword)((float)rgba.g * 0xff00);
	verts[2].Blue	= (dword)((float)rgba.r * 0xff00);
	verts[2].Alpha	= (dword)((float)rgba.a * 0xff00);

	rgba = DWORD2FCOLOR( GetInt(IICOLOR+3) );
	verts[3].x		= rect.left;
	verts[3].y		= rect.bottom;
	verts[3].Red	= (dword)((float)rgba.b * 0xff00);
	verts[3].Green	= (dword)((float)rgba.g * 0xff00);
	verts[3].Blue	= (dword)((float)rgba.r * 0xff00);
	verts[3].Alpha	= (dword)((float)rgba.a * 0xff00);

	GRADIENT_TRIANGLE tris[2];
	tris[0].Vertex1 = 0;
	tris[0].Vertex2 = 1;
	tris[0].Vertex3 = 2;
	tris[1].Vertex1 = 0;
	tris[1].Vertex2 = 2;
	tris[1].Vertex3 = 3;

	GradientFill( CRT_HDC, verts, 4, tris, 2, GRADIENT_FILL_TRIANGLE );
	unguard()
}

void cWUIItem::DrawImg()
{
	guard(cWUIItem::DrawImg)

	HDC hdc = CRT_HDC;
	cResBitmap* res = WUIResource()->GetBitmap( GetInt(IIIMGRES) );
	if(!res) return;
	RECT	rect	= GetRect(IIRECT);
	RECT	map		= GetRect(IIIMGMAP);
	int		align	= GetInt(IIIMGALIGN);
	int		wrap	= GetInt(IIIMGWRAP);

	RECT	drect = rect;
	RECT	srect = map;

	if(GetInt(IIIMGSTRETCHMODE))
		SetStretchBltMode( hdc, STRETCH_HALFTONE );
	else
		SetStretchBltMode( hdc, COLORONCOLOR );

	int dx = rect.left;
	int dy = rect.top;
	int dw = rect.right - rect.left;
	int dh = rect.bottom - rect.top;
	int sx = map.left;
	int sy = map.top;
	int sw = map.right - map.left;
	int sh = map.bottom - map.top;

	if( wrap == IIWRAP_REPEAT )
	{
		if( dw<=0 || dh<=0 ) return; // invalid rect

		int sw0, sh0;
		int dx0, dy0;
		int sx0, sy0;
		int rx = dw % sw;
		int ry = dh % sh;
		int nx = dw / sw + (rx==0?0:1);
		int ny = dh / sh + (ry==0?0:1);

		for( int i=0; i<ny; i++ )
		{
			dy0 = dy + i * sh;
			sy0 = sy;
			sh0 = sh;
			if( ry!=0 && i==ny-1 ) sh0 = ry; // rest

			for( int j=0; j<nx; j++ )
			{
				dx0 = dx + j * sw;
				sx0 = sx;
				sw0 = sw;
				if( rx!=0 && j==nx-1 ) sw0 = rx; // rest

				// blend
				if( GetInt(IIIMGBLEND) == IIBLEND_NORMAL )
					StretchBlt( hdc, dx0, dy0, sw0, sh0, res->m_hdc, sx0, sy0, sw0, sh0, SRCCOPY );
				else
				if( GetInt(IIIMGBLEND) == IIBLEND_COLORKEY )
					TransparentBlt( hdc, dx0, dy0, sw0, sh0, res->m_hdc, sx0, sy0, sw0, sh0, GetInt(IICOLOR+3) );
				else
				if( GetInt(IIIMGBLEND) == IIBLEND_ALPHA )
				{
					;//return FALSE; // return AlphaBlt( hdc, dx0, dy0, sw0, sh0, res->m_hdc, sx, sy, sw, sh, colorkey );
				}
			}
		}
	} 

	else // not repeat

	{
		if( wrap != IIWRAP_STRETCH )
		{
			// crop
			if(dw<sw) sw=dw;
			if(dh<sh) sh=dh;
			if(dw>sw) dw=sw;
			if(dh>sh) dh=sh;

			// align
			if( (align & IIALIGN_CENTERX) == IIALIGN_CENTERX ) // center x
			{
				dx = dx + (drect.right-drect.left)/2 - sw/2;
				sx = sx + (srect.right-srect.left)/2 - sw/2;
			}
			else
			if( align & IIALIGN_RIGHT ) 
			{
				dx = drect.right - sw;
				sx = srect.right - sw;
			}
			else
			if( align & IIALIGN_LEFT ) 
			{
				dx = drect.left;
				sx = srect.left;
			}
			else // not aligned x
			{
				dx = drect.left;
				sx = srect.left;
			}

			if( (align & IIALIGN_CENTERY) == IIALIGN_CENTERY ) // center y
			{
				dy = dy + (drect.bottom-drect.top)/2 - sh/2;
				sy = sy + (srect.bottom-srect.top)/2 - sh/2;
			}
			else
			if( align & IIALIGN_BOTTOM )
			{
				dy = drect.bottom - sh;
				sy = srect.bottom - sh;
			}
			else
			if( align & IIALIGN_TOP )
			{
				dy = drect.top;
				sy = srect.top;
			}
			else // not aligned y
			{
				dy = drect.top;
				sy = srect.top;
			}
		}

		// blend
		BOOL ok;
		if( GetInt(IIIMGBLEND) == IIBLEND_NORMAL )
			ok=StretchBlt( hdc, dx, dy, dw, dh, res->m_hdc, sx, sy, sw, sh, SRCCOPY );
		else
		if( GetInt(IIIMGBLEND) == IIBLEND_COLORKEY )
			ok=TransparentBlt( hdc, dx, dy, dw, dh, res->m_hdc, sx, sy, sw, sh, GetInt(IICOLOR+3)&0x00ffffff );
		else
		if( GetInt(IIIMGBLEND) == IIBLEND_ALPHA )
			return; // return AlphaBlt( hdc, dx, dy, dw, dh, res->m_hdc, sx, sy, sw, sh, colorkey );
	}

	return;

	unguard()
}

void cWUIItem::DrawImg9()
{
	guard(cWUIItem::DrawImg9)

	HDC hdc = CRT_HDC;
	cResBitmap* res = WUIResource()->GetBitmap( GetInt(IIIMGRES) );
	if(!res) return;
	RECT	rect	= GetRect(IIRECT);
	RECT	map		= GetRect(IIIMGMAP);
	int		align	= GetInt(IIIMGALIGN);
	int		wrap	= GetInt(IIIMGWRAP);

	RECT oldmap		= map;
	int	 oldalign	= align;
	int	 oldwrap	= wrap;

	RECT drect = rect;
	RECT srect = map;

	if(GetInt(IIIMGSTRETCHMODE))
		SetStretchBltMode( hdc, STRETCH_HALFTONE );
	else
		SetStretchBltMode( hdc, COLORONCOLOR );

	RECT drect0;
	RECT srect0;

	float sw = (float)(map.right - map.left);
	float sh = (float)(map.bottom - map.top);
	float w0 = (float)GetInt(IIIMGBORDERLEFT);
	float w1 = (float)GetInt(IIIMGBORDERRIGHT);
	float h0 = (float)GetInt(IIIMGBORDERTOP);
	float h1 = (float)GetInt(IIIMGBORDERBOTTOM);

	// top-left
	if( w0!=0 && h0!=0 ) 
	{
		drect0.left		= drect.left;
		drect0.top		= drect.top;
		drect0.right	= (long)(drect.left + w0);
		drect0.bottom	= (long)(drect.top + h0);
		
		srect0.left		= srect.left;
		srect0.top		= srect.top;
		srect0.right	= (long)(srect.left + w0);
		srect0.bottom	= (long)(srect.top + h0);

		if( drect0.left < drect.left )		drect0.left = drect.left;
		if( drect0.top < drect.top )		drect0.top = drect.top;
		if( drect0.right > drect.right )	drect0.right = drect.right;
		if( drect0.bottom > drect.bottom )	drect0.bottom = drect.bottom;

		SetInt(IIIMGALIGN, IIALIGN_TOP | IIALIGN_LEFT);
		SetInt(IIIMGWRAP, IIWRAP_NORMAL);
		SetRect(IIIMGMAP, srect0);

		RECT oldrect = rect;
		SetRect(IIRECT,drect0);
		DrawImg();
		SetRect(IIRECT,oldrect);
	}

	// top-middle
	if( h0!=0 ) 
	{
		drect0.left		= (long)(drect.left + w0);
		drect0.top		= drect.top;
		drect0.right	= (long)(drect.right - w1);
		drect0.bottom	= (long)(drect.top + h0);
		
		srect0.left		= (long)(srect.left + w0);
		srect0.top		= srect.top;
		srect0.right	= (long)(srect.right - w1);
		srect0.bottom	= (long)(srect.top + h0);

		if( drect0.left < drect.left )		drect0.left = drect.left;
		if( drect0.top < drect.top )		drect0.top = drect.top;
		if( drect0.right > drect.right )	drect0.right = drect.right;
		if( drect0.bottom > drect.bottom )	drect0.bottom = drect.bottom;

		SetInt(IIIMGALIGN, IIALIGN_TOP | IIALIGN_CENTERX);
		SetInt(IIIMGWRAP, GetInt(IIIMGWRAPTOP));
		SetRect(IIIMGMAP, srect0);
	
		RECT oldrect = rect;
		SetRect(IIRECT,drect0);
		DrawImg();
		SetRect(IIRECT,oldrect);
	}

	// top-right
	if( w1!=0 && h0!=0 ) 
	{
		drect0.left		= (long)(drect.right - w1);
		drect0.top		= drect.top;
		drect0.right	= drect.right;
		drect0.bottom	= (long)(drect.top + h0);
		
		srect0.left		= (long)(srect.right - w1);
		srect0.top		= srect.top;
		srect0.right	= srect.right;
		srect0.bottom	= (long)(srect.top + h0);

		if( drect0.left < drect.left )		drect0.left = drect.left;
		if( drect0.top < drect.top )		drect0.top = drect.top;
		if( drect0.right > drect.right )	drect0.right = drect.right;
		if( drect0.bottom > drect.bottom )	drect0.bottom = drect.bottom;

		SetInt(IIIMGALIGN, IIALIGN_TOP | IIALIGN_RIGHT);
		SetInt(IIIMGWRAP, IIWRAP_NORMAL);
		SetRect(IIIMGMAP, srect0);

		RECT oldrect = rect;
		SetRect(IIRECT,drect0);
		DrawImg();
		SetRect(IIRECT,oldrect);
	}

	// middle-left
	if( w0!=0 ) 
	{
		drect0.left		= drect.left;
		drect0.top		= (long)(drect.top + h0);
		drect0.right	= (long)(drect.left + w0);
		drect0.bottom	= (long)(drect.bottom - h1);
		
		srect0.left		= srect.left;
		srect0.top		= (long)(srect.top + h0);
		srect0.right	= (long)(srect.left + w0);
		srect0.bottom	= (long)(srect.right - h1);

		if( drect0.left < drect.left )		drect0.left = drect.left;
		if( drect0.top < drect.top )		drect0.top = drect.top;
		if( drect0.right > drect.right )	drect0.right = drect.right;
		if( drect0.bottom > drect.bottom )	drect0.bottom = drect.bottom;

		SetInt(IIIMGALIGN, IIALIGN_LEFT | IIALIGN_CENTERY);
		SetInt(IIIMGWRAP, GetInt(IIIMGWRAPLEFT));
		SetRect(IIIMGMAP, srect0);

		RECT oldrect = rect;
		SetRect(IIRECT,drect0);
		DrawImg();
		SetRect(IIRECT,oldrect);
	}

	// middle-middle
	if( w0+w1<sw && h0+h1<sh )
	{
		drect0.left		= (long)(drect.left + w0);
		drect0.top		= (long)(drect.top + h0);
		drect0.right	= (long)(drect.right - w1);
		drect0.bottom	= (long)(drect.bottom - h1);
		
		srect0.left		= (long)(srect.left + w0);
		srect0.top		= (long)(srect.top + h0);
		srect0.right	= (long)(srect.right - w1);
		srect0.bottom	= (long)(srect.bottom - h1);

		if( drect0.left < drect.left )		drect0.left = drect.left;
		if( drect0.top < drect.top )		drect0.top = drect.top;
		if( drect0.right > drect.right )	drect0.right = drect.right;
		if( drect0.bottom > drect.bottom )	drect0.bottom = drect.bottom;

		SetInt(IIIMGALIGN, IIALIGN_CENTERX | IIALIGN_CENTERY);
		SetInt(IIIMGWRAP, oldwrap);
		SetRect(IIIMGMAP, srect0);

		RECT oldrect = rect;
		SetRect(IIRECT,drect0);
		DrawImg();
		SetRect(IIRECT,oldrect);
	}

	// middle-right
	if( w1!=sw ) 
	{
		drect0.left		= (long)(drect.right - w1);
		drect0.top		= (long)(drect.top + h0);
		drect0.right	= drect.right;
		drect0.bottom	= (long)(drect.bottom - h1);
		
		srect0.left		= (long)(srect.right - w1);
		srect0.top		= (long)(srect.top + h0);
		srect0.right	= srect.right;
		srect0.bottom	= (long)(srect.bottom - h1);

		if( drect0.left < drect.left )		drect0.left = drect.left;
		if( drect0.top < drect.top )		drect0.top = drect.top;
		if( drect0.right > drect.right )	drect0.right = drect.right;
		if( drect0.bottom > drect.bottom )	drect0.bottom = drect.bottom;

		SetInt(IIIMGALIGN, IIALIGN_RIGHT | IIALIGN_CENTERY);
		SetInt(IIIMGWRAP, GetInt(IIIMGWRAPRIGHT));
		SetRect(IIIMGMAP, srect0);

		RECT oldrect = rect;
		SetRect(IIRECT,drect0);
		DrawImg();
		SetRect(IIRECT,oldrect);
	}

	// bottom-left
	if( w0!=0 && h1!=0 ) 
	{
		drect0.left		= drect.left;
		drect0.top		= (long)(drect.bottom - h1);
		drect0.right	= (long)(drect.left + w0);
		drect0.bottom	= drect.bottom;
		
		srect0.left		= srect.left;
		srect0.top		= (long)(srect.bottom - h1);
		srect0.right	= (long)(srect.right + w0);
		srect0.bottom	= srect.bottom;

		if( drect0.left < drect.left )		drect0.left = drect.left;
		if( drect0.top < drect.top )		drect0.top = drect.top;
		if( drect0.right > drect.right )	drect0.right = drect.right;
		if( drect0.bottom > drect.bottom )	drect0.bottom = drect.bottom;

		SetInt(IIIMGALIGN, IIALIGN_LEFT | IIALIGN_BOTTOM);
		SetInt(IIIMGWRAP, IIWRAP_NORMAL);
		SetRect(IIIMGMAP, srect0);

		RECT oldrect = rect;
		SetRect(IIRECT,drect0);
		DrawImg();
		SetRect(IIRECT,oldrect);
	}

	// bottom-middle
	if( h1!=0  ) 
	{
		drect0.left		= (long)(drect.left + w0);
		drect0.top		= (long)(drect.bottom - h1);
		drect0.right	= (long)(drect.right - w1);
		drect0.bottom	= drect.bottom;
		
		srect0.left		= (long)(srect.left + w0);
		srect0.top		= (long)(srect.bottom - h1);
		srect0.right	= (long)(srect.right - w1);
		srect0.bottom	= srect.bottom;

		if( drect0.left < drect.left )		drect0.left = drect.left;
		if( drect0.top < drect.top )		drect0.top = drect.top;
		if( drect0.right > drect.right )	drect0.right = drect.right;
		if( drect0.bottom > drect.bottom )	drect0.bottom = drect.bottom;

		SetInt(IIIMGALIGN, IIALIGN_CENTERX |IIALIGN_BOTTOM);
		SetInt(IIIMGWRAP, GetInt(IIIMGWRAPBOTTOM));
		SetRect(IIIMGMAP, srect0);

		RECT oldrect = rect;
		SetRect(IIRECT,drect0);
		DrawImg();
		SetRect(IIRECT,oldrect);
	}

	// bottom-right
	if( w1!=0 && h1!=0 ) 
	{
		drect0.left		= (long)(drect.right - w1);
		drect0.top		= (long)(drect.bottom - h1);
		drect0.right	= drect.right;
		drect0.bottom	= drect.bottom;
		
		srect0.left		= (long)(srect.right - w1);
		srect0.top		= (long)(srect.bottom - h1);
		srect0.right	= srect.right;
		srect0.bottom	= srect.bottom;

		if( drect0.left < drect.left )		drect0.left = drect.left;
		if( drect0.top < drect.top )		drect0.top = drect.top;
		if( drect0.right > drect.right )	drect0.right = drect.right;
		if( drect0.bottom > drect.bottom )	drect0.bottom = drect.bottom;

		SetInt(IIIMGALIGN, IIALIGN_RIGHT | IIALIGN_BOTTOM);
		SetInt(IIIMGWRAP, IIWRAP_NORMAL);
		SetRect(IIIMGMAP, srect0);

		RECT oldrect = rect;
		SetRect(IIRECT,drect0);
		DrawImg();
		SetRect(IIRECT,oldrect);
	}

	SetInt(IIIMGALIGN, oldalign);
	SetInt(IIIMGWRAP, oldwrap);
	SetRect(IIIMGMAP, oldmap);

	unguard()
}

void cWUIItem::DrawText()
{
	guard(cWUIItem::DrawText)
	if(!GetText(IITEXT)) return;

	HDC hdc = CRT_HDC;
	cResFont* res = WUIResource()->GetFont( GetInt(IITXTRES) );
	if(!res) return;
	HFONT hfont = res->m_hfont;
	if(!hfont) return;
	
	HFONT hfontold = NULL;
	hfontold = (HFONT)SelectObject( hdc, hfont );

	SetBkMode( hdc, TRANSPARENT );
	SetTextColor( hdc, GetInt(IICOLOR) );

	int align = GetInt(IITXTALIGN);
	dword param = 0;
	if( GetInt(IITXTSINGLELINE) ) param = DT_SINGLELINE;
	if((align & IIALIGN_CENTERX) == IIALIGN_CENTERX ) param |= DT_CENTER;
	else
	if( align & IIALIGN_LEFT ) param |= DT_LEFT;
	else
	if( align & IIALIGN_RIGHT )	param |= DT_RIGHT;

	if((align & IIALIGN_CENTERY) == IIALIGN_CENTERY ) param |= DT_VCENTER;
	else
	if( align & IIALIGN_TOP ) param |= DT_TOP;
	else
	if( align & IIALIGN_BOTTOM ) param |= DT_BOTTOM;
	
	param |= DT_WORDBREAK;

	RECT rect = GetRect(IIRECT);
	if(GetInt(IIBUTPRESSED))
	{
		rect.left++;
		rect.top++;
		rect.right++;
		rect.bottom++;
	}
	BOOL ret = ::DrawText( hdc, GetText(IITEXT), strlen(GetText(IITEXT)), &rect, param );

	SelectObject( hdc, hfontold );
	unguard()
}

void cWUIItem::DrawButton()
{
	guard(cWUIItem::DrawButton)
	int pressed = GetInt(IIBUTPRESSED);
	RECT oldmap = GetRect(IIIMGMAP);
	RECT map = oldmap;
	
	if(pressed==1) 
	{
		map.left = oldmap.right;
		map.right = oldmap.right*2;
	}

	SetRect(IIIMGMAP,map);
	if(GetInt(IIIMG9)) DrawImg9(); else	DrawImg();
	SetRect(IIIMGMAP,oldmap);

	int oldcolor = GetInt(IICOLOR);
	if(pressed)	SetInt(IICOLOR,GetInt(IICOLOR+1));
	DrawText();
	SetInt(IICOLOR,oldcolor);
	unguard()
}



void cWUIItem::Notify( int msg, int p1, int p2 )
{
	guard(cWUIItem::Notify)
	if(GetInt(IIINTERACTIVE)==0) return;
	if(GetInt(IITYPE)!=IITYPE_BUTTON) return;
	m_dlg->ItemSetCmdText( NULL );

	// Button
	int x = p1;
	int y = p2;
	RECT rect = GetRect(IIRECT);

	if( msg==WUIMSG_LBUTTONDOWN )
	{
		if(!INRECT(x, y, rect)) return;
		// toggle
		SetInt(IIBUTPRESSED, (GetInt(IIBUTPRESSED))?0:1);
		m_dlg->InvalidateRect(&rect);
		// action
		if(GetInt(IIBUTCHECK)) 
			m_dlg->ItemSetCmdText( GetText(IIBUTCMDTEXT) );
	}
	else
	if( msg==WUIMSG_LBUTTONUP )
	{
		if(m_dlg->m_capture!=m_idx) return; // not captured before
		if(!GetInt(IIBUTCHECK))
		{
			// toggle
			SetInt(IIBUTPRESSED, 0);
			m_dlg->InvalidateRect(&rect);
			// action
			if(INRECT(x, y, rect)) // if inside button
				m_dlg->ItemSetCmdText( GetText(IIBUTCMDTEXT) );
		}
	}
	else
	if( msg==WUIMSG_ACTIVATE && p1==0 )
	{
		if(!GetInt(IIBUTCHECK)) // only buttons
		{
			// toggle
			SetInt(IIBUTPRESSED, 0);
			m_dlg->InvalidateRect(&rect);
		}
	}

	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
//////////////////////////////////////////////////////////////////////////////////////////////////
cWUIItem* ItemNew()
{
	guard(ItemNew)
	cWUIItem* item = (cWUIItem*)snew cWUIItem();
	if(!item) return NULL;
	return (cWUIItem*)item;	
	unguard()
}

cWUIItem* ItemFind( HWND hwnd )
{
	guard(ItemFind)
	//cWnd* wnd = cWnd::GetWindow(hwnd);
	//if(!wnd) return NULL;
	//return (cWndDlg*)wnd;
	return NULL;
	unguard()
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
