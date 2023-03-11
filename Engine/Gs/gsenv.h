#ifndef _GSENV_
#define _GSENV_

#include "gsutil.h"
#include "gsasm.h"

/////////////////////////////////////////////////////////////////////////////////////////
// DEFINES AND TYPES
// Exported functions use the type gsFunc
/////////////////////////////////////////////////////////////////////////////////////////
#define GSFN_EXPORTED		(1<<0)		// for all registered c functions
#define GSFN_SCRIPTED		(1<<1)		// for all scripted functions
#define GSFN_TEMPORARY		(1<<2)		// for some scripted functions (that must be removed after execution)
#define GSFN_INVALID		(1<<3)		// default add flag; scripted empty function or registered without m_func
#define	GSVAR_DEFINE		(1<<0)		// treated as a define (not much difference)


class gsVM;
class gsEnv;
typedef int (*gsFunc) (gsVM* vm);
typedef int (*gseFunc) (gsEnv* env);

/////////////////////////////////////////////////////////////////////////////////////////
// GSCODE
// Code (operation) structure; those form the machine code and are executed by vm
/////////////////////////////////////////////////////////////////////////////////////////
struct gsCode
{
	int		op;			// operation id (GS_NOP,...)
	union				// additional data
	{
		int			i;	// int
		float		f;	// flt
		char*		s;	// str
		gsObj*		o;	// tab
	};
};

/////////////////////////////////////////////////////////////////////////////////////////
// GSFN
// Function structure; keeps info and machinecode
/////////////////////////////////////////////////////////////////////////////////////////
struct gsFn
{
	char*	m_name;
	int		m_flags;

	int		m_parsize;		// params count
	int		m_localsize;	// total local count (params+locals)
	int		m_strsize;
	char**	m_str;			// str
	int		m_codesize;
	gsCode*	m_code;			// code

	gsFunc	m_func;			// exported

	char*	m_file;			// file
	int		m_linesize; 
	int*	m_line;			// lines info
	int		m_scope;		// scope
};

/////////////////////////////////////////////////////////////////////////////////////////
// GSVAR
// Global variable structure; keeps info and value
/////////////////////////////////////////////////////////////////////////////////////////
struct gsVar
{
	char*	m_name;
	int		m_flags;
	gsObj	m_obj;			// value
	int		m_scope;		// scope
};

/////////////////////////////////////////////////////////////////////////////////////////
// GSENV
// Env class keeps the script content (functions and global variables lists)
/////////////////////////////////////////////////////////////////////////////////////////
class gsEnv
{
public:
					gsEnv		();
				   ~gsEnv		();
		
		   void		Init		();							// initialize the script; call after creation
		   void		Done		();							// destroy (free) the script; call before deletion

	// Fn
	inline	int		GetFnSize	()							{ return m_fnsize; }
	inline	gsFn*	GetFn		( int idx )					{ if(0<=idx && idx<m_fnsize) return m_fn[idx]; else return NULL; }
			int		FindFn		( char* name );				// finds a function by its name and return its index
			int		AddFn		( char* name );				// adds a new empty function with a given name; function is added in the first empty slot; default flag is set to invalid
			void	DelFn		( int idx );				// delete (free) a function; the slot remains empty

	// Var
	inline	int		GetVarSize	()							{ return m_varsize; }
	inline	gsVar*	GetVar		( int idx )					{ if(0<=idx && idx<m_varsize) return m_var[idx]; else return NULL; }
			int		FindVar		( char* name );				// finds a variable by its name and return its index
			int		AddVar		( char* name );				// adds a new variable with a given name; variable is added in the first empty slot
			void	DelVar		( int idx );				// delete (free) a variable; the slot remains empty

	// Scope
			void	DelScope	( int scope );				// delete all functions and variables from a scope; the slots remain empty

	// Utils
	inline	gsCode*	GetCodeAddr	( int fp, int pp );			// return code addr
			int		GetLineInfo ( int fp, int pp );			// return line info (corresponding line number)
	inline	BOOL	IsValidFn	( int idx )					{ return (0<=idx && idx<m_fnsize && m_fn[idx] && !(m_fn[idx]->m_flags & GSFN_INVALID) ); }
															
	// Debug helpers												
			char*	GetCodeText	( int fp, int pp );			// get static printable code content
			BOOL	LogVar		( int idx );				// log variable
			BOOL	LogFn		( int idx );				// log function 
			void	LogGlobals	();							// log all globals
			void	LogEnv		();							// log all functions

	
	int		m_scope;			// current env scope; compiling in this env will be added to this scope; set it before compiling

protected:

	int		m_fnsize;			// size of fn list (slots count);
	gsFn**	m_fn;				// fn list
	gsHash*	m_fnhash;			// fn hash for fast search
	
	int		m_varsize;			// size of var list (slots count);
	gsVar**	m_var;				// var list
	gsHash*	m_varhash;			// var hash for fast search
};

/////////////////////////////////////////////////////////////////////////////////////////
// INLINES
/////////////////////////////////////////////////////////////////////////////////////////
gsCode*	gsEnv::GetCodeAddr( int fp, int pp )
{
	if( fp<0 || fp>=m_fnsize || m_fn[fp]==NULL ) return NULL;
	if( m_fn[fp]->m_codesize==0 ) return NULL;
	if( pp<0 || pp>=m_fn[fp]->m_codesize ) return NULL;
	return &m_fn[fp]->m_code[pp];
}


/////////////////////////////////////////////////////////////////////////////////////////
#endif