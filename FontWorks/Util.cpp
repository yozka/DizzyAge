//////////////////////////////////////////////////////////////////////////////////////////////////
// Util.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "commctrl.h"
#include "Util.h"


extern HWND g_hwnd;
extern HWND g_hwndlog;
extern HWND g_hwndprogress;
extern HINSTANCE g_hinstance;

char* file_path2ext(char* path)
{
	char *p = NULL;
	if(!path) return NULL;
	if(!(p = strrchr(path, '.')))	return NULL;
	if(strlen((p + 1)) > 0)			return  (p + 1);
	return NULL;
}

BOOL file_exists( char* path )
{
	FILE* f=fopen(path,"rb");
	if(!f) return FALSE;
	fclose(f);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// LOG and PROGRESS
//////////////////////////////////////////////////////////////////////////////////////////////////
void Log( HWND hwnd, char* text )
{
	if(text==NULL)
	{
		SetWindowText(hwnd,"");
		return;
	}

	int size = strlen(text);
	if(size==0) return;

	// buffer
	char* buffer;
	int buffersize;
	buffersize = GetWindowTextLength(hwnd);
	buffersize += size+1;
	buffer = (char*)malloc(buffersize); buffer[0]=0;
	GetWindowText(hwnd,buffer,buffersize);
	strcat(buffer,text);
	SetWindowText(hwnd,buffer);
	free(buffer);

	// scroll
	int lines = SendMessage( hwnd, EM_GETLINECOUNT, 0, 0 );
	SendMessage( hwnd, EM_LINESCROLL, 0, lines );
	// update
	UpdateWindow(hwnd);
}

void CenterDialog( HWND hwnd )
{
	int cx = GetSystemMetrics(SM_CXSCREEN) / 2;
	int cy = GetSystemMetrics(SM_CYSCREEN) / 2;
	RECT r;
	GetWindowRect(hwnd,&r);
	SetWindowPos(hwnd,NULL,cx-(r.right-r.left)/2,cy-(r.bottom-r.top)/2,0,0,SWP_NOSIZE|SWP_NOREPOSITION|SWP_NOZORDER);
}

void ProgressInit( HWND hwnd, int range )
{
	SendMessage(hwnd,PBM_SETRANGE,0,MAKELPARAM(0, range));
	SendMessage(hwnd,PBM_SETSTEP,1,0);
	SendMessage(hwnd,PBM_SETPOS,0,0);
}

void ProgressStep( HWND hwnd )
{
	SendMessage(hwnd,PBM_STEPIT,0,0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// INPUTBOX
//////////////////////////////////////////////////////////////////////////////////////////////////
static char g_inputbox_title[128] = "Input";
static char g_inputbox_msg[256] = "";
static char g_inputbox_content[256] = "";

BOOL CALLBACK DialogProcInputBox( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam ) 
{
	if( uMsg==WM_INITDIALOG ) 
	{
		CenterDialog(hwndDlg);
		SetWindowText(hwndDlg,g_inputbox_title);
		SetDlgItemText(hwndDlg,IDC_MSG,g_inputbox_msg);
		SetDlgItemText(hwndDlg,IDC_EDIT,g_inputbox_content);
		return TRUE; // autofocus
	}
	if( uMsg==WM_CLOSE ) 
	{ 
		EndDialog(hwndDlg,0); 
		return TRUE; 
	}

	if( uMsg==WM_COMMAND && LOWORD(wParam)==IDOK )
	{
		GetDlgItemText(hwndDlg,IDC_EDIT,g_inputbox_content,255);
		EndDialog(hwndDlg,1); 
		return TRUE; 
	}
	return FALSE;
}

BOOL InputBox( HINSTANCE hinstance, HWND hwnd, char* title, char* msg, char* content )
{
	strcpy(g_inputbox_title,	title);
	strcpy(g_inputbox_msg,		msg);
	strcpy(g_inputbox_content,	content);
	int ret = DialogBox( hinstance, MAKEINTRESOURCE(IDD_INPUTBOX), hwnd, DialogProcInputBox );
	if(!ret) return FALSE;
	strcpy(content,g_inputbox_content);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// FONT
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL SaveFont( char* filename, tFont& font, int char_count, byte* char_list )
{
	int i;
	word w;
	byte a;
	char szid[5] = "FNT0";

	FILE* f = fopen(filename,"wb"); if(!f) return FALSE;
	
	// HEADER (start=0, size=24)
	fwrite(&szid, 4, 1, f);
	w = font.m_chrw;	fwrite(&w, 2, 1, f);
	w = font.m_chrh;	fwrite(&w, 2, 1, f);
	w = font.m_ofsx;	fwrite(&w, 2, 1, f);
	w = font.m_ofsy;	fwrite(&w, 2, 1, f);
	w = font.m_texw;	fwrite(&w, 2, 1, f);
	w = font.m_texh;	fwrite(&w, 2, 1, f);
	w = font.m_scale;	fwrite(&w, 2, 1, f);
	w = font.m_aspect;	fwrite(&w, 2, 1, f);
	w = font.m_italic;	fwrite(&w, 2, 1, f);
	w = 0;				fwrite(&w, 2, 1, f); // empty

	// BODY (start=24, size=validchars*6)
	int count = 0;
	for(i=0;i<char_count;i++)
	{
		byte chr = char_list[i];
		fwrite(&chr, 1, 1, f);
		w = font.m_chars[chr].x; fwrite(&w, 2, 1, f);
		w = font.m_chars[chr].y; fwrite(&w, 2, 1, f);
		a = (byte)font.m_chars[chr].w; fwrite(&a, 1, 1, f);
		count++;
	}

	i = ftell(f);
	fclose(f);
	printf("Saved font file size: %i bytes, %i chars.\n",i,count);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// TGA
//////////////////////////////////////////////////////////////////////////////////////////////////
void FlipTGA( byte* buffer, int w, int h, int bpp )
{
	int linesize = w*bpp/8;
	byte* temp = (byte*)malloc(linesize);
	for(int i=0;i<h/2;i++)
	{
		memcpy( temp, buffer + i*linesize, linesize );
		memcpy( buffer + i*linesize, buffer + (h-1-i)*linesize, linesize );
		memcpy( buffer + (h-1-i)*linesize, temp, linesize );
	}
	free(temp);

}

BOOL SaveTGA(char* filename, byte* buffer, int w, int h, int bpp)
{
	tTGA header;
	memset (&header, 0, sizeof (header));
	header.IDLength = 0;
	header.ColormapType = 0;
	header.ImageType = 2;
	header.ColormapSpecification[0] = 0;
	header.ColormapSpecification[1] = 0;
	header.ColormapSpecification[2] = 0;
	header.ColormapSpecification[3] = 0;
	header.ColormapSpecification[4] = bpp;
	header.XOrigin = 0;
	header.YOrigin = 0;
	header.width = w;
	header.height = h;
	header.PixelDepth = bpp;
	header.ImageDescriptor = 0; // upside down

	FILE* f = fopen(filename,"wb"); if(!f) return FALSE;
	fwrite(&header, sizeof(header), 1, f);

	for(int i=0;i<h;i++)
	fwrite(buffer+(h-i-1)*w*bpp/8, 1, w*bpp/8, f);	
	fclose(f);

	return TRUE;
}

BOOL LoadTGA(char* filename, byte* &buffer, int &w, int &h, int &bpp)
{
	int r;
	tTGA header;
	memset (&header, 0, sizeof (header));

	FILE* f = fopen(filename,"rb"); if(!f) return FALSE;
	r = fread(&header, 1, sizeof(header), f);
	if(r!=sizeof(header)) { fclose(f); return FALSE; }
	if(header.ImageType!=2) { fclose(f); return FALSE; } // wrong format
	w = header.width;
	h = header.height;
	bpp = header.PixelDepth;
	int linesize = w*bpp/8;
	buffer = (byte*)malloc(linesize*h);
	r=fread(buffer, 1, linesize*h, f);	
	if(r!=h*linesize) { fclose(f); free(buffer); return FALSE; }
	fclose(f);

	// flip upsidedown tgas
	if((header.ImageDescriptor & 32) == 0) 
		FlipTGA(buffer,w,h,bpp);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
