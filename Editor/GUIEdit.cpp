//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIEdit.cpp
// Action param (while editing): 0=click out, 1=enter, 2=right click, 3=tab
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GUIEdit.h"
#include "GUI.h"
#include "E9App.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// cGUIEdit
//////////////////////////////////////////////////////////////////////////////////////////////////
cGUIEdit::cGUIEdit()
{
	guard(cGUIEdit::cGUIEdit)
	m_sel1 = 0;
	m_sel2 = 0;
	m_edit = FALSE;
	m_bktxt = NULL;
	SetInt(IV_TXTSIZE,64);
	Build();
	unguard()
}

cGUIEdit::~cGUIEdit()
{
	guard(cGUIEdit::~cGUIEdit)
		char *txt = m_var[IV_TXT].m_str;
	if(txt) sfree(txt);
	m_var[IV_TXT].m_str = NULL;
	SetBkTxt(NULL);
	unguard()
}

void cGUIEdit::Build()
{
	guard(cGUIEdit::Build)
	char* txt = m_var[IV_TXT].m_str;
	txt = (char*)srealloc(txt, GetInt(IV_TXTSIZE)+1);
	m_var[IV_TXT].m_str = txt;
	unguard()
}

void cGUIEdit::Update()
{
	guard(cGUIEdit::Update)
	RECT rc;
	GetScrRect(rc);
	m_mousein = INRECT( g_gui->m_mousex, g_gui->m_mousey, rc);
	int mx = g_gui->m_mousex - rc.left;
	int my = g_gui->m_mousey - rc.top;
	BOOL shift = I9_GetKeyValue(I9K_LSHIFT) || I9_GetKeyValue(I9K_RSHIFT);
	BOOL ctrl = I9_GetKeyValue(I9K_LCONTROL) || I9_GetKeyValue(I9K_RCONTROL);
	BOOL enter = I9_GetKeyDown(I9K_RETURN) || I9_GetKeyDown(I9K_NUMPADENTER);
	
	int width = GetInt(IV_X2)-GetInt(IV_X);
	int height = GetInt(IV_Y2)-GetInt(IV_Y);
	int cursorx = Chr2Pos(m_sel2);
	int deltax = 0;
	if(cursorx>width-height/2) deltax = -(cursorx-(width-height/2));

	BOOL captured = (g_gui->m_capture == this);
	BOOL clicked = (I9_GetKeyDown(I9_MOUSE_B1));
	BOOL clickedin = clicked && m_mousein;

	if(clicked)
	{
		if(clickedin)
		{
			if(!m_edit) SetBkTxt(GetTxt(IV_TXT));
			Capture(TRUE);
			m_edit = TRUE;
			m_sel1 = m_sel2 = Pos2Chr(mx-deltax);
		
			// double click
			static int dblclicktick = 0;
			int tick = GetTickCount();
			if(tick-dblclicktick<500) // select all
			{
				m_sel1=0; m_sel2=(int)strlen(GetTxt(IV_TXT));
				Capture(FALSE);
			}
			dblclicktick = tick;
		}
		else
		{
			if(m_edit)
			{
				SetInt(IV_CMDACTIONPARAM,0);
				Action();
				m_edit = FALSE;
				m_sel1 = m_sel2 = 0;
			}
		}
	}
	else
	if(captured)
	{
		m_sel2 = Pos2Chr(mx-deltax);
		if( !I9_GetKeyValue(I9_MOUSE_B1) ) // for selections
		{
			Capture(FALSE);
		}
	}

	// ordered selection
	int s1,s2;
	if(m_sel1<m_sel2)
	{
		s1 = m_sel1;
		s2 = m_sel2;
	}
	else
	{
		s1 = m_sel2;
		s2 = m_sel1;
	}

	if(m_edit)
	{
		if(I9_GetKeyDown(I9K_ESCAPE))
		{
			if(m_bktxt)	SetTxt(IV_TXT, m_bktxt); // restore
			m_edit = FALSE;
			m_sel1 = m_sel2 = 0;
			return;
		}

		if(enter || I9_GetKeyDown(I9K_TAB))
		{
			SetInt(IV_CMDACTIONPARAM,enter?1:3);
			Action();
			m_edit = FALSE;
			m_sel1 = m_sel2 = 0;
			return;
		}

		if(I9_GetKeyDown(I9K_DELETE))
		{
			if(s1!=s2)
				ShiftLeft(s2,s2-s1); // selection cut
			else
				ShiftLeft(s1+1,1);
			m_sel2 = m_sel1 = s1;
			return;
		}	

		if(I9_GetKeyDown(I9K_BACK))
		{
			if(s1!=s2)
				ShiftLeft(s2,s2-s1); // selection cut
			else
				ShiftLeft(s1,1);
			s1--;
			if(s1<0) s1 = 0;
			m_sel2 = m_sel1 = s1;
			return;
		}	
		
		if(I9_GetKeyDown(I9K_HOME))
		{
			m_sel2 = 0;
			if(!shift) m_sel1 = m_sel2;
			return;
		}
		
		if(I9_GetKeyDown(I9K_END))
		{
			m_sel2 = (int)strlen(GetTxt(IV_TXT));
			if(!shift) m_sel1 = m_sel2;
			return;
		}
		
		if(I9_GetKeyDown(I9K_LEFT))
		{
			if(m_sel1==m_sel2 || shift)
			{
				m_sel2--;
				if(m_sel2<0) m_sel2 = 0;
				if(!shift) m_sel1 = m_sel2;
			}
			else // break selection
			{
				if(m_sel2<m_sel1) 
					m_sel1 = m_sel2;
				else 
					m_sel2 = m_sel1;
			}
			return;
		}
		
		if(I9_GetKeyDown(I9K_RIGHT))
		{
			if(m_sel1==m_sel2 || shift)
			{
				m_sel2++;
				if(m_sel2>(int)strlen(GetTxt(IV_TXT))) m_sel2 = (int)strlen(GetTxt(IV_TXT));
				if(!shift) m_sel1 = m_sel2;
			}
			else // break selection
			{
				if(m_sel2<m_sel1) 
					m_sel2 = m_sel1;
				else 
					m_sel1 = m_sel2;
			}
			return;
		}

		if(ctrl && I9_GetKeyDown(I9K_C))
		{
			ClipboardCopy();
			return;
		}
		
		if(ctrl && I9_GetKeyDown(I9K_V))
		{
			ClipboardPaste();
			return;
		}

		if(ctrl && I9_GetKeyDown(I9K_X))
		{
			ClipboardCopy();
			SelectionCut();
			return;
		}

		for(int i=0;i<I9_GetKeyQCount();i++)
		{
			if(I9_GetKeyQValue(i))  
			{
				int key = I9_GetKeyQCode(i);
				char ch = shift ? I9_GetKeyShifted(key) : I9_GetKeyAscii(key); 
				if(ch>=32 && ch<128)
				{
					if(s1!=s2)
						ShiftLeft(s2,s2-s1); // selection cut
					m_sel2 = m_sel1 = s1;
					int size = (int)strlen(GetTxt(IV_TXT));
					if(size==GetInt(IV_TXTSIZE))	break;
					char* txt=GetTxt(IV_TXT);
					// check rect(box) sixe (limit text)
					// int w = g_gui->m_font->GetTextWidth(txt) + g_gui->m_font->GetCharWidth(ch) + g_gui->m_font->GetOfsX();
					// if(w>GetInt(IV_X2) - GetInt(IV_X) - GetInt(IV_TXTOFFSET)) break;
					// insert
					ShiftRight(s1,1);
					txt[s1] = ch;
					m_sel1++;
					s1 = s2 = m_sel2 = m_sel1;
				}

			}
		}
	}

	// right click action
	if(!IsCaptured() && m_mousein && I9_GetKeyDown(I9_MOUSE_B2))
	{
		SetInt(IV_CMDACTIONPARAM,2);
		Action();
	}
	
	unguard()
}

void cGUIEdit::Draw()
{
	guard(cGUIEdit::Draw)
	RECT rc; 
	fRect rc1;
	GetScrRect(rc);
	dword col = GetInt(IV_COLOR);
	if(m_edit)	col = GUIColorShift(col,GUICOLORSHIFT/2); 

	int style = GetInt(IV_STYLE);

	int width = GetInt(IV_X2)-GetInt(IV_X);
	int height = GetInt(IV_Y2)-GetInt(IV_Y);
	int cursorx = Chr2Pos(m_sel2);
	int deltax = 0;
	if(cursorx>width-height/2) deltax = -(cursorx-(width-height/2));

	// background
	if( style & GUISTYLE_BACKGR )
		GUIDrawBar( rc.left, rc.top, rc.right, rc.bottom, col );
	else
	if( style & GUISTYLE_GRADIENT )
		GUIDrawGradient( rc.left, rc.top, rc.right, rc.bottom, GetInt(IV_COLOR), GetInt(IV_COLOR+1) );
	
	// image
	GUIDrawImg( rc.left, rc.top, rc.right, rc.bottom, GetInt(IV_IMG), GetInt(IV_IMGCOLOR), GetInt(IV_IMGALIGN));

	// clip content
	fRect oldclip = R9_GetClipping();
	fRect newclip = fRect( rc.left,rc.top,rc.right,rc.bottom );
	R9_AddClipping(newclip);
	if( R9_IsClipping() )
	{
		// selection
		if(m_sel1!=m_sel2)
		{
			rc1.left = (float)rc.left + Chr2Pos(m_sel1) + deltax;
			rc1.top = (float)rc.top;
			rc1.right =  (float)rc.left + Chr2Pos(m_sel2) + deltax;
			rc1.bottom = (float)rc.bottom; 
			R9_ClipBar(rc1);
			GUIDrawBar( (int)rc1.left, (int)rc1.top, (int)rc1.right, (int)rc1.bottom, GetInt(IV_COLOR+3));
		}

		// text
		GUIDrawText( rc.left+deltax, rc.top, rc.right+deltax, rc.bottom, GetTxt(IV_TXT), GetInt(IV_TXTCOLOR), GUIALIGN_LEFT|GUIALIGN_CENTERY, GetInt(IV_TXTOFFSET) );

		// cursor
		if(m_edit)
		{
			static dword s_time = 0;
			s_time += E9_AppGetInt(E9_APP_DELTATIME);
			if(s_time<=400)
			{
				rc1.left = (float)rc.left + Chr2Pos(m_sel2) + deltax;
				rc1.top = (float)rc.top+2;
				rc1.right =  rc1.left+1;
				rc1.bottom = (float)rc.bottom-2;
				R9_ClipBar(rc1);
				GUIDrawBar( (int)rc1.left, (int)rc1.top, (int)rc1.right, (int)rc1.bottom, GetInt(IV_TXTCOLOR));			
			}
			if(s_time>700) s_time = 0;
		}
	}

	R9_SetClipping(oldclip);

	// border
	if( style & GUISTYLE_BORDER )
		GUIDrawRect( rc.left, rc.top, rc.right, rc.bottom, GetInt(IV_COLOR+2) );
	else
	if( style & GUISTYLE_BORDER3D )
		GUIDrawRect3D( rc.left, rc.top, rc.right, rc.bottom, GetInt(IV_COLOR+2), style & GUISTYLE_PRESSED );

	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Access
//////////////////////////////////////////////////////////////////////////////////////////////////

void cGUIEdit::SetTxt( int idx, char* text )
{
	guard(cGUIEdit::SetTxt)
	if(idx==IV_TXT)
	{
		if(text==NULL) return;
		char *txt = m_var[idx].m_str;
		strncpy(txt,text,GetInt(IV_TXTSIZE));
		txt[GetInt(IV_TXTSIZE)] = 0;
		return;
	}
	cGUIItem::SetTxt(idx,text);
	unguard()
}

int cGUIEdit::Pos2Chr(int pos)
{
	guard(cGUIEdit::Pos2Chr)
	int  i,w, size;
	pos = pos - GetInt(IV_TXTOFFSET);
	char *sztxt = GetTxt(IV_TXT);
	size = (int)strlen(sztxt);
	w = 0;
	for(i=0;i<size;i++)
	{
		w += (int)(g_gui->m_font->GetCharWidth(sztxt[i]) + g_gui->m_font->GetOfsX()); 
		if(w>pos)
			return i ;
	}
	return size;
	unguard()
}

int cGUIEdit::Chr2Pos(int chr)
{
	guard(cGUIEdit::Chr2Pos)
	sassert(chr<=(int)strlen(GetTxt(IV_TXT)));
	return (int)( (g_gui->m_font->GetTextWidth(GetTxt(IV_TXT),chr) + GetInt(IV_TXTOFFSET)) ); 
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// clipboard
//////////////////////////////////////////////////////////////////////////////////////////////////
void cGUIEdit::ClipboardCopy()
{
	guard(cGUIEdit::ClipboardCopy)
	char* text = m_var[IV_TXT].m_str;
	if(!text) return;

	// ordered selection
	int s1,s2;
	if(m_sel1<m_sel2)
	{
		s1 = m_sel1;
		s2 = m_sel2;
	}
	else
	{
		s1 = m_sel2;
		s2 = m_sel1;
	}
	int size = s2-s1;
	if(size==0) return; // no selection

	if(!OpenClipboard(NULL)) return;
	if(EmptyClipboard())
	{
		HGLOBAL handler = GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE,size+1); sassert(handler!=NULL);
		void* data = GlobalLock(handler); sassert(data!=NULL);
		memcpy(data,text+s1,size);
		((char*)data)[size]=0;
		GlobalUnlock(handler);
		SetClipboardData(CF_TEXT,handler);
	}
	CloseClipboard();
	unguard()
}

void cGUIEdit::ClipboardPaste()
{
	guard(cGUIEdit::ClipboardPaste)
	if(!IsClipboardFormatAvailable(CF_TEXT)) return;
	if(!OpenClipboard(NULL)) return;
	HGLOBAL handler = GetClipboardData(CF_TEXT);
	if(handler)
	{
		void* data = GlobalLock(handler);
		if(data)
		{
			SelectionCut();
			SelectionPaste((char*)data);
			GlobalUnlock(handler); 
		}
	}
	CloseClipboard();
	unguard()
}

void cGUIEdit::SelectionCut()
{
	guard(cGUIEdit::SelectionCut);
	if(m_sel1==m_sel2) return; // no selection
	// ordered selection
	int s1,s2;
	if(m_sel1<m_sel2)
	{
		s1 = m_sel1;
		s2 = m_sel2;
	}
	else
	{
		s1 = m_sel2;
		s2 = m_sel1;
	}
	ShiftLeft(s2,s2-s1);
	m_sel1 = m_sel2 = s1;
	unguard();
}

void cGUIEdit::SelectionPaste( char* txt )
{
	guard(cGUIEdit::SelectionPaste);
	if(txt==NULL) return;
	int size = (int)strlen(txt);
	if(size==0) return;
	ShiftRight(m_sel1,size); // make space
	if(m_sel1+size>GetInt(IV_TXTSIZE)) size = GetInt(IV_TXTSIZE)-m_sel1; // check space
	char* text = GetTxt(IV_TXT);
	if(size>0)
		memcpy(text+m_sel1,txt,size);
	m_sel1 += size;
	m_sel2 = m_sel1;
	unguard();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
