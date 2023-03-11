#include "stdafx.h"
#include "gs.h"
#include "gslib_default.h"

/////////////////////////////////////////////////////////////////////////////////////////
// CREATION
/////////////////////////////////////////////////////////////////////////////////////////
gsVM* gs_init( int stacksize )
{
	guard(gs_init)
	gsEnv* env = gs_initenv();
	gsVM* vm = gs_initvm(env,stacksize);

	// register
	gslib_default(vm); // register default lib

	return vm;
	unguard()
}

void gs_done( gsVM* vm )
{
	guard(gs_done)
	if(vm==NULL) return;
	if(vm->m_env) { gs_doneenv(vm->m_env); vm->m_env=NULL; }
	gs_donevm(vm);
	unguard()
}

gsEnv* gs_initenv()
{
	guard(gs_initenv)
	gsEnv* env = gsnew gsEnv();
	env->Init();
	return env;
	unguard()
}

void gs_doneenv( gsEnv* env )
{
	guard(gs_doneenv)
	if(env==NULL) return;
	env->Done();
	gsdelete(env);
	unguard()
}

gsVM* gs_initvm( gsEnv* env, int stacksize )
{
	guard(gs_initvm)
	gsVM* vm = gsnew gsVM();
	vm->Init( stacksize );
	vm->m_env = env;
	return vm;
	unguard()
}

void gs_donevm( gsVM* vm )
{
	guard(gs_donevm)
	if(vm==NULL) return;
	vm->Done();
	gsdelete(vm);
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// STACK
/////////////////////////////////////////////////////////////////////////////////////////
void gs_remove( gsVM* vm, int idx )
{
	guard(gs_remove)
	gsassert(idx>=0 && idx<gs_top(vm));
	int i;
	for(i=vm->m_sb+idx;i<vm->m_sp-1;i++)
	{
		vm->m_stack[i] = vm->m_stack[i+1];
	}
	vm->m_sp--;
	unguard()
}

void gs_insert( gsVM* vm, int idx, gsObj& obj )
{
	guard(gs_insert)
	gsassert(vm->m_sp<vm->m_ss);
	gsassert(idx>=0 && idx<gs_top(vm));
	vm->m_sp++;
	int i;
	for(i=vm->m_sp-1; i>vm->m_sb+idx; i--)
	{
		vm->m_stack[i] = vm->m_stack[i-1];
	}
	vm->m_stack[i] = obj;
	unguard()
}

const char* gs_typename( gsVM* vm, int type )
{
	guard(gs_typename)
	gsassert(type>=0 && type<7);
	static char* name[7] = { "NUL", "INT", "FLT", "STR", "TAB", "REF", "PTR" };
	return name[type];
	unguard()
}

BOOL gs_cktype( gsVM* vm, int idx, int type )
{
	guard(gs_cktype)
	gsObj o = vm->Get(vm->m_sb+idx);
	if(o.type==type) return TRUE;
	gs_error(vm, GSE_BADTYPE, type, o.type);
	return FALSE;
	unguard()
}

BOOL gs_ckreftype( gsVM* vm, int idx, int type )
{
	guard(gs_ckreftype)
	gsObj o = vm->Get(vm->m_sb+idx);
	if(o.type!=GS_REF)
	{
		gs_error(vm, GSE_BADTYPE, GS_REF, o.type);
		return FALSE;
	}
	if(o.o==NULL) 
	{
		gs_error(vm, GSE_NULLREF);
		return FALSE;
	}
	if(o.o->type==type) return TRUE;
	gs_error(vm, GSE_BADTYPE, type, o.o->type);
	return FALSE;
	unguard()
}

void gs_pushtab( gsVM* vm, int size )
{
	guard(gs_pushtab)
	gsObj& t = gso_tabnew(size);
	while(size>0)
	{
		size--;
		t.o[size] = vm->Pop();
	}
	vm->Push(t);
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// COMPILE AND DO
/////////////////////////////////////////////////////////////////////////////////////////
int	gs_parse( gsVM* vm, char* buffer, int size, gsNode* &tree, char* file )
{
	guard(gs_parse)
	gsassert(vm!=NULL);
	gsassert(buffer!=NULL && size>=2);
	gsassert(buffer[size-2]==0 && buffer[size-1]==0); // parser need

	YY_BUFFER_STATE yybs;
	yybs = YYScanBuffer(buffer, size);
	tree = YYParse();
	YYDeleteBuffer(yybs);
	
	if(YYError())
	{
		vm->m_errhandler( vm, gsErrInfo(GSE_PARSER, 0, 0, YYErrorText(), YYErrorLine(), file) );
		return GSE_PARSER;
	}

	return GS_OK;

	unguard()
}

int	gs_compilefile( gsVM* vm, char* file )
{
	guard(gs_compilefile)
	gsassert(vm!=NULL);
	if(file==NULL || file[0]==0) return GS_OK;

	int size = 0;
	char* buffer = NULL;
	void* f = gsfile_open(file);
	if(f==NULL)
	{
		gs_error(vm, GSE_FILEERROR, 0, 0, file);
		return GSE_FILEERROR;
	}
	gsfile_seek(f,0,2); size = gsfile_tell(f); gsfile_seek(f,0,0);
	buffer = (char*)gsmalloc(size+2); 
	buffer[size]=0; buffer[size+1]=0; // parser need
	int ret = gsfile_read(buffer,size,f);
	gsfile_close(f);
	if(ret!=size) 
	{
		gsfree(buffer);
		gs_error(vm, GSE_FILEERROR, 0, 0, file);
		return(GSE_FILEERROR);
	}
	ret = gs_compilebuffer( vm, buffer, size+2, file );
	gsfree(buffer);
	return ret;
	unguard()
}

int	gs_compilestring( gsVM* vm, char* sz )
{
	guard(gs_compilestring)
	gsassert(vm!=NULL);
	int size = gsstrlen(sz);
	if(size==0) return GS_OK;

	char* buffer = (char*)gsmalloc(size+2);
	buffer[size]=0; buffer[size+1]=0; // parser need
	strcpy(buffer,sz);
	int ret = gs_compilebuffer( vm, buffer, size+2 );
	gsfree(buffer);
	return ret;
	unguard()
}

int	gs_compilebuffer( gsVM* vm, char* buffer, int size, char* file )
{
	guard(gs_compilebuffer)
	gsassert(vm!=NULL);
	gsassert(vm->m_env!=NULL);
	if(buffer==NULL || size==0) return GS_OK;
	int ret;

	// parser ................
	gsNode* tree;
	ret = gs_parse( vm, buffer, size, tree, file );
	if(ret!=GS_OK) return ret;
	if(tree==NULL) return GS_OK;

	// compiler ................
	// gslog("LOGTREE: %s\n",file);tree->LogTree(); // debug
	gsCompiler* compiler = gsnew gsCompiler();
	compiler->m_file = file;
	ret = compiler->CompileProgram( vm, tree );
	gsdelete(compiler);
	gsNode::DestroyTree(tree);
	return ret;
	unguard();
}

int	gs_dofile( gsVM* vm, char* file )
{
	guard(gs_dofile)
	gsassert(vm!=NULL);
	if(file==NULL || file[0]==0) return GS_OK;

	int size = 0;
	char* buffer = NULL;
	void* f = gsfile_open(file);
	if(f==NULL) 
	{
		gs_error(vm, GSE_FILEERROR, 0, 0, file);
		return GSE_FILEERROR;
	}
	gsfile_seek(f,0,2); size = gsfile_tell(f); gsfile_seek(f,0,0);
	buffer = (char*)gsmalloc(size+2); 
	buffer[size]=0; buffer[size+1]=0; // parser need
	int ret = gsfile_read(buffer,size,f);
	gsfile_close(f);
	if(ret!=size) 
	{
		gsfree(buffer);
		gs_error(vm, GSE_FILEERROR, 0, 0, file);
		return(GSE_FILEERROR);
	}
	ret = gs_dobuffer( vm, buffer, size+2, file );
	gsfree(buffer);
	return ret;	
	unguard()
}

int	gs_dostring( gsVM* vm, char* sz )
{
	guard(gs_dostring)
	gsassert(vm!=NULL);
	int size = gsstrlen(sz);
	if(size==0) return GS_OK;

	char* buffer = (char*)gsmalloc(size+2);
	buffer[size]=0; buffer[size+1]=0; // parser need
	strcpy(buffer,sz);
	int ret = gs_dobuffer( vm, buffer, size+2 );
	gsfree(buffer);
	return ret;
	unguard()
}

int	gs_dobuffer( gsVM* vm, char* buffer, int size, char* file )
{
	guard(gs_dobuffer)
	gsassert(vm!=NULL);
	gsassert(vm->m_env!=NULL);
	if(buffer==NULL || size==0) return GS_OK;
	int ret;

	// parser ................
	gsNode* tree;
	ret = gs_parse( vm, buffer, size, tree, file );
	if(ret!=GS_OK) return ret;
	if(tree==NULL) return GS_OK;
	
	// compiler ................
	// gslog("LOGTREE:\n");GSTreeLog(tree); // debug
	int fn = -1;
	gsCompiler* compiler = gsnew gsCompiler();
	ret = compiler->CompileFunction( vm, tree, fn );
	gsdelete(compiler);
	gsNode::DestroyTree(tree);

	if(ret!=GS_OK || fn==-1) return ret;

	// run ................
	ret = gs_runfn(vm,fn,0);
	gs_pop(vm); // always something is returned
	vm->m_env->DelFn(fn); // delete temporary function

	return ret;
	unguard()
}


/////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS AND RUN
/////////////////////////////////////////////////////////////////////////////////////////
int	gs_regfn( gsVM* vm, char* name, gsFunc func )
{
	guard(gs_regfn)
	gsassert(name!=NULL);
	gsassert(vm->m_env!=NULL);
	int i = vm->m_env->AddFn(name);
	if( i==-1 ) return -1;
	gsFn* fn=vm->m_env->GetFn(i);
	fn->m_flags = GSFN_EXPORTED;
	fn->m_func = func;
	return i;
	unguard()
}

int	gs_runfn( gsVM* vm, int fp, int params )
{ 
	guard(gs_runfn)
	int ret;
	gsassert(vm!=NULL);
	gsassert(vm->m_env!=NULL);
	gsFn* fn = vm->m_env->GetFn(fp);
	gsassert(fn!=NULL);

	if(fn->m_flags & GSFN_EXPORTED) 
	{
		ret = vm->CallC(fp,params); // always pushes one returned value
	}
	else
	{
		int tcb = vm->m_cb;		// store callstack base
		vm->m_cb = vm->m_cp;	// set callback base to force return on this level

		ret = vm->CallS(fp, params); // try to call (may fail if wrong params)
		if(ret==GS_OK) // if ok, run it
		{		
			ret = vm->Run();
			if(ret!=GS_OK)
			{
				gs_recover(vm); // recover callstack
			}
		}
	
		if(ret!=GS_OK) vm->Push(gsNUL); // we need to leave always push one returned value 
		vm->m_cb = tcb; // restore callstack base
	}
	
	return ret;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// GLOBALS
/////////////////////////////////////////////////////////////////////////////////////////
int	gs_regint( gsVM* vm, char* name, int value )
{
	guard(gs_regint)
	gsassert(name!=NULL);
	gsassert(vm->m_env!=NULL);
	int i=vm->m_env->AddVar(name); if(i==-1) return -1;
	gsVar* var = vm->m_env->GetVar(i);
	var->m_obj.type = GS_INT;
	var->m_obj.i = value;
	return i;
	unguard()
}

int	gs_regflt( gsVM* vm, char* name, float value )
{
	guard(gs_regflt)
	gsassert(name!=NULL);
	gsassert(vm->m_env!=NULL);
	int i=vm->m_env->AddVar(name); if(i==-1) return -1;
	gsVar* var = vm->m_env->GetVar(i);
	var->m_obj.type = GS_FLT;
	var->m_obj.f = value;
	return i;
	unguard()
}

int	gs_regstr( gsVM* vm, char* name, char* value )
{
	guard(gs_regstr)
	gsassert(name!=NULL);
	gsassert(vm->m_env!=NULL);
	int i=vm->m_env->AddVar(name); if(i==-1) return -1;
	gsVar* var = vm->m_env->GetVar(i);
	var->m_obj.type = GS_STR;
	var->m_obj.s = gsstrdup(value);
	return i;
	unguard()
}

int	gs_regref( gsVM* vm, char* name, gsObj* value )
{
	guard(gs_regref)
	gsassert(name!=NULL);
	gsassert(vm->m_env!=NULL);
	int i=vm->m_env->AddVar(name); if(i==-1) return -1;
	gsVar* var = vm->m_env->GetVar(i);
	var->m_obj.type = GS_REF;
	var->m_obj.o = value;
	return i;
	unguard()
}

int	gs_regptr( gsVM* vm, char* name, void* value )
{
	guard(gs_regref)
	gsassert(name!=NULL);
	gsassert(vm->m_env!=NULL);
	int i=vm->m_env->AddVar(name); if(i==-1) return -1;
	gsVar* var = vm->m_env->GetVar(i);
	var->m_obj.type = GS_PTR;
	var->m_obj.p = value;
	return i;
	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG HELPERS
/////////////////////////////////////////////////////////////////////////////////////////
void gs_logcall(gsVM* vm)
{
	guard(gs_logcall)

	gsEnv* env = vm?vm->m_env:NULL;
	if(vm==NULL || env==NULL) return;
	if(vm->m_fp==-1) return;

	gslog("CALLSTACK:  ");

	// call stack
	int fp = vm->m_fp;
	int pp = vm->m_pp;

	for(int i=vm->m_cp; i>=0; i--) // for all callstack entries + current
	{
		if(i<vm->m_cp) // next level
		{
			fp = vm->m_call[i].m_fp;
			pp = vm->m_call[i].m_pp;
		}

		char* szfn = NULL;
		if( env->GetFn(fp) )	szfn = env->GetFn(fp)->m_name;
		if( szfn )	gslog( "%s:%i",szfn,env->GetLineInfo(fp,pp) );
		else		gslog( "." );
		if(i>0) gslog(" < ");
	}
	gslog("\n");
	
	unguard()
}

void gs_logcallex(gsVM* vm)
{
	guard(gs_logcallex)

	int i,j;
	gsEnv* env = vm?vm->m_env:NULL;
	if(vm==NULL || env==NULL) return;
	if(vm->m_fp==-1) return;
	
	int fp = vm->m_fp;
	int pp = vm->m_pp;
	int sb = vm->m_sb;
	int sp = vm->m_sp;

	for(i=vm->m_cp; i>=0; i--) // for all callstack entries + current
	{
		if(i<vm->m_cp) // next level
		{
			fp = vm->m_call[i].m_fp;
			pp = vm->m_call[i].m_pp;
			sb = vm->m_call[i].m_sb;
			sp = vm->m_call[i].m_sp;
		}

		char* szfn = NULL;
		char* szfile = NULL;
		if( env->GetFn(fp) )	szfn = env->GetFn(fp)->m_name;
		if( env->GetFn(fp) )	szfile = env->GetFn(fp)->m_file;

		gslog(	"===================================================================\n");
		gslog(	"CALLSTACK LEVEL %i\n", i );
		gslog(	"func=%s (#%i), addr=%i, stack=%i, line=%i, file=\"%s\"\n",
				szfn ? szfn : "unknown",
				fp,
				pp,
				sp-sb,
				env->GetLineInfo(fp,pp),
				szfile? szfile : "unknown"
				);

		if( (vm->m_debug & GSDBG_CODE) && fp!=-1 )
		{
			gslog(	"...................................................................\n");
			env->LogFn(fp);
		}

		if( (vm->m_debug & GSDBG_STACK) && sp-sb!=0 )
		{
			gslog(	"...................................................................\n");
			gslog(	"base=%i, top=%i\n", sb, sp );
			for(j=0;j<sp-sb;j++)
			{
				gslog("%03i ",j);
				gs_logobj(vm->m_stack[sb+j]);
				gslog("\n");
			}
		}
	}
	
	gslog(	"===================================================================\n\n");

	unguard()
}

void gs_cpycall( gsVM* vm, char* buffer, int size )
{
	guard(gs_cpycall)
	if(size<=32 || buffer==NULL) return;
	buffer[0]=0;
	gsEnv* env = vm?vm->m_env:NULL;
	if(vm==NULL || env==NULL) return;
	if(vm->m_fp==-1) return;
	int count = size; // how many chars can we write
	int len;

	// call stack
	int fp = vm->m_fp;
	int pp = vm->m_pp;

	for(int i=vm->m_cp; i>=0; i--) // for all callstack entries + current
	{
		if(i<vm->m_cp) // next level
		{
			fp = vm->m_call[i].m_fp;
			pp = vm->m_call[i].m_pp;
		}

		char* szfn = NULL;
		if( env->GetFn(fp) ) szfn = env->GetFn(fp)->m_name;
		if( szfn )
		{ 
			len=gsstrlen(szfn);
			strncat(buffer,szfn,count);
			count-=len;
			char szt[32];
			_snprintf(szt,32,":%i",env->GetLineInfo(fp,pp));
			szt[31]=0;
			len=gsstrlen(szt);
			strncat(buffer,szt,count);
			count-=len;
		}
		else		
		{
			strncat(buffer,".",count);
			count--;
		}

		if(i>0) { strncat(buffer," < ",count); count-=3; }
		if(count<=0) break;
	}
	
	buffer[size-1]=0;

	unguard()
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

