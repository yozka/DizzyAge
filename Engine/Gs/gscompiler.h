#ifndef _GSCOMPILER_
#define _GSCOMPILER_

#include "gsvm.h"
#include "gsnode.h"

/////////////////////////////////////////////////////////////////////////////////////////
// DEFINES
/////////////////////////////////////////////////////////////////////////////////////////
#define	GSC_NONE		0		// not in compiling (ready to use)
#define	GSC_PROGRAM		1		// in PROGRAM mode (busy compiling - fn list)
#define	GSC_FUNCTION	2		// in FUNCTION mode (busy compiling - temp fn code)

/////////////////////////////////////////////////////////////////////////////////////////
// GSCSYN
// structure for symbol table; keeps local variables for linking
/////////////////////////////////////////////////////////////////////////////////////////
struct gscSym
{
	char*		m_name;			// var name
	int			m_type;			// var type
};

/////////////////////////////////////////////////////////////////////////////////////////
// GSCGOTO
// goto struct for goto table; keeps labels for linking
/////////////////////////////////////////////////////////////////////////////////////////
struct gscGoto
{
	char*		m_name;			// label name
	int			m_addr;			// label addr
	int			m_line;			// line of use (set by goto)
};

/////////////////////////////////////////////////////////////////////////////////////////
// GSCOMPILER
// the compiler class used to compile parser trees in a gsEnv (using a gsVM)
/////////////////////////////////////////////////////////////////////////////////////////
class gsCompiler
{
public:
					gsCompiler		();
					~gsCompiler		();

		// compiling functions; call those to compile a parser tree
		int			CompileProgram	( gsVM* vm, gsNode* tree );			// compile tree as a program (list of functions) (return error)
		int			CompileFunction	( gsVM* vm, gsNode* tree, int& fn );// compile tree as code (return error); code is placed in a temporary function who's index is set in fn parameter

private:
		// variables
		int			VarFind			( char* name );						// find entry
		int			VarAdd			( char* name, int type=GS_NUL );	// add new entry
inline	gsVar*		VarGet			( int idx )							{ gsassert(m_env!=NULL); return m_env->GetVar(idx); }

		// functions
		int			FnFind			( char* name );						// find entry
		int			FnAdd			( char* name );						// add new entry
inline	gsFn*		FnGet			( int idx )							{ gsassert(m_env!=NULL); return m_env->GetFn(idx); }

		// strings
		int			StrAdd			( char* str );						// add new string in current function

		// symbol table
		void		SymDestroy		();									// destroy (dealloc symtab)
		void		SymClear		();									// clear list elements 
		int			SymFind			( char* name );						// find entry
		int			SymAdd			( char* name, int type=GS_NUL );	// add new entry
inline	gscSym*		SymGet			( int idx )							{ gsassert(0<=idx && idx<m_symcnt); return &m_symtab[idx]; }

		// goto table
		void		GotoDestroy		();									// destroy (dealloc gototab)
		void		GotoClear		();									// clear list elements
		int			GotoFind		( char* name );						// find entry
		int			GotoAdd			( char* name, int addr, int line );	// add new entry
inline	gscGoto*	GotoGet			( int idx )							{ gsassert(0<=idx && idx<m_gotocnt); return &m_gototab[idx]; }

		// code ops
		int			Code			( gsCode& cod );					// add new instruction code to the level function
inline	int			CodeInt			( int op, int i )					{ gsCode cod; cod.op = op; cod.i = i; return Code(cod); }
inline	int			CodeFlt			( int op, float f )					{ gsCode cod; cod.op = op; cod.f = f; return Code(cod); }
inline	int			CodeStr			( int op, char* s )					{ gsCode cod; cod.op = op; cod.s = s; return Code(cod); }
		int			LineInfo		( int addr, int line );				// add new line info
		
		// compilation utils
		int			CompileDir		( gsNode* node, BOOL& abort );		// compile directors (return error) can request abort compilation
		int			CompileCode		( gsNode* node );					// compile code in current fn (return error)
		int			CompileNodes	( gsNode* node );					// compile generic nodes list (return error)
		int			CompileExp		( gsNode* node );					// compile expression left or right (return error)
		int			CompileCall		( gsNode* node );					// compile fn call (return error)

		// linking
		void		Link			( int a0, int a1, int br, int cn );	// check code between [a0, a1] and link breaks to br and continues to cn
		int			LinkGoto		();									// check current function and link invalid jump (goto) (return error)

		// utils
inline	gsFn*		GetFn			()									{ if(m_env && m_fn!=-1) return m_env->GetFn(m_fn); else return NULL; }
inline	int			GetCodePos		()									{ if(GetFn()!=NULL) return GetFn()->m_codesize; else return -1; }
inline	gsCode*		GetCode			(int pos)							{ if(GetFn()!=NULL) return &(GetFn()->m_code[pos]); else return NULL; }

		// errors
inline	void		Error			( int err, char* text )							{ Error( err, 0, 0, text ); }
inline	void		Error			( int err, int param1=0, int param2=0 )			{ Error( err, param1, param2, NULL ); }
inline	void		Error			( int err, int param1, int param2, char* text )	{ m_vm->m_errhandler( m_vm, gsErrInfo(err, param1, param2, text, m_line, m_file) ); }


private:
		// compile status
		int			m_mode;			// 0=GSC_NONE 1=GSC_PROGRAM, 2=GSC_FUNCTION
		int			m_fn;			// current function index
		int			m_codemax;		// code buffer max size
		int			m_strmax;		// string buffer max size
		int			m_linemax;		// line buffer max size

		// symbol table
		int			m_symmax;		// table max size
		int			m_symcnt;		// table entries count
		gscSym*		m_symtab;		// symbol table
		gsHash*		m_symhash;		// symbol hash for fast search

		// goto table
		int			m_gotomax;		// table max size
		int			m_gotocnt;		// table entries count
		gscGoto*	m_gototab;		// goto table

		// tempo
		gsVM*		m_vm;			// current vm
		gsEnv*		m_env;			// current env; is non-NULL while compiling; can't start compiling if non-NULL
		int			m_line;			// current line; 

public:
		char*		m_file;			// current file; users set this for debug purpose before start compiling

};

#endif
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
