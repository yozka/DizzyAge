%{

/* ------------------------------------------------------------------
   Initial code (copied verbatim to the output file)
   ------------------------------------------------------------------ */

// comment those for no extended debug info (faster)
#define	YYDEBUG 1
#define	YYERROR_VERBOSE

// Includes
#include "gsparser.h"
#include "gsasm.h"


extern	int	lexLine;			// line index for current token; declared in gslexer

int		parserError = 0;		// errors count
char	parserErrorText[256];	// error description
gsNode* parserTree = NULL;		// perser tree resulted after a yyparse() call

void	LinkNodes	(gsNode *n1, gsNode *n2);	// link node n2 at the end of the node chain that starts with node n1 (if non NULL)
gsNode* YYParse		();							// wrapper for yyparse; return parser tree if successful or NULL if failed
int		YYError		();							// return error code after a failed call to YYParse; (0=ok, 1=error)
char*	YYErrorText	();							// return error description after a failed call to YYParse; (YYERROR_VERBOSE define is required)
int		YYErrorLine	();							// return error line after a failed call to YYParse;
void	yyerror		(char *s);					// called by yyparse on error

char*	LexIdd		( char* sz, int len );
char*	LexString	( char* sz, int len );
void	LexStringCollapse(char* sz, int size, char gc); // size is strlen(sz);

%}

/* ------------------------------------------------------------------
   Yacc declarations
   ------------------------------------------------------------------ */

/* The structure for passing values between lexer and parser */
%union {
	int		i;
	float	f;
	struct
	{
		char*	sz;
		int		len;
	} str;
	class gsNode* node;
}


/* Token definitions */

%token TOK_ERROR

%token TOK_NULVAL
%token TOK_NULLVAL
%token TOK_VOIDVAL
%token <i> TOK_INTVAL
%token <f> TOK_FLTVAL
%token <str> TOK_STRVAL
%token <str> TOK_IDD

%left ';', ','

%token '='
%token TOK_ASGADD, TOK_ASGSUB, TOK_ASGMUL, TOK_ASGDIV, TOK_ASGMOD
%token TOK_ASGAND, TOK_ASGOR, TOK_ASGXOR, TOK_ASGSHR, TOK_ASGSHL
%token TOK_ASGINC, TOK_ASGDEC

%left TOK_OROR
%left TOK_ANDAND
%left '|'
%left '^'
%left '&'
%left TOK_CPE TOK_CPNE 
%left '<' TOK_CPLE '>' TOK_CPGE
%left TOK_SHL TOK_SHR
%left '+' '-'
%left '*' '/' '%'
%right '!' '~' TOK_NEG

%right '(' '{' '[' TOK_BRKL 
%left ')' '}' ']' TOK_BRKR

%token '?' ':', '.'

%token TOK_TYPEOF, TOK_SIZEOF

%token TOK_INT TOK_FLT TOK_STR TOK_TAB TOK_REF TOK_PTR

%token TOK_IF TOK_ELSE
%token TOK_DO TOK_WHILE TOK_FOR TOK_BREAK TOK_CONTINUE
%token TOK_RETURN TOK_GOTO
%token TOK_BREAKPOINT TOK_STOP

%token TOK_DEFINE TOK_IFDEF TOK_IFNDEF 
%token TOK_INCLUDE TOK_ABORT TOK_SCOPE

%token TOK_VAR TOK_FUNC


%type <node> program
%type <node> stmprg_lst
%type <node> stmprg
%type <node> cdir
%type <node> decvar
%type <node> decfn
%type <node> decidd_lst
%type <node> decidd
%type <node> defobj
%type <node> stmcod_lst
%type <node> stmcod
%type <node> stmdo
%type <node> stmfor
%type <node> stmif
%type <node> stmwhile
%type <node> callfn
%type <node> stmasg
%type <node> var
%type <node> varidd
%type <node> castvar
%type <node> expr_lst
%type <node> expr
%type <i>	 deftip

%expect 122

%%

program		: stmprg_lst												{parserTree=$1}
			;

stmprg_lst	:															{$$ = NULL}
			| stmprg													{$$ = $1}
			| stmprg_lst stmprg											{LinkNodes($1, $2)}

stmprg		: cdir														{$$ = $1}
			| decvar													{$$ = $1}
			| decfn														{$$ = $1}
			| stmcod													{$$ = $1}
			;

cdir		: TOK_DEFINE TOK_IDD defobj									{$$ = gsnew gsNode(GSNODE_DEFINE, lexLine, 0, LexIdd($2.sz,$2.len), $3)}
			| TOK_IFDEF TOK_IDD cdir									{$$ = gsnew gsNode(GSNODE_IFDEF, lexLine, 0, LexIdd($2.sz,$2.len), $3)}
			| TOK_IFNDEF TOK_IDD cdir									{$$ = gsnew gsNode(GSNODE_IFNDEF, lexLine, 0, LexIdd($2.sz,$2.len), $3)}
			| TOK_INCLUDE TOK_STRVAL									{$$ = gsnew gsNode(GSNODE_INCLUDE, lexLine, 0, LexString($2.sz,$2.len))}
			| TOK_ABORT													{$$ = gsnew gsNode(GSNODE_ABORT, lexLine, 0)}
			| TOK_SCOPE TOK_INTVAL										{$$ = gsnew gsNode(GSNODE_SCOPE, lexLine, 0, $2)}
			;
			
defobj		:															{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_NUL)}
			| TOK_NULVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_NUL)}
			| TOK_NULLVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_PTR)}
			| TOK_VOIDVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_STR)}
			| TOK_INTVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_INT, $1)}
			| TOK_FLTVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_FLT, $1)}
			| TOK_STRVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_STR, LexString($1.sz,$1.len))}
			| '-' TOK_INTVAL											{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_INT, -$2)}
			| '-' TOK_FLTVAL											{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_FLT, -$2)}
			;

decvar		: TOK_VAR decidd_lst										{$$ = gsnew gsNode(GSNODE_DECVAR, lexLine, GS_NUL, $2)}
			| deftip decidd_lst											{$$ = gsnew gsNode(GSNODE_DECVAR, lexLine, $1, $2); }
			;	

decfn		: TOK_FUNC TOK_IDD '(' decidd_lst ')' '{' stmcod_lst '}'	{$$ = gsnew gsNode(GSNODE_DECFN, lexLine, 0, LexIdd($2.sz,$2.len), $4, $7)}
			| TOK_FUNC TOK_IDD '(' ')' '{' stmcod_lst '}'				{$$ = gsnew gsNode(GSNODE_DECFN, lexLine, 0, LexIdd($2.sz,$2.len), NULL, $6)}
			| TOK_FUNC decidd_lst										{$$ = gsnew gsNode(GSNODE_DECFN, lexLine, 0, $2)}
			;

decidd_lst	: decidd													{$$ = $1}
			| decidd_lst ',' decidd										{LinkNodes($1,$3)}
			;

decidd		: TOK_IDD													{$$ = gsnew gsNode(GSNODE_IDD, lexLine, 0, LexIdd($1.sz,$1.len))}
			;

stmcod_lst	: 															{$$ = NULL}/*$$ = gsnew gsNode(GSNODE_NONE, lexLine, 0, -1)}*/
			| stmcod													{$$ = $1}
			| stmcod_lst stmcod											{LinkNodes($1, $2)}
			;

stmcod		: TOK_BREAK ';'												{$$ = gsnew gsNode(GSNODE_BREAK, lexLine, 0)}
			| TOK_CONTINUE ';'											{$$ = gsnew gsNode(GSNODE_CONTINUE, lexLine, 0)}
			| TOK_GOTO TOK_IDD ';'										{$$ = gsnew gsNode(GSNODE_GOTO, lexLine, 0, LexIdd($2.sz,$2.len))}
			| TOK_BREAKPOINT ';'										{$$ = gsnew gsNode(GSNODE_COD0, lexLine, GSOP_BREAKPOINT)}
			| TOK_STOP ';'												{$$ = gsnew gsNode(GSNODE_COD0, lexLine, GSOP_STOP)}
			| TOK_RETURN ';'											{$$ = gsnew gsNode(GSNODE_RET, lexLine, 0)}
			| TOK_RETURN expr ';'										{$$ = gsnew gsNode(GSNODE_RET, lexLine, 1, $2)}
			| TOK_RETURN '(' expr ')' ';'								{$$ = gsnew gsNode(GSNODE_RET, lexLine, 1, $3)}
			| stmdo														{$$ = $1}
			| stmfor													{$$ = $1}
			| stmif														{$$ = $1}
			| stmwhile													{$$ = $1}
			| callfn ';'												{LinkNodes($1, gsnew gsNode(GSNODE_COD0, lexLine, GSOP_POP))}
			| stmasg ';'												{$$ = $1}
			| '{' stmcod_lst '}'										{$$ = gsnew gsNode(GSNODE_BLOCK, lexLine, 0, $2)}
			| ';'														{$$ = NULL}
			| TOK_IDD ':'												{$$ = gsnew gsNode(GSNODE_LABEL, lexLine, 0, LexIdd($1.sz,$1.len))}
			;

stmdo		: TOK_DO stmcod TOK_WHILE '(' expr ')'						{$$ = gsnew gsNode(GSNODE_DO, lexLine, 0, $2, $5)}
			;

stmfor		: TOK_FOR '(' stmasg ';' expr ';' stmasg ')' stmcod			{$$ = gsnew gsNode(GSNODE_FOR, lexLine, 0, $3, $5, $7, $9)}
			;

stmif		: TOK_IF '(' expr ')' stmcod								{$$ = gsnew gsNode(GSNODE_IF, lexLine, 0, $3, $5)}
			| TOK_IF '(' expr ')' stmcod TOK_ELSE stmcod				{$$ = gsnew gsNode(GSNODE_IF, lexLine, 0, $3, $5, $7)}
			;															
																
stmwhile	: TOK_WHILE '(' expr ')' stmcod								{$$ = gsnew gsNode(GSNODE_WHILE, lexLine, 0, $3, $5)}
			;

stmasg		: var '=' expr 												{$$ = gsnew gsNode(GSNODE_ASG0, lexLine, GSOP_SET, $1, $3)}
			| var TOK_ASGINC											{$$ = gsnew gsNode(GSNODE_ASG1, lexLine, GSOP_INC, $1)}
			| var TOK_ASGDEC											{$$ = gsnew gsNode(GSNODE_ASG1, lexLine, GSOP_DEC, $1)}
			| var TOK_ASGADD expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_ADD, $1, $3)}
			| var TOK_ASGSUB expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_SUB, $1, $3)}
			| var TOK_ASGMUL expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_MUL, $1, $3)}
			| var TOK_ASGDIV expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_DIV, $1, $3)}
			| var TOK_ASGMOD expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_MOD, $1, $3)}
			| var TOK_ASGAND expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_AND, $1, $3)}
			| var TOK_ASGOR expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_OR, $1, $3)}
			| var TOK_ASGXOR expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_XOR, $1, $3)}
			| var TOK_ASGSHR expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_SHR, $1, $3)}
			| var TOK_ASGSHL expr										{$$ = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_SHL, $1, $3)}
			| var TOK_BRKL expr TOK_BRKR '=' expr						{$$ = gsnew gsNode(GSNODE_ASG3, lexLine, GSOP_SETI, $1, $3, $6)}
			;
	
var			: varidd													{$$ = $1}
			| TOK_VAR '(' expr ')'										{$$ = gsnew gsNode(GSNODE_VAR, lexLine, 1, $3)}
			| var '[' expr ']'											{$$ = gsnew gsNode(GSNODE_IDX, lexLine, 0, $1, $3)}
			| '*' var													{$$ = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_GET, $2 )}
			| '(' var ')'												{$$ = $2}
			;

varidd		: TOK_IDD													{$$ = gsnew gsNode(GSNODE_VAR, lexLine, 0, LexIdd($1.sz,$1.len))}
			;

callfn		: TOK_IDD '(' expr_lst ')'									{$$ = gsnew gsNode(GSNODE_CALL, lexLine, 0, LexIdd($1.sz,$1.len), $3)}
			;

expr_lst	:															{$$ = NULL}
			| expr														{$$ = $1}
			| expr_lst ',' expr											{LinkNodes($1, $3)}
			;

expr		: TOK_NULVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_NUL)}
			| TOK_NULLVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_PTR)}
			| TOK_VOIDVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_STR)}
			| TOK_INTVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_INT, $1)}
			| TOK_FLTVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_FLT, $1)}
			| TOK_STRVAL												{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_STR, LexString($1.sz,$1.len))}
			| '{' expr_lst '}'											{$$ = gsnew gsNode(GSNODE_OBJ, lexLine, GS_TAB, $2)}
			| TOK_STR '(' expr ')'										{$$ = gsnew gsNode(GSNODE_NEW, lexLine, GS_STR, $3)}
			| TOK_TAB '(' expr ')'										{$$ = gsnew gsNode(GSNODE_NEW, lexLine, GS_TAB, $3)}
			| var														{$$ = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_GET, $1)}
			| '&' var 													{$$ = $2}
			| '?' var													{$$ = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_TYPEOF, $2)}
			| var TOK_BRKL expr TOK_BRKR								{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_GETI, $1, $3)}
			| TOK_TYPEOF '(' var ')'									{$$ = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_TYPEOF, $3)}
			| TOK_SIZEOF '(' var ')'									{$$ = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_SIZEOF, $3)}
			| castvar													{$$ = $1}
			| callfn													{$$ = $1}
			| expr '?' expr ':' expr									{$$ = gsnew gsNode(GSNODE_IFEXP, lexLine, 0, $1, $3, $5)}
			| expr TOK_OROR expr										{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_OROR, $1, $3)}
			| expr TOK_ANDAND expr										{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_ANDAND, $1, $3)}
			| expr '|' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_OR, $1, $3)}
			| expr '^' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_XOR, $1, $3)}
			| expr '&' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_AND, $1, $3)}
			| expr TOK_CPE expr											{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPE, $1, $3)}
			| expr TOK_CPNE expr										{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPNE, $1, $3)}
			| expr '<' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPL, $1, $3)}
			| expr '>' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPG, $1, $3)}
			| expr TOK_CPGE expr										{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPGE, $1, $3)}
			| expr TOK_CPLE expr										{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPLE, $1, $3)}
			| expr TOK_SHL expr											{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_SHL, $1, $3)}
			| expr TOK_SHR expr											{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_SHR, $1, $3)}
			| expr '+' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_ADD, $1, $3)}
			| expr '-' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_SUB, $1, $3)}
			| expr '*' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_MUL, $1, $3)}
			| expr '/' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_DIV, $1, $3)}
			| expr '%' expr												{$$ = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_MOD, $1, $3)}
			| '-' expr %prec TOK_NEG									{$$ = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_SGN, $2)}
			| '!' expr %prec TOK_NEG									{$$ = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_NOT, $2)}
			| '~' expr %prec TOK_NEG									{$$ = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_INV, $2)}
			| '(' expr ')'												{$$ = $2}
			;	

castvar		: '(' deftip ')' expr										{$$ = gsnew gsNode(GSNODE_CAST, lexLine, $2, $4)}
			| '(' deftip TOK_STRVAL ')' expr							{$$ = gsnew gsNode(GSNODE_CAST, lexLine, $2, LexString($3.sz,$3.len), $5)}
			;

deftip		: TOK_INT													{$$ = GS_INT}
			| TOK_FLT													{$$ = GS_FLT}
			| TOK_STR													{$$ = GS_STR}
			| TOK_TAB													{$$ = GS_TAB}
			| TOK_REF													{$$ = GS_REF}
			| TOK_PTR													{$$ = GS_PTR}
			;

%%


/* ------------------------------------------------------------------
   Additional code (again copied verbatim to the output file)
   ------------------------------------------------------------------ */

void LinkNodes(gsNode *n1, gsNode *n2)
{
	if(n2==NULL) return;
	if(n1!=NULL)
	{
		gsNode* n=n1;
		while(n->m_next!=NULL) n=n->m_next;
		n->m_next = n2;
		n2->m_prev = n;
		n2->m_parent = n->m_parent;
	}
	else
	{
		n2->m_prev = NULL;
		n2->m_parent = NULL;
	}
}

gsNode* YYParse()
{
	lexLine = 1;
	parserError = 0;
	parserErrorText[0] = 0;
	parserTree = NULL;
	gsNode::InitTree();
	int ret = yyparse();
	if(ret!=0 || parserError!=0)
	{
		gsNode::DoneTree(TRUE); // clear
		parserTree = NULL;
	}
	else
	{
		gsNode::DoneTree(FALSE); // just reset
	}
	return parserTree;
}

int YYError()
{
	return parserError;
}

char* YYErrorText()
{
	return parserErrorText;
}

int	YYErrorLine()
{
	return lexLine;
}

void yyerror(char* text)
{
	parserError = 1;
	strncpy(parserErrorText, text, 255); 
	parserErrorText[255]=0;
}

////////////////

char* LexIdd( char* sz, int len )
{
	char* s = (char*)gsmalloc(len+1);
	strncpy(s, sz, len); s[len]=0;
	return s;
}

char* LexString( char* sz, int len )
{
	char* s = (char*)gsmalloc(len-2+1);
	if( len-2>0 )
		strncpy(s, sz+1, len-2);  
	s[len-2]=0;
	LexStringCollapse(s, len-2, sz[0]); // collapse escape sequences
	return s;
}

void LexStringCollapse(char* sz, int size, char gc) // size is strlen(sz)
{
	int i=0;
	while(i<size)
	{
		// add support for hex values ...
		// escape sequences
		if( i<size-1 && sz[i]=='\\' )
		{
			int c = 0;
			if(sz[i+1]==gc)
			{
				sz[i]=gc;
				c=1;
			}
			else
			{
				switch(sz[i+1])
				{
				case '\\':	c=1; break;
				case 'n':	sz[i]='\n'; c=1; break;
				case 'r':	sz[i]='\r'; c=1; break;
				case 't':	sz[i]='\t'; c=1; break;
				}
			}

			if( c!=0 )
			{
				if(size-(i+2)>0) memcpy(sz+i+1, sz+i+2, size-(i+2));
				size--;
			}
			i++;
			continue;
		}
		else
		// concat
		if( sz[i]==gc )
		{
			int j = i+1;
			while(j<size && sz[j]!=gc) j++;
			if(j<size) j++;
			if(i<j && j<size) memcpy(sz+i, sz+j, size-j);	
			size-=(j-i);
			continue;
		}
		else
		// normal character
		i++;
	}

	sz[size]=0; // cut
}
