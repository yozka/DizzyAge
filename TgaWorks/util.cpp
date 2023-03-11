//////////////////////////////////////////////////////////////////////////////////////////////////
// Util.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "util.h"
#include "shlobj.h"

#include "R9Img.h"
#include "R9ImgLoader.h"
#include "resource.h"

extern HWND g_hwnd;
extern HINSTANCE g_hinstance;

#define RGB2BGR( argb )	( (argb & 0xff00ff00) | ((argb & 0x00ff0000)>>16) | ((argb & 0x000000ff)<<16) )
//////////////////////////////////////////////////////////////////////////////////////////////////
// LOG
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

//////////////////////////////////////////////////////////////////////////////////////////////////
// Browse 
//////////////////////////////////////////////////////////////////////////////////////////////////
int OnBrowseFile(char *title, int filter, char *path)
{
	dword flags=0;
	flags |= OFN_NOCHANGEDIR;
	flags |= OFN_PATHMUSTEXIST;

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize			= sizeof(ofn);
	ofn.hwndOwner			= g_hwnd;
	ofn.hInstance			= g_hinstance;
	if(filter==TGA_FILTER)
		ofn.lpstrFilter		= TGAFILTER;
	if(filter==BMP_FILTER)
		ofn.lpstrFilter		= BMPFILTER;
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
	//ofn.lpstrDefExt			= ext;

	if(!GetOpenFileName( &ofn ))
		return 0;
		
	return 1;
}

int OnBrowseDir(char *title, char *path)
{
	BROWSEINFO bi;
	bi.hwndOwner = g_hwnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path;
	bi.lpszTitle = title;
	bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = NULL;
	bi.iImage = 0;

	LPITEMIDLIST pidl;
	pidl=SHBrowseForFolder(&bi);
	if(pidl==NULL) return 0;
	if(!SHGetPathFromIDList(pidl, path)) return 0;

	int s=strlen(path);
	if(s==0) return 0;
	if(path[s-1]=='\\') path[s-1]=0;

	return 1;
}

BOOL OnBrowseColor( dword* color, BOOL extended )
{
	guard(OnBrowseColor)

	static COLORREF custom[16];

	CHOOSECOLOR cc;
	memset(&cc,0,sizeof(cc));
	cc.lStructSize			= sizeof(cc);
	cc.hwndOwner			= g_hwnd;
	cc.hInstance			= NULL;
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
// Images
//////////////////////////////////////////////////////////////////////////////////////////////////
BOOL LoadTGA(char* file, dword* &data, int &width, int &height, int &bpp)
{
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	char sz[256];

	r9Img img;
	if(!R9_ImgLoadFile(file,&img)) { sprintf(sz,"Can not open %s\r\n",file);Log(hwnd,sz);return FALSE;}
	int spp = R9_PFSpp(img.m_pf);
	if(!(spp==3||spp==4)) { sprintf(sz,"%s must be 24bit or 32bit\r\n",file);Log(hwnd,sz);return FALSE;}
	int datasize = img.m_size/spp;
	data = (dword*)smalloc(datasize*sizeof(dword));
	dword color;
	byte r,g,b,a;
	int i,datapos;
	datapos = 0;
	for(i=0;i<(int)img.m_size;i+=spp)
	{
		r = img.m_data[i];
		g = img.m_data[i+1];
		b = img.m_data[i+2];
		a = (spp==4)?img.m_data[i+3]:255;
		color = (a<<24)|(r<<16)|(g<<8)|b;
		data[datapos] = color;
		datapos++;
	}
	width = img.m_width;
	height = img.m_height;
	bpp = spp*8;
	return TRUE;
}

BOOL SaveTGA(char* file, dword* data, int width, int height, int bpp)
{
	r9Img img;
	img.m_width = width;
	img.m_height = height;
	img.m_pf = (bpp==24)?R9_PF_RGB:R9_PF_ARGB;
	R9_ImgCreate(&img);
	int i,j;
	int spp = R9_PFSpp(img.m_pf);
	j = 0;
	for(i=0;i<(int)img.m_size;i+=spp)
	{
		dword color = (dword)data[j];
		j++;
		
		img.m_data[i] = (byte)((color & 0x00ff0000)>>16);
		img.m_data[i+1] = (byte)((color & 0x0000ff00)>>8);
		img.m_data[i+2] = (byte)(color & 0x000000ff);
		if(spp==4)
			img.m_data[i+3] = (byte)((color & 0xff000000)>>24);
	}
	return R9_ImgSaveFile(file,&img);
}

#define LINESIZE(w) (4*((3*(w)+3)/4)) //((3*(w)%4==0) ? (3*(w)) : (3*(w)+4-(3*(w))%4))
// loads only BMP 24bit format
BOOL LoadBMP(char* filename, dword* &data, int &width, int &height, int &bpp)
{
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	char sz[256];

	F9FILE file = F9_FileOpen(filename, F9_READ); 
	if(!file) {	sprintf(sz,"Can not open %s\r\n",filename);Log(hwnd,sz);return FALSE; }
	int size;
	BITMAPFILEHEADER bmfh; 
	size = F9_FileRead(&bmfh,sizeof(bmfh),file);
	if(size<sizeof(bmfh))	{ sprintf(sz,"%s invalid header\r\n",filename);Log(hwnd,sz);F9_FileClose(file);return FALSE; }
	if(bmfh.bfType!=0x4D42)	{ sprintf(sz,"%s invalid header\r\n",filename);Log(hwnd,sz);F9_FileClose(file);return FALSE; }
	
	BITMAPINFOHEADER bmih;
	F9_FileRead(&bmih,sizeof(bmih),file);
	width = bmih.biWidth;
	height = bmih.biHeight;
	bpp = bmih.biBitCount;
	if(bpp!=24) { sprintf(sz,"%s must be 24bit\r\n",filename);Log(hwnd,sz);return FALSE; }
	if(width==0 || height==0) { sprintf(sz,"%s invalid header\r\n",filename);Log(hwnd,sz);return FALSE; }

	int imgsize = LINESIZE(width)*height;
	byte *imgcolors = (byte*)smalloc(imgsize*sizeof(byte));
	F9_FileSeek(file,bmfh.bfOffBits,SEEK_SET);
	F9_FileRead(imgcolors,imgsize,file);

	int datasize = width*height;
	data = (dword*)smalloc(datasize*sizeof(dword));
	dword *datainv = (dword*)smalloc(datasize*sizeof(dword));
	int i,j,imgpos,datapos;
	byte r,g,b,a;
	imgpos = 0;
	datapos = 0;
	for(j=0;j<height;j++)
	{
		datapos = (height-j-1)*width;
		for(i=0;i<width;i++) //  row
		{
			r = imgcolors[imgpos+3*i];
			g = imgcolors[imgpos+3*i+1];
			b = imgcolors[imgpos+3*i+2];
			a = 255;
			data[datapos] = (a<<24) | (r<<16) | (g<<8) | b;
			datapos++;
		}
		imgpos = imgpos + LINESIZE(width);
	}
	//for(i=0;i<datasize;i++)
	//	data[i] = datainv[datasize-1-i];
	sfree(imgcolors);
	sfree(datainv);
	F9_FileClose(file);
	return TRUE;
}

// saves only BMP 24bit format
BOOL SaveBMP(char* filename, dword* data, int width, int height, int bpp)
{
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	char sz[256];

	F9FILE file = F9_FileOpen(filename, F9_WRITE); 
	if(!file) {	sprintf(sz,"Can not open %s\r\n",filename);Log(hwnd,sz); return FALSE; }

	BITMAPFILEHEADER bmfh; 
	memset(&bmfh,0,sizeof(BITMAPFILEHEADER));  
	bmfh.bfType = 0x4D42;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+height*LINESIZE(width);
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	F9_FileWrite(&bmfh,sizeof(BITMAPFILEHEADER),file);

	BITMAPINFOHEADER bmih;
	memset(&bmih,0,sizeof(BITMAPINFOHEADER));  
	bmih.biSize = sizeof(BITMAPINFOHEADER); 
	bmih.biBitCount = bpp;
	bmih.biWidth = width;
	bmih.biHeight = height;
	bmih.biPlanes = 1; 
	F9_FileWrite(&bmih,sizeof(BITMAPINFOHEADER),file);

	dword color;
	int i,j,imgpos,datapos,imgsize,datasize;
	imgsize = height*LINESIZE(width);
	datasize = width*height;
	byte *imgcolors = (byte*)smalloc(imgsize*sizeof(byte));
	memset(imgcolors,0,imgsize);
	imgpos = 0;
	for(j=0;j<height;j++)
	{
		datapos = (height-j-1)*width;
		for(i=0;i<width;i++) //  row
		{
			color = data[datapos];
			datapos++;
			imgcolors[imgpos+3*i+0] = (color & 0x00ff0000) >> 16;
			imgcolors[imgpos+3*i+1] = (color & 0x0000ff00) >> 8;
			imgcolors[imgpos+3*i+2] = (color & 0x000000ff);
		}
		imgpos = imgpos + LINESIZE(width);
	}
	F9_FileWrite(imgcolors,imgsize,file);
	sfree(imgcolors);

	F9_FileClose(file);
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
