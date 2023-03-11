#include "stdafx.h"
#include "gslib_file.h"
#include "gs.h"
#include <time.h>
#include <io.h>

static char tmp_fullpath[256];

/////////////////////////////////////////////////////////////////////////////////////////
// files IO
/////////////////////////////////////////////////////////////////////////////////////////
int gsFileOpen( gsVM* vm )
{
	guard(gsFileOpen)
	if(!gs_ckparams(vm,2))		GS_RETURNPTR(vm,NULL);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNPTR(vm,NULL);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNPTR(vm,NULL);
	void* f = gsfile_open( gs_tostr(vm,0), gs_toint(vm,1) );
	gs_pushptr(vm,f);
	return 1;
	unguard()
}

int gsFileSeek( gsVM* vm )
{
	guard(gsFileSeek)
	if(!gs_ckparams(vm,3))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_PTR)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_INT)) GS_RETURNINT(vm,0);
	int ret = gsfile_seek( gs_toptr(vm,0), gs_toint(vm,1), gs_toint(vm,2) );
	gs_pushint(vm,ret?0:1);
	return 1;
	unguard()
}

int gsFileTell( gsVM* vm )
{
	guard(gsFileTell)
	if(!gs_ckparams(vm,1))		GS_RETURNINT(vm,-1);
	if(!gs_cktype(vm,0,GS_PTR)) GS_RETURNINT(vm,-1);
	int ret = gsfile_tell( gs_toptr(vm,0) );
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsFileWriteInt( gsVM* vm )
{
	guard(gsFileWriteInt)
	if(!gs_ckparams(vm,2))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_PTR)) GS_RETURNINT(vm,0);
	int val = gs_toint(vm,0);
	int ret = gsfile_write( &val, sizeof(int), gs_toptr(vm,1) );
	gs_pushint(vm,ret==sizeof(int));
	return 1;
	unguard()
}

int gsFileWriteFloat( gsVM* vm )
{
	guard(gsFileWriteFloat)
	if(!gs_ckparams(vm,2))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_FLT)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_PTR)) GS_RETURNINT(vm,0);
	float val = gs_toflt(vm,0);
	int ret = gsfile_write( &val, sizeof(float), gs_toptr(vm,1) );
	gs_pushint(vm,ret==sizeof(float));
	return 1;
	unguard()
}

int gsFileWriteText( gsVM* vm )
{
	guard(gsFileWriteText)
	if(!gs_ckparams(vm,2))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_PTR)) GS_RETURNINT(vm,0);
	char* val = gs_tostr(vm,0);
	int ret=0;
	int size = gsstrlen(val);
	if(size) ret = gsfile_write( val, size, gs_toptr(vm,1) );
	gsfile_write( "\r\n", 2, gs_toptr(vm,1) ); // eol
	gs_pushint(vm,ret==size); // hm!
	return 1;
	unguard()
}

int gsFileWriteData( gsVM* vm )
{
	guard(gsFileWriteData)
	if(!gs_ckparams(vm,3))			GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_PTR))		GS_RETURNINT(vm,0);
	gsObj* obj = gs_toref(vm,0);
	char* data = obj->s;
	int size = gs_toint(vm,1);
	void* f = gs_toptr(vm,2);
	if(data==NULL)		GS_RETURNINT(vm,0);
	if(obj->size<size)	GS_RETURNINT(vm,0);
	int ret=0;
	if(size) ret = gsfile_write( data, size, f );
	gs_pushint(vm,ret==size); // hm!
	return 1;
	unguard()
}

int gsFileReadInt( gsVM* vm )
{
	guard(gsFileReadInt)
	if(!gs_ckparams(vm,2))			GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,0,GS_INT))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_PTR))		GS_RETURNINT(vm,0);
	gsObj* obj = gs_toref(vm,0);
	int ret = gsfile_read( &obj->i, sizeof(int), gs_toptr(vm,1) );
	gs_pushint(vm,ret==sizeof(int));
	return 1;
	unguard()
}

int gsFileReadFloat( gsVM* vm )
{
	guard(gsFileReadFloat)
	if(!gs_ckparams(vm,2))			GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,0,GS_FLT))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_PTR))		GS_RETURNINT(vm,0);
	gsObj* obj = gs_toref(vm,0);
	int ret = gsfile_write( &obj->f, sizeof(float), gs_toptr(vm,1) );
	gs_pushint(vm,ret==sizeof(float));
	return 1;
	unguard()
}

int gsFileReadText( gsVM* vm )
{
	guard(gsFileReadText)
	if(!gs_ckparams(vm,2))			GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_PTR))		GS_RETURNINT(vm,0);
	gsObj* obj = gs_toref(vm,0);
	void* f = gs_toptr(vm,1);

	int ret = 0;
	int count = 0;
	int size = 64;
	char* sz=(char*)gsmalloc(size);
	while(true)
	{
		char c;
		int ret = gsfile_read( &c, 1, f );
		if(ret!=1 || c=='\n') c=0; // eof or eol -> eos
		if(count==size) { size+=64; sz=(char*)gsrealloc(sz,size); }
		sz[count] = c;
		count++;
		if(c==0) break;
	}

	if(count>1 && sz[count-2]=='\r') { sz[count-2]=0; count--; }

	gso_del(*obj); // release old string
	*obj = gsObj( sz, count ); // make new one
	gs_pushint(vm,(ret!=1 && count==1) ? 0 : 1 ); // false if eof and nothing read
	return 1;
	unguard()
}

int gsFileReadData( gsVM* vm )
{
	guard(gsFileReadData)
	if(!gs_ckparams(vm,3))			GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_INT))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_PTR))		GS_RETURNINT(vm,0);
	gsObj* obj = gs_toref(vm,0);
	char* data = obj->s;
	int size = gs_toint(vm,1);
	void* f = gs_toptr(vm,2);
	if(data==NULL)		GS_RETURNINT(vm,0);
	if(obj->size<size)	GS_RETURNINT(vm,0);
	int ret=0;
	if(size) ret = gsfile_read( data, size, f );
	gs_pushint(vm,ret==size);
	return 1;
	unguard()
}

int gsFileClose( gsVM* vm )
{
	guard(gsFileClose)
	if(!gs_ckparams(vm,1))			GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_PTR))		GS_RETURNINT(vm,0);
	int ret =gsfile_close( gs_toptr(vm,0) );
	gs_pushint(vm,ret?0:1);
	return 1;
	unguard()
}

int gsFileInfo( gsVM* vm )
{
	guard(gsFileInfo)
	if(!gs_ckparams(vm,1))			GS_RETURNSTR(vm,NULL);
	if(!gs_cktype(vm,0,GS_STR))		GS_RETURNSTR(vm,NULL);

	struct _finddata_t filedata;
	long h = (long)_findfirst( gs_tostr(vm,0), &filedata );
	if( h==-1 ) GS_RETURNSTR(vm,NULL); // not found
	if( filedata.attrib & _A_SUBDIR ) { _findclose(h); GS_RETURNSTR(vm,NULL); }
	
	char sz[128]; // 26+
	sprintf(sz,"%s%i",ctime(&(filedata.time_write)),filedata.size);
	gs_pushstr( vm, sz );
	_findclose(h);
	return 1;
	unguard()
}

int gsFileDelete( gsVM* vm )
{
	guard(gsFileDelete)
	if(!gs_ckparams(vm,1))			GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,0);
	int ret = DeleteFile(gs_tostr(vm,0));
	gs_pushint(vm,ret);
	return 1;
	unguard()
}


void gsFindFilesRec( gsVM* vm, char* path, int fid, BOOL recursive )
{
	guard(gsFindFilesRec);
	char fpath[256];
	char spath[256];
	if(path) strcpy(spath,path); else spath[0]=0;
	strcat(spath,"*.*");

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

			if(path) strcpy(fpath,path); else fpath[0]=0;
			strcat(fpath,data.cFileName);
			_strlwr(fpath);
			// run callback
			gs_pushstr(vm, fpath);
			gs_pushint(vm, 1);
			gs_runfn(vm,fid,2);
			gs_pop(vm);
			strcat(fpath,"\\");
			if( recursive )	gsFindFilesRec( vm, fpath, fid, recursive );
		}
		else // file
		{
			if(path) strcpy(fpath,path); else fpath[0]=0;
			strcat(fpath,data.cFileName);
			_strlwr(fpath);
			// run callback
			gs_pushstr(vm, fpath);
			gs_pushint(vm, 0);
			gs_runfn(vm,fid,2);
			gs_pop(vm);
		}

	}while(FindNextFile(h,&data));
	
	FindClose(h);
	unguard()
}

int gsFileFind( gsVM* vm )
{
	guard(gsFileFind)
	if(!gs_ckparams(vm,3))			return 0;
	if(!gs_cktype(vm,0,GS_STR))		return 0;
	if(!gs_cktype(vm,1,GS_STR))		return 0;
	if(!gs_cktype(vm,2,GS_INT))		return 0;
	int fid = gs_findfn(vm,gs_tostr(vm,1));
	if(fid==-1) return 0; // callback not found
	gsFindFilesRec(vm,gs_tostr(vm,0),fid,gs_toint(vm,2));
	return 0;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// directory utils
/////////////////////////////////////////////////////////////////////////////////////////
int gsDirectoryGet( gsVM* vm )
{
	guard(gsDirectoryGet)
	tmp_fullpath[0]=0;
	int ret = GetCurrentDirectory(255, tmp_fullpath);
	gs_pushstr(vm,tmp_fullpath);
	return 1;
	unguard()
}

int gsDirectorySet( gsVM* vm )
{
	guard(gsDirectorySet)
	if(!gs_ckparams(vm,1))			GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,0);
	BOOL ok = SetCurrentDirectory(gs_tostr(vm,0));
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsDirectoryCreate( gsVM* vm )
{
	guard(gsDirectoryCreate)
	if(!gs_ckparams(vm,1))			return 0;
	if(!gs_cktype(vm,0,GS_STR))		return 0;
	CreateDirectory(gs_tostr(vm,0),NULL);
	return 0;
	unguard()
}

int gsDirectoryDelete( gsVM* vm )
{
	guard(gsDirectoryDelete)
	if(!gs_ckparams(vm,1))			GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,0);
	int ret = RemoveDirectory(gs_tostr(vm,0));
	gs_pushint(vm,ret);
	return 1;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// ini files
/////////////////////////////////////////////////////////////////////////////////////////
int gsIniGetInt( gsVM* vm )
{
	guard(gsIniGetInt)
	if(!gs_ckparams(vm,4))			GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_STR))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_STR))		GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,3,GS_INT))	GS_RETURNINT(vm,0);
	if(!GetFullPathName( gs_tostr(vm,0), 255, tmp_fullpath, NULL )) GS_RETURNINT(vm,0);
	gsObj* obj = gs_toref(vm,3);

	int tmp=0;
	char value[64]; value[0]=0;
	GetPrivateProfileString( gs_tostr(vm,1), gs_tostr(vm,2), "", value, 64, tmp_fullpath );
	BOOL ok=(1==sscanf( value, "%i", &tmp ) );
	if(ok) obj->i = tmp;
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsIniGetStr( gsVM* vm )
{
	guard(gsIniGetStr)
	if(!gs_ckparams(vm,4))			GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,1,GS_STR))		GS_RETURNINT(vm,0);
	if(!gs_cktype(vm,2,GS_STR))		GS_RETURNINT(vm,0);
	if(!gs_ckreftype(vm,3,GS_STR))	GS_RETURNINT(vm,0);
	if(!GetFullPathName( gs_tostr(vm,0), 255, tmp_fullpath, NULL )) GS_RETURNINT(vm,0);
	gsObj* obj = gs_toref(vm,3);
	
	static char value[256];
	BOOL ok = (0!=GetPrivateProfileString( gs_tostr(vm,1), gs_tostr(vm,2), "", value, 256, tmp_fullpath ));
	if(ok) { gso_del(*obj);	*obj = gso_dup( gsObj(value,gsstrsize(value)) ); }
	gs_pushint(vm,ok);
	return 1;
	unguard()
}

int gsIniSetInt( gsVM* vm )
{
	guard(gsIniSetInt)
	if(!gs_ckparams(vm,4))		return 0;
	if(!gs_cktype(vm,0,GS_STR)) return 0;
	if(!gs_cktype(vm,1,GS_STR)) return 0;
	if(!gs_cktype(vm,2,GS_STR)) return 0;
	if(!gs_cktype(vm,3,GS_INT)) return 0;
	if(!GetFullPathName( gs_tostr(vm,0), 255, tmp_fullpath, NULL )) return 0;

	char value[64];
	sprintf(value,"%i",gs_toint(vm,3));
	WritePrivateProfileString( gs_tostr(vm,1), gs_tostr(vm,2), value, tmp_fullpath );
	return 0;
	unguard()
}

int gsIniSetStr( gsVM* vm )
{
	guard(gsIniSetStr)
	if(!gs_ckparams(vm,4))		return 0;
	if(!gs_cktype(vm,0,GS_STR)) return 0;
	if(!gs_cktype(vm,1,GS_STR)) return 0;
	if(!gs_cktype(vm,2,GS_STR)) return 0;
	if(!gs_cktype(vm,3,GS_STR)) return 0;
	if(!GetFullPathName( gs_tostr(vm,0), 255, tmp_fullpath, NULL )) return 0;

	char* value = gs_tostr(vm,3);
	WritePrivateProfileString( gs_tostr(vm,1), gs_tostr(vm,2), value, tmp_fullpath );
	return 0;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// register lib
/////////////////////////////////////////////////////////////////////////////////////////
int gslib_file( gsVM* vm )
{
	guard(gslib_file)
	
	// files IO
	gs_regfn( vm, "gs_fileopen",		gsFileOpen );		// str(filename), int(0="rb",1="wb") > ptr
	gs_regfn( vm, "gs_fileseek",		gsFileSeek );		// ptr, int(pos), int(mode 0=start,1=current,2=end) > int(1/0)
	gs_regfn( vm, "gs_filetell",		gsFileTell );		// ptr > int(pos/-1)
	gs_regfn( vm, "gs_filewriteint",	gsFileWriteInt );	// int, ptr > int(1/0)
	gs_regfn( vm, "gs_filewritefloat",	gsFileWriteFloat );	// flt, ptr > int(1/0)
	gs_regfn( vm, "gs_filewritetext",	gsFileWriteText );	// str, ptr > int(1/0); -eos, +eol (/r/n)
	gs_regfn( vm, "gs_filewritedata",	gsFileWriteData );	// ref_str, int(size), ptr > int(1/0); str must be large enought (>=size) !!!
	gs_regfn( vm, "gs_filereadint",		gsFileReadInt );	// ref_int, ptr > int(1/0)
	gs_regfn( vm, "gs_filereadfloat",	gsFileReadFloat );	// ref_flt, ptr > int(1/0)
	gs_regfn( vm, "gs_filereadtext",	gsFileReadText );	// ref_str, ptr > int(1/0); -eol (/r/n), +eos
	gs_regfn( vm, "gs_filereaddata",	gsFileReadData );	// ref_str, int(size), ptr > int(1/0) ; str must be large enought (>=size) !!!
	gs_regfn( vm, "gs_fileclose",		gsFileClose );		// ptr > int(1/0)
	gs_regfn( vm, "gs_fileinfo",		gsFileInfo );		// str(filename) > str("Wed Jan 02 02:03:55 1980\nFILE_SIZE" / NULL)
	gs_regfn( vm, "gs_filedelete",		gsFileDelete );		// str(filename) > int(1/0)
	gs_regfn( vm, "gs_filefind",		gsFileFind );		// str(filepath), str(callback), int(recursive); callback function format is: str(filepath), int(isdir)

	// directory
	gs_regfn( vm, "gs_directoryget",	gsDirectoryGet );	// > str(path); static data
	gs_regfn( vm, "gs_directoryset",	gsDirectorySet );	// str(path) > int(1/0)
	gs_regfn( vm, "gs_directorycreate",	gsDirectoryCreate );// str(path); no error returned, path must exist
	gs_regfn( vm, "gs_directorydelete", gsDirectoryDelete );// str(path) > int(1/0)

	// ini files
	gs_regfn( vm, "gs_inigetint",		gsIniGetInt );		// str(file), str(group), str(key), refint(val) > int(1/0); static data
	gs_regfn( vm, "gs_inigetstr",		gsIniGetStr );		// str(file), str(group), str(key), refstr(val) > int(1/0); static data
	gs_regfn( vm, "gs_inisetint",		gsIniSetInt );		// str(file), str(group), str(key), int(val); static data
	gs_regfn( vm, "gs_inisetstr",		gsIniSetStr );		// str(file), str(group), str(key), str(val); static data

	return 0;	

	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
