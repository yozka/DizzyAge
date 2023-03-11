#ifndef _GS_
#define _GS_
#include "gscompiler.h"
#include "gsparser.h"

/////////////////////////////////////////////////////////////////////////////////////////
// DEFINES
/////////////////////////////////////////////////////////////////////////////////////////
#define GS_RETURNINT( vm, val )			{ gs_pushint(vm, (int)val);		return 1; }
#define GS_RETURNFLT( vm, val )			{ gs_pushflt(vm, (float)val);	return 1; }
#define GS_RETURNSTR( vm, val )			{ gs_pushstr(vm, (char*)val);	return 1; }
#define GS_RETURNTAB( vm, size )		{ gs_pushtab(vm, (int)size);	return 1; }
#define GS_RETURNREF( vm, obj )			{ gs_pushref(vm, (gsObj*)NULL); return 1; }
#define GS_RETURNPTR( vm, ptr )			{ gs_pushptr(vm, (void*)NULL);	return 1; }

/////////////////////////////////////////////////////////////////////////////////////////
// CREATION
/////////////////////////////////////////////////////////////////////////////////////////
		gsVM*		gs_init			( int stacksize = GS_STACKSIZE );				// create a vm (with a new env); also register the default lib
		void		gs_done			( gsVM* vm );									// destroy a vm and its env
		gsEnv*		gs_initenv		();												// create an env (manual)
		void		gs_doneenv		( gsEnv* env );									// destroy an env (manual)
		gsVM*		gs_initvm		( gsEnv* env, int stacksize = GS_STACKSIZE );	// create a vm and link it to a given enviroment
		void		gs_donevm		( gsVM* vm );									// destroy a vm (don't destroy it's env)

/////////////////////////////////////////////////////////////////////////////////////////
// ERRORS
// Use these to spawn an error
/////////////////////////////////////////////////////////////////////////////////////////
inline	void		gs_error		( gsVM* vm, int err, int param1, int param2, char* text )	{ vm->Error(err,param1,param2,text); }
inline	void		gs_error		( gsVM* vm, int err, int param1=0, int param2=0 )			{ vm->Error(err,param1,param2); }
inline	void		gs_error		( gsVM* vm, int err, char* text )							{ vm->Error(err,text); }
inline	void		gs_setdebug		( gsVM* vm, int flags )										{ vm->m_debug = flags; }
inline	int			gs_getdebug		( gsVM* vm )												{ return vm->m_debug; }

/////////////////////////////////////////////////////////////////////////////////////////
// STACK
// Those work on the current stack level; idx=[0..top-base]
/////////////////////////////////////////////////////////////////////////////////////////
inline	int			gs_stack		( gsVM* vm )									{ return vm->m_ss - vm->m_sp; }		// stack free space; how many objects can I push without causing an overflow
inline	int			gs_top			( gsVM* vm )									{ return vm->m_sp - vm->m_sb; }		// stack size; how many objects are on current stack level
inline	gsObj		gs_get			( gsVM* vm, int idx )							{ return vm->Get(vm->m_sb+idx); }
inline	void		gs_set			( gsVM* vm, int idx, gsObj& obj )				{ vm->Set(vm->m_sb+idx,obj); }
inline	void		gs_del			( gsVM* vm, int idx )							{ gso_del(vm->Get(vm->m_sb+idx)); vm->Set(vm->m_sb+idx,gsNUL); } // destroy (free) an object from the stack; set it to gsNUL
		void		gs_remove		( gsVM* vm, int idx );							// remove an obj (do not free it! use gso_del to do that)
		void		gs_insert		( gsVM* vm, int idx, gsObj& obj );				// insert an obj
inline	void		gs_settop		( gsVM* vm, int idx )							{ vm->m_sp = vm->m_sb+idx; } // force stack top

inline	int			gs_params		( gsVM* vm )									{ return gs_top(vm); } // number of stack elements (acctually parameters for exported functions)
inline	int			gs_type			( gsVM* vm, int idx )							{ return vm->Get(vm->m_sb+idx).type; } // get the type of an object on the stack
		const char*	gs_typename		( gsVM* vm, int type );							// get the name of a type

inline	BOOL		gs_isnul		( gsVM* vm, int idx )							{ return (gs_type(vm,idx)==GS_NUL); }
inline	BOOL		gs_isint		( gsVM* vm, int idx )							{ return (gs_type(vm,idx)==GS_INT); }
inline	BOOL		gs_isflt		( gsVM* vm, int idx )							{ return (gs_type(vm,idx)==GS_FLT); }
inline	BOOL		gs_isstr		( gsVM* vm, int idx )							{ return (gs_type(vm,idx)==GS_STR); }
inline	BOOL		gs_istab		( gsVM* vm, int idx )							{ return (gs_type(vm,idx)==GS_TAB); }
inline	BOOL		gs_isref		( gsVM* vm, int idx )							{ return (gs_type(vm,idx)==GS_REF); }
inline	BOOL		gs_isptr		( gsVM* vm, int idx )							{ return (gs_type(vm,idx)==GS_PTR); }

inline	int			gs_toint		( gsVM* vm, int idx )							{ if(gs_isint(vm,idx)) return vm->Get(vm->m_sb+idx).i; else return 0; }
inline	float		gs_toflt		( gsVM* vm, int idx )							{ if(gs_isflt(vm,idx)) return vm->Get(vm->m_sb+idx).f; else return 0.0f; }
inline	char*		gs_tostr		( gsVM* vm, int idx )							{ if(gs_isstr(vm,idx)) return vm->Get(vm->m_sb+idx).s; else return NULL; }
inline	gsObj*		gs_toref		( gsVM* vm, int idx )							{ if(gs_isref(vm,idx)) return vm->Get(vm->m_sb+idx).o; else return NULL; }
inline	void*		gs_toptr		( gsVM* vm, int idx )							{ if(gs_isptr(vm,idx)) return vm->Get(vm->m_sb+idx).p; else return NULL; }

inline	BOOL		gs_ckparams		( gsVM* vm, int count )							{ if(gs_top(vm)!=count) { gs_error(vm, GSE_BADARGCOUNT, count, gs_top(vm)); return FALSE; } else return TRUE; }
inline	BOOL		gs_ckparamsmin	( gsVM* vm, int countmin )						{ if(gs_top(vm)<countmin) { gs_error(vm, GSE_BADARGCOUNT, countmin, gs_top(vm)); return FALSE; } else return TRUE; }
		BOOL		gs_cktype		( gsVM* vm, int idx, int type );				// checks if type matched
		BOOL		gs_ckreftype	( gsVM* vm, int idx, int type );				// checks reference's type mached (reference must be valid)

inline	void		gs_pop			( gsVM* vm )									{ vm->Pop(); }
inline	void		gs_push			( gsVM* vm, gsObj obj )							{ vm->Push(obj); }
inline	void		gs_pushint		( gsVM* vm, int i )								{ vm->Push(gsObj(i)); }
inline	void		gs_pushflt		( gsVM* vm, float f )							{ vm->Push(gsObj(f)); }
inline	void		gs_pushstr		( gsVM* vm, char* sz )							{ vm->Push( gso_dup(gsObj(sz,gsstrsize(sz))) ); }
		void		gs_pushtab		( gsVM* vm, int size );							// pops size objects and make a new table with them; pushes this table
inline	void		gs_pushref		( gsVM* vm, gsObj* obj )						{ vm->Push(gsObj(obj)); }
inline	void		gs_pushptr		( gsVM* vm, void* ptr )							{ vm->Push(gsObj(ptr)); }

/////////////////////////////////////////////////////////////////////////////////////////
// COMPILE AND DO
// Use those to compile and do scripts
/////////////////////////////////////////////////////////////////////////////////////////
		int			gs_parse		( gsVM* vm, char* buffer, int size, gsNode* &tree, char* file = NULL );	// parse a buffer and return parser tree; (return error if failed); if provided file is used for debug as file being processed
		int			gs_compilefile	( gsVM* vm, char* file );									// compile file (ret error)
		int			gs_compilestring( gsVM* vm, char* sz );										// compile string (ret error)
		int			gs_compilebuffer( gsVM* vm, char* buffer, int size, char* file = NULL );	// compile buffer (ret error); if provided file is used for debug as file being processed

		int			gs_dofile		( gsVM* vm, char* file );									// compile and execute a file (remove temp fn) (return error)
		int			gs_dostring		( gsVM* vm, char* sz );										// compile and execute a string (remove temp fn) (return error)
		int			gs_dobuffer		( gsVM* vm, char* buffer, int size, char* file = NULL );	// compile and execute a buffer (remove temp fn) (return error); if provided file is used for debug as file being processed

/////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS AND RUN
/////////////////////////////////////////////////////////////////////////////////////////
		int			gs_regfn		( gsVM* vm, char* name, gsFunc func );			// register fn and return fnidx (-1 if failed)
inline	int			gs_findfn		( gsVM* vm, char* name )						{ return vm->m_env->FindFn(name); }
inline	int			gs_call			( gsVM* vm, int fp, int params=0 )				{ return vm->CallS(fp,params); } // script only
inline	int			gs_run			( gsVM* vm )									{ return vm->Run(); }
		int			gs_runfn		( gsVM* vm, int fp, int params=0 );				// call a function (ret err) + always return obj on the stack; run the function (even if scripted)
inline	int			gs_isrunning	( gsVM* vm )									{ return vm->m_fp!=-1; }
inline	BOOL		gs_recover		( gsVM* vm )									{ return vm->Recover(); }

/////////////////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////////////////
		int			gs_regint		( gsVM* vm, char* name, int value );			// register a global int; return index or -1
		int			gs_regflt		( gsVM* vm, char* name, float value );			// register a global float; return index or -1
		int			gs_regstr		( gsVM* vm, char* name, char* value );			// register a global string; return index or -1
		int			gs_regref		( gsVM* vm, char* name, gsObj* value );			// register a global reference; return index or -1
		int			gs_regptr		( gsVM* vm, char* name, void* value );			// register a global pointer; return index or -1
inline	int			gs_findvar		( gsVM* vm, char* name )						{ return vm->m_env->FindVar(name); }
inline	gsObj		gs_getvar		( gsVM* vm, int idx )							{ return vm->m_env->GetVar(idx)->m_obj; }
inline	void		gs_setvar		( gsVM* vm, int idx, gsObj& obj )				{ vm->m_env->GetVar(idx)->m_obj = obj; }

/////////////////////////////////////////////////////////////////////////////////////////
// SCOPE
/////////////////////////////////////////////////////////////////////////////////////////
inline	int			gs_getscope		( gsVM* vm )									{ return vm->m_env->m_scope; }
inline	void		gs_setscope		( gsVM* vm, int scope )							{ vm->m_env->m_scope = scope; }
inline	void		gs_delscope		( gsVM* vm, int scope )							{ vm->m_env->DelScope(scope); }

/////////////////////////////////////////////////////////////////////////////////////////
// USER DATA
/////////////////////////////////////////////////////////////////////////////////////////
inline	int			gs_getusr		( gsVM* vm, int idx )							{ return vm->m_usr[idx]; }
inline	int			gs_setusr		( gsVM* vm, int idx, int data )					{ vm->m_usr[idx] = data; }

/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG HELPERS
/////////////////////////////////////////////////////////////////////////////////////////
		void		gs_logcall		( gsVM* vm );									// log short call stack info
		void		gs_logcallex	( gsVM* vm );									// log full call stack info
		void		gs_cpycall		( gsVM* vm, char* buffer, int size );			// copy short call stack info in a buffer; size include space for eos

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
#endif
