//////////////////////////////////////////////////////////////////////////////////////////////////
// Util.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "util.h"
#include "shlobj.h"

#include "Lzh.h"	// lzh.h.cpp din stsound
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
	if(filter==YM_FILTER)
		ofn.lpstrFilter		= YMFILTER;
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
// YM file
//////////////////////////////////////////////////////////////////////////////////////////////////
int LZHUnpack( byte* src, int srcsize, byte* &dst, int &dstsize )
{
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	char sz[256];
	
	if(srcsize<sizeof(lzhHeader_t)) return 0;
	lzhHeader_t* pHeader = (lzhHeader_t*)src;
	if( (pHeader->size==0) || (strncmp(pHeader->id,"-lh5-",5)) ) return 0; // header size or ID error
	if( pHeader->level!=0 ) return 0; // compression LH5 error
	dstsize = *(dword*)&(pHeader->original); // little endian
	dst = (byte*)malloc(dstsize); if(dst==NULL) return 0; // memory error
	src = src + sizeof(lzhHeader_t) + pHeader->name_lenght + 2; // skip to data
	dword packsize = *(dword*)&(pHeader->packed); //little endian
	//sprintf(sz,"LZH unpacking ... %i -> %i bytes\r\n", srcsize, dstsize);	Log(hwnd,sz);

	CLzhDepacker* pDepacker = new CLzhDepacker;	
	bool ret = pDepacker->LzUnpack(src,packsize,dst,dstsize);
	delete pDepacker;
	if(ret) return 1;

	// depacking error
	sprintf(sz,"LZH unpack error\n"); Log(hwnd,sz);
	free(dst);
	dst = NULL;
	return 0;
}

BOOL LoadYM( char* srcfile, byte* &data, int &datasize )
{
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	char sz[256];

	FILE* file = fopen(srcfile, "rb"); 
	if(!file) {	sprintf(sz,"Can not open %s\r\n",srcfile);Log(hwnd,sz);return FALSE; }
	fseek(file,0,2);
	datasize = ftell(file);
	fseek(file,0,0);
	data = (byte*)malloc(datasize);
	fread(data,1,datasize,file);
	fclose(file);
	if(datasize<32) { sprintf(sz,"ym file size error\r\n");Log(hwnd,sz);return FALSE; }

	// unpack
	int ymupsize = 0;
	byte* ymup = NULL;
	if(LZHUnpack(data,datasize,ymup,ymupsize)) // unpack if necessary
	{
		free(data);
		data=ymup;
		datasize = ymupsize;
	//	sprintf(sz,"\r\n");Log(hwnd,sz);
	}

	// format
	if( data[0]!='Y' || data[1]!='M' || data[2]!='6' || data[3]!='!' ) { sprintf(sz,"invalid format (YM6! only)\r\n"); free(data);Log(hwnd,sz);return FALSE; }
	if( strncmp((const char*)(data+4),"LeOnArD!",8)!=0 ) {sprintf(sz,"invalid YM6! format\r\n"); free(data);Log(hwnd,sz);return FALSE; }

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
