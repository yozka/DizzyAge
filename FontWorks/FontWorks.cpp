//////////////////////////////////////////////////////////////////////////////////////////////////
// FontWorks.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "windows.h"
#include "stdio.h"
#include "conio.h"
#include "malloc.h"
#include "string.h"
#include "shlobj.h"
#include "resource.h"
#include "Util.h"

#define APPTITLE						"FontWorks v2.3"
#define MSGINFO( text )					MessageBox( g_hwnd, text, "FontWorks", MB_OK | MB_ICONINFORMATION )
#define MSGERROR( text )				MessageBox( g_hwnd, text, "FontWorks", MB_OK | MB_ICONERROR )
#define MSGQUESTION( text )				MessageBox( g_hwnd, text, "FontWorks", MB_YESNO | MB_ICONQUESTION )

HWND		g_hwnd = NULL;
HWND		g_hwndlog = NULL;
HINSTANCE	g_hinstance = NULL;

inline void ItemSetBool( int id, BOOL on ) 
{ 
	SendDlgItemMessage(g_hwnd, id, BM_SETCHECK, on ? BST_CHECKED : BST_UNCHECKED, 0);
}

inline BOOL ItemGetBool( int id ) 
{ 
	int ret = SendDlgItemMessage(g_hwnd, id, BM_GETCHECK, 0, 0);
	return ret==BST_CHECKED;
}

inline void ItemSetChoice( int id, int ch )
{
	HWND hwnd = GetDlgItem(g_hwnd,id);
	SendMessage(hwnd,CB_SETCURSEL,ch,0);
}

inline int ItemGetChoice( int id )
{
	HWND hwnd = GetDlgItem(g_hwnd,id);
	return SendMessage(hwnd,CB_GETCURSEL,0,0);
}

inline void ItemSetNumber( int id, int number ) 
{ 
	SetDlgItemInt(g_hwnd,id,number,TRUE);
}

inline int ItemGetNumber( int id, BOOL sgn=TRUE ) 
{ 
	return GetDlgItemInt(g_hwnd,id,NULL,sgn);
}

inline void	ItemSetText( int id, char* text )
{
	SetDlgItemText(g_hwnd,id,text);
}

inline char* ItemGetText( int id ) 
{ 
	static char sz[320];
	GetDlgItemText(g_hwnd,id,sz,320);
	sz[319]=0;
	return sz;
}

inline void ItemSetDword( int id, dword dw )
{
	static char sz[32];
	sprintf(sz,"%08X",dw);
	SetDlgItemText(g_hwnd,id,sz);
}

inline dword ItemGetDword( int id )
{
	static char sz[32];
	GetDlgItemText(g_hwnd,id,sz,32);
	sz[32]=0;
	dword dw=0;
	sscanf(sz,"%x",&dw);
	return dw;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// FONTWORKS
//////////////////////////////////////////////////////////////////////////////////////////////////

#define CHRSET_LOW				(1<<0)	// [0-31]
#define CHRSET_SPACE			(1<<1)	// [32]				
#define CHRSET_SIGN1			(1<<2)	// [33-47]
#define CHRSET_NUM				(1<<3)	// [48-57]
#define CHRSET_SIGN2			(1<<4)	// [58-64]
#define CHRSET_UPPERCASE		(1<<5)	// [65-90]
#define CHRSET_SIGN3			(1<<6)	// [91-96]
#define CHRSET_LOWERCASE		(1<<7)	// [97-122]
#define CHRSET_SIGN4			(1<<8)	// [123-127]
#define CHRSET_EXTRA			(1<<9)	// [128-255]

#define CHRSET_NONE				0		// use just chars from 'charplus'
#define CHRSET_STANDARD			(CHRSET_SIGN1 + CHRSET_NUM + CHRSET_SIGN2 + CHRSET_UPPERCASE + CHRSET_SIGN3 + CHRSET_LOWERCASE + CHRSET_SIGN4)
#define CHRSET_STANDARDEXTRA	(CHRSET_SIGN1 + CHRSET_NUM + CHRSET_SIGN2 + CHRSET_UPPERCASE + CHRSET_SIGN3 + CHRSET_LOWERCASE + CHRSET_SIGN4 + CHRSET_EXTRA)
#define CHRSET_FULL				(CHRSET_LOW + CHRSET_SPACE + CHRSET_SIGN1 + CHRSET_NUM + CHRSET_SIGN2 + CHRSET_UPPERCASE + CHRSET_SIGN3 + CHRSET_LOWERCASE + CHRSET_SIGN4 + CHRSET_EXTRA )

int		g_character_count = 0;	// characters count
byte	g_character_list[256];	// characters list

int		g_tgaw = 0;
int		g_tgah = 0;
int		g_tgabpp = 0;
byte*	g_tgabuffer = NULL;		// tga buffer

tFont	g_font;

BOOL PaintProcess();
BOOL MakeProcess(); 

#define MAX_CHARACTER_SETS	6

dword ColorBGR( dword color )
{
	dword a = (color & 0xff000000)>>24;
	dword r = (color & 0x00ff0000)>>16;
	dword g = (color & 0x0000ff00)>>8;
	dword b = (color & 0x000000ff);
	return (a<<24)|(b<<16)|(g<<8)|r;
}

dword ColorGen( dword color )
{
	dword a = (color & 0xff000000)>>24;
	dword r = (color & 0x00ff0000)>>16;
	dword g = (color & 0x0000ff00)>>8;
	dword b = (color & 0x000000ff);
	if(r>=128 || g>=128 || b>=128)
	{
		r=r*2/3; g=g*2/3; b=b*2/3;
	}
	else
	{
		r=r*4/3; g=g*4/3; b=b*4/3;
	}
	if(r>255) r=255;
	if(g>255) g=255;
	if(b>255) b=255;
	return (a<<24)|(r<<16)|(g<<8)|b;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// IDD_DIALOG1 Functions
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL LoadTexture( char* filepath, byte* &buffer, int &w, int &h, int &bpp )
{
	buffer=NULL;
	w=0,h=0,bpp=0;
	if(LoadTGA( filepath, buffer, w, h, bpp ))
	{
		if(bpp!=24 && bpp!=32) { MSGERROR("Tga is not 24 or 32 bits."); return FALSE; }
		if(w<=0 || h<=0) { MSGERROR("Tga is has invalid size."); return FALSE; }
		ItemSetText(IDC_TEXTURE_FILE,filepath);
		ItemSetNumber(IDC_TEXTURE_W,w);
		ItemSetNumber(IDC_TEXTURE_H,h);
		ItemSetBool( IDC_TEXTURE_ALPHA,	bpp==32 );
		return TRUE;
	}
	else
	{
		MSGERROR("Can't load the texture file."); return FALSE;
	}
}

BOOL OnTgaDrop( char* filepath )
{
	byte* buffer=NULL;
	int w=0,h=0,bpp=0;
	BOOL ret = LoadTexture( filepath, buffer, w, h, bpp );
	if(buffer) free(buffer);
	ItemSetBool( IDC_FONT_ALPHA, bpp==32 ); // help
	return ret;
}

BOOL OnTgaBrowse()
{
	char path[MAX_PATH];
	path[0]=0;
	strcat(path,"unnamed.pak");

	dword flags=0;
	flags |= OFN_NOCHANGEDIR;
	flags |= OFN_PATHMUSTEXIST;
	//flags |= OFN_FILEMUSTEXIST;
	char title[128]; title[0]=0;
	strcpy(title,"Select TGA font file.");
	
	char ext[] = "tga";
	char filter[64];
	if(ext)
	{
		int extsize=strlen(ext);
		strcpy(filter,"*.");
		strcat(filter,ext);
		strcpy(filter+extsize+3,"*.");
		strcat(filter+extsize+3,ext);
		filter[(extsize+3)*2] = 0;
	}

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize			= sizeof(ofn);
	ofn.hwndOwner			= g_hwnd;
	ofn.hInstance			= g_hinstance;
	ofn.lpstrFilter			= ext ? filter : NULL;
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= path; // in+out
	ofn.nMaxFile			= MAX_PATH;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= title;
	ofn.Flags				= flags;
	ofn.nFileExtension		= 0; //out
	ofn.lpstrDefExt			= ext;

	BOOL ok;
	ok = GetOpenFileName( &ofn );
	if(ok)
	{
		if(file_exists(path))
		{
			byte* buffer=NULL;
			int w=0,h=0,bpp=0;
			BOOL ret = LoadTexture( path, buffer, w, h, bpp );
			if(buffer) free(buffer);
			ItemSetBool( IDC_FONT_ALPHA, bpp==32 ); // help
			return ret;
		}
		else
		{
			ItemSetText(IDC_TEXTURE_FILE,path);
		}
	}
	return TRUE;
}

BOOL OnFontBrowse()
{
	LOGFONT logfont;
	memset(&logfont,0,sizeof(logfont));
//@	logfont.lfHeight = ItemGetNumber(IDC_PAINT_H);
	//logfont.lfWidth = ItemGetNumber(IDC_PAINT_W);
	logfont.lfWeight = ItemGetBool(IDC_PAINT_BOLD) ? FW_BOLD : FW_NORMAL;
	logfont.lfItalic = ItemGetBool(IDC_PAINT_ITALIC);
	// logfont.lfQuality = ItemGetBool(IDC_PAINT_AALISING) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY; // force with CLEARTYPE_QUALITY=5
	strcpy(logfont.lfFaceName,ItemGetText(IDC_PAINT_FONT)); 

	dword ink = ItemGetDword(IDC_PAINT_INK);

	CHOOSEFONT cf;
	memset(&cf,0,sizeof(cf));
	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = g_hwnd;
	cf.lpLogFont = &logfont;
	cf.Flags = CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT;
	cf.rgbColors = ColorBGR(ink) & 0x00ffffff;

	int ok = ChooseFont(&cf);
	if(ok)
	{
		ItemSetText(IDC_PAINT_FONT,logfont.lfFaceName);
		// ItemSetNumber(IDC_PAINT_W,logfont.lfWidth);
//@		ItemSetNumber(IDC_PAINT_H,logfont.lfHeight);
		ItemSetBool(IDC_PAINT_BOLD,logfont.lfWeight==FW_BOLD);
		ItemSetBool(IDC_PAINT_ITALIC,logfont.lfItalic!=0);
		// ItemSetBool(IDC_PAINT_AALISING,logfont.lfQuality!=NONANTIALIASED_QUALITY);
		ink &= 0xff000000;
		ink |= ColorBGR(cf.rgbColors);
		ItemSetDword(IDC_PAINT_INK,ink);
	}
	return TRUE;
}

void OnChangeCharacterSet()
{
	int chrset = ItemGetChoice(IDC_CHARACTER_SET);
	int chrsetflags[MAX_CHARACTER_SETS] = { 0, CHRSET_NUM, CHRSET_UPPERCASE, CHRSET_STANDARD, CHRSET_STANDARDEXTRA, CHRSET_FULL };
	if(chrset<0 || chrset>=MAX_CHARACTER_SETS) chrset=0;
	int flags = chrsetflags[chrset];

	ItemSetBool( IDC_CHARACTER_BIT0, flags & (1<<0) );
	ItemSetBool( IDC_CHARACTER_BIT1, flags & (1<<1) );
	ItemSetBool( IDC_CHARACTER_BIT2, flags & (1<<2) );
	ItemSetBool( IDC_CHARACTER_BIT3, flags & (1<<3) );
	ItemSetBool( IDC_CHARACTER_BIT4, flags & (1<<4) );
	ItemSetBool( IDC_CHARACTER_BIT5, flags & (1<<5) );
	ItemSetBool( IDC_CHARACTER_BIT6, flags & (1<<6) );
	ItemSetBool( IDC_CHARACTER_BIT7, flags & (1<<7) );
	ItemSetBool( IDC_CHARACTER_BIT8, flags & (1<<8) );
	ItemSetBool( IDC_CHARACTER_BIT9, flags & (1<<9) );

}

void OnInitDialog( HWND hwnd )
{
	g_hwnd = hwnd;
	g_hwndlog = GetDlgItem(hwnd,IDC_LOG);

	CenterDialog(hwnd);

	SetWindowText(hwnd,APPTITLE);
	HICON hIcon = LoadIcon(g_hinstance,MAKEINTRESOURCE(IDI_ICON1));
	PostMessage(hwnd,WM_SETICON,ICON_BIG,(LPARAM) (HICON) hIcon);

	// set defaults
	ItemSetText(	IDC_TEXTURE_FILE,	"font.tga" );
	ItemSetNumber(	IDC_TEXTURE_W,		128 );
	ItemSetNumber(	IDC_TEXTURE_H,		128 );
	ItemSetBool(	IDC_TEXTURE_ALPHA,	1 );
	ItemSetNumber(	IDC_MAP_X,			0 );
	ItemSetNumber(	IDC_MAP_Y,			0 );
	ItemSetNumber(	IDC_MAP_W,			16 );
	ItemSetNumber(	IDC_MAP_H,			16 );
	ItemSetNumber(	IDC_CEL_W,			8 );
	ItemSetNumber(	IDC_CEL_H,			8 );

	ItemSetText(	IDC_PAINT_FONT,		"Arial" );
	ItemSetNumber(	IDC_PAINT_W,		0 );
	ItemSetNumber(	IDC_PAINT_H,		8 );
	ItemSetNumber(	IDC_PAINT_OFSX,		0 );
	ItemSetNumber(	IDC_PAINT_OFSY,		0 );
	ItemSetDword(	IDC_PAINT_PAPER,	0x00FF0000 );
	ItemSetDword(	IDC_PAINT_INK,		0xFFFFFFFF );

	ItemSetNumber(	IDC_FONT_CHRW,		8 );
	ItemSetNumber(	IDC_FONT_CHRH,		8 );
	ItemSetNumber(	IDC_FONT_OFSX,		0 );
	ItemSetNumber(	IDC_FONT_OFSY,		0 );
	ItemSetNumber(	IDC_FONT_SCALE,		100 );
	ItemSetNumber(	IDC_FONT_ASPECT,	100 );
	ItemSetBool(	IDC_FONT_ALPHA,		1 );

	ItemSetChoice(	IDC_CHARACTER_SET,	0 );
	ItemSetText(	IDC_CHARACTER_LIST,	"ABCDEFGHIJKLMNOPQRSTUVWXYZ" );

	char* chrset[MAX_CHARACTER_SETS] = {"custom","numbers ['0'-'9']","letters['A'-'Z']","standard [33-127]","extra [33-255]","full [0-255]" };
	hwnd = GetDlgItem(g_hwnd,IDC_CHARACTER_SET);
	for(int i=0;i<MAX_CHARACTER_SETS;i++)
		SendMessage(hwnd,CB_ADDSTRING,0,(LPARAM)((LPCTSTR)chrset[i]));
	ItemSetChoice(IDC_CHARACTER_SET,MAX_CHARACTER_SETS-1);

	OnChangeCharacterSet();

	LOGLN("Welcome!");
}

void OnHelp()
{
	MessageBox(g_hwnd,
		"\n" \
		"  FontWorks Tool for DizzyAGE v2.3 (2009)\n" \
		"  by Alexandru Simion\n" \
		"  http://www.yolkfolk.com/dizzyage\n" \
		"\n" \
		"  The TEXTURE group is used in both paint and make processes.\n" \
		"It represent the TGA texture file, with the font's characters. If alpha is specifed,\n" \
		"characters alre painted in alpha channel too, for use with an alpha blending shader.\n" \
		"\n" \
		"  The characters table is specified by it's top left position in pixels and\n" \
		"it's width and height, in characters. The characer cell size is specified in pixels.\n" \
		"\n" \
		"  The PAINT group specifies options about how to paint the font.\n"
		"The width of the font can be 0 by default, but the height represents the font's size.\n" \
		"The ink and paper colors are specified in hexadecimal (ARGB) and support alpha too.\n" \
		"The chessboard option will alternate paper color for debug or other purposes.\n" \
		"\n" \
		"  The MAKE group specifies options of the compiling font.\n" \
		"Usually the character sizes match the cell sizes specified in the TEXTURE group.\n" \
		"The offsets can be used to add spaces between characters or rows, when the font is used.\n" \
		"Scale and aspect are percents with values from 0 to 100. The use of the font's options\n" \
		"depends on the each aplication.\n" \
		"\n"
		"  The \"best fit\" option can be used to auto-detect minimal width for each character and\n" \
		"it can either use the RGB channels or the ALPHA channel, from the texture.\n" \
		"\n"
		"  The CHARACTERS group specifies which characters of the ASCII set are to be painted\n" \
		"or compiled. If custom mode is used, characters can be entered in the long edit field.\n" \
		"If not, subsets of characters can be enabled by checking the bit check boxes,\n" \
		"or simply by choosing them from the combo box.\n" \
		"\n"
		"  Characters subsets:\n" \
		"  1=LOW [0-31], 2=SPACE [32], 3=SIGN1 [33-47], 4=NUM [48-57], 5=SIGN2 [58-64],\n" \
		"  6=UPPERCASE [65-90], 7=SIGN3 [91-96], 8=LOWERCASE [97-122], 9=SIGN4 [123-127],\n" \
		"  10=EXTRA [128-255].\n" \
		"\n",
		
		"FontWorks HELP", MB_OK);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// IDD_DIALOG1 Proc
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK DialogProc1( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
		case WM_INITDIALOG:	OnInitDialog( hwndDlg ); return TRUE;
		case WM_COMMAND:
		{
			switch(LOWORD(wParam)) // by ID
			{
				case IDC_PAINT:				PaintProcess(); return TRUE;
				case IDC_MAKE:				MakeProcess(); return TRUE;
				case IDC_CHARACTER_SET:		if(HIWORD(wParam)==CBN_SELCHANGE) OnChangeCharacterSet(); break;
				case IDC_TEXTURE_BROWSE:	OnTgaBrowse(); return TRUE;
				case IDC_PAINT_BROWSE:		OnFontBrowse(); return TRUE;
				case IDC_EXIT:				EndDialog(hwndDlg,0); return TRUE;
				case IDC_HLP:				OnHelp(); return TRUE;
			}
			break;
		}
		case WM_CLOSE: EndDialog(hwndDlg,0); return TRUE;
		case WM_DROPFILES:
		{
			char filepath[MAX_PATH];
			if(DragQueryFile((HDROP)wParam, 0xFFFFFFFF, NULL, 0)<1) return 0; // count
			if(DragQueryFile((HDROP)wParam, 0, filepath, MAX_PATH)==0) return 0; // error
			char* ext = file_path2ext(filepath);
			if(ext==NULL) // folder
			{
				//...
			}
			else
			{
				if(0==strcmp(ext,"tga")) // tga file
				{
					OnTgaDrop(filepath);
				}
				else
					MSGERROR("Invalid file format.\nDrag and drop the map file or the tiles folder.");
			}
			return 0;
		}
	}
	return FALSE;
}

LRESULT CALLBACK WndProcDummy( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	return DefWindowProc( hwnd, msg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// PAINT
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL PaintChar( HDC hdc1, HDC hdc2, RECT& rect, int chr, HFONT font, LOGFONT& logfont, dword ink, dword paper, BOOL cell )
{
	int x,y,chrw;
	HFONT fontold;

	if(cell) // draw cell's paper (for chessboard)
	{
		HBRUSH brush = CreateSolidBrush(paper);
		//HBRUSH brushold = (HBRUSH)SelectObject(hdc1,brush);
		FillRect(hdc1,&rect,brush);
		//SelectObject(hdc1,brushold);
		DeleteObject(brush);
	}

	// HDC1 - COLOR
	fontold = (HFONT)SelectObject(hdc1,font);
	GetCharWidth32( hdc1, chr, chr, &chrw );
  	x = (rect.left + rect.right)/2 - chrw/2;
	y = (rect.top + rect.bottom)/2 - logfont.lfHeight/2;
	
	// normal
	SetBkMode(hdc1,TRANSPARENT);
	SetTextColor(hdc1,ink & 0x00ffffff);
	TextOut(hdc1,x,y,(char*)&chr,1);

	SelectObject(hdc1,fontold);

	// HDC2 - ALPHA
	fontold = (HFONT)SelectObject(hdc2,font);

	// normal
	SetBkMode(hdc2,TRANSPARENT);
	SetTextColor(hdc2,(ink & 0xff000000)>>24);
	TextOut(hdc2,x,y,(char*)&chr,1);

	SelectObject(hdc2,fontold);

	// LOG
	char szc[32];
	if(chr>33 && chr<128)
		sprintf(szc,"%c ", chr);
	else
		sprintf(szc,"#%03d ", (byte)chr);
	if(chr && chr%16==0) LOGLN("");
	LOG(szc);
	return TRUE;
}

BOOL Paint( HDC hdc1, HDC hdc2, HFONT font, LOGFONT& logfont )
{
	int i,j;

	int texw = ItemGetNumber(IDC_TEXTURE_W);
	int texh = ItemGetNumber(IDC_TEXTURE_H);
	dword ink = ItemGetDword(IDC_PAINT_INK);
	dword paper1 = ItemGetDword(IDC_PAINT_PAPER);
	dword paper2 = ColorGen(paper1);
	dword drawcells = ItemGetBool(IDC_PAINT_CELLS);

	// clear hdc
	for(i=0;i<texh;i++)
	{
		for(j=0;j<texw;j++)
		{
			dword color = paper1 & 0x00ffffff;
			dword alpha = (paper1 & 0xff000000)>>24;
			SetPixel(hdc1,j,i,color);
			SetPixel(hdc2,j,i,alpha);
		}
	}

	int mapx = ItemGetNumber(IDC_MAP_X);
	int mapy = ItemGetNumber(IDC_MAP_Y);
	int mapw = ItemGetNumber(IDC_MAP_W);
	int maph = ItemGetNumber(IDC_MAP_H);
	int celw = ItemGetNumber(IDC_CEL_W);
	int celh = ItemGetNumber(IDC_CEL_H);

	// draw chars
	int cp=0; // chr pos in map
	for(i=0;i<g_character_count;i++)
	{
		int chr = g_character_list[i];
		int cx = cp % mapw;
		int cy = cp / mapw;
		int x = mapx + celw * cx;
		int y = mapy + celh * cy;
		RECT rect = {x,y,x+celw,y+celh};
		BOOL cell = drawcells && (cx+cy)%2;
		PaintChar( hdc1, hdc2, rect, chr, font, logfont, ink, paper2, cell );
		cp++;
	}
	LOGLN("");
	return TRUE;
}

BOOL Save( HDC hdc1, HDC hdc2 )
{
	int i,j;
	int w = ItemGetNumber(IDC_TEXTURE_W);
	int h = ItemGetNumber(IDC_TEXTURE_H);
	int bpp = ItemGetBool(IDC_TEXTURE_ALPHA) ? 32 : 24;
	int spp = bpp/8;

	// buffer
	byte* buffer = (byte*)malloc(w*h*spp);
	for(i=0;i<h;i++)
	{
		for(j=0;j<w;j++)
		{
			dword color = GetPixel(hdc1,j,i);
			dword alpha = GetPixel(hdc2,j,i);
			dword dw = color + (alpha<<24);
			memcpy(	buffer+(i*w+j)*spp, &dw, spp ); // 3 or 4 bytes
		}
	}

	BOOL ok = SaveTGA(ItemGetText(IDC_TEXTURE_FILE),buffer,w,h,bpp);
	if(!ok) { MSGERROR("Can't save the texture file."); }

	free(buffer);
	return ok;
}

BOOL ReadCharacters()
{
	int i;
	int charset = ItemGetChoice(IDC_CHARACTER_SET);
	g_character_count = 0;
	if(charset==0)
	{
		char* text = ItemGetText(IDC_CHARACTER_LIST);
		while(*text)
		{
			g_character_list[g_character_count] = *(byte*)text;
			g_character_count++;
		}
	}
	else
	{
		int charbits = 0;
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT0) << 0 );
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT1) << 1 );
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT2) << 2 );
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT3) << 3 );
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT4) << 4 );
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT5) << 5 );
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT6) << 6 );
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT7) << 7 );
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT8) << 8 );
		charbits |= ( ItemGetBool(IDC_CHARACTER_BIT9) << 9 );

		for(i=0;i<256;i++)
		{
			BOOL accept = FALSE;
			accept |= ((i<=31) && (charbits & CHRSET_LOW));
			accept |= ((i==32) && (charbits & CHRSET_SPACE));
			accept |= ((i>=33 && i<=47) && (charbits & CHRSET_SIGN1));
			accept |= ((i>=48 && i<=57) && (charbits & CHRSET_NUM));
			accept |= ((i>=58 && i<=64) && (charbits & CHRSET_SIGN2));
			accept |= ((i>=65 && i<=90) && (charbits & CHRSET_UPPERCASE));
			accept |= ((i>=91 && i<=96) && (charbits & CHRSET_SIGN3));
			accept |= ((i>=97 && i<=122) && (charbits & CHRSET_LOWERCASE));
			accept |= ((i>=123 && i<=127) && (charbits & CHRSET_SIGN4));
			accept |= ((i>=128 && i<=255) && (charbits & CHRSET_EXTRA));
			if(accept)
			{
				g_character_list[g_character_count] = i;
				g_character_count++;
			}
		}
	}

	if(g_character_count==0)
	{
		MSGERROR("No characters selected.");
		return FALSE;
	}

	return TRUE;
}

BOOL PaintProcess()
{
	LOG(NULL); LOGLN("PAINT");
	if(file_exists(ItemGetText(IDC_TEXTURE_FILE)))
		if(MSGQUESTION("Overwrite texture?")!=IDYES) return FALSE;

	// make characters list
	if(!ReadCharacters()) return FALSE;

	int texw = ItemGetNumber(IDC_TEXTURE_W);
	int texh = ItemGetNumber(IDC_TEXTURE_H);
	if(texw<=0 || texh<=0)
	{
		MSGERROR("Invalid texture sizes.");
		return FALSE;
	}

	// create font
	LOGFONT logfont;
	memset(&logfont,0,sizeof(logfont));
	logfont.lfHeight = ItemGetNumber(IDC_PAINT_H);
	logfont.lfWidth = ItemGetNumber(IDC_PAINT_W);
	logfont.lfWeight = ItemGetBool(IDC_PAINT_BOLD) ? FW_BOLD : FW_NORMAL;
	logfont.lfItalic = ItemGetBool(IDC_PAINT_ITALIC);
	logfont.lfQuality = ItemGetBool(IDC_PAINT_AALISING) ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY; // force with CLEARTYPE_QUALITY=5
	strcpy(logfont.lfFaceName,ItemGetText(IDC_PAINT_FONT)); 
	HFONT font = CreateFontIndirect(&logfont);
	if(!font) { MSGERROR("Can't create font."); goto error; }

	// create hdc for color and alpha
	HDC hdc1 = CreateCompatibleDC(0);
	if(!hdc1) goto errormsg;
	HBITMAP bmp1 = CreateBitmap(texw,texh,1,32,NULL);
	if(!bmp1) goto errormsg;
	HBITMAP bmpold1 = (HBITMAP)SelectObject(hdc1,bmp1);
	HDC hdc2 = CreateCompatibleDC(0);
	if(!hdc2) goto errormsg;
	HBITMAP bmp2 = CreateBitmap(texw,texh,1,32,NULL);
	if(!bmp2) goto errormsg;
	HBITMAP bmpold2 = (HBITMAP)SelectObject(hdc2,bmp2);
	
	// paint
	if(!Paint( hdc1, hdc2, font, logfont )) goto error;

	// save
	if(!Save( hdc1, hdc2 )) goto error;

	MSGINFO("Paint successful.");
	goto done;

	errormsg:
	MSGERROR("Paint failed.");
	error:

	// close
	done:
	if(font) DeleteObject(font);
	if(hdc1 && bmpold1) SelectObject(hdc1,bmpold1);
	if(bmp1) DeleteObject(bmp1);
	if(hdc1) DeleteDC(hdc1);
	if(hdc2 && bmpold2) SelectObject(hdc2,bmpold2);
	if(bmp2) DeleteObject(bmp2);
	if(hdc2) DeleteDC(hdc2);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// MAKE
//////////////////////////////////////////////////////////////////////////////////////////////////
#define TGACOLOR(px,py) (*(dword*)(g_tgabuffer + ( (px) + (py)*g_tgaw ) * (g_tgabpp/8) ) )

BOOL MakeBestFit( RECT& rect, dword paper1, dword paper2, BOOL usealpha )
{
	paper1 &= 0x00ffffff; // rgb only
	paper2 &= 0x00ffffff; // rgb only
	int cx = rect.left;
	int cy = rect.top;
	int cw = rect.right-rect.left;
	int ch = rect.bottom-rect.top;
	
	int l0, r0, j;

	l0 = 0;
	int l;
	for(l=0; l<cw; l++)
	{
		for(j=0;j<ch;j++)
		{
			dword dw = TGACOLOR( cx+l, cy+j );
			if( usealpha )
			{
				if( (dw & 0xff000000)!=0 ) break; // alpha check
			}
			else
			{
				if( (dw & 0x00ffffff)!=paper1 && (dw & 0x00ffffff)!=paper2 ) break; // rgb check
			}
		}
		if(j<ch) { l0=l; break; } // found
	}
	if(l==cw) return FALSE; // empty cell

	r0 = cw-1;
	int r;
	for(r=cw-1; r>=0; r--)
	{
		for(j=0;j<ch;j++)
		{
			dword dw = TGACOLOR( cx+r, cy+j );
			if( usealpha )
			{
				if( (dw & 0xff000000)!=0 ) break; // alpha check
			}
			else
			{
				if( (dw & 0x00ffffff)!=paper1 && (dw & 0x00ffffff)!=paper2 ) break; // rgb check
			}
		}
		if (j<ch) { r0=r; break; } // found
	}
	if(r==-1) return FALSE; // empty cell

	if(r0<l0) return FALSE; // empty cell (nothing inside!)
	rect.right = rect.left + r0 + 1;
	rect.left += l0;
	return TRUE;
}

void MakeComputeMapping( BOOL bestfit, BOOL usealpha )
{
	int mapx = ItemGetNumber(IDC_MAP_X);
	int mapy = ItemGetNumber(IDC_MAP_Y);
	int mapw = ItemGetNumber(IDC_MAP_W);
	int maph = ItemGetNumber(IDC_MAP_H);
	int celw = ItemGetNumber(IDC_CEL_W);
	int celh = ItemGetNumber(IDC_CEL_H);

	dword paper1 = ItemGetDword(IDC_PAINT_PAPER);
	dword paper2 = ColorGen(paper1);

	// draw chars
	int cp=0; // chr pos in map
	for(int i=0;i<=g_character_count;i++)
	{
		int chr = g_character_list[i];
		int cx = cp % mapw;
		int cy = cp / mapw;
		int x = mapx + celw * cx;
		int y = mapy + celh * cy;
		RECT rect = {x,y,x+celw,y+celh};

		if(bestfit && chr!=32) // not on space chr!
			MakeBestFit(rect,paper1,paper2,usealpha);

		g_font.m_chars[chr].x = (word)rect.left;
		g_font.m_chars[chr].y = (word)rect.top;
		g_font.m_chars[chr].w = (word)(rect.right-rect.left);
		
		cp++;
	}
}

void MakePostProcess()
{
	// inspect characters
	int cnt = 0;	// count valid characters
	int chrw = 0;	// find max character width
	for(int i=0;i<=255;i++)
	{
		int chr = g_character_list[i];
		if(g_font.m_chars[i].w>0)
		{
			cnt++;
			if(chrw<g_font.m_chars[i].w) chrw = g_font.m_chars[i].w;
		}
	}
	
	char sz[128];
	// g_font.m_chrw = chrw; // overwrite general characters' width
	sprintf(sz,"Max char width: %i",chrw);
	LOGLN(sz);
	if(cnt != g_character_count)
		sprintf(sz,"Some characters where fount empty (%i<%i).",cnt,g_character_count);
	else
		sprintf(sz,"All characters are valid (%i)",g_character_count);
	LOGLN(sz);
}

BOOL MakeProcess()
{
	LOG(NULL); LOGLN("MAKE");
	
	// make characters list
	if(!ReadCharacters()) return FALSE;

	// load texture
	g_tgabuffer=NULL;
	g_tgaw=0, g_tgah=0, g_tgabpp=0;
	if(!LoadTexture(ItemGetText(IDC_TEXTURE_FILE), g_tgabuffer, g_tgaw, g_tgah, g_tgabpp)) return FALSE;

	BOOL bestfit = ItemGetBool(IDC_FONT_BESTFIT);
	BOOL usealpha = ItemGetBool(IDC_FONT_ALPHA);
	if(usealpha && g_tgabpp!=32) { MSGERROR("Texture has no alpha.\nCan't use alpha for best fit."); goto error; }

	// prepare font
	g_font.m_chrw = ItemGetNumber(IDC_FONT_CHRW);
	g_font.m_chrh = ItemGetNumber(IDC_FONT_CHRH);
	g_font.m_ofsx = ItemGetNumber(IDC_FONT_OFSX);
	g_font.m_ofsy = ItemGetNumber(IDC_FONT_OFSY);
	g_font.m_scale = ItemGetNumber(IDC_FONT_SCALE);
	g_font.m_aspect = ItemGetNumber(IDC_FONT_ASPECT);
	g_font.m_italic = ItemGetBool(IDC_FONT_ITALIC);
	g_font.m_color = ItemGetDword(IDC_PAINT_INK);
	g_font.m_texw = g_tgaw;
	g_font.m_texh = g_tgah;
	g_font.m_charscount = g_character_count;
	memset(g_font.m_chars,0,sizeof(tFontChr)*256);

	// process mapping
	MakeComputeMapping( bestfit, usealpha );
	MakePostProcess();

	free(g_tgabuffer);
	g_tgabuffer = NULL;

	// save font
	char szfile[MAX_PATH];
	strcpy(szfile,ItemGetText(IDC_TEXTURE_FILE));
	char* szext = file_path2ext(szfile);
	if(!szext) goto error;
	*szext=0;
	strcat(szfile,"fnt");
	if(!SaveFont( szfile, g_font, g_character_count, g_character_list ))
	{
		MSGERROR("Can't save fnt file.");
		goto error;
	}

	MSGINFO("Make successful.");
	return TRUE;

	error:
	if(g_tgabuffer) free(g_tgabuffer);
	g_tgabuffer = NULL;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// WINMAIN
//////////////////////////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	InitCommonControls();
	g_hinstance = hInstance;

	int ret = DialogBox( hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc1 );
	dword err = GetLastError();

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
