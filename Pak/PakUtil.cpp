//////////////////////////////////////////////////////////////////////////////////////////////////
// PakUtil.cpp
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"
#include "commctrl.h"
#include "PakUtil.h"
#include "zlib.h"

// make sure the paths to the libs are ok
#ifdef _DEBUG
#pragma comment( lib, "..\\Libs\\ZLib\\zlib_d.lib" )
#else
#pragma comment( lib, "..\\Libs\\ZLib\\zlib.lib" )
#endif

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

void encrypt_data( void* data, dword size, char* key )
{
	if(data==NULL || size==0 || key==NULL) return;
	int len = strlen(key);
	for(dword i=0;i<size;i++)
		*((byte*)data+i) ^= (byte)i + (byte)key[i%len];
}

void decrypt_data( void* data, dword size, char* key )
{
	if(data==NULL || size==0 || key==NULL) return;
	encrypt_data( data, size, key ); // we use a simple xor method
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
// InputBox
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
// PAK  STRUCTURES
//////////////////////////////////////////////////////////////////////////////////////////////////
#define PAK_VERSION				2
#define FSPAK_COMPRESSED		(1<<0)
#define FSPAK_PROTECTED			(1<<1)

struct tPakHeader
{
	dword	m_id;				// pak id 'PAKv'
	dword	m_fatfiles;			// files in the archive
	dword	m_fatoffset;		// fat offset (in archive)
	dword	m_fatsizec;			// fat size compressed (in archive)
	dword	m_fatsize;			// fat size uncompressed
	dword	m_fatcrc;			// fat crc
	dword	m_fatcrc2;			// fat crc2 used for password
};

struct tPakFileInfo
{
	dword	m_flags;			// flags (compressed, protected...)
	dword	m_offset;			// start offset (in archive)
	dword	m_sizec;			// compressed size (in archive)
	dword	m_size;				// uncompressed size
	char*	m_name;				// file name
};

tPakHeader		m_header;
tPakFileInfo*	m_fat = NULL;
int				m_fatfiles = 0;
int				m_fatmax = 0;
int				m_fatfileoffset = 0;
char			m_sztemp[2048];

//////////////////////////////////////////////////////////////////////////////////////////////////
// PACK
//////////////////////////////////////////////////////////////////////////////////////////////////
#define	MAX_EXTENTIONS 128
char	m_extentions[MAX_EXTENTIONS][16];

// recursive files find
void PakReadFAT_rec( char* path )
{
	int i;
	char fpath[1024];
	char spath[1024];
	strcpy(spath,path); strcat(spath,"*.*");

	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFile( spath, &data );
	if(h==INVALID_HANDLE_VALUE) return;
	do
	{	// check
		if(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) continue;

		// directory
		if( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
		{
			if( 0==strcmp(data.cFileName,"." ) ) continue;
			if( 0==strcmp(data.cFileName,".." ) ) continue;

			strcpy(fpath,path);
			strcat(fpath,data.cFileName);
			strcat(fpath,"\\");
			_strlwr(fpath);
			PakReadFAT_rec(fpath);
		}
		else // file
		{

			// path
			strcpy(fpath,path);
			strcat(fpath,data.cFileName);
			_strlwr(fpath);
			int pathsize = strlen(fpath);
			char ext[256]; ext[0]=0;
			for(i=pathsize-1;i>=0;i--)
				if(fpath[i]=='.') strcpy(ext,fpath+i+1);
			if(0==stricmp(ext,"pak")) continue; // ignore pak files @WARNING !


			//add
			if(m_fatfiles==m_fatmax)
			{
				m_fatmax += 1024;
				m_fat = (tPakFileInfo*)realloc(m_fat, m_fatmax*sizeof(tPakFileInfo));
			}

			// flags (compression)
			int flags = FSPAK_COMPRESSED;
			if(ext[0]!=0) // check extention
			{
				int i=0;
				while( m_extentions[i][0]!=0 && i<MAX_EXTENTIONS )
				{
					if(stricmp(ext,m_extentions[i])==0)
					{
						flags = 0; // don't compress
						break; 
					}
					i++;
				}
			}

			// set
			tPakFileInfo* fi = &m_fat[m_fatfiles];
			fi->m_offset	= 0;				// will be determined later
			fi->m_size		= 0;				// will be determined later !
			fi->m_sizec		= 0;				// will be determined later !
			fi->m_flags		= flags;
			fi->m_name		= (char*)malloc(pathsize+1);
			strcpy(fi->m_name, fpath);

			// increment files
			m_fatfiles++;
			m_fatfileoffset += fi->m_size;
		}

	}while(FindNextFile(h,&data));
	
	FindClose(h);
}

// path must have '\' ( ex: "MyFolder\" )
int PakReadFAT( char* path )
{
	m_fatfiles		= 0;
	m_fatmax		= 1024;
	m_fat			= (tPakFileInfo*)malloc(m_fatmax*sizeof(tPakFileInfo));
	m_fatfileoffset = sizeof(m_header); // starting right after the header
	
	PakReadFAT_rec( path );
	return 1;
}

// write file in pak (idx=index in fat)
int PakWriteFile( int idx, FILE* f )
{
	m_fat[idx].m_offset = ftell(f);

	unsigned long size, sizec;
	byte* buffer = NULL;
	byte* bufferc = NULL;

	// read
	FILE* f2 = fopen( m_fat[idx].m_name, "rb" ); if(!f2) return 0;
	fseek(f2,0,2); size = ftell(f2); fseek(f2,0,0);
	if(size==0) { fclose(f2); return 1; } // nothing to write
	buffer = (byte*)malloc( size ); if(!buffer) { fclose(f2); return 0; }
	if(fread(buffer,1,size,f2) != size) { free(buffer); fclose(f2); return 0; }

	m_fat[idx].m_size = size;

	// write
	if( m_fat[idx].m_flags & FSPAK_COMPRESSED )
	{
		sizec = size + size/1000 + 32;
		bufferc = (byte*)malloc(sizec);
		if(Z_OK!=compress(bufferc, &sizec, buffer, size)) { free(bufferc); free(buffer); fclose(f2); return 0; }
		fwrite( bufferc, 1, sizec, f );
		m_fat[idx].m_sizec = sizec;
	}
	else
	{
		fwrite( buffer, 1, size, f );
		m_fat[idx].m_sizec = size;
	}
		
	if(bufferc) free(bufferc);
	if(buffer) free(buffer);
	fclose(f2);
	return 1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
// Pack proceed
// input = folder name to pack (without '\'); not full path
// current directory must be correctly set to input parent
//////////////////////////////////////////////////////////////////////////////////////////////////
#define PAK_FREEDATA()										\
{															\
	if(m_fat)												\
	{														\
		for(i=0;i<m_fatfiles;i++) free(m_fat[i].m_name);	\
		free(m_fat); m_fat = NULL;							\
	}														\
	if(buffer) free(buffer); buffer=NULL;					\
	if(bufferc) free(bufferc); bufferc=NULL;				\
	if(f) fclose(f);										\
}						

int PackProceed( char* input, char* output, char* extentions, int options, char* password )
{
	int i;
	FILE* f=NULL;
	byte* buffer=NULL;
	byte* bufferc=NULL;

	if(m_fat!=NULL) { LOG("BUG#1\r\n"); return 0; }
	m_fat = NULL;
	m_fatfiles = 0;
	m_fatmax = 0;

	// ...................................................
	// extentions count list
	LOG("Extentions: ");
	m_extentions[0][0]=0;
	if(extentions!=NULL)
	{
		int extcount = 0;
		i=0;
		while(1)
		{
			while( extentions[i]==' ' ) i++;
			if(extentions[i]==0) break;
			
			int i0=i;
			while( extentions[i]!=' ' && extentions[i]!=0 ) i++;
			if(i-i0>0)
			{
				memcpy( m_extentions[extcount], extentions+i0, i-i0 );
				m_extentions[extcount][i-i0]=0;
				LOG(m_extentions[extcount]); LOG(" ");
				extcount++;
			}
		}
	}
	LOG("\r\n");

	// ...................................................
	// init
	m_header.m_id			= (((dword)PAK_VERSION)<<24) + (((dword)'K')<<16) + (((dword)'A')<<8) + ((dword)'P');
	m_header.m_fatfiles		= 0;
	m_header.m_fatoffset	= sizeof(m_header);
	m_header.m_fatsize		= 0;
	m_header.m_fatsizec		= 0;

	// ...................................................
	// read fat and set header; we will rewrite the header when we know all file infos
	char inputpath[128];
	strcpy(inputpath,input);
	strcat(inputpath,"\\");
	int ret = PakReadFAT(inputpath);
	if(m_fatfiles==0) { LOG("ERROR: no input files found.\r\n"); PAK_FREEDATA(); return 0; }
	m_header.m_fatfiles = m_fatfiles;

	// ...................................................
	// write default header
	f = fopen(output,"wb"); if(!f) { LOG("ERROR: can't create output file.\r\n"); PAK_FREEDATA(); return 0; }
	fwrite(&m_header,1,sizeof(m_header),f);

	// ...................................................
	// write files
	int cf;
	int errors = 0;
	int size = 0;
	int sizec = 0;

	LOG("\r\n" );
	PROGRESSINIT(m_fatfiles);

	for(i=0;i<m_fatfiles;i++)
	{
		LOG(m_fat[i].m_name); LOG("  .....  ");

		if(PakWriteFile( i, f )) 
		{
			if(m_fat[i].m_flags & FSPAK_COMPRESSED)
			{
				cf = 100;
				if(m_fat[i].m_size>0)
					cf = 100 - (m_fat[i].m_size - m_fat[i].m_sizec) * 100 / m_fat[i].m_size;

				sprintf(m_sztemp,"ok %i%%",cf); 
				LOG(m_sztemp);
				if(options)
				{
					sprintf(m_sztemp,"       [ ofs=%i sz=%i szc=%i ]", m_fat[i].m_offset, m_fat[i].m_size, m_fat[i].m_sizec );
					LOG(m_sztemp);
				}
				LOG("\r\n");
			}
			else
			{
				LOG("ok");
				if(options)
				{
					sprintf(m_sztemp,"       [ ofs=%i sz=%i szc=NA ]", m_fat[i].m_offset, m_fat[i].m_size);
					LOG(m_sztemp);
				}
				LOG("\r\n");
			}

			size += m_fat[i].m_size;
			sizec += m_fat[i].m_sizec;
		}	
		else
		{	
			errors++;
			LOG("FAILED\r\n");
		}

		m_fatfileoffset += m_fat[i].m_sizec;
		PROGRESSSTEP();
	}

	// report
	cf = 100;
	if(size>0) cf = 100 - (size - sizec) * 100 / size;

	sprintf(m_sztemp,"\r\ndone: %i files\r\n", m_header.m_fatfiles);
	LOG(m_sztemp);
	sprintf(m_sztemp,"reduced to: %i%%\r\n", cf);
	LOG(m_sztemp);
	sprintf(m_sztemp,"size: %i (%.02fMB)  >  %i (%.02fMB)\r\n", size, (float)size/(1024*1024), sizec, (float)sizec/(1024*1024) );
	LOG(m_sztemp);

	if(errors) 
	{ 
		sprintf(m_sztemp,"\r\n%i errors found!\r\n", errors ); 
		LOG(m_sztemp);
		PAK_FREEDATA();
		return 0; 
	}

	m_header.m_fatoffset = ftell(f);

	// ...................................................
	// compute uncompressed fat size
	int fatsize = 0;
	for(i=0;i<m_fatfiles;i++)
		fatsize+= 16 + strlen(m_fat[i].m_name) + 1;
	m_header.m_fatsize = fatsize;

	// write fat entires
	buffer = (byte*)malloc(m_header.m_fatsize);
	int pos=0;
	for(i=0;i<m_fatfiles;i++)
	{
		tPakFileInfo* fi = &m_fat[i];
		*(dword*)(buffer+pos+0)	= fi->m_flags;
		*(dword*)(buffer+pos+4)	= fi->m_offset;
		*(dword*)(buffer+pos+8)	= fi->m_sizec;
		*(dword*)(buffer+pos+12)= fi->m_size;
		strcpy((char*)(buffer+pos+16), fi->m_name);
		pos += 16 + strlen(fi->m_name) + 1;
	}
	if(pos!=(int)m_header.m_fatsize) { LOG("ERROR: unexpected error #1.\r\n"); PAK_FREEDATA(); return 0; }

	// compress fat
	bufferc = (byte*)malloc(fatsize+fatsize/1000+32);
	unsigned long fatsizec = fatsize+fatsize/1000+32;
	if(Z_OK!=compress( bufferc, &fatsizec, buffer, fatsize )) { LOG("ERROR: can not compress FAT.\r\n"); PAK_FREEDATA(); return 0; }
	m_header.m_fatsizec = fatsizec;

	// crc
	dword crc=0;
	for(i=0;i<(int)m_header.m_fatsizec;i++) crc+=bufferc[i];
	m_header.m_fatcrc = crc;

	// crc2
	DWORD passcode = GetPasswordCode(password);
	m_header.m_fatcrc2 =	m_header.m_fatcrc ^ 
							m_header.m_fatsize ^ 
							m_header.m_fatsizec ^ 
							0xdeadc0de ^ passcode;

	// real password; the game should know it to be able to open the archive!
	// encrypt_data(bufferc,m_header.m_fatsizec,password);

	// write fat
	if(m_header.m_fatsizec!=fwrite(bufferc,1,m_header.m_fatsizec,f))
	{
		LOG("ERROR: error writing FAT.\r\n"); 
		PAK_FREEDATA();
		return 0;
	}

	// free data
	PAK_FREEDATA();

	// ...................................................
	// rewrite header
	f = fopen( output, "r+b" );
	if(!f) { LOG("ERROR: unexpected error #2.\r\n"); return 0; }
	fwrite(&m_header,1,sizeof(m_header),f);
	fclose(f);

	if(password && password[0]) { LOG("\r\nGame protected: \""); LOG(password); LOG("\"\r\n"); }

	LOG("\r\nPack successful!\r\n");
	return 1;
}



//////////////////////////////////////////////////////////////////////////////////////////////////
// UNPACK
//////////////////////////////////////////////////////////////////////////////////////////////////
int UnpakReadHeader( FILE *f )
{
	// read and check version id
	byte id[4];
	if(4!=fread(&id,1,4,f)) return 0;
	if( id[0]!='P' || id[1]!='A' || id[2]!='K' ) return 0; // no pak
	byte ver = id[3];
	if( ver!=1 && ver!=2 ) return 0; // wrong pack version
	fseek(f,0,0);
	int headersize = sizeof(tPakHeader);
	memset(&m_header,0,headersize);

	if(ver==1) headersize-=4; // old version (no crc2)

	if( headersize != fread( &m_header, 1, headersize, f ) ) return 0;
	return ver;
}

int UnpakReadFAT( FILE* f )
{
	int i;
	// header must be all valid
	if(m_header.m_fatfiles==0) return 1; // no fat entries
	if(m_header.m_fatsizec==0) return 0; // check
	if(m_header.m_fatsize==0) return 0;  // check

	// read fat from file
	if(fseek(f,m_header.m_fatoffset,0)!=0) return 0;
	byte* bufferc = (byte*)malloc(m_header.m_fatsizec);
	int fatsizec = fread(bufferc,1,m_header.m_fatsizec,f);
	if(fatsizec != (int)m_header.m_fatsizec) { free(bufferc); return 0; }

	// password @TODO: read from user
	// char* password = "mypass";
	// encrypt_data(bufferc,m_header.m_fatsizec,password);

	// check crc
	dword crc=0;
	for(i=0;i<(int)m_header.m_fatsizec;i++) crc+=bufferc[i];
	if(crc!=m_header.m_fatcrc) { free(bufferc); return 0; }

	// uncompress fat
	dword buffersize = m_header.m_fatsize;
	if(buffersize<m_header.m_fatsizec) buffersize = m_header.m_fatsizec; // safety
	// sassert(m_header.m_fatsize>=m_header.m_fatsizec);
	byte* buffer = (byte*)malloc(m_header.m_fatsize);

	unsigned long ss = m_header.m_fatsizec;
	unsigned long ds = m_header.m_fatsize;
	if(Z_OK!=uncompress( buffer, &ds, bufferc, ss)) { free(bufferc); free(buffer); return 0; }
	if(ds!=m_header.m_fatsize) { free(bufferc); free(buffer); return 0; }

	// read fat entries
	int m_files=0;
	int pos=0;
	while(pos<(int)m_header.m_fatsize-17) // a fat entry have at least 17 bytes @WARNING !
	{
		// add file info
		if(m_fatfiles==m_fatmax)
		{
			m_fatmax+=128;
			m_fat = (tPakFileInfo*)realloc(m_fat,m_fatmax*sizeof(tPakFileInfo));
		}

		tPakFileInfo* fi = &m_fat[m_fatfiles];
		fi->m_flags	= *(dword*)(buffer+pos+0);
		fi->m_offset= *(dword*)(buffer+pos+4);
		fi->m_sizec	= *(dword*)(buffer+pos+8);
		fi->m_size	= *(dword*)(buffer+pos+12);
		fi->m_name	= strdup((char*)(buffer+pos+16));
		pos += 16 + strlen(fi->m_name) + 1;
		
		m_fatfiles++;
	}
	
	free(buffer);
	free(bufferc);

	if(m_fatfiles!=(int)m_header.m_fatfiles)
	{
		if(m_fat)												
		{														
			for(i=0;i<m_fatfiles;i++) free(m_fat[i].m_name);	
			free(m_fat); m_fat = NULL;							
		}														
		return 0;
	}

	return 1;
}

// create all dirs in a path to a file
void UnpackCreatePath( char* path )
{
	if(!path || !path[0]) return;
	int len = strlen(path);
	int p=0;
	while(p<len)
	{
		if(path[p]=='\\')
		{
			path[p]=0;
			CreateDirectory(path,NULL);
			path[p]='\\';
		}
		p++;
	}
}

int UnpackWriteFile( int idx, FILE* f )
{
	dword size;
	FILE* fo=NULL;
	byte* buffer=NULL;
	byte* bufferc=NULL;
	unsigned long ss;
	unsigned long ds;

	tPakFileInfo* fi= &m_fat[idx];
	if(fseek(f,fi->m_offset,0)!=0) return 0;
	if(fi->m_name[0]==0) return 0;
	
	UnpackCreatePath(fi->m_name);

	fo = fopen(fi->m_name,"wb"); if(!fo) goto error;
	if(fi->m_size==0) { fclose(fo); return 1; } // allow 0 sized files
	if(fi->m_sizec==0) goto error;

	
	bufferc = (byte*)malloc(fi->m_sizec); if(!bufferc) goto error;
	size = fread(bufferc,1,fi->m_sizec,f); if(size!=fi->m_sizec) goto error;

	if(fi->m_flags & FSPAK_COMPRESSED)
	{
		buffer = (byte*)malloc(fi->m_size);  if(!buffer) goto error;

		ss = fi->m_sizec;
		ds = fi->m_size;
		if(Z_OK!=uncompress( buffer, &ds, bufferc, ss)) goto error;
		if(ds!=fi->m_size) goto error;
		
		size = fwrite(buffer,1,fi->m_size,fo); if(size!=fi->m_size) goto error;
	}
	else
	{
		size = fwrite(bufferc,1,fi->m_sizec,fo); if(size!=fi->m_sizec) goto error;
	}
	
	fclose(fo);
	free(bufferc);
	free(buffer);
	return 1;

error:
	if(fo) fclose(fo);
	if(bufferc) free(bufferc);
	if(buffer) free(buffer);
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Unpack proceed
// input = file name to unpack
// current directory must be correctly set to output directory
//////////////////////////////////////////////////////////////////////////////////////////////////

int UnpackProceed( char* input, int options )
{
	int i;
	int ret;
	int size;
	FILE* f;

	f = fopen(input,"rb"); if(!f) { LOG("ERROR: can't open input file.\r\n"); return 0; }

	if(m_fat!=NULL) { LOG("BUG#2\r\n"); return 0; }
	m_fat = NULL;
	m_fatfiles = 0;
	m_fatmax = 0;
	
	int ver = UnpakReadHeader(f);
	if(ver==0) { LOG("ERROR: unknown header.\r\n"); fclose(f); return 0; }
	
	// check crc2
	dword crc2 = m_header.m_fatcrc ^ 
				 m_header.m_fatsize ^ 
				 m_header.m_fatsizec ^ 
				 0xdeadc0de;
	
	if(ver!=1 && crc2 != m_header.m_fatcrc2) // game protected
	{
		char password[256];
		password[0]=0;
		if(!InputBox(g_hinstance,g_hwnd,"Input password","This archive was protected by it's author.\r\nPlese contact him if you don't have the\r\nright password.", password)) 
		{
			LOG("ABORT: password required.\r\n"); 
			return 0;
		}
		DWORD passcode = GetPasswordCode(password);
		crc2 ^= passcode;
		if(crc2 != m_header.m_fatcrc2) // bad pass
		{
			LOG("ERROR: bad password.\r\n"); 
			fclose(f); return 0;
		}
	}

	if( !UnpakReadFAT(f) )	{ LOG("ERROR: broken FAT.\r\n"); fclose(f); return 0; }

	LOG("\r\n" );
	PROGRESSINIT(m_fatfiles);
	int errors=0;
	size = 0;

	for(i=0;i<m_fatfiles;i++)
	{
		tPakFileInfo* fi= &m_fat[i];
		LOG(fi->m_name); LOG("  .....  ");

		ret = UnpackWriteFile(i,f);
		
		if(ret) 
		{
			size += fi->m_size;
			LOG("ok\r\n"); 
		}
		else 
		{
			errors++;
			LOG("FAILED\r\n");
		}

		PROGRESSSTEP();
	}

	sprintf(m_sztemp,"\r\ndone: %i files.\r\n", m_fatfiles); 
	LOG(m_sztemp);
	sprintf(m_sztemp,"size: %i (%.02fMB) written.\r\n", size, (float)size/(1024*1024) );
	LOG(m_sztemp);

	
	if(m_fat) // free fat
	{														
		for(i=0;i<m_fatfiles;i++) free(m_fat[i].m_name);	
		free(m_fat); m_fat = NULL;							
	}														

	if(errors) 
	{ 
		sprintf(m_sztemp,"\r\n%i errors found!\r\n", errors ); 
		LOG(m_sztemp);
		fclose(f);
		return 0; 
	}

	fclose(f);
	LOG("\r\nUnpack successful!\r\n");
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// Read packed file and return buffer
// input = archive to unpack
// file = file path from the archive to unpack
//////////////////////////////////////////////////////////////////////////////////////////////////
void* ReadPackedFile( char* input, char* file, dword& filesize )
{
	int i;
	FILE* f;

	f = fopen(input,"rb"); if(!f) return NULL;

	if(m_fat!=NULL) return NULL;
	m_fat = NULL;
	m_fatfiles = 0;
	m_fatmax = 0;
	
	if( !UnpakReadHeader(f) ) { fclose(f); return NULL; }
	if( !UnpakReadFAT(f) )	{ fclose(f); return NULL; }

	// results
	filesize = 0;
	void* filebuffer = NULL;

	// search
	for(i=0;i<m_fatfiles;i++)
	{
		tPakFileInfo* fi= &m_fat[i];
		if(0!=strcmp(fi->m_name,file)) continue;

		// unpack file
		dword size;
		byte* buffer=NULL;
		byte* bufferc=NULL;
		unsigned long ss;
		unsigned long ds;

		if(fseek(f,fi->m_offset,0)!=0) break;

		if(fi->m_sizec==0 || fi->m_size==0) break;
		
		bufferc = (byte*)malloc(fi->m_sizec); if(!bufferc) break;
		size = fread(bufferc,1,fi->m_sizec,f); if(size!=fi->m_sizec) { free(bufferc); break; }

		if(fi->m_flags & FSPAK_COMPRESSED)
		{
			buffer = (byte*)malloc(fi->m_size);  if(!buffer) { free(bufferc); break; }

			ss = fi->m_sizec;
			ds = fi->m_size;
			if(Z_OK!=uncompress( buffer, &ds, bufferc, ss)) { free(bufferc); free(buffer); break; }
			if(ds!=fi->m_size) { free(bufferc); free(buffer); break; }
			
			filesize = fi->m_size;
			filebuffer = buffer;
			break; // done
		}
		else
		{
			filesize = fi->m_sizec;
			filebuffer = bufferc;
			break; // done
		}

	} 

	if(m_fat) // free fat
	{														
		for(i=0;i<m_fatfiles;i++) free(m_fat[i].m_name);	
		free(m_fat); m_fat = NULL;							
	}

	return filebuffer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
// PROTECTION
//////////////////////////////////////////////////////////////////////////////////////////////////
dword GetPasswordCode( char* password )
{
	if(!password) return 0;
	int len = strlen(password);
	if(len==0) return 0;
	dword passcode=0;
	for(int i=0; i<len; i++)
	{
		// some crazy scramble rumble
		passcode ^= ((dword)(password[i])<<(i%3)) + ((i%8)<<(i%32));
	}
	return passcode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
