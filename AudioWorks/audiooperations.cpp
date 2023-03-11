//////////////////////////////////////////////////////////////////////////////////////////////////
// audiooperations.h
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "audiooperations.h"

#include "resource.h"
#include "util.h"

extern HWND g_hwnd;
//////////////////////////////////////////////////////////////////////////////////////////////////
// operations
//////////////////////////////////////////////////////////////////////////////////////////////////
void OpInfo(char *srcfile)
{
	if(strrchr(srcfile,'.')==NULL) strcat(srcfile,".ym");

	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	char sz[256];
	
	byte *ym = NULL;
	int ymsize = 0;
	if(!LoadYM(srcfile,ym,ymsize)) return;

	// info
	dword frames = read_dword(ym+12);
	word frequency = read_word(ym+26);
	int duration = frames / frequency;
	int digidrums = read_word(ym+20);
	sprintf(sz,"frames = %i\r\n",	frames); Log(hwnd,sz);
	sprintf(sz,"frequency = %i (frames per second)\r\n",frequency); Log(hwnd,sz);
	sprintf(sz,"digidrums = %i\r\n", digidrums); Log(hwnd,sz);
	sprintf(sz,"clock = %i\r\n", read_dword(ym+22)); Log(hwnd,sz);
	sprintf(sz,"duration sec = %i (frames/frequency)\r\n",duration); Log(hwnd,sz);
	sprintf(sz,"duration min = %.2f\r\n",	(float)duration / 60.0f); Log(hwnd,sz);

	// find data ptr
	int i;
	byte* ymdata = ym+34+read_word(ym+32);
	for(i=0;i<digidrums;i++)
		ymdata += 4 + read_dword(ymdata);
	sprintf(sz,"name = %s\r\n", (char*)ymdata); Log(hwnd,sz);
	ymdata += strlen((char*)ymdata)+1; // name
	sprintf(sz,"author = %s\r\n", (char*)ymdata); Log(hwnd,sz);		
	ymdata += strlen((char*)ymdata)+1; // author
	sprintf(sz,"comment = %s\r\n", (char*)ymdata); Log(hwnd,sz);	
	ymdata += strlen((char*)ymdata)+1; // comment
	Log(hwnd,"done\r\n");	
	return;
}

void OpCrop(char *srcfile,char *destfile, int startpos, int endpos, BOOL frame)
{
	if(strrchr(srcfile,'.')==NULL) strcat(srcfile,".ym");
	if(strrchr(destfile,'.')==NULL) strcat(destfile,".ym");
	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	char sz[256];
	
	byte *ym = NULL;
	int ymsize = 0;
	if(!LoadYM(srcfile,ym,ymsize)) return;

	// info
	dword frames = read_dword(ym+12);
	word frequency = read_word(ym+26);
	int duration = frames / frequency;
	int digidrums = read_word(ym+20);
	// find data ptr
	int i;
	byte* ymdata = ym+34+read_word(ym+32);
	for(i=0;i<digidrums;i++)
		ymdata += 4 + read_dword(ymdata);
	ymdata += strlen((char*)ymdata)+1; // name
	ymdata += strlen((char*)ymdata)+1; // author
	ymdata += strlen((char*)ymdata)+1; // comment

	// crop
	dword cropstart,cropend; // frames
	if(frame)
	{
		cropstart = startpos;
		cropend = endpos;
	}
	else
	{
		cropstart = startpos*frequency;
		cropend = endpos*frequency;
	}
	sprintf(sz,"crop frames %i - %i\r\n", cropstart, cropend); Log(hwnd,sz);	
	if( cropstart<0 || cropstart>=frames || cropend<=cropstart || cropend>=frames ) { sprintf(sz,"invalid crop\r\n"); free(ym); Log(hwnd,sz);	return;}

	// interlaced
	int frames2 = cropend-cropstart;
	byte* ymdata2 = (byte*)malloc(frames2*16);
	for(i=0;i<16;i++)
	{
		byte* ymdataplane = ymdata + frames*i;
		byte* ymdataplane2 = ymdata2 + frames2*i;
		memcpy( ymdataplane2, ymdataplane+cropstart, frames2);
	}
	
	// save
	FILE* file = fopen("outraw.ym","wb"); 
	if(!file) {sprintf(sz,"fail\r\n"); free(ymdata2); free(ym); Log(hwnd,sz);return; }
	write_dword(ym+12,frames2);
	fwrite(ym,1,(int)(ymdata-ym),file); // header
	fwrite(ymdata2,1,frames2*16,file); // data
	fwrite("End!",1,4,file);
	sprintf(sz,"crop successful.\r\n\r\n"); Log(hwnd,sz);
	// close
	fclose(file);
	free(ymdata2);
	free(ym);
	
	// lzh compress
	char sztmp[512];
	printf("LZH compress (using lha.exe if found)\r\n");
	sprintf(sztmp,"lha.exe a -h0 out.ym -m outraw.ym");
	system(sztmp);

	//copy
	file = fopen("out.ym","rb");
	if(file)
	{
		fseek(file,0,2);
		int size=ftell(file);
		fseek(file,0,0);
		byte* buffer = (byte*)smalloc(size);
		fread(buffer,1,size,file);
		fclose(file);
		file = fopen(destfile,"wb");
		if(file)
		{
			fwrite(buffer,1,size,file);
			fclose(file);
		}
		else
			Log(hwnd,"failed to save destination.");
	}
	else
		Log(hwnd,"failed to compress with lha.");

	Log(hwnd,"done\r\n");
}

#define FRAMECOMP(pi,pj) ((ym2[(pi)*4]==ym2[(pj)*4])&&(ym2[(pi)*4+1]==ym2[(pj)*4+1])&&(ym2[(pi)*4+2]==ym2[(pj)*4+2])&&(ym2[(pi)*4+3]==ym2[(pj)*4+3]))
void OpDetectLoop(char *srcfile)
{
	if(strrchr(srcfile,'.')==NULL) strcat(srcfile,".ym");

	HWND hwnd = GetDlgItem(g_hwnd,IDC_LOG);
	char sz[256];
	
	byte *ym = NULL;
	int ymsize = 0;
	if(!LoadYM(srcfile,ym,ymsize)) return;
	// info
	dword frames = read_dword(ym+12);
	word frequency = read_word(ym+26);
	int duration = frames / frequency;
	int digidrums = read_word(ym+20);
	// find data ptr
	int i;
	byte* ymdata = ym+34+read_word(ym+32);
	for(i=0;i<digidrums;i++)
		ymdata += 4 + read_dword(ymdata);
	ymdata += strlen((char*)ymdata)+1; // name
	ymdata += strlen((char*)ymdata)+1; // author
	ymdata += strlen((char*)ymdata)+1; // comment
	// work interlaced
	byte* ymi = (byte*)malloc(frames*16);
	memcpy(ymi,ymdata,frames*16);
	int fi,fj,delta,newfi,newfj,loopcount,posi,posj;
	BOOL looping,frameijeq;
	fi = 0;fj=fi+1;
	looping = FALSE;
	int framesi = frames;
	int k,l;
	while(fi<framesi-1 && !looping)
	{
		fj = fi+1;
		while(fj<framesi && !looping)
		{
			frameijeq = TRUE;
			for(k=0;k<16;k++)
			{
				posi = k*framesi+fi; 
				posj = k*framesi+fj; 
				frameijeq = frameijeq && (ymi[posi]==ymi[posj]);
			}
			if(frameijeq)
			{
				// potential loop
				looping = TRUE;
				loopcount=0;
				if(fi==450 && fj==2755)
				{
					int ghfdsg=0;
				}
				delta = fj - fi;
				while(fj<framesi && looping)
				{
					for(l=0;l<delta && looping;l++)
					{
						// frame fi+delta must be equal with frame fj+delta
						newfi = fi+delta;
						newfj = fj+delta;
						if(newfi<framesi&&newfj<framesi)
						{
							frameijeq = TRUE;
							for(k=0;k<16;k++)
							{
								posi = k*framesi+fi; 
								posj = k*frames+fj; 
								frameijeq = frameijeq && (ymi[posi]==ymi[posj]);
							}
							if(frameijeq)
							{
								continue;// test next from for
							}
							else
							{
								fj++;looping = FALSE;
							}
						}
					}
					if(looping) 
					{  
						fj = fj+delta; 
						if(fj<framesi)
							loopcount++; 
						// otherwise last part is only the begining of a new loop(not completed)
					}
				}
			}
			else
				fj++; // increase fj
		}
		if(!looping)
			fi++; // increase fi
	}
	if(loopcount>0 && looping)
	{
		sprintf(sz,"DETECT LOOP: %i - %i from %i frames\r\nloop number: %i \r\n", fi,fi+delta,frames,loopcount);
		Log(hwnd,sz);	
		Log(hwnd,"loop info:\r\n");	
		int loopstart = fi;
		int cnt = 1;
		char t1[6],t2[6];
		int s1,s2;
		fj = loopstart+delta;
		while(fj<framesi)
		{
			s1 = fi/frequency;
			s2 = fj/frequency;
			sprintf(t1,"%i:%.2i",(int)(s1/60),(int)(s1%60));
			sprintf(t2,"%i:%.2i",(int)(s2/60),(int)(s2%60));
			if(fj+delta<framesi)
				sprintf(sz,"loop %i: %i - %i (%s - %s)\r\n",cnt,fi,fj,t1,t2);
			else
			{
				s2 = framesi/frequency;
				sprintf(t2,"%i:%.2i",(int)(s2/60),(int)(s2%60));
				sprintf(sz,"loop %i: %i - %i (%s - %s) (incomplete)\r\n",cnt,fi,framesi,t1,t2);
			}
			Log(hwnd,sz);	
			fi=fi+delta;
			fj=fj+delta;
			cnt++;
		}
		s1 = loopstart/frequency;
		sprintf(t1,"%i:%.2i",(int)(s1/60),(int)(s1%60));
		Log(hwnd,"\r\n");	
		sprintf(sz,"SUGEST CROP: 0 - %i (0:00 - %s)\r\n", loopstart+delta,t1); Log(hwnd,sz);	
	}
	else
	{
		sprintf(sz,"NO LOOP DETECTED\r\n"); Log(hwnd,sz);	
	}
	Log(hwnd,"done\r\n");	
	sfree(ym);
	sfree(ymi);
	return;
}






       
 