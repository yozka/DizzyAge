#ifndef _GSVM_
#define _GSVM_
#include "gsenv.h"
#include "gserror.h"

/////////////////////////////////////////////////////////////////////////////////////////
// DEFINES
/////////////////////////////////////////////////////////////////////////////////////////
#define	GS_STACKSIZE	1024
#define	GS_USRDATA		8

/////////////////////////////////////////////////////////////////////////////////////////
// GSCE
// Call entry structure; keeps info about all call states
/////////////////////////////////////////////////////////////////////////////////////////
struct gsCE
{
	int		m_fp;
	int		m_pp;
	int		m_sb;
	int		m_sp;
};

/////////////////////////////////////////////////////////////////////////////////////////
// GSVM
// the virtual machine class; is linked to an enviroment (gsEnv) and can execute machine code
/////////////////////////////////////////////////////////////////////////////////////////
class gsVM
{
public:
				gsVM			();

		void	Init			( int stacksize = GS_STACKSIZE ); // initialize vm; creates internal stacks and stuff; call after creation
		void	Done			();								  // destroy (free) vm; call before deletion

		// run
		int		Run				();								// run machine code from the current function and from the current position

		// call
inline	int		Call			( int fp, int params );			// call function; test if exported and calls CallS or CallC
inline	int		CallS			( int fp, int params );			// call script function; only prepares to run the function; if succeded, only params are on the new stack level; if failed nothing is changed
inline	int		CallC			( int fp, int params );			// call registered function; will return immediatly; always returns one value on the stack;
inline	int		Return			();								// return from function
		BOOL	Recover			();								// flush callstack level (all levels are cleared until base level is reached); base level should be a C++ function; return TRUE if no more entries in the callstack

		// stack operations
inline	void	Push			( gsObj& obj );					// push an object; add it on stack
inline	gsObj	Pop				();								// pop object; remove it from stack
inline	gsObj	PopInt			();								// pop int (delete value if not int and return gsNUL)
inline	gsObj	PopTab			();								// pop tab (delete value if not tab and return gsNUL)
inline	gsObj	PopRef			( int type = -1 );				// pop ref; if type!=-1 restrict refered object (delete value if not a valid (non-null) ref (or restriction type not matched) and return gsNUL);
inline	BOOL	PopBool			();								// pop object and return boolean value; remove it from stack
inline	gsObj	Get				( int idx );					// get obj from pos idx; index must be on current stack level
inline	void	Set				( int idx, gsObj& obj );		// set obj at pos idx; index must be on current stack level

		// utils
inline	char*	GetStr			( int idx );					// get string from the current function's string table

		// errors
inline	void	Error			( int err, char* text )							{ Error( err, 0, 0, text ); }
inline	void	Error			( int err, int param1=0, int param2=0 )			{ Error( err, param1, param2, NULL ); }
		void	Error			( int err, int param1, int param2, char* text );


public:
		int		m_fp;			// function pointer; -1 if no function is set for running
		int		m_pp;			// program pointer; value inside current function's code

		int		m_sp;			// stack pointer (top); top of the stack
		int		m_sb;			// stack base; start position of this stack level
		int		m_ss;			// object stack size;
		gsObj*	m_stack;		// object stack
								
		int		m_cp;			// call stack pointer (top); top of the call stack; call stack levels
		int		m_cb;			// call stack base; run will stop successfully when this level is reached
		int		m_cs;			// call stack size
		gsCE*	m_call;			// call stack
								
		gsEnv*	m_env;			// gs enviroment pointer; the vm will run from this enviroment
								
		int		m_status;				// running status (GS_OK, ...)
		int		m_usr[GS_USRDATA];		// user data; for miscelious purposes

		int				m_debug;		// error flags (GSDBG_HALT, ...); used by error handler; user change this to get more or less debug info
		gsErrHandler	m_errhandler;	// error handler
};


/////////////////////////////////////////////////////////////////////////////////////////
// INLINES
/////////////////////////////////////////////////////////////////////////////////////////
void gsVM::Push( gsObj& obj )
{
	if(m_sp>=m_ss) { Error(GSE_STACKOVERFLOW); return; }
	m_stack[m_sp] = obj;
	m_sp++;
}

gsObj gsVM::Pop()
{
	if(m_sp>m_sb) return m_stack[--m_sp];
	Error(GSE_STACKUNDERFLOW);
	return gsNUL;
}

gsObj gsVM::PopInt()
{
	if(m_sp<=m_sb) { Error(GSE_STACKUNDERFLOW); return gsNUL; }
	m_sp--;
	if(gso_isint(m_stack[m_sp])) return m_stack[m_sp];
	Error(GSE_BADTYPE, GS_INT,m_stack[m_sp].type);
	gso_del(m_stack[m_sp]);
	return gsNUL;
}

gsObj gsVM::PopTab()
{
	if(m_sp<=m_sb) { Error(GSE_STACKUNDERFLOW); return gsNUL; }
	m_sp--;
	if(gso_istab(m_stack[m_sp])) return m_stack[m_sp];
	Error(GSE_BADTYPE, GS_TAB,m_stack[m_sp].type);
	gso_del(m_stack[m_sp]);
	return gsNUL;
}

gsObj gsVM::PopRef(int type)
{
	if(m_sp<=m_sb) { Error(GSE_STACKUNDERFLOW); return gsNUL; }
	m_sp--;
	if(gso_isref(m_stack[m_sp]))
	{
		if(m_stack[m_sp].o != NULL)
		{
			if(type==-1 || m_stack[m_sp].o->type==type)
				return m_stack[m_sp]; // ok
			else
			{
				Error(GSE_BADREFTYPE, type, m_stack[m_sp].o->type);
				return gsNUL;
			}
		}
		else
		{
			Error(GSE_NULLREF);
			return gsNUL;
		}
	}
	Error(GSE_BADTYPE, GS_REF,m_stack[m_sp].type);
	gso_del(m_stack[m_sp]);
	return gsNUL;
}

BOOL gsVM::PopBool()
{
	if(m_sp<=m_sb) { Error(GSE_STACKUNDERFLOW); return FALSE; }
	m_sp--;
	BOOL b;
	if( gso_isint(m_stack[m_sp]) || gso_isflt(m_stack[m_sp]) )
		b = (m_stack[m_sp].i!=0) ? TRUE : FALSE; // safe if 64bit
	else
	if( !gso_isnul(m_stack[m_sp]) )
		b = (m_stack[m_sp].p!=NULL) ? TRUE : FALSE;
	else // NUL isn't considered bool
	{
		Error(GSE_BADTYPE, GS_NUL, GS_NUL);
		b=0;
	}
	gso_del(m_stack[m_sp]);
	return b;
}

gsObj gsVM::Get( int i )
{
	if(i<m_sb || i>=m_sp) { Error(GSE_UNKNOWNLOC, i-m_sb, m_sp-m_sb); return gsNUL; }
	return m_stack[i];
}

void gsVM::Set( int i, gsObj& obj )
{
	if(i<m_sb || i>=m_sp) { Error(GSE_UNKNOWNLOC, i-m_sb, m_sp-m_sb); }
	m_stack[i] = obj;
}

char* gsVM::GetStr( int idx )
{
	gsFn* fn = m_env->GetFn(m_fp); 
	gsassert(fn!=NULL);
	gsassert(0<=idx && idx<fn->m_strsize);
	return fn->m_str[idx];
}


/////////////////////////////////////////////////////////////////////////////////////////
// CALL
/////////////////////////////////////////////////////////////////////////////////////////
inline int gsVM::Call( int fp, int params )
{
	gsFn* fn = m_env->GetFn(fp);
	if(fn==NULL) { Error(GSE_UNKNOWNFUNC, fp); return GSE_UNKNOWNFUNC; } // invalid function
	if(fn->m_flags & GSFN_INVALID) { Error(GSE_UNKNOWNFUNC, fp, 0, fn->m_name); return GSE_UNKNOWNFUNC; } // invalid function
	if(fn->m_flags & GSFN_EXPORTED) return CallC(fp,params);
	if(fn->m_flags & GSFN_SCRIPTED) return CallS(fp,params);
	Error(GSE_UNKNOWNFUNC, fp, 0, fn->m_name); 
	return GSE_UNKNOWNFUNC; // bad error
}

inline int gsVM::CallS( int fp, int params )
{
	int i;
	gsassert(m_cp<m_cs); // call stack overflow check
	gsassert(m_sp-params>=m_sb);
	gsFn* fn = m_env->GetFn(fp);
	gsassert(fn!=NULL);
	gsassert( (fn->m_flags & GSFN_EXPORTED)==0 );

	// check params count
	if(params>fn->m_parsize) // too many - error
	{
		Error(GSE_BADARGCOUNT,fn->m_parsize,params);
		return GSE_BADARGCOUNT;
	}
	else
	if(params<fn->m_parsize) // too few - push NULs
	{
		for(i=0;i<fn->m_parsize-params;i++)	Push(gsNUL);
		params = fn->m_parsize;
	}

	// callstack
	m_call[m_cp].m_fp = m_fp;
	m_call[m_cp].m_pp = m_pp+1; // return to the next instruction
	m_call[m_cp].m_sb = m_sb;
	m_call[m_cp].m_sp = m_sp-params;
	m_cp++;

	// new stack base
	m_sb = m_sp-params;

	// add local vars
	for( i=fn->m_parsize; i<fn->m_localsize; i++ ) Push(gsNUL);

	// set current function and run position
	m_fp = fp;
	m_pp = 0;

	return GS_OK;
}

inline int gsVM::CallC( int fp, int params )
{
	gsassert(m_cp<m_cs); // call stack overflow check
	gsassert(m_sp-params>=m_sb);
	gsFn* fn = m_env->GetFn(fp);
	gsassert(fn!=NULL);
	gsassert( (fn->m_flags & GSFN_EXPORTED)!=0 );

	// callstack
	m_call[m_cp].m_fp = m_fp;
	m_call[m_cp].m_pp = m_pp+1; // return to the next instruction
	m_call[m_cp].m_sb = m_sb;
	m_call[m_cp].m_sp = m_sp-params;
	m_cp++;
	int tcp = m_cp; // callstack pointer stored for savety check

	// new stack base
	m_sb = m_sp-params;

	// no locals

	// set current function
	m_fp = fp;
	m_pp = -1;

	// call C function
	int ret = fn->m_func(this);
	gsassert(ret==0 || ret==1); // only 0 or 1 values are accepted

	// return
	gsassert(m_sp-m_sb>=ret);	// safety check (at least ret values must be on the stack)
	gsassert(m_cp==tcp);		// safety check (call stack pointer must be the same)

	gsObj o; // nul
	if(ret) o=Pop();

	Return();
	Push(o); // always push something

	return GS_OK;
}

inline	int	gsVM::Return()
{
	if(m_cp==m_cb) return GS_FAIL; // callstack level is empty

	// free all left on this stack level
	for(int i=m_sb;i<m_sp;i++) gso_del(m_stack[i]);
	
	m_cp--;
	m_fp = m_call[m_cp].m_fp;
	m_pp = m_call[m_cp].m_pp;
	m_sb = m_call[m_cp].m_sb;
	m_sp = m_call[m_cp].m_sp;
	
	return GS_OK;
}

#endif
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


