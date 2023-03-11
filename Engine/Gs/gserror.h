#ifndef _GSERROR_
#define _GSERROR_

class	gsVM;

/////////////////////////////////////////////////////////////////////////////////////////
// ERROR CODES
/////////////////////////////////////////////////////////////////////////////////////////

// general
#define GS_OK				0
#define GSE_NONE			0
#define GS_FAIL				1
#define GSE_UNKNOWN			1
#define GSE_FILEERROR		2

// parser
#define GSE_PARSER			3

// compiler
#define GSE_UNKNOWNNODE		4
#define GSE_REDECLARATION	5
#define GSE_LABELNOTFOUND	6
#define GSE_LABELEXISTS		7

// runtime
#define GSE_UNKNOWNOP		8
#define GSE_BADTYPE			9
#define GSE_BADREFTYPE		10
#define GSE_BADARGTYPE		11
#define GSE_BADARGCOUNT		12
#define GSE_NULLREF			13
#define GSE_OUTBOUND		14
#define GSE_STACKUNDERFLOW	15
#define GSE_STACKOVERFLOW	16
#define GSE_UNKNOWNLOC		17
#define GSE_UNKNOWNGLB		18
#define GSE_UNKNOWNFUNC		19
#define GSE_DIVIDEBYZERO	20
#define GSE_STOPPED			21
#define GSE_ASSERT			22
#define GSE_USER			23

#define GS_MAXERROR			24	// total number of errors

/////////////////////////////////////////////////////////////////////////////////////////
// DEBUG FLAGS
/////////////////////////////////////////////////////////////////////////////////////////
#define GSDBG_HALT			(1<<0)		// halt on errors
#define GSDBG_LOGDESC		(1<<1)		// log error description and short call stack info 
#define GSDBG_CODE			(1<<2)		// list function machine code
#define GSDBG_STACK			(1<<3)		// list data stack
#define GSDBG_CALLSTACK		(1<<4)		// list extended info about the current call stack
#define GSDBG_GLOBALS		(1<<5)		// dump globals
#define GSDBG_FUNCTIONS		(1<<6)		// dump all functions asm code

#define GSDBG_LOW			( GSDBG_HALT | GSDBG_LOGDESC )
#define GSDBG_MEDIUM		( GSDBG_HALT | GSDBG_LOGDESC | GSDBG_CALLSTACK )
#define GSDBG_HIGH			( GSDBG_HALT | GSDBG_LOGDESC | GSDBG_CALLSTACK | GSDBG_CODE | GSDBG_STACK )
#define GSDBG_FULL			( 127 )

/////////////////////////////////////////////////////////////////////////////////////////
// ERROR HANDLER
/////////////////////////////////////////////////////////////////////////////////////////
struct gsErrInfo
{
			gsErrInfo		( int err, int param1, int param2, char* text, int line, char* file )
			{ m_err=err; m_param1=param1; m_param2=param2; m_text=text; m_line=line; m_file=file; }

	int		m_err;			// error code
	int		m_param1;		// error param 1
	int		m_param2;		// error param 2
	char*	m_text;			// error text description
	int		m_line;			// error line
	char*	m_file;			// error file
};

typedef void (*gsErrHandler) (gsVM* vm, gsErrInfo& errinfo );

char*	gs_nameerr			( int err );						// return the static error name
void	GSErrHandler		( gsVM* vm, gsErrInfo& errinfo );	// default error handler

#endif
/////////////////////////////////////////////////////////////////////////////////////////
