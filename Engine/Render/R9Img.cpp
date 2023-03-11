///////////////////////////////////////////////////////////////////////////////////////////////////
// R9Img.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "R9Img.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// PIXEL FORMAT
///////////////////////////////////////////////////////////////////////////////////////////////////
r9PFInfo r9_pfinfo[R9_PF_COUNT] =
{
	{  0, 0, {0,0,0,0}, {0x00000000,0x00000000,0x00000000,0x00000000}, { 0, 0, 0, 0 }, "UNKNOWN" },
	{  8, 1, {0,0,0,8}, {0x00000000,0x00000000,0x00000000,0x000000ff}, { 0, 0, 0, 0 }, "A" },
	{ 24, 3, {8,8,8,0}, {0x000000ff,0x0000ff00,0x00ff0000,0x00000000}, { 0, 8,16, 0 }, "RGB" },
	{ 24, 3, {8,8,8,0}, {0x00ff0000,0x0000ff00,0x000000ff,0x00000000}, {16, 8, 0, 0 }, "BGR" },
	{ 32, 4, {8,8,8,8}, {0x000000ff,0x0000ff00,0x00ff0000,0xff000000}, { 0, 8,16,24 }, "ARGB" },
	{ 32, 4, {8,8,8,8}, {0x00ff0000,0x0000ff00,0x000000ff,0xff000000}, {16, 8, 0,24 }, "ABGR" }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
// IMAGE
///////////////////////////////////////////////////////////////////////////////////////////////////
r9Img::r9Img() 
{
	m_width		= 0;
	m_height	= 0;
	m_pf		= R9_PF_UNKNOWN;
	m_size		= 0;
	m_data		= NULL;
};

BOOL R9_ImgCreate( r9Img* img, BOOL clear )
{
	guard(R9_ImgCreate)
	if(img==NULL) return FALSE;
	if(img->m_width==0 || img->m_height==0 || img->m_pf==R9_PF_UNKNOWN) return FALSE;
	img->m_size	= img->m_width * img->m_height * R9_PFSpp(img->m_pf);
	img->m_data	= (byte*)smalloc(img->m_size);
	if(clear) memset(img->m_data,0,img->m_size);
	return TRUE;
	unguard()
}

void R9_ImgDestroy( r9Img* img )
{ 
	guard(R9_ImgDestroy)
	if(img==NULL) return;
	if(img->m_data) sfree(img->m_data); 
	img->m_width	= 0;
	img->m_height	= 0;
	img->m_pf		= R9_PF_UNKNOWN;
	img->m_size		= 0;
	img->m_data		= NULL;	
	unguard()
}

BOOL R9_ImgDuplicate( r9Img* src, r9Img* dst )
{
	guard(R9_ImgDuplicate);
	if(!src || !R9_ImgIsValid(src)) return FALSE;
	if(!dst || R9_ImgIsValid(dst)) return FALSE;
	dst->m_width = src->m_width;
	dst->m_height = src->m_height;
	dst->m_pf = src->m_pf;
	dst->m_size = src->m_size;
	dst->m_data = (byte*)smalloc(dst->m_size);
	memcpy(dst->m_data,src->m_data,dst->m_size);
	return TRUE;
	unguard();
}

BOOL R9_ImgFlipV( r9Img* img )
{
	guard(R9_ImgFlipV)
	if(!img || !R9_ImgIsValid(img)) return FALSE;
	int linesize = R9_ImgLineSize(img);
	byte* temp = (byte*)smalloc(linesize);
	for(int i=0;i<img->m_height/2;i++)
	{
		memcpy( temp, img->m_data + i*linesize, linesize );
		memcpy( img->m_data + i*linesize, img->m_data + (img->m_height-1-i)*linesize, linesize );
		memcpy( img->m_data + (img->m_height-1-i)*linesize, temp, linesize );
	}
	sfree(temp);
	return TRUE;
	unguard()
}

BOOL R9_ImgFlipRGB( r9Img* img )
{
	guard(R9_ImgFlipRGB)
	if(!img || !R9_ImgIsValid(img)) return FALSE;
	int spp = R9_PFSpp(img->m_pf);
	if( spp!=3 && spp!=4 ) return FALSE;
	for(int i=0;i<(int)img->m_size;i+=spp)
	{
		byte t = img->m_data[i];
		img->m_data[i] = img->m_data[i+2];
		img->m_data[i+2] = t;
	}
	return TRUE;
	unguard()
}

BOOL R9_ImgScale( r9Img* src, r9Img* dst )
{
	guard(R9_ImgScale)
	if(!src || !R9_ImgIsValid(src)) return FALSE;
	if(!dst || R9_ImgIsValid(dst)) return FALSE; // dst must not be valid (only width, height are needed)

	int spp = R9_PFSpp(src->m_pf);
	if( spp!=3 && spp!=4 ) return FALSE; // only 24 and 32 bits supported
	if( dst->m_width<=0 || dst->m_height<=0) return FALSE;
	if( dst->m_width==src->m_width && dst->m_height==src->m_height) return FALSE;

	// create dst image
	dst->m_pf = src->m_pf;
	if(!R9_ImgCreate(dst)) return FALSE;

	// helpers
	int	newwidth	= dst->m_width;
	int	newheight	= dst->m_height;
	int	oldwidth	= src->m_width;
	int	oldheight	= src->m_height;
	byte* olddata	= src->m_data;
	byte* newdata	= dst->m_data;
	

	// bigger ........................................................
	if( newwidth > oldwidth && newheight > oldheight )
	{
		int du = oldwidth * 0xffff / newwidth;
		int dv = oldheight * 0xffff / newheight;

		int newadr = 0;
		int u=0, v=0;
		for(int y=0; y<newheight; y++)
		{
			u=0;
			for(int x=0; x<newwidth; x++)
			{
				byte c[4];	 // result
				byte c00[4]; // [x,y]
				byte c01[4]; // [x+1,y]
				byte c10[4]; // [x,y+1]
				byte c11[4]; // [x+1,y+1]

				int oldadr = ((u>>16) + (v>>16) * oldwidth) * spp;

				memcpy(c00,olddata+oldadr,spp);
				memcpy(c01,olddata+oldadr,spp);
				memcpy(c10,olddata+oldadr,spp);
				memcpy(c11,olddata+oldadr,spp);

				if( (u>>16) < oldwidth-1 ) 
				{
					memcpy(c01,olddata+oldadr+spp,spp);
					memcpy(c11,olddata+oldadr+spp,spp);
				}

				if( (v>>16) < oldheight-1 )
				{
					memcpy(c10,olddata+oldadr+oldwidth*spp,spp);
					memcpy(c11,olddata+oldadr+oldwidth*spp,spp);
				}

				if( (u>>16) < oldwidth-1 && (v>>16) < oldheight-1 )
				{
					memcpy(c11,olddata+oldadr+oldwidth*spp+spp,spp);
				}


				// biliniar interpolation
				int fu = u & 0xffff;
				int fv = v & 0xffff;
				int left, right;

				for(int k=0;k<spp;k++)
				{
					left  = c00[k] + ( ((int)(c10[k]-c00[k])*fv) >> 16 );
					right = c01[k] + ( ((int)(c11[k]-c01[k])*fv) >> 16 );
					c[k] = left + ( ((right-left)*fu) >> 16 );
				}

				newdata[newadr+0] = c[0];
				newdata[newadr+1] = c[1];
				newdata[newadr+2] = c[2];
				if(spp==4) newdata[newadr+3] = c[3];

				newadr += spp;
				u += du;
			}
			v += dv;
		}
	}
	
	else // bigger width ........................................................
	
	if( newwidth > oldwidth && newheight <= oldheight)
	{
		int du = oldwidth * 0xffff / newwidth; 
		int dv = oldheight * 0xffff / newheight;

		int newadr=0;
		int u=0, v=0;
		for(int y=0; y<newheight; y++)
		{
			u=0;
			for(int x=0; x<newwidth; x++)
			{
				int left, right;
				int a, r, g, b;
				int fu = u & 0xffff;

				left = right = R9_ImgGetColorRect( src, u>>16, v>>16, (u>>16)+1, (v+dv)>>16 );
				if( u >> 16 < oldwidth-1 )
					right = R9_ImgGetColorRect( src, (u>>16)+1, v>>16, (u>>16)+2, (v+dv)>>16 );

				a = ((left >> 24)       ) + (((((right >> 24)       ) - ((left >> 24)       )) * fu) >> 16);
				r = ((left >> 16) & 0xff) + (((((right >> 16) & 0xff) - ((left >> 16) & 0xff)) * fu) >> 16);
				g = ((left >>  8) & 0xff) + (((((right >>  8) & 0xff) - ((left >>  8) & 0xff)) * fu) >> 16);
				b = ((left      ) & 0xff) + (((((right      ) & 0xff) - ((left      ) & 0xff)) * fu) >> 16);

				newdata[newadr+0] = b;
				newdata[newadr+1] = g;
				newdata[newadr+2] = r;
				if(spp==4) newdata[newadr+3] = a;
				
				newadr += spp;
				u += du;
			}
			v += dv;
		}
		
	}

	else // bigger height ........................................................

	if( newwidth <= oldwidth && newheight > oldheight)
	{
		int du = oldwidth * 0xffff / newwidth;
		int dv = oldheight * 0xffff / newheight;

		int newadr=0;
		int u=0, v=0;
		for(int y=0; y<newheight; y++)
		{
			u=0;
			for(int x=0; x<newwidth; x++)
			{
				int left, right;
				int a, r, g, b;
				int fv = v & 0xffff;

				left = right = R9_ImgGetColorRect( src,  u>>16, v>>16, (u+du)>>16, (v>>16)+1 );
				if( v >> 16 < oldheight-1 )
					right = R9_ImgGetColorRect ( src, u>>16, (v>>16)+1, (u+du)>>16, (v>>16)+2 );

				a = ((left >> 24)       ) + (((((right >> 24)       ) - ((left >> 24)       )) * fv) >> 16);
				r = ((left >> 16) & 0xff) + (((((right >> 16) & 0xff) - ((left >> 16) & 0xff)) * fv) >> 16);
				g = ((left >>  8) & 0xff) + (((((right >>  8) & 0xff) - ((left >>  8) & 0xff)) * fv) >> 16);
				b = ((left      ) & 0xff) + (((((right      ) & 0xff) - ((left      ) & 0xff)) * fv) >> 16);

				newdata[newadr+0] = b;
				newdata[newadr+1] = g;
				newdata[newadr+2] = r;
				if(spp==4) newdata[newadr+3] = a;
				
				newadr += spp;
				u += du;
			}
			v += dv;
		}
	}

	else // smaller ........................................................

	if( newwidth <= oldwidth && newheight <= oldheight)
	{
		int du = oldwidth * 0xffff / newwidth;
		int dv = oldheight * 0xffff / newheight;
		int newadr=0;
		int u=0, v=0;
		for(int y=0; y<newheight; y++)
		{
			u=0;
			for(int x=0; x<newwidth; x++)
			{
				dword color = R9_ImgGetColorRect (src, u>>16, v>>16, (u+du)>>16, (v+dv)>>16 );
				newdata[newadr+0] = color & 0xff;
				newdata[newadr+1] = (color>>8) & 0xff;
				newdata[newadr+2] = (color>>16) & 0xff;
				if(spp==4) newdata[newadr+3] = color>>24;
				newadr+=spp;
				u += du;
			}
			v += dv;
		}
	}

	return TRUE;
	unguard();
}

BOOL R9_ImgScale( r9Img* img, int w, int h )
{
	guard(R9_ImgScale)
	if(!img || !R9_ImgIsValid(img)) return FALSE;
	r9Img imgnew;
	imgnew.m_width = w;
	imgnew.m_height = h;
	if(!R9_ImgScale(img, &imgnew)) return FALSE;
	R9_ImgDestroy(img);
	*img = imgnew;
	return TRUE;
	unguard()
}

BOOL R9_ImgCrop( r9Img* src, int x, int y, r9Img* dst )
{
	guard(R9_ImgCrop)
	if(!src || !R9_ImgIsValid(src)) return FALSE;
	if(!dst || R9_ImgIsValid(dst)) return FALSE; // dst must not be valid (only width, height are needed)

	int spp = R9_PFSpp(src->m_pf);
	if( spp!=3 && spp!=4 ) return FALSE; // only 24 and 32 bits supported
	if( dst->m_width<=0 || dst->m_height<=0) return FALSE;
	if( dst->m_width==src->m_width && dst->m_height==src->m_height) return FALSE;

	if( x<0 || x+dst->m_width > src->m_width ) return FALSE;
	if( y<0 || y+dst->m_height > src->m_height ) return FALSE;

	dst->m_pf = src->m_pf;
	R9_ImgCreate(dst);

	int srclinesize = src->m_width*spp;
	int dstlinesize = dst->m_width*spp;
	
	for(int i=0;i<dst->m_height;i++)
	{
		memcpy( dst->m_data + dstlinesize*i, 
				src->m_data + srclinesize*(y+i) + x*spp, dstlinesize );
	}

	return TRUE;
	unguard()
}

BOOL R9_ImgCrop( r9Img* img, int x, int y, int w, int h )
{
	guard(R9_ImgCrop)
	if(!img || !R9_ImgIsValid(img)) return FALSE;
	r9Img imgnew;
	imgnew.m_width = w;
	imgnew.m_height = h;
	if(!R9_ImgCrop(img, x, y, &imgnew)) return FALSE;
	R9_ImgDestroy(img);
	*img = imgnew;
	return TRUE;
	unguard()
}

BOOL R9_ImgConvertPF( r9Img* src, r9Img* dst, int pf )
{
	guard(R9_ImgConvertPF);
	if(!src || !R9_ImgIsValid(src)) return FALSE;
	if(!dst || R9_ImgIsValid(dst)) return FALSE;
	dst->m_width = src->m_width;
	dst->m_height = src->m_height;
	dst->m_pf = pf;
	if(!R9_ImgCreate(dst)) return FALSE;
	for(int y=0;y<dst->m_height;y++)
	{
		for(int x=0;x<dst->m_width;x++)
		{
			dword color = R9_ImgGetColor(src,x,y);
			R9_ImgSetColor(dst,x,y,color);
		}
	}
	return TRUE;
	unguard();
}

BOOL R9_ImgCopy2Mem( r9Img* img, void* buffer, int pitch )
{
	guard(R9_ImgCopy2Mem)
	if(!img || !R9_ImgIsValid(img)) return FALSE;
	byte* mdata = (byte*)buffer;
	byte* idata = img->m_data;
	int lnsize = R9_ImgLineSize(img);
	for(int i=0; i<img->m_height; i++)
	{
		memcpy(mdata,idata,lnsize);
		mdata += pitch;
		idata += lnsize;
	}
	return TRUE;
	unguard()
}

BOOL R9_ImgCopy( r9Img* src, r9Img* dst, int x, int y )
{
	guard(R9_ImgCopy2Mem)
	if(!src || !R9_ImgIsValid(src)) return FALSE;
	if(!dst || !R9_ImgIsValid(dst)) return FALSE;
	if(R9_PFSpp(src->m_pf)!=R9_PFSpp(src->m_pf)) return FALSE;
	int spp = R9_PFSpp(dst->m_pf);
	int pitch = dst->m_width * spp;
	byte* buffer = dst->m_data + y*pitch + x*spp;
	return R9_ImgCopy2Mem( src, buffer, pitch );
	unguard()
}

BOOL R9_ImgBitBlt( r9Img* src, int sx, int sy, int sw, int sh, r9Img* dst, int dx, int dy )
{
	guard(R9_ImgBitBlt)
	if(!src || !R9_ImgIsValid(src)) return FALSE;
	if(!dst || !R9_ImgIsValid(dst)) return FALSE;
	if(R9_PFSpp(src->m_pf)!=R9_PFSpp(src->m_pf)) return FALSE;
	int spp = R9_PFSpp(dst->m_pf);
	
	int srcpitch = src->m_width * spp;
	byte* srcbuffer = src->m_data + sy*srcpitch + sx*spp;

	int dstpitch = dst->m_width * spp;
	byte* dstbuffer = dst->m_data + dy*dstpitch + dx*spp;

	int lnsize = sw*spp;

	for(int i=0; i<sh; i++)
	{
		memcpy( dstbuffer, srcbuffer, lnsize );
		srcbuffer += srcpitch;
		dstbuffer += dstpitch;
	}

	return TRUE;
	unguard()
}


BOOL R9_ImgBitBltSafe( r9Img* src, int sx, int sy, int sw, int sh, r9Img* dst, int dx, int dy )
{
	guard(R9_ImgBitBltSafe)
	if(!src || !R9_ImgIsValid(src)) return FALSE;
	if(!dst || !R9_ImgIsValid(dst)) return FALSE;
	for(int i=0; i<sh; i++)
	{
		for(int j=0; j<sw; j++)
		{
			dword color = R9_ImgGetColorSafe(src,sx+j,sy+i);
			R9_ImgSetColorSafe(dst,dx+j,dy+i,color);
		}
	}
	return TRUE;
	unguard()
}

BOOL R9_ImgWriteBuffer( r9Img* img, void* buffer, r9PFInfo* pfinfo, int pitch )
{
	guard(R9_ImgWriteBuffer);
	if(!img || !R9_ImgIsValid(img)) return FALSE;
	byte* idata = img->m_data;
	r9PFInfo* pfinfoimg = &r9_pfinfo[img->m_pf];
	float argb[4];
	for(int y=0; y<img->m_height; y++)
	{
		byte* mdata = (byte*)buffer + y*pitch;
		for(int x=0; x<img->m_width; x++)
		{
			R9_PFGetARGB(idata,pfinfoimg,argb);
			R9_PFSetARGB(mdata,pfinfo,argb);
			idata+=pfinfoimg->m_spp;
			mdata+=pfinfo->m_spp;
		}
	}
	return TRUE;	
	unguard();
}

BOOL R9_ImgReadBuffer( r9Img* img, void* buffer, r9PFInfo* pfinfo, int pitch )
{
	guard(img_readbuffer);
	if(!img || !R9_ImgIsValid(img)) return FALSE;
	byte* idata = img->m_data;
	r9PFInfo* pfinfoimg = &r9_pfinfo[img->m_pf];
	float argb[4];
	for(int y=0; y<img->m_height; y++)
	{
		byte* mdata = (byte*)buffer + y*pitch;
		for(int x=0; x<img->m_width; x++)
		{
			R9_PFGetARGB(mdata,pfinfo,argb);
			R9_PFSetARGB(idata,pfinfoimg,argb);
			mdata+=pfinfo->m_spp;
			idata+=pfinfoimg->m_spp;
		}
	}
	return TRUE;	
	unguard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
