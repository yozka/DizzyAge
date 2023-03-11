#include "stdafx.h"
#include "gserror.h"
#include "gscfg.h"
#include "string.h"
#include "gsvm.h"
#include "gsenv.h"
#include "gs.h"

/////////////////////////////////////////////////////////////////////////////////////////
// ERROR HANDLER
/////////////////////////////////////////////////////////////////////////////////////////
void GSErrHandler( gsVM* vm, gsErrInfo& errinfo )
{
	guard(GSErrHandler)

	gsEnv* env = NULL;
	if(vm) env = vm->m_env;
	if(env == NULL)
	{
		gslog( "GS_ERROR: %s - INVALID ENVIROMENT!\n", gs_nameerr(errinfo.m_err) );
		gsassert(FALSE);
		return;
	}

	// general info
	gsFn* fn = env->GetFn(vm->m_fp);
	gslog( "GS_ERROR:   %s, fn=%s, line=%i, file=\"%s\"\n", 
			gs_nameerr(errinfo.m_err),
			(fn && fn->m_name) ? fn->m_name : "unknown",
			errinfo.m_line,
			errinfo.m_file ? errinfo.m_file : "unknown"
			);

	// description log
	if( vm->m_debug & GSDBG_LOGDESC )
	{
		gslog( "GS_DESC:    %i, %i, \"%s\"\n", 
				errinfo.m_param1, 
				errinfo.m_param2, 
				errinfo.m_text ? errinfo.m_text:"" );
		
		if(fn)
		{
			gsCode* code = env->GetCodeAddr(vm->m_fp, vm->m_pp);
			if(code!=NULL)
			{
				char* szop = gs_nameop(code->op);
				gslog("GS_CODE:    %04i %s\n", vm->m_pp, szop ? szop:"unknown");
			}
			gs_logcall(vm);
			gslog("\n");
		}
	}
	
	// call stack extended
	if( vm->m_debug & GSDBG_CALLSTACK )
		gs_logcallex(vm);

	// globals
	if( vm->m_debug & GSDBG_GLOBALS ) 
		env->LogGlobals();
	
	// enviroment
	if( vm->m_debug & GSDBG_FUNCTIONS )
		env->LogEnv();
	
	// halt
	if( vm->m_debug & GSDBG_HALT )
	{
		static char sz[512];
		_snprintf(	sz, 511, 
					"%s, %i, %i, \"%s\"\n%s : %i\n",
					gs_nameerr(errinfo.m_err),
					errinfo.m_param1,
					errinfo.m_param2,
					errinfo.m_text ? errinfo.m_text : "",
					errinfo.m_file ? errinfo.m_file : "code",
					errinfo.m_line );
		int len = gsstrlen(sz);
		gs_cpycall(vm,sz+len,512-len);
		sz[511]=0;
		
		gserrorexit(sz);
	}
	
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG HELPERS
/////////////////////////////////////////////////////////////////////////////////////////
char* gs_nameerr( int err )
{
	guard(gs_nameerr)
	static char* name[GS_MAXERROR] = 
	{
		"NONE",
		"UNKNOWN",
		"FILEERROR",
		"PARSER",
		"UNKNOWNNODE",		
		"REDECLARATION",
		"LABELNOTFOUND",	
		"LABELEXISTS",		
		"UNKNOWNOP",		
		"BADTYPE",		
		"BADREFTYPE",	
		"BADARGTYPE",	
		"BADARGCOUNT",
		"NULLREF",		
		"OUTBOUND",
		"STACKUNDERFLOW",
		"STACKOVERFLOW",
		"UNKNOWNLOC",	
		"UNKNOWNGLB",	
		"UNKNOWNFUNC",	
		"DIVIDEBYZERO",
		"STOPPED",
		"ASSERT",
		"USER",
	};

	if(0<=err && err<GS_MAXERROR) return name[err];

	static char sz[32];
	sprintf( sz, "ERR#%i", err );
	return sz;	
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
