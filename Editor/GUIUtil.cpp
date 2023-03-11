//////////////////////////////////////////////////////////////////////////////////////////////////
// GUIUtil.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GUIUtil.h"
#include "GUI.h"
#include "E9App.h"
#include "shlobj.h"

R9TEXTURE g_texdot = NULL;

void GUIInitResources()
{
	guard(GUIInitResources);
	sassert(R9_IsReady());
	// create chess texture for dot lines
	r9Img img;
	img.m_width = 64;
	img.m_height = 64;
	img.m_pf = R9_PF_ARGB;
	if(!R9_ImgCreate(&img)) return;
	for(int y=0;y<img.m_height;y++)
	{
		for(int x=0;x<img.m_width;x++)
		{
			BOOL chess = ( (x/8) + (y/8)%2 ) % 2;
			((dword*)img.m_data)[y*img.m_width+x] = chess ? 0xffffffff : 0xff000000;
		}
	}
	g_texdot = R9_TextureCreate(&img);
	R9_ImgDestroy(&img);
	unguard();
}

void GUIDoneResources()
{
	guard(GUIDoneResources);
	if(g_texdot) R9_TextureDestroy(g_texdot);
	g_texdot = NULL;
	unguard();
}

dword GUIColorShift( dword color, int delta )
{
	guard(GUIColorShift)
	int r = (color>>16) & 0xff;
	int g = (color>>8) & 0xff;
	int b = color & 0xff;
	r += delta; 
	if(r>255) r = 255; 
	if(r<0)	  r = 0;
	g += delta; 
	if(g>255) g = 255; 
	if(g<0)	  g = 0;
	b += delta; 
	if(b>255) b = 255; 
	if(b<0)	  b = 0;
	return ( (color & 0xff000000) | (r<<16) | (g<<8) | b );
	unguard()
}

void GUIDrawGradient( int x1, int y1, int x2, int y2, dword color1, dword color2 )
{
	guard(GUIDrawGradient);
	if(R9_GetTexture()!=NULL) R9_SetTexture(NULL);

	r9Vertex vx[6];
	memset(vx,0,sizeof(vx));

	vx[0].x = (float)x1;
	vx[0].y = (float)y1;
	vx[0].color = color1;

	vx[1].x = (float)x2;
	vx[1].y = (float)y1;
	vx[1].color = color2;

	vx[2].x = (float)x2;
	vx[2].y = (float)y2;
	vx[2].color = color2;

	vx[3].x = (float)x1;
	vx[3].y = (float)y1;
	vx[3].color = color1;

	vx[4].x = (float)x2;
	vx[4].y = (float)y2;
	vx[4].color = color2;

	vx[5].x = (float)x1;
	vx[5].y = (float)y2;
	vx[5].color = color1;

	R9_Push(vx,6,R9_PRIMITIVE_TRIANGLE);
	unguard();
}

void GUIDrawLineDot( int x1, int y1, int x2, int y2, dword color, float dx, float dy )
{
	guard(GUIDrawLineDot);
	if(!g_texdot) return;
	if(R9_GetTexture()!=g_texdot) R9_SetTexture(g_texdot);
	r9Vertex vx[2];
	int tw = R9_TextureGetWidth(g_texdot);
	int th = R9_TextureGetHeight(g_texdot);
	vx[0].x		= (float)x1;
	vx[0].y		= (float)y1;
	vx[0].color	= color;
	vx[0].u		= ((float)x1+dx) / (float)tw;
	vx[0].v		= ((float)y1+dy) / (float)th;
	vx[1].x		= (float)x2;
	vx[1].y		= (float)y2;
	vx[1].color	= color;
	vx[1].u		= ((float)x2+dx) / (float)tw;
	vx[1].v		= ((float)y2+dy) / (float)th;

	R9_Push(vx,2,R9_PRIMITIVE_LINE);
	unguard();
}

void GUIDrawRectDot( int x1, int y1, int x2, int y2, dword color )
{
	guard(GUIDrawRectDot)
	float d = (float)(sys_gettickcount()%1024); // offsets not too big
	GUIDrawLineDot( x1,y1,	 x2,y1,	  color, d, 0.0f );
	GUIDrawLineDot( x1,y1,	 x1,y2,	  color, 0.0f, d );
	GUIDrawLineDot( x2-1,y1, x2-1,y2, color, 0.0f, d );
	GUIDrawLineDot( x1,y2-1, x2,y2-1, color, d, 0.0f );
	unguard()
}

void GUIDrawText( int x1, int y1, int x2, int y2, char* text, dword color, int align, int offset )
{
	guard(GUIDrawText)
	if(text==NULL) return;
	if(color==0) return;
	dword oldcolor = g_gui->m_font->GetColor(); 
	g_gui->m_font->SetColor(color);
	float w,h;
	g_gui->m_font->GetTextBox(text,w,h);
//	int w = g_gui->m_font->GetTextWidth(text);
//	int h = g_gui->m_font->GetSize(); 
	int x = x1;
	int y = y1;
	if((align & GUIALIGN_CENTERX) == GUIALIGN_CENTERX)	x = (x1+x2-(int)w)/2;	else	
	if(align & GUIALIGN_LEFT)		x = x1+offset;				else
	if(align & GUIALIGN_RIGHT)		x = x2-(int)w-offset;
	if((align & GUIALIGN_CENTERY) == GUIALIGN_CENTERY)	y = (y1+y2-(int)h)/2;	else	
	if(align & GUIALIGN_TOP)		y = y1;				else
	if(align & GUIALIGN_BOTTOM)		y = y2-(int)h;
	
	// clip not needed
	g_gui->m_font->Print((float)x,(float)y,text);  

	g_gui->m_font->SetColor(oldcolor);
	unguard()
}

void GUIDrawImg( int x1, int y1, int x2, int y2, int img, dword color, int align )
{
	guard(GUIDrawImg)
	if(img==-1) return;
	if(color==0) return;
	R9TEXTURE tex = g_gui->m_texturepool.GetTexture(img);
	if(!tex) return;

	int w = R9_TextureGetWidth(tex);
	int h = R9_TextureGetHeight(tex);
	int x = x1;
	int y = y1;
	if((align & GUIALIGN_CENTERX) == GUIALIGN_CENTERX)	x = (x1+x2-w)/2;	else	
	if(align & GUIALIGN_LEFT)		x = x1;				else
	if(align & GUIALIGN_RIGHT)		x = x2-w;
	if((align & GUIALIGN_CENTERY) == GUIALIGN_CENTERY)	y = (y1+y2-h)/2;	else	
	if(align & GUIALIGN_TOP)		y = y1;				else
	if(align & GUIALIGN_BOTTOM)		y = y2-h;
	
	R9_SetState(R9_STATE_BLEND, R9_BLEND_ALPHA);
	R9_DrawSprite(fV2(x,y),fRect(0,0,w,h),tex,color);
	unguard()
}



//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WinDlgOpenFile( char* filename, char* ext, int mode )
{
	guard(WinDlgOpenFile)
	if(mode!=0 && mode!=1) return FALSE;
	
	dword flags=0;
	char title[32]; title[0]=0;

	if(mode==0)
	{
		strcpy(title,"Open File");
		flags |= OFN_FILEMUSTEXIST;
		flags |= OFN_NOCHANGEDIR;
		flags |= OFN_PATHMUSTEXIST;
	} 
	else
	if(mode==1)
	{
		flags |= OFN_NOCHANGEDIR;
		flags |= OFN_OVERWRITEPROMPT;
		strcpy(title,"Save File");
	}
	// other flags ? OFN_EXTENSIONDIFFERENT

	char filter[64];
	if(ext)
	{
		int extsize=(int)strlen(ext);
		strcpy(filter,"*.");
		strcat(filter,ext);
		strcpy(filter+extsize+3,"*.");
		strcat(filter+extsize+3,ext);
		filter[(extsize+3)*2] = 0;
	}

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize			= sizeof(ofn);
	ofn.hwndOwner			= E9_GetHWND();
	ofn.hInstance			= E9_GetHINSTANCE();
	ofn.lpstrFilter			= ext ? filter : NULL;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= filename; // in+out
	ofn.nMaxFile			= 256;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= title;
	ofn.Flags				= flags;
	ofn.nFileExtension		= 0; //out
	ofn.lpstrDefExt			= ext;

	BOOL ok;
	if(mode==0)
		ok = GetOpenFileName( &ofn );
	else
	if(mode==1)
		ok = GetSaveFileName( &ofn );
	return ok;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WinDlgOpenFolder( char* foldername )
{
	guard(WinDlgOpenFolder)
	foldername[0]=0;

	BROWSEINFO bi;
	bi.hwndOwner = E9_GetHWND();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = foldername;
	bi.lpszTitle = "Select folder";
	bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST pidl;
	pidl=SHBrowseForFolder(&bi);
	if(pidl==NULL) return 0;
	if(!SHGetPathFromIDList(pidl, foldername)) return 0;

	int s=(int)strlen(foldername);
	if(s==0) return 0;
	if(foldername[s-1]=='\\') foldername[s-1]=0;
	return 1;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL WinDlgOpenColor( dword* color, BOOL extended )
{
	guard(WinDlgOpenColor)

	static COLORREF custom[16];

	CHOOSECOLOR cc;
	memset(&cc,0,sizeof(cc));
	cc.lStructSize			= sizeof(cc);
	cc.hwndOwner			= E9_GetHWND();
	cc.hInstance			= NULL;//App()->GetHINSTANCE();
	cc.rgbResult			= RGB2BGR(*color) & 0x00ffffff;
	cc.lpCustColors			= custom;
	cc.Flags				= CC_ANYCOLOR | CC_RGBINIT | (extended ? CC_FULLOPEN : 0);


	BOOL ok = ChooseColor( &cc );
	if( ok ) // keep alpha and reverse red and blue
		*color = (*color & 0xff000000) | RGB2BGR(cc.rgbResult);
	return ok;
	unguard()
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
