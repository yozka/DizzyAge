//////////////////////////////////////////////////////////////////////////////////////////////////
// tgaoperations.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tgaoperations.h"

#include "R9Img.h"
#include "R9ImgLoader.h"

#include "resource.h"
#include "util.h"

extern HWND g_hwnd;

dword g_OpMultiBuildAlphaColor = 0x00000000;
char  g_OpFolder[MAX_PATH];

//////////////////////////////////////////////////////////////////////////////////////////////////
// operations
//////////////////////////////////////////////////////////////////////////////////////////////////
void OpExplode(char *srcfile,char *file1, char *file2)
{
	if(strrchr(srcfile,'.')==NULL) strcat(srcfile,".bmp");
	if(strlen(file1)>0 && strrchr(file1,'.')==NULL) strcat(file1,".tga");
	if(strlen(file2)>0 && strrchr(file2,'.')==NULL) strcat(file2,".tga");

	dword *data = NULL;
	int width,height,bpp;
	if(!LoadTGA(srcfile,data,width,height,bpp))
	{
		sfree(data);
		char sz[256];
		sprintf(sz,"ERROR  %s\r\n",srcfile);
		HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
		Log(hwnd,sz);
		return;
	}
	dword *data2 = NULL;
	data2 = (dword*)smalloc(width*height*sizeof(dword));
	memcpy(data2,data,width*height*sizeof(dword));

	for(int i=0;i<width*height;i++)
	{
		dword color = (dword)data[i];
		dword alphacolor = (dword)data[i];
		color = color & 0x00ffffff;
		data[i] = color;
		byte alpha = alphacolor>>24;
		alphacolor = (alpha<<16)|(alpha<<8)|alpha;  
		data2[i] = alphacolor;
	}
	if(strlen(file1)>0)
		SaveBMP(file1,data,width,height,24);
	if(strlen(file2)>0)
		SaveBMP(file2,data2,width,height,24);
	sfree(data);
	sfree(data2);
	char sz[256];
	sprintf(sz,"done\r\n");
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	Log(hwnd,sz);
}

void OpImplode(char *file1, char *file2,char *destfile)
{
	char sz[256];
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	if(strlen(file1)==0 || strlen(destfile)==0)
	{
		sprintf(sz,"ERROR no files to implode\r\n");	Log(hwnd,sz);
		return;
	}
	if(strrchr(destfile,'.')==NULL) strcat(destfile,".tga");
	if(strrchr(file1,'.')==NULL) strcat(file1,".bmp");
	if(strlen(file1)>0 && strrchr(file2,'.')==NULL) strcat(file2,".bmp");

	dword *data1 = NULL;
	dword *data2 = NULL;
	int width1,height1,bpp1;
	int width2,height2,bpp2;
	
	if(!LoadBMP(file1,data1,width1,height1,bpp1))
	{
		sprintf(sz,"ERROR  %s\r\n",file1);	Log(hwnd,sz);
		sfree(data1);
		return;
	}
	if(strlen(file2)>0)
	{
		if(!LoadBMP(file2,data2,width2,height2,bpp2))
		{
			sprintf(sz,"ERROR  %s\r\n",file2);	Log(hwnd,sz);
			sfree(data1);
			sfree(data2);
			return;
		}
	}
	if(strlen(file2)>0  && (width1!=width2 || height1!=height2))
	{
		sprintf(sz,"ERROR  file size\r\n");	Log(hwnd,sz);
		sfree(data1);
		sfree(data2);
		return;
	}
	dword color;
	byte alphacolor;
	for(int i=0;i<width1*height1;i++)
	{
		color = ((dword)data1[i]) & 0x00ffffff;
		if(strlen(file2)>0)
		{
			alphacolor = ((dword)data2[i]) & 0x000000ff; // keep only B channel (R,G,B must be the same)
			data1[i] = (alphacolor<<24) | color;
		}
		else
			data1[i] = color;
	}
	int bpp = 32;
	if(strlen(file2)==0)
		bpp = 24;

	SaveTGA(destfile,data1,width1,height1,bpp);
	sfree(data1);
	sfree(data2);
	sprintf(sz,"done\r\n");	Log(hwnd,sz);
}

void OpBuildAlpha(char *file,dword alphacolor)
{
	dword *data = NULL;
	int width,height,bpp;
	if(!LoadTGA(file,data,width,height,bpp))
	{
		sfree(data);
		return;
	}
	alphacolor = alphacolor & 0x00ffffff; 
	for(int i=0;i<width*height;i++)
	{
		dword color = (dword)data[i];
		color = color & 0x00ffffff;
		if(color!=alphacolor)
			color = color | 0xff000000;
		data[i] = color;
	}
	SaveTGA(file,data,width,height,bpp);
	sfree(data);
	char sz[256];
	sprintf(sz,"done       %s\r\n",file);
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	Log(hwnd,sz);
}

void OpMultiBuildAlpha(char *file, BOOL dir)
{
	OpBuildAlpha(file,g_OpMultiBuildAlphaColor);	
}

void OpCheckColor(char *file)
{
	char* ext = file_path2ext(file);
	if( stricmp(ext,"tga")!=0 ) return;

	dword colors[COLOR_MAX];	// max COLOR_MAX values for colors
	int colorcount;				// colors count
	dword alpha[ALPHA_MAX];		// max ALPHA_MAX values for alpha
	int alphacount;				// alpha count
	colorcount = 0;
	alphacount = 0;

	dword *data = NULL;
	int width,height,bpp;
	if(!LoadTGA(file,data,width,height,bpp))
	{
		sfree(data);
		return;
	}
	int i,j;
	BOOL colorfound;
	BOOL alphafound;

	for(i=0;i<width*height;i++)
	{
		dword color = (dword)data[i];
		color = color & 0x00ffffff;
		if(colorcount<COLOR_MAX)
		{
			colorfound = FALSE;
			for(j=0;j<colorcount && !colorfound;j++)
				if(color==colors[j])
					colorfound = TRUE;
			if(!colorfound)
			{
				colors[colorcount] = color;
				colorcount++;
			}
		}
	
		if(bpp==32)
		{
			dword alphacolor = (dword)data[i];
			alphacolor = alphacolor & 0xff000000;
			if(alphacount<ALPHA_MAX)
			{
				alphafound = FALSE;
				for(j=0;j<alphacount && !alphafound;j++)
					if(alphacolor==alpha[j])
						alphafound = TRUE;
				if(!alphafound)
				{
					alpha[alphacount] = alphacolor;
					alphacount++;
				}
			}
		}
		if(!(colorcount<COLOR_MAX) && !(alphacount<ALPHA_MAX))
			break;			
	}
	sfree(data);
	char sz[256],szfile[MAX_PATH];
	if(strlen(g_OpFolder)>0)
		strcpy(szfile,file+strlen(g_OpFolder)); // strcpy(szfile,".\\"); strcat(szfile,file+strlen(g_OpFolder));
	else
		strcpy(szfile,file);
	
	char szcolors[2][16];
	if(colorcount>=COLOR_MAX)
		sprintf(szcolors[0],"%i+",COLOR_MAX-1);
	else
		sprintf(szcolors[0],"%i ",colorcount);
	if(alphacount>=ALPHA_MAX)
		sprintf(szcolors[1],"%i+",ALPHA_MAX-1);
	else
		sprintf(szcolors[1],"%i ",alphacount);

	sprintf(sz," %s %s %s\r\n",szcolors[0],szcolors[1],szfile);
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	Log(hwnd,sz);
}

void OpMultiCheckColor(char *file, BOOL dir)
{
	OpCheckColor(file);	
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
