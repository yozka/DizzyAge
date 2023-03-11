#include "stdafx.h"
#include "gslib_default.h"
#include "gs.h"


/////////////////////////////////////////////////////////////////////////////////////////
// errors
/////////////////////////////////////////////////////////////////////////////////////////
int gsError( gsVM* vm )
{
	guard(gsError)
	char* sz = NULL;
	if(gs_params(vm)==1 && gs_cktype(vm,0,GS_STR)) sz = gs_tostr(vm,0);
	gs_error(vm,GSE_USER,sz);
	vm->m_status = GSE_USER; // stop execution
	return 0;
	unguard()
}

int gsDebug( gsVM* vm )
{
	guard(gsDebug)
	if(!gs_cktype(vm,0,GS_INT)) return 0;
	vm->m_debug = gs_toint(vm,0);
	return 0;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// Compile and Do
/////////////////////////////////////////////////////////////////////////////////////////
int gsCompileFile( gsVM* vm )
{
	guard(gsCompileFile)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	int ret = gs_compilefile(vm,gs_tostr(vm,0));
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsCompileString( gsVM* vm )
{
	guard(gsCompileString)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	int ret = gs_compilestring(vm,gs_tostr(vm,0));
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsDoFile( gsVM* vm )
{
	guard(gsDoFile)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	int ret = gs_dofile(vm,gs_tostr(vm,0));
	gs_pushint(vm,ret);
	return 1;
	unguard()
}

int gsDoString( gsVM* vm )
{
	guard(gsDoString)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,0);
	int ret = gs_dostring(vm,gs_tostr(vm,0));
	gs_pushint(vm,ret);
	return 1;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// print logs
/////////////////////////////////////////////////////////////////////////////////////////
int gsPrint( gsVM* vm )
{
	guard(gsPrint)
	for(int i=0;i<gs_top(vm);i++) gs_logobj(gs_get(vm,i));
	return 0;
	unguard()
}
int gsPrintln( gsVM* vm )
{
	guard(gsPrintln)
	for(int i=0;i<gs_top(vm);i++) gs_logobj(gs_get(vm,i));
	gslog("\n");
	return 0;
	unguard()
}

int gsScanln( gsVM* vm )
{
	guard(gsScanln);
	char input[256]; input[0]=0;
	gets_s(input, 256);
	input[255]=0;
	gs_pushstr(vm,input);
	return 1;
	unguard();
}

int gsGetch( gsVM* vm )
{
	guard(gsGetch);
	int ch = (int)_getch();
	if(ch==0 || ch==0xe0)
	{
		ch = (int)_getch();
		gs_pushint(vm,ch<<8);
	}
	else
		gs_pushint(vm,ch);
	return 1;
	unguard();
}
/////////////////////////////////////////////////////////////////////////////////////////
// variables and functions handlers IDDs
/////////////////////////////////////////////////////////////////////////////////////////
int gsGid( gsVM* vm )
{
	guard(gsGid)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,-1);
	int gid = vm->m_env->FindVar(gs_tostr(vm,0));
	gs_pushint(vm,gid);
	return 1;
	unguard()
}

int gsFid( gsVM* vm )
{
	guard(gsFid)
	if(!gs_cktype(vm,0,GS_STR)) GS_RETURNINT(vm,-1);
	int fid = vm->m_env->FindFn(gs_tostr(vm,0));
	gs_pushint(vm,fid);
	return 1;
	unguard()
}

int gsGidName( gsVM* vm )
{
	guard(gsGidName)
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNSTR(vm,NULL);
	gsVar* var = vm->m_env->GetVar(gs_toint(vm,0));
	if(var!=NULL)	gs_pushstr(vm,var->m_name);
	else			gs_pushstr(vm,NULL);
	return 1;
	unguard()
}

int gsFidName( gsVM* vm )
{
	guard(gsFidName)
	if(!gs_cktype(vm,0,GS_INT)) GS_RETURNSTR(vm,NULL);
	gsFn* fn = vm->m_env->GetFn(gs_toint(vm,0));
	if(fn!=NULL)	gs_pushstr(vm,fn->m_name);
	else			gs_pushstr(vm,NULL);
	return 1;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// table functions
/////////////////////////////////////////////////////////////////////////////////////////
int gsTabSize( gsVM* vm )
{
	guard(gsTabSize)
	if(!gs_ckreftype(vm,0,GS_TAB)) GS_RETURNINT(vm,0);
	gs_pushint(vm, gs_get(vm,0).o->size);
	return 1;
	unguard()
}

int gsTabResize( gsVM* vm )
{
	guard(gsTabSize)
	if(!gs_ckreftype(vm,0,GS_TAB)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	gsObj* o = gs_get(vm,0).o;
	int size = gs_toint(vm,1);
	gso_tabresize(*o,size);
	return 0;
	unguard()
}

int gsTabDel( gsVM* vm )
{
	guard(gsTabDel)
	if(!gs_ckreftype(vm,0,GS_TAB)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_INT)) return 0;
	gsObj* o = gs_get(vm,0).o;
	int pos = gs_toint(vm,1);
	int count = gs_toint(vm,2);
	gso_tabdel(*o,pos,count);
	return 0;
	unguard()
}

int gsTabIns( gsVM* vm )
{
	guard(gsTabIns)
	if(!gs_ckreftype(vm,0,GS_TAB)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	if(!gs_cktype(vm,2,GS_INT)) return 0;
	gsObj* o = gs_get(vm,0).o;
	int pos = gs_toint(vm,1);
	int count = gs_toint(vm,2);
	gso_tabins(*o,pos,count);
	return 0;
	unguard()
}

int gsTabAdd( gsVM* vm )
{
	guard(gsTabAdd)
	if(!gs_ckreftype(vm,0,GS_TAB)) return 0;
	if(!gs_cktype(vm,1,GS_INT)) return 0;
	gsObj* o = gs_get(vm,0).o;
	int count = gs_toint(vm,1);
	gso_tabadd(*o,count);
	return 0;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// string functions
/////////////////////////////////////////////////////////////////////////////////////////
/*
other suggestions:

int str2int(str)
str int2str(int)
flt str2flt(str)
str flt2str(flt)
str chr2str(int)

 PHP:
str strreplace(s1,s2,s3)		replace s2 with s3 in s1
str strireplace(s1,s2,s3)		replace s2 with s3 in s1
str strrepeat(s1,count)			repeats s1 count times
int stripos

str = sprintf(format,...)
str = strftime( str format, int time );
*/


int gsStrEmpty( gsVM* vm )
{
	guard(gsStrEmpty)
	gsObj o;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,1);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,1);
	if(gs_isref(vm,0))	o = *gs_get(vm,0).o; else o = gs_get(vm,0);
	if(o.s==NULL || o.s[0]==0)	gs_pushint(vm,1);
	else						gs_pushint(vm,0);
	return 1;
	unguard()
}

int gsStrLen( gsVM* vm )
{
	guard(gsStrLen)
	gsObj o;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,0);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,0);
	if(gs_isref(vm,0))	o = *gs_get(vm,0).o; else o = gs_get(vm,0);
	if(o.s==NULL) GS_RETURNINT(vm,0);
	gs_pushint(vm,gsstrlen(o.s));
	return 1;
	unguard()
}
	
int gsStrPos( gsVM* vm )
{
	guard(gsStrPos)
	int start=0;
	gsObj o1, o2;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,-1);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,-1);
	if(gs_isref(vm,1) && !gs_ckreftype(vm,1,GS_STR))	GS_RETURNINT(vm,-1);
	if(!gs_isref(vm,1) && !gs_cktype(vm,1,GS_STR))		GS_RETURNINT(vm,-1);
	if(gs_top(vm)==3 && gs_isint(vm,2)) start = gs_toint(vm,2);
	if(gs_isref(vm,0))	o1 = *gs_get(vm,0).o; else o1 = gs_get(vm,0);
	if(gs_isref(vm,1))	o2 = *gs_get(vm,1).o; else o2 = gs_get(vm,1);

	if(o1.s==NULL || o2.s==NULL) GS_RETURNINT(vm,-1);
	int s1 = gsstrlen(o1.s);
	if(start<0) start=0;
	if(start<s1)
	{
		char* sz = strstr(o1.s+start, o2.s);
		if(sz) { gs_pushint(vm, (int)(intptr)(sz-o1.s)); return 1; }
	}
	GS_RETURNINT(vm,-1);
	unguard()
}

int gsStrChr( gsVM* vm )
{
	guard(gsStrChr)
	int start = 0;
	gsObj o1, o2;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,-1);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,-1);
	if(gs_isref(vm,1) && !gs_ckreftype(vm,1,GS_STR))	GS_RETURNINT(vm,-1);
	if(!gs_isref(vm,1) && !gs_cktype(vm,1,GS_STR))		GS_RETURNINT(vm,-1);
	if(gs_top(vm)==3 && gs_isint(vm,2)) start = gs_toint(vm,2);
	if(gs_isref(vm,0))	o1 = *gs_get(vm,0).o; else o1 = gs_get(vm,0);
	if(gs_isref(vm,1))	o2 = *gs_get(vm,1).o; else o2 = gs_get(vm,1);

	if(o1.s==NULL || o2.s==NULL) GS_RETURNINT(vm,-1);
	int s1 = gsstrlen(o1.s);
	if(start<0) start=0;
	if(start>=s1) GS_RETURNINT(vm,-1);
	
	int pos = (int)strcspn(o1.s+start, o2.s);
	if(start+pos==s1) GS_RETURNINT(vm,-1); // not found
	gs_pushint(vm, start+pos); 
	return 1;
	unguard()
}

int gsStrSkip( gsVM* vm )
{
	guard(gsStrSkip)
	int start = 0;
	gsObj o1, o2;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNINT(vm,-1);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNINT(vm,-1);
	if(gs_isref(vm,1) && !gs_ckreftype(vm,1,GS_STR))	GS_RETURNINT(vm,-1);
	if(!gs_isref(vm,1) && !gs_cktype(vm,1,GS_STR))		GS_RETURNINT(vm,-1);
	if(gs_top(vm)==3 && gs_isint(vm,2)) start = gs_toint(vm,2);
	if(gs_isref(vm,0))	o1 = *gs_get(vm,0).o; else o1 = gs_get(vm,0);
	if(gs_isref(vm,1))	o2 = *gs_get(vm,1).o; else o2 = gs_get(vm,1);

	if(o1.s==NULL || o2.s==NULL) GS_RETURNINT(vm,-1);
	int s1 = gsstrlen(o1.s);
	if(start<0) start=0;
	if(start>=s1) GS_RETURNINT(vm,s1); // skip all
	
	while(start<s1 && strchr(o2.s, o1.s[start])!=NULL )	start++;
	gs_pushint(vm, start);
	return 1;
	unguard()
}

int gsStrLwr( gsVM* vm )
{
	guard(gsStrLwr)
	gsObj o;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNSTR(vm,NULL);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNSTR(vm,NULL);
	if(gs_isref(vm,0))	o = *gs_get(vm,0).o; else o = gs_get(vm,0);
	gsObj o2 = gso_dup(o);
	if(o2.s!=NULL) strlwr(o2.s);
	gs_push(vm,o2); 
	return 1;
	unguard()
}

int gsStrUpr( gsVM* vm )
{
	guard(gsStrUpr)
	gsObj o;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNSTR(vm,NULL);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNSTR(vm,NULL);
	if(gs_isref(vm,0))	o = *gs_get(vm,0).o; else o = gs_get(vm,0);
	gsObj o2 = gso_dup(o);
	if(o2.s!=NULL) strupr(o2.s);
	gs_push(vm,o2); 
	return 1;
	unguard()
}

int gsStrSub( gsVM* vm )
{
	guard(gsStrSub)
	gsObj o;
	int pos = 0;
	int count = -1; // copy all to the end
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNSTR(vm,NULL);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNSTR(vm,NULL);
	if(!gs_cktype(vm,1,GS_INT))							GS_RETURNSTR(vm,NULL);
	if(gs_isref(vm,0))	o = *gs_get(vm,0).o; else o = gs_get(vm,0);
	if(o.s==NULL) GS_RETURNSTR(vm,NULL);
	int s = gsstrlen(o.s);
	pos = gs_toint(vm,1);
	if(pos<0 || pos>=s) GS_RETURNSTR(vm,NULL);
	if(gs_top(vm)==3 && gs_isint(vm,2)) count = gs_toint(vm,2);
	if(count==0) GS_RETURNSTR(vm,"");
	if(count<0 || pos+count>s) count = s-pos;

	gsObj o2 = gso_strnew(count+1); // inc eos
	if(count>0) 
	{ 
		strncpy(o2.s, o.s+pos, count); 
		o2.s[count]=0; 
	}
	gs_push(vm,o2);
	return 1;
	unguard()
}

int gsStrTrim( gsVM* vm )
{
	guard(gsStrTrim)
	gsObj o1, o2;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNSTR(vm,NULL);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNSTR(vm,NULL);
	if(gs_isref(vm,1) && !gs_ckreftype(vm,1,GS_STR))	GS_RETURNSTR(vm,NULL);
	if(!gs_isref(vm,1) && !gs_cktype(vm,1,GS_STR))		GS_RETURNSTR(vm,NULL);
	if(gs_isref(vm,0))	o1 = *gs_get(vm,0).o; else o1 = gs_get(vm,0);
	if(gs_isref(vm,1))	o2 = *gs_get(vm,1).o; else o2 = gs_get(vm,1);
	if(o1.s==NULL) GS_RETURNSTR(vm,NULL);
	if(o2.s==NULL) GS_RETURNSTR(vm,o1.s); // keep the same
	int c1=0; int c2=gsstrlen(o1.s)-1;
	
	while(c1<=c2)
	{
		BOOL done = TRUE;
		if(strchr(o2.s, o1.s[c1])!=NULL) { c1++; done=FALSE; }
		if(strchr(o2.s, o1.s[c2])!=NULL) { c2--; done=FALSE; }
		if(done) break;
	}

	if(c1>c2) GS_RETURNSTR(vm,""); // empty
	gsObj o3 = gso_strnew(c2-c1+1+1); // inc eos
	if(c2-c1+1>0)
	{
		strncpy(o3.s,o1.s+c1, c2-c1+1);
		o3.s[c2-c1+1]=0;
	}

	gs_push(vm, o3);
	return 1;
	unguard()
}

int gsStrExplode( gsVM* vm )
{
	guard(gsStrExplode)
	gsObj o;
	int chr;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_STR))	GS_RETURNTAB(vm,0);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_STR))		GS_RETURNTAB(vm,0);
	if(!gs_cktype(vm,1,GS_INT))							GS_RETURNTAB(vm,0);
	if(gs_isref(vm,0))	o = *gs_get(vm,0).o; else o = gs_get(vm,0);
	if(o.s==NULL) GS_RETURNTAB(vm,0);
	chr = gs_toint(vm,1);

	gsObj o2 = gso_tabnew(0);
	int s = gsstrlen(o.s);
	int j=0;
	for(int i=0; i<s; i++)
	{
		if(o.s[i]==chr)
		{
			gsObj o3 = gso_strnew(i-j+1); // inc eos
			if(i-j>0) 
			{
				strncpy(o3.s, o.s+j, i-j);
				o3.s[i-j]=0;
			}
			gso_tabadd(o2,1);
			o2.o[o2.size-1]=o3;
			j=i+1;
		}
	}
	if(j<=s)
	{
		gsObj o3 = gso_strnew(s-j+1); // inc eos
		if(s-j>0) 
		{
			strncpy(o3.s, o.s+j, s-j);
			o3.s[s-j]=0;
		}
		gso_tabadd(o2,1);
		o2.o[o2.size-1]=o3;
	}
	
	gs_push(vm,o2);
	return 1;
	unguard()
}

int gsStrImplode( gsVM* vm )
{
	guard(gsStrImplode)
	gsObj o;
	int chr;
	if(gs_isref(vm,0) && !gs_ckreftype(vm,0,GS_TAB))	GS_RETURNSTR(vm,NULL);
	if(!gs_isref(vm,0) && !gs_cktype(vm,0,GS_TAB))		GS_RETURNSTR(vm,NULL);
	if(!gs_cktype(vm,1,GS_INT))							GS_RETURNSTR(vm,NULL);
	if(gs_isref(vm,0))	o = *gs_get(vm,0).o; else o = gs_get(vm,0);
	chr = gs_toint(vm,1);
	
	int i;
	// get count
	int count=0;
	for(i=0;i<o.size;i++)
	{
		if(o.o[i].type==GS_STR && o.o[i].s!=NULL) count+=gsstrlen(o.o[i].s);
		if(chr!=0 && i<o.size-1) count++;
	}

	// create str
	int j=0;
	gsObj o2=gso_strnew(count+1);
	for(i=0;i<o.size;i++)
	{
		if(o.o[i].type==GS_STR && o.o[i].s!=NULL) 
		{
			strcpy(o2.s+j,o.o[i].s);
			j+=gsstrlen(o.o[i].s);
		}
		if(chr!=0 && i<o.size-1) 
		{
			o2.s[j]=chr;
			j++;
		}
	}
	o2.s[j]=0; //eos

	gs_push(vm,o2);
	return 1;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// system
/////////////////////////////////////////////////////////////////////////////////////////
int gsTime( gsVM* vm )
{
	guard(gsTime)
	gs_pushint(vm,(int)GetTickCount());
	return 1;
	unguard()
}

int gsSystemTime( gsVM* vm )
{
	guard(gsSystemTime)
	if(!gs_ckreftype(vm,0,GS_INT)) return 0;
	if(!gs_ckreftype(vm,1,GS_INT)) return 0;
	if(!gs_ckreftype(vm,2,GS_INT)) return 0;

	int local = 0;
	if( gs_params(vm)==4 && gs_cktype(vm,3,GS_INT) )
		local = gs_toint(vm,3);
	SYSTEMTIME time;
	GetLocalTime( &time );
	gs_get(vm,0).o->i = time.wHour;
	gs_get(vm,1).o->i = time.wMinute;
	gs_get(vm,2).o->i = time.wSecond;
	return 0;
	unguard()
}

int gsSystemDate( gsVM* vm )
{
	guard(gsSystemDate)
	if(!gs_ckreftype(vm,0,GS_INT)) return 0;
	if(!gs_ckreftype(vm,1,GS_INT)) return 0;
	if(!gs_ckreftype(vm,2,GS_INT)) return 0;
	SYSTEMTIME time;
	GetLocalTime( &time );
	gs_get(vm,0).o->i = time.wYear;
	gs_get(vm,1).o->i = time.wMonth;
	gs_get(vm,2).o->i = time.wDay;
	return 0;
	unguard()
}

int gsRand( gsVM* vm )
{
	guard(gsRand)
	int range=100;
	if(gs_params(vm)==1 && gs_cktype(vm,0,GS_INT)) range = gs_toint(vm,0);
	gs_pushint(vm,rand()%range);
	return 1;
	unguard()
}

int gsFRand( gsVM* vm )
{
	guard(gsFRand)
	float range=1.0f;
	if(gs_params(vm)==1 && gs_cktype(vm,0,GS_FLT)) range = gs_toflt(vm,0);
	gs_pushflt(vm, ( (float)(rand())/(float)RAND_MAX ) * range );
	return 1;
	unguard()
}

int gsSRand( gsVM* vm )
{
	guard(gsRand)
	int gen=GetTickCount();
	if(gs_params(vm)==1 && gs_cktype(vm,0,GS_INT)) gen = gs_toint(vm,0);
	srand(gen);
	return 0;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// register lib
/////////////////////////////////////////////////////////////////////////////////////////
int gslib_default( gsVM* vm )
{
	guard(gslib_default)
	
	// errors
	gs_regfn( vm, "gs_error",			gsError );			// [str(error info)]
	gs_regfn( vm, "gs_debug",			gsDebug );			// int(debug level)

	// compile and do
	gs_regfn( vm, "gs_compilefile",		gsCompileFile );	// str(filename) > int(1/0)
	gs_regfn( vm, "gs_compilestring",	gsCompileString );	// str(code) > int(1/0)
	gs_regfn( vm, "gs_dofile",			gsDoFile );			// str(filename) > int(1/0)
	gs_regfn( vm, "gs_dostring",		gsDoString );		// str(code) > int(1/0)

	// print and scan
	gs_regfn( vm, "print",				gsPrint );			// obj, ...
	gs_regfn( vm, "println",			gsPrintln );		// obj, ...
	gs_regfn( vm, "scanln",				gsScanln );			// > str(input line)
	gs_regfn( vm, "getch",				gsGetch );			// > int(key char code)
									
	// handlers						
	gs_regfn( vm, "gs_gid",				gsGid );			// str(global name) > int(id/-1)
	gs_regfn( vm, "gs_fid",				gsFid );			// str(func name) > int(id/-1)
	gs_regfn( vm, "gs_gidname",			gsGidName );		// int(id) > str(global name/NULL)
	gs_regfn( vm, "gs_fidname",			gsFidName );		// int(id) > str(funct name/NULL)
									
	// table						
	gs_regfn( vm, "tabsize",			gsTabSize );		// ref_tab > int(size/0)
	gs_regfn( vm, "tabresize",			gsTabResize );		// ref_tab, int(size)
	gs_regfn( vm, "tabdel",				gsTabDel );			// ref_tab, int(pos), int(count)
	gs_regfn( vm, "tabins",				gsTabIns );			// ref_tab, int(pos), int(count)
	gs_regfn( vm, "tabadd",				gsTabAdd );			// ref_tab, int(count)
									
	// string						
	gs_regfn( vm, "strempty",			gsStrEmpty );		// ref_str|str > int(1/0)
	gs_regfn( vm, "strlen",				gsStrLen );			// ref_str|str > int(chars count eos excluded/0)
	gs_regfn( vm, "strpos",				gsStrPos );			// ref_str|str, ref_str|str [,int(start pos)] > int(pos/-1)
	gs_regfn( vm, "strchr",				gsStrChr );			// ref_str|str, ref_str|str [,int(start pos)] > int(pos/-1)
	gs_regfn( vm, "strskip",			gsStrSkip );		// ref_str|str, ref_str|str [,int(start pos)] > int(pos/-1)
	gs_regfn( vm, "strlwr",				gsStrLwr );			// ref_str|str > str(lowered/NULL)
	gs_regfn( vm, "strupr",				gsStrUpr );			// ref_str|str > str(uppered/NULL)
	gs_regfn( vm, "strsub",				gsStrSub );			// ref_str|str, int(start pos) [,int(count)] > str(substring/NULL)
	gs_regfn( vm, "strtrim",			gsStrTrim );		// ref_str|str, ref_str/str > str(trimmed/NULL)
	gs_regfn( vm, "strexplode",			gsStrExplode );		// ref_str|str, int(separator char) > tab(/EMPTY)
	gs_regfn( vm, "strimplode",			gsStrImplode );		// ref_tab|tab, int(separator char) > str(/NULL)
	
	// system
	gs_regfn( vm, "gs_time",			gsTime );			// > int(tickcount in ms)
	gs_regfn( vm, "gs_systemtime",		gsSystemTime );		// ref_int(hour), ref_int(minute), ref_int(second)
	gs_regfn( vm, "gs_systemdate",		gsSystemDate );		// ref_int(year), ref_int(month), ref_int(day)
	gs_regfn( vm, "gs_rand",			gsRand );			// [int(range)] > int; default in [0..100)
	gs_regfn( vm, "gs_frand",			gsFRand );			// [flt(range)] > flt; default in [0..1)
	gs_regfn( vm, "gs_srand",			gsSRand );			// [int(generator start)]; default tickcount

	return 0;	

	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
