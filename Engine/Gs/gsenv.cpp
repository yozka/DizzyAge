#include "stdafx.h"
#include "gsenv.h"

/////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION
/////////////////////////////////////////////////////////////////////////////////////////
gsEnv::gsEnv()
{
	guard(gsEnv::gsEnv)
	m_scope		= 0;
	m_fnsize	= 0;
	m_fn		= NULL;
	m_fnhash	= NULL;
	m_varsize	= 0;
	m_var		= NULL;
	m_varhash	= NULL;
	unguard()
}

gsEnv::~gsEnv()
{
	guard(gsEnv::~gsEnv)
	unguard()
}

void gsEnv::Init()
{
	guard(gsEnv::Init)
	m_fnhash = gsnew gsHash();
	m_varhash = gsnew gsHash();
	unguard()
}

void gsEnv::Done()
{
	guard(gsEnv::Done)
	int i;

	// m_fnhash->Report();
	// m_varhash->Report();

	// Fn
	for(i=0;i<m_fnsize;i++)
		if(m_fn[i]!=NULL) DelFn(i);
	if(m_fn) { gsfree(m_fn); m_fn=NULL; }
	if(m_fnhash) { gsdelete(m_fnhash); m_fnhash=NULL; } // hash should be empty anyway
	m_fnsize = 0;

	// Var
	for(i=0;i<m_varsize;i++)
		if(m_var[i]!=NULL) DelVar(i);
	if(m_var) { gsfree(m_var); m_var=NULL; }
	if(m_varhash) { gsdelete(m_varhash); m_varhash=NULL; } // hash should be empty anyway
	m_varsize = 0;
	m_var = NULL;
	m_varhash = NULL;

	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////////////////////////////////
int gsEnv::FindFn( char* name )
{
	guard(gsEnv::FindFn)
	gsassert(name && name[0]);
	int i=-1;
	if( !m_fnhash->Find(name,i) ) return -1; // hash find
	gsassert( i!=-1 && m_fn[i]!=NULL ); // safety test (consistency)
	return i;
	unguard()
}

int gsEnv::AddFn( char* name )
{
	guard(gsEnv::AddFn)
	gsassert(name && name[0]);

	int i = FindFn(name);
	if(i==-1) // new
	{
		// find free position
		for(i=0;i<m_fnsize;i++)
			if(m_fn[i]==NULL) break;
		
		// realloc list
		if(i==m_fnsize)
		{
			m_fnsize++;
			m_fn = (gsFn**)gsrealloc(m_fn,(m_fnsize)*sizeof(gsFn*));
		}
	}
	else
	{
		DelFn(i);
	}

	// create
	gsFn* fn = gsnew gsFn;
	fn->m_name = (char*)gsmalloc(1+gsstrlen(name));
	strcpy(fn->m_name,name);
	fn->m_flags		= GSFN_INVALID;
	fn->m_parsize	= 0;
	fn->m_localsize = 0;
	fn->m_strsize	= 0;
	fn->m_str		= NULL;
	fn->m_codesize	= 0;
	fn->m_code		= NULL;
	fn->m_func		= NULL;
	fn->m_file		= NULL;
	fn->m_linesize	= 0;
	fn->m_line		= NULL;
	fn->m_scope		= m_scope;

	m_fn[i] = fn;

	BOOL ret = m_fnhash->Add(fn->m_name,i); // hash add
	gsassert(ret); // safety

	return i;
	unguard()
}

void gsEnv::DelFn( int idx )
{
	guard(gsEnv::DelFn)
	gsFn* fn=GetFn(idx);
	if(fn==NULL) return;

	BOOL ret = m_fnhash->Del(fn->m_name); // hash delete
	gsassert(ret); // safety

	for(int i=0;i<fn->m_strsize;i++)
	{
		if(fn->m_str[i]) gsfree(fn->m_str[i]);
	}

	if( fn->m_name )	gsfree(fn->m_name);
	if(	fn->m_str )		gsfree(fn->m_str);
	if(	fn->m_code )	gsfree(fn->m_code);
	if( fn->m_file )	gsfree(fn->m_file);
	if( fn->m_line )	gsfree(fn->m_line);

	gsdelete(m_fn[idx]);
	m_fn[idx]=NULL;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// VARIABLES
/////////////////////////////////////////////////////////////////////////////////////////
int gsEnv::FindVar( char* name )
{
	guard(gsEnv::FindVar)
	gsassert(name && name[0]);
	int i=-1;
	if( !m_varhash->Find(name,i) ) return -1; // hash find
	gsassert( i!=-1 && m_var[i]!=NULL ); // safety test (consistency)
	return i;
	unguard()
}

int	gsEnv::AddVar( char* name )
{
	guard(gsEnv::AddVar)
	int i;
	gsassert(name);

	i = FindVar(name);
	if(i==-1) // new
	{
		// find free position
		for(i=0;i<m_varsize;i++)
			if(m_var[i]==NULL) break;
	
		// realloc list
		if(i==m_varsize)
		{
			m_varsize++;
			m_var = (gsVar**)gsrealloc(m_var,(m_varsize)*sizeof(gsVar*));
		}
	}
	else // redefine
	{
		DelVar(i);
	}

	// create
	gsVar* var = gsnew gsVar();
	var->m_name = (char*)gsmalloc(1+gsstrlen(name));
	strcpy(var->m_name,name);
	var->m_flags = 0;
	var->m_obj = gsNUL;
	var->m_scope = m_scope;

	m_var[i] = var;

	BOOL ret = m_varhash->Add(var->m_name,i); // hash add
	gsassert(ret); // safety

	return i;
	unguard()
}

void gsEnv::DelVar( int idx )
{
	guard(gsEnv::DelVar)
	if(m_var[idx]==NULL) return;

	BOOL ret = m_varhash->Del(m_var[idx]->m_name); // hash delete
	gsassert(ret); // safety
	
	// destroy
	if(	m_var[idx]->m_name )	gsfree(m_var[idx]->m_name);
	gso_del(m_var[idx]->m_obj); // delete value

	gsdelete(m_var[idx]);
	m_var[idx]=NULL;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// SCOPE
/////////////////////////////////////////////////////////////////////////////////////////
void gsEnv::DelScope( int scope )
{
	guard(gsEnv::DelScope)
	int i;
	for(i=0;i<m_fnsize;i++)
	{
		if(m_fn[i] && m_fn[i]->m_scope==scope) DelFn(i);
	}
	for(i=0;i<m_varsize;i++)
	{
		if(m_var[i] && m_var[i]->m_scope==scope) DelVar(i);
	}
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// UTILS
/////////////////////////////////////////////////////////////////////////////////////////
int	gsEnv::GetLineInfo( int fp, int pp )
{
	guard(gsEnv::GetLineInfo)
	if( fp<0 || fp>=m_fnsize || m_fn[fp]==NULL ) return -1;
	if( m_fn[fp]->m_codesize==0 ) return -1;
	if( pp<0 || pp>=m_fn[fp]->m_codesize ) return -1;
	if( m_fn[fp]->m_linesize==0 ) return -1;

	int i;
	for(i=0;i<m_fn[fp]->m_linesize-1;i++)
	{
		if( pp < (m_fn[fp]->m_line[i+1] & 0xffff))
		{
			return m_fn[fp]->m_line[i] >> 16;
		}
	}

	return m_fn[fp]->m_line[m_fn[fp]->m_linesize-1] >> 16; // consider last line
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG HELPERS
/////////////////////////////////////////////////////////////////////////////////////////
char* gsEnv::GetCodeText( int fp, int pp )
{
	guard(gsEnv::GetCodeText)
	int i;
	static char sz[128];
	sz[0]=0;

	gsFn* fn=GetFn(fp);
	gsCode& code = fn->m_code[pp];
	char* szop = gs_nameop(code.op);
	if(szop==NULL) 
	{
		strcpy(sz, "<unknown op>");
		return sz;
	}
	sprintf(sz,"%-10s",szop);
	
	char sz2[64];

	switch(code.op)
	{
		case GSOP_PUSHINT:
		case GSOP_PUSHTAB:
		case GSOP_PUSHLOC:
		case GSOP_JMP:
		case GSOP_JZ:
		case GSOP_JNZ:
		case GSOP_RET:		sprintf(sz2,"%i",code.i); strcat(sz,sz2); break;
		case GSOP_PUSHFLT:	sprintf(sz2,"%f",code.i); strcat(sz,sz2); break;
		case GSOP_PUSHSTR:	{
								sprintf(sz2,"%i    ",code.i); strcat(sz,sz2);
								if(0<=code.i && code.i<fn->m_strsize)
								{
									if(fn->m_str[code.i]!=NULL)
									{
										strcat(sz,"\""); 
										i=gsstrlen(sz); strncat(sz,fn->m_str[code.i],64); sz[i+64]=0;
										strcat(sz,"\""); 
									}
									else strcat(sz,"NULL");
								}
								else strcat(sz,"<invalid>");
								break;
							}
		case GSOP_PUSHGLB:	{
								sprintf(sz2,"%i    ",code.i); strcat(sz,sz2);
								if( GetVar(code.i) && GetVar(code.i)->m_name ) 
									strcat(sz,GetVar(code.i)->m_name);
								else
									strcat(sz,"<invalid>"); 
								break;
							}
		case GSOP_CALL:		{
								sprintf(sz2,"%i    ",code.i); strcat(sz,sz2);
								if( pp>0 && fn->m_code[pp-1].op==GSOP_PUSHINT )
								{
									i=fn->m_code[pp-1].i;
									if( GetFn(i) && GetFn(i)->m_name )
										strcat(sz,GetFn(i)->m_name);
									else
										strcat(sz,"<invalid>");
								}
								break;
							}
		case GSOP_CAST:		{
								sprintf(sz2,"%i    ",code.i & 127); strcat(sz,sz2);
								break;
							}
	}
	return sz;
	unguard()
}

BOOL gsEnv::LogVar( int idx )
{
	guard(gsEnv::LogVar)
	if(idx==-1) return FALSE;
	gsVar* var = GetVar(idx);
	if(var==NULL) return FALSE;
	gslog("%3i %s %s = ", idx, gs_nametype(var->m_obj.type), var->m_name);
	gs_logobj(var->m_obj);
	gslog("\n");
	return TRUE;
	unguard()
}

#define LOGENV_SKIP_EXPORTED	// user can config this
BOOL gsEnv::LogFn( int idx )
{
	guard(gsEnv::LogFn)
	int i;
	if(idx==-1) return FALSE;
	gsFn* fn = GetFn(idx); 
	if(fn==NULL) return FALSE;

	#ifdef LOGENV_SKIP_EXPORTED
	if(fn->m_flags & GSFN_EXPORTED) return FALSE;
	#endif

	gslog("  func	 = %i\n", idx);
	gslog("  name    = '%s'\n", fn->m_name);
	gslog("  flags   = %i\n", fn->m_flags);
	gslog("  params  = %i\n", fn->m_parsize);
	gslog("  locals  = %i\n", fn->m_localsize);
	gslog("  strings = %i\n", fn->m_strsize);
	if(fn->m_strsize>0) 
	{
		gslog("  string  :\n");
		for(i=0;i<fn->m_strsize;i++) 
			gslog("  %4i \"%s\"\n",i,fn->m_str[i]);
	}
	gslog("  codes   = %i\n", fn->m_codesize);
	if(fn->m_codesize>0) 
	{
		gslog("  code    :\n");
		for(i=0;i<fn->m_codesize;i++) 
			gslog("  %04i.%3iL  %s\n",i,GetLineInfo(idx,i),GetCodeText(idx,i) );
	}
	gslog("\n");
	return TRUE;
	unguard()
}

void gsEnv::LogGlobals()
{
	guard(gsEnv::LogGlobals)
	gslog("===================================================================\n");
	gslog("LOGGLOBALS: %i variables\n",m_varsize);
	for(int i=0;i<m_varsize;i++) 
	{
		LogVar(i);
	}
	gslog("===================================================================\n\n");
	unguard()
}

void gsEnv::LogEnv()
{
	gslog("===================================================================\n");
	gslog("LOGENV: %i functions\n",m_fnsize);
	for(int i=0;i<m_fnsize;i++) 
	{
		if(LogFn(i))
			gslog(	"...................................................................\n");
	}
	gslog(	"===================================================================\n\n");
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
