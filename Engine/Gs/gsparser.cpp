#include "stdafx.h"
/*  A Bison parser, made from gsparser.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	TOK_ERROR	258
#define	TOK_NULVAL	259
#define	TOK_NULLVAL	260
#define	TOK_VOIDVAL	261
#define	TOK_INTVAL	262
#define	TOK_FLTVAL	263
#define	TOK_STRVAL	264
#define	TOK_IDD	265
#define	TOK_ASGADD	266
#define	TOK_ASGSUB	267
#define	TOK_ASGMUL	268
#define	TOK_ASGDIV	269
#define	TOK_ASGMOD	270
#define	TOK_ASGAND	271
#define	TOK_ASGOR	272
#define	TOK_ASGXOR	273
#define	TOK_ASGSHR	274
#define	TOK_ASGSHL	275
#define	TOK_ASGINC	276
#define	TOK_ASGDEC	277
#define	TOK_OROR	278
#define	TOK_ANDAND	279
#define	TOK_CPE	280
#define	TOK_CPNE	281
#define	TOK_CPLE	282
#define	TOK_CPGE	283
#define	TOK_SHL	284
#define	TOK_SHR	285
#define	TOK_NEG	286
#define	TOK_BRKL	287
#define	TOK_BRKR	288
#define	TOK_TYPEOF	289
#define	TOK_SIZEOF	290
#define	TOK_INT	291
#define	TOK_FLT	292
#define	TOK_STR	293
#define	TOK_TAB	294
#define	TOK_REF	295
#define	TOK_PTR	296
#define	TOK_IF	297
#define	TOK_ELSE	298
#define	TOK_DO	299
#define	TOK_WHILE	300
#define	TOK_FOR	301
#define	TOK_BREAK	302
#define	TOK_CONTINUE	303
#define	TOK_RETURN	304
#define	TOK_GOTO	305
#define	TOK_BREAKPOINT	306
#define	TOK_STOP	307
#define	TOK_DEFINE	308
#define	TOK_IFDEF	309
#define	TOK_IFNDEF	310
#define	TOK_INCLUDE	311
#define	TOK_ABORT	312
#define	TOK_SCOPE	313
#define	TOK_VAR	314
#define	TOK_FUNC	315

#line 1 "gsparser.y"


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


#line 40 "gsparser.y"
typedef union {
	int		i;
	float	f;
	struct
	{
		char*	sz;
		int		len;
	} str;
	class gsNode* node;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		266
#define	YYFLAG		-32768
#define	YYNTBASE	85

#define YYTRANSLATE(x) ((unsigned)(x) <= 315 ? yytranslate[x] : 108)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    44,     2,     2,     2,    43,    30,     2,    47,
    51,    41,    39,    12,    40,    57,    42,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    56,    11,    33,
    13,    35,    55,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    49,     2,    53,    29,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    48,    28,    52,    45,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    14,    15,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    26,    27,    31,
    32,    34,    36,    37,    38,    46,    50,    54,    58,    59,
    60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
    70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
    80,    81,    82,    83,    84
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     3,     5,     8,    10,    12,    14,    16,    20,
    24,    28,    31,    33,    36,    37,    39,    41,    43,    45,
    47,    49,    52,    55,    58,    61,    70,    78,    81,    83,
    87,    89,    90,    92,    95,    98,   101,   105,   108,   111,
   114,   118,   124,   126,   128,   130,   132,   135,   138,   142,
   144,   147,   154,   164,   170,   178,   184,   188,   191,   194,
   198,   202,   206,   210,   214,   218,   222,   226,   230,   234,
   241,   243,   248,   253,   256,   260,   262,   267,   268,   270,
   274,   276,   278,   280,   282,   284,   286,   290,   295,   300,
   302,   305,   308,   313,   318,   323,   325,   327,   333,   337,
   341,   345,   349,   353,   357,   361,   365,   369,   373,   377,
   381,   385,   389,   393,   397,   401,   405,   408,   411,   414,
   418,   423,   429,   431,   433,   435,   437,   439
};

static const short yyrhs[] = {    86,
     0,     0,    87,     0,    86,    87,     0,    88,     0,    90,
     0,    91,     0,    95,     0,    77,    10,    89,     0,    78,
    10,    88,     0,    79,    10,    88,     0,    80,     9,     0,
    81,     0,    82,     7,     0,     0,     4,     0,     5,     0,
     6,     0,     7,     0,     8,     0,     9,     0,    40,     7,
     0,    40,     8,     0,    83,    92,     0,   107,    92,     0,
    84,    10,    47,    92,    51,    48,    94,    52,     0,    84,
    10,    47,    51,    48,    94,    52,     0,    84,    92,     0,
    93,     0,    92,    12,    93,     0,    10,     0,     0,    95,
     0,    94,    95,     0,    71,    11,     0,    72,    11,     0,
    74,    10,    11,     0,    75,    11,     0,    76,    11,     0,
    73,    11,     0,    73,   105,    11,     0,    73,    47,   105,
    51,    11,     0,    96,     0,    97,     0,    98,     0,    99,
     0,   103,    11,     0,   100,    11,     0,    48,    94,    52,
     0,    11,     0,    10,    56,     0,    68,    95,    69,    47,
   105,    51,     0,    70,    47,   100,    11,   105,    11,   100,
    51,    95,     0,    66,    47,   105,    51,    95,     0,    66,
    47,   105,    51,    95,    67,    95,     0,    69,    47,   105,
    51,    95,     0,   101,    13,   105,     0,   101,    24,     0,
   101,    25,     0,   101,    14,   105,     0,   101,    15,   105,
     0,   101,    16,   105,     0,   101,    17,   105,     0,   101,
    18,   105,     0,   101,    19,   105,     0,   101,    20,   105,
     0,   101,    21,   105,     0,   101,    22,   105,     0,   101,
    23,   105,     0,   101,    50,   105,    54,    13,   105,     0,
   102,     0,    83,    47,   105,    51,     0,   101,    49,   105,
    53,     0,    41,   101,     0,    47,   101,    51,     0,    10,
     0,    10,    47,   104,    51,     0,     0,   105,     0,   104,
    12,   105,     0,     4,     0,     5,     0,     6,     0,     7,
     0,     8,     0,     9,     0,    48,   104,    52,     0,    62,
    47,   105,    51,     0,    63,    47,   105,    51,     0,   101,
     0,    30,   101,     0,    55,   101,     0,   101,    50,   105,
    54,     0,    58,    47,   101,    51,     0,    59,    47,   101,
    51,     0,   106,     0,   103,     0,   105,    55,   105,    56,
   105,     0,   105,    26,   105,     0,   105,    27,   105,     0,
   105,    28,   105,     0,   105,    29,   105,     0,   105,    30,
   105,     0,   105,    31,   105,     0,   105,    32,   105,     0,
   105,    33,   105,     0,   105,    35,   105,     0,   105,    36,
   105,     0,   105,    34,   105,     0,   105,    37,   105,     0,
   105,    38,   105,     0,   105,    39,   105,     0,   105,    40,
   105,     0,   105,    41,   105,     0,   105,    42,   105,     0,
   105,    43,   105,     0,    40,   105,     0,    44,   105,     0,
    45,   105,     0,    47,   105,    51,     0,    47,   107,    51,
   105,     0,    47,   107,     9,    51,   105,     0,    60,     0,
    61,     0,    62,     0,    63,     0,    64,     0,    65,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   131,   134,   135,   136,   138,   139,   140,   141,   144,   145,
   146,   147,   148,   149,   152,   153,   154,   155,   156,   157,
   158,   159,   160,   163,   164,   167,   168,   169,   172,   173,
   176,   179,   180,   181,   184,   185,   186,   187,   188,   189,
   190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
   200,   203,   206,   209,   210,   213,   216,   217,   218,   219,
   220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
   232,   233,   234,   235,   236,   239,   242,   245,   246,   247,
   250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
   260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
   270,   271,   272,   273,   274,   275,   276,   277,   278,   279,
   280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
   292,   293,   296,   297,   298,   299,   300,   301
};

static const char * const yytname[] = {   "$","error","$undefined.","TOK_ERROR",
"TOK_NULVAL","TOK_NULLVAL","TOK_VOIDVAL","TOK_INTVAL","TOK_FLTVAL","TOK_STRVAL",
"TOK_IDD","';'","','","'='","TOK_ASGADD","TOK_ASGSUB","TOK_ASGMUL","TOK_ASGDIV",
"TOK_ASGMOD","TOK_ASGAND","TOK_ASGOR","TOK_ASGXOR","TOK_ASGSHR","TOK_ASGSHL",
"TOK_ASGINC","TOK_ASGDEC","TOK_OROR","TOK_ANDAND","'|'","'^'","'&'","TOK_CPE",
"TOK_CPNE","'<'","TOK_CPLE","'>'","TOK_CPGE","TOK_SHL","TOK_SHR","'+'","'-'",
"'*'","'/'","'%'","'!'","'~'","TOK_NEG","'('","'{'","'['","TOK_BRKL","')'","'}'",
"']'","TOK_BRKR","'?'","':'","'.'","TOK_TYPEOF","TOK_SIZEOF","TOK_INT","TOK_FLT",
"TOK_STR","TOK_TAB","TOK_REF","TOK_PTR","TOK_IF","TOK_ELSE","TOK_DO","TOK_WHILE",
"TOK_FOR","TOK_BREAK","TOK_CONTINUE","TOK_RETURN","TOK_GOTO","TOK_BREAKPOINT",
"TOK_STOP","TOK_DEFINE","TOK_IFDEF","TOK_IFNDEF","TOK_INCLUDE","TOK_ABORT","TOK_SCOPE",
"TOK_VAR","TOK_FUNC","program","stmprg_lst","stmprg","cdir","defobj","decvar",
"decfn","decidd_lst","decidd","stmcod_lst","stmcod","stmdo","stmfor","stmif",
"stmwhile","stmasg","var","varidd","callfn","expr_lst","expr","castvar","deftip",
""
};
#endif

static const short yyr1[] = {     0,
    85,    86,    86,    86,    87,    87,    87,    87,    88,    88,
    88,    88,    88,    88,    89,    89,    89,    89,    89,    89,
    89,    89,    89,    90,    90,    91,    91,    91,    92,    92,
    93,    94,    94,    94,    95,    95,    95,    95,    95,    95,
    95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
    95,    96,    97,    98,    98,    99,   100,   100,   100,   100,
   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
   101,   101,   101,   101,   101,   102,   103,   104,   104,   104,
   105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
   105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
   105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
   105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
   106,   106,   107,   107,   107,   107,   107,   107
};

static const short yyr2[] = {     0,
     1,     0,     1,     2,     1,     1,     1,     1,     3,     3,
     3,     2,     1,     2,     0,     1,     1,     1,     1,     1,
     1,     2,     2,     2,     2,     8,     7,     2,     1,     3,
     1,     0,     1,     2,     2,     2,     3,     2,     2,     2,
     3,     5,     1,     1,     1,     1,     2,     2,     3,     1,
     2,     6,     9,     5,     7,     5,     3,     2,     2,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     6,
     1,     4,     4,     2,     3,     1,     4,     0,     1,     3,
     1,     1,     1,     1,     1,     1,     3,     4,     4,     1,
     2,     2,     4,     4,     4,     1,     1,     5,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     2,     2,     2,     3,
     4,     5,     1,     1,     1,     1,     1,     1
};

static const short yydefact[] = {     2,
    76,    50,     0,     0,    32,   123,   124,   125,   126,   127,
   128,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    13,     0,     0,     0,     1,
     3,     5,     6,     7,     8,    43,    44,    45,    46,     0,
     0,    71,     0,     0,    78,    51,    76,     0,    74,     0,
     0,    33,     0,     0,     0,     0,    35,    36,    81,    82,
    83,    84,    85,    86,    76,    40,     0,     0,     0,     0,
     0,    78,     0,     0,     0,     0,     0,    90,    97,     0,
    96,     0,    38,    39,    15,     0,     0,    12,    14,    31,
     0,    24,    29,    31,    28,     4,    48,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,    58,    59,
     0,     0,    47,    25,     0,     0,    79,    75,    49,    34,
     0,     0,     0,     0,    91,   117,   118,   119,   125,   126,
    90,     0,     0,     0,    92,     0,     0,     0,     0,     0,
    41,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    37,    16,    17,    18,    19,    20,    21,     0,     9,    10,
    11,     0,     0,     0,    57,    60,    61,    62,    63,    64,
    65,    66,    67,    68,    69,     0,     0,     0,     0,    77,
     0,     0,     0,     0,   120,     0,     0,    87,     0,     0,
     0,     0,     0,    99,   100,   101,   102,   103,   104,   105,
   106,   109,   107,   108,   110,   111,   112,   113,   114,   115,
   116,     0,    22,    23,    72,    30,     0,     0,    73,     0,
   120,    80,    54,     0,    56,     0,    42,     0,   121,    94,
    95,    88,    89,    93,     0,    32,     0,     0,     0,    52,
     0,   122,    98,     0,    32,    70,    55,     0,    27,     0,
     0,    26,    53,     0,     0,     0
};

static const short yydefgoto[] = {   264,
    30,    31,    32,   169,    33,    34,    92,    93,    51,    52,
    36,    37,    38,    39,    40,    78,    42,    79,   116,   117,
    81,    44
};

static const short yypact[] = {   413,
   -35,-32768,    -4,    -4,   374,-32768,-32768,-32768,-32768,-32768,
-32768,   -18,   374,   -11,     7,    42,    46,   179,    12,    52,
    61,    64,    67,    68,    98,-32768,    84,     1,    99,   413,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    97,
   376,-32768,   101,   103,   372,-32768,-32768,    69,    65,    11,
    28,-32768,   372,    48,   372,    -4,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    71,-32768,    -4,   372,   372,   372,
    80,   372,    -4,    72,    75,    79,    85,    56,-32768,   487,
-32768,   120,-32768,-32768,   162,   267,   267,-32768,-32768,-32768,
   372,   121,-32768,   102,   121,-32768,-32768,   372,   372,   372,
   372,   372,   372,   372,   372,   372,   372,   372,-32768,-32768,
   372,   372,-32768,   121,    80,     4,   911,-32768,-32768,-32768,
   581,   112,   611,   135,    65,   105,   105,   105,    79,    85,
   -25,   641,    -2,     6,    65,    -4,    -4,   372,   372,   372,
-32768,   372,   372,   372,   372,   372,   372,   372,   372,   372,
   372,   372,   372,   372,   372,   372,   372,   372,   372,   372,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    34,-32768,-32768,
-32768,   671,   103,    -1,   911,   911,   911,   911,   911,   911,
   911,   911,   911,   911,   911,   701,   731,   761,   372,-32768,
   374,   372,   374,   372,-32768,   110,   372,-32768,    15,    19,
   791,   821,   851,   940,   968,   995,   300,  1020,   139,   139,
   173,   173,   173,   173,    -8,    -8,    40,    40,   105,   105,
   105,   550,-32768,-32768,-32768,-32768,   114,     8,-32768,   151,
-32768,   911,   125,   881,-32768,   520,-32768,   372,   105,-32768,
-32768,-32768,-32768,-32768,   372,   374,   147,   372,   374,-32768,
    -4,   105,   911,    82,   374,   911,-32768,   146,-32768,   225,
   374,-32768,-32768,   198,   199,-32768
};

static const short yypgoto[] = {-32768,
-32768,   170,   -41,-32768,-32768,-32768,   -27,    31,  -227,    14,
-32768,-32768,-32768,-32768,   -55,     0,-32768,    10,   134,   214,
-32768,   -63
};


#define	YYLAST		1075


static const short yytable[] = {    41,
   124,    95,    49,    50,    41,    47,   196,   133,    90,    43,
    90,    45,    41,    35,    43,   189,   114,   189,   254,   173,
    46,    82,    43,   111,   140,   118,    54,   260,    53,    41,
   155,   156,   157,   158,   159,    55,     3,     1,     2,    43,
   223,   224,     4,    35,   170,   171,   160,    91,   197,   227,
    41,   133,    57,    56,   190,    41,    58,   198,   247,   111,
    43,   118,    83,   111,   120,   240,   125,   111,     3,   241,
   131,    84,   135,    85,     4,     5,    86,    87,    48,   119,
   157,   158,   159,    59,    60,    61,    62,    63,    64,    65,
    89,     1,     2,    12,   160,    13,    14,    15,    16,    17,
    18,    19,    20,    21,   111,   140,    88,    97,    94,    67,
    48,   113,    90,   111,   131,    91,   122,    45,   136,    68,
     3,   137,     3,    69,    70,   138,   115,    72,     4,     5,
   161,   139,   173,   259,    73,   199,   200,    74,    75,     6,
     7,   129,   130,    10,    11,   194,   228,    12,   174,    13,
    14,    15,    16,    17,    18,    19,    20,    21,   192,   160,
   238,   246,    48,   248,    48,   162,   163,   164,   165,   166,
   167,   149,   150,   151,   152,   153,   154,   155,   156,   157,
   158,   159,    59,    60,    61,    62,    63,    64,    65,    66,
    41,   249,    41,   160,   255,   258,   261,   265,   266,    96,
    43,   168,    43,   226,   233,   134,   235,     0,    67,   153,
   154,   155,   156,   157,   158,   159,     0,     0,    68,     3,
     0,     0,    69,    70,     0,    71,    72,   160,     0,     0,
     0,    80,     0,    73,     1,     2,    74,    75,     0,     0,
    76,    77,     0,     0,     0,    41,     0,     0,    41,     0,
    41,     0,     0,    41,    41,    43,     0,     0,    43,    41,
    41,    48,   257,    43,    43,     3,   121,   120,   123,    43,
    43,     4,     5,   120,   263,     0,   262,     0,     0,     0,
     0,   126,   127,   128,   132,     0,     0,     0,     0,     0,
    12,     0,    13,    14,    15,    16,    17,    18,    19,    20,
    21,     0,     0,     0,   172,     0,     0,    48,     0,     0,
     0,   175,   176,   177,   178,   179,   180,   181,   182,   183,
   184,   185,     0,     0,   186,   187,     0,     0,   188,   146,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   158,   159,    22,    23,    24,    25,    26,    27,     0,
     0,   201,   202,   203,   160,   204,   205,   206,   207,   208,
   209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
   219,   220,   221,   222,     0,    59,    60,    61,    62,    63,
    64,    65,     0,     1,     2,     0,     0,     0,    98,    99,
   100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
   110,    67,   232,     0,     0,   234,     0,   236,     0,     0,
   239,    68,     3,     0,     3,    69,    70,     0,   115,    72,
     4,     5,     1,     2,   111,   112,    73,     0,     0,    74,
    75,     0,     0,    76,    77,     0,     0,     0,     0,    12,
     0,    13,    14,    15,    16,    17,    18,    19,    20,    21,
     0,   252,     0,     3,    48,     0,    48,     0,   253,     4,
     5,   256,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     6,     7,     8,     9,    10,    11,    12,     0,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
    23,    24,    25,    26,    27,    28,    29,   141,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   142,   143,   144,   145,   146,   147,   148,   149,
   150,   151,   152,   153,   154,   155,   156,   157,   158,   159,
   251,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,   160,     0,     0,     0,   142,   143,   144,   145,   146,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   158,   159,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   160,   142,   143,   144,   145,   146,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   158,   159,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   160,   245,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,   191,     0,     0,     0,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,   193,     0,     0,     0,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,   195,     0,     0,     0,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,   225,     0,     0,     0,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,     0,     0,   229,     0,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   230,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,   231,     0,     0,     0,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,   242,     0,     0,     0,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,   243,     0,     0,     0,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   244,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,   250,     0,     0,     0,   160,   142,   143,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
   156,   157,   158,   159,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   160,   143,   144,   145,   146,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   158,   159,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   160,   144,   145,   146,   147,   148,
   149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
   159,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   160,   145,   146,   147,   148,   149,   150,   151,
   152,   153,   154,   155,   156,   157,   158,   159,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   160,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   158,   159,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   160
};

static const short yycheck[] = {     0,
    56,    29,     3,     4,     5,    10,     9,    71,    10,     0,
    10,    47,    13,     0,     5,    12,    44,    12,   246,    12,
    56,    10,    13,    49,    50,    51,    13,   255,    47,    30,
    39,    40,    41,    42,    43,    47,    41,    10,    11,    30,
     7,     8,    47,    30,    86,    87,    55,    47,    51,    51,
    51,   115,    11,    47,    51,    56,    11,    52,    51,    49,
    51,    51,    11,    49,    51,    51,    67,    49,    41,    51,
    71,    11,    73,    10,    47,    48,    10,    10,    83,    52,
    41,    42,    43,     4,     5,     6,     7,     8,     9,    10,
     7,    10,    11,    66,    55,    68,    69,    70,    71,    72,
    73,    74,    75,    76,    49,    50,     9,    11,    10,    30,
    83,    11,    10,    49,   115,    47,    69,    47,    47,    40,
    41,    47,    41,    44,    45,    47,    47,    48,    47,    48,
    11,    47,    12,    52,    55,   136,   137,    58,    59,    60,
    61,    62,    63,    64,    65,    11,   174,    66,    47,    68,
    69,    70,    71,    72,    73,    74,    75,    76,    47,    55,
    51,    48,    83,    13,    83,     4,     5,     6,     7,     8,
     9,    33,    34,    35,    36,    37,    38,    39,    40,    41,
    42,    43,     4,     5,     6,     7,     8,     9,    10,    11,
   191,    67,   193,    55,    48,   251,    51,     0,     0,    30,
   191,    40,   193,   173,   191,    72,   193,    -1,    30,    37,
    38,    39,    40,    41,    42,    43,    -1,    -1,    40,    41,
    -1,    -1,    44,    45,    -1,    47,    48,    55,    -1,    -1,
    -1,    18,    -1,    55,    10,    11,    58,    59,    -1,    -1,
    62,    63,    -1,    -1,    -1,   246,    -1,    -1,   249,    -1,
   251,    -1,    -1,   254,   255,   246,    -1,    -1,   249,   260,
   261,    83,   249,   254,   255,    41,    53,   254,    55,   260,
   261,    47,    48,   260,   261,    -1,    52,    -1,    -1,    -1,
    -1,    68,    69,    70,    71,    -1,    -1,    -1,    -1,    -1,
    66,    -1,    68,    69,    70,    71,    72,    73,    74,    75,
    76,    -1,    -1,    -1,    91,    -1,    -1,    83,    -1,    -1,
    -1,    98,    99,   100,   101,   102,   103,   104,   105,   106,
   107,   108,    -1,    -1,   111,   112,    -1,    -1,   115,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    77,    78,    79,    80,    81,    82,    -1,
    -1,   138,   139,   140,    55,   142,   143,   144,   145,   146,
   147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
   157,   158,   159,   160,    -1,     4,     5,     6,     7,     8,
     9,    10,    -1,    10,    11,    -1,    -1,    -1,    13,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    25,    30,   189,    -1,    -1,   192,    -1,   194,    -1,    -1,
   197,    40,    41,    -1,    41,    44,    45,    -1,    47,    48,
    47,    48,    10,    11,    49,    50,    55,    -1,    -1,    58,
    59,    -1,    -1,    62,    63,    -1,    -1,    -1,    -1,    66,
    -1,    68,    69,    70,    71,    72,    73,    74,    75,    76,
    -1,   238,    -1,    41,    83,    -1,    83,    -1,   245,    47,
    48,   248,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    60,    61,    62,    63,    64,    65,    66,    -1,
    68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
    78,    79,    80,    81,    82,    83,    84,    11,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    26,    27,    28,    29,    30,    31,    32,    33,
    34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    55,    -1,    -1,    -1,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    55,    26,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    55,    56,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    51,    -1,    -1,    -1,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    51,    -1,    -1,    -1,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    51,    -1,    -1,    -1,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    51,    -1,    -1,    -1,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    53,    -1,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    54,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    51,    -1,    -1,    -1,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    51,    -1,    -1,    -1,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    51,    -1,    -1,    -1,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    54,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    51,    -1,    -1,    -1,    55,    26,    27,    28,    29,
    30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    55,    27,    28,    29,    30,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    55,    28,    29,    30,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    55,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
    31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    55
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */

#line 3 "bison.simple"



/* Skeleton output parser for bison,

   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.



   This program is free software; you can redistribute it and/or modify

   it under the terms of the GNU General Public License as published by

   the Free Software Foundation; either version 2, or (at your option)

   any later version.



   This program is distributed in the hope that it will be useful,

   but WITHOUT ANY WARRANTY; without even the implied warranty of

   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

   GNU General Public License for more details.



   You should have received a copy of the GNU General Public License

   along with this program; if not, write to the Free Software

   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */



/* As a special exception, when this file is copied by Bison into a

   Bison output file, you may use that output file without restriction.

   This special exception was added by the Free Software Foundation

   in version 1.24 of Bison.  */



#ifndef alloca

#ifdef __GNUC__

#define alloca __builtin_alloca

#else /* not GNU C.  */

#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)

#include <alloca.h>

#else /* not sparc */

#if defined (MSDOS) && !defined (__TURBOC__)

#include <malloc.h>

#else /* not MSDOS, or __TURBOC__ */

#if defined(_AIX)

#include <malloc.h>

 #pragma alloca

#else /* not MSDOS, __TURBOC__, or _AIX */

#ifdef __hpux

#ifdef __cplusplus

extern "C" {

void *alloca (unsigned int);

};

#else /* not __cplusplus */

void *alloca ();

#endif /* not __cplusplus */

#endif /* __hpux */

#endif /* not _AIX */

#endif /* not MSDOS, or __TURBOC__ */

#endif /* not sparc.  */

#endif /* not GNU C.  */

#endif /* alloca not defined.  */



/* This is the parser code that is written into each bison parser

  when the %semantic_parser declaration is not specified in the grammar.

  It was written by Richard Stallman by simplifying the hairy parser

  used when %semantic_parser is specified.  */



/* Note: there must be only one dollar sign in this file.

   It is replaced by the list of actions, each action

   as one case of the switch.  */



#define yyerrok		(yyerrstatus = 0)

#define yyclearin	(yychar = YYEMPTY)

#define YYEMPTY		-2

#define YYEOF		0

#define YYACCEPT	return(0)

#define YYABORT 	return(1)

#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.

   This remains here temporarily to ease the

   transition to the new meaning of YYERROR, for GCC.

   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)



#define YYTERROR	1

#define YYERRCODE	256



#ifndef YYPURE

#define YYLEX		yylex()

#endif



#ifdef YYPURE

#ifdef YYLSP_NEEDED

#ifdef YYLEX_PARAM

#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)

#else

#define YYLEX		yylex(&yylval, &yylloc)

#endif

#else /* not YYLSP_NEEDED */

#ifdef YYLEX_PARAM

#define YYLEX		yylex(&yylval, YYLEX_PARAM)

#else

#define YYLEX		yylex(&yylval)

#endif

#endif /* not YYLSP_NEEDED */

#endif



/* If nonreentrant, generate the variables here */



#ifndef YYPURE



int	yychar;			/*  the lookahead symbol		*/

YYSTYPE	yylval;			/*  the semantic value of the		*/

				/*  lookahead symbol			*/



#ifdef YYLSP_NEEDED

YYLTYPE yylloc;			/*  location data for the lookahead	*/

				/*  symbol				*/

#endif



int yynerrs;			/*  number of parse errors so far       */

#endif  /* not YYPURE */



#if YYDEBUG != 0

int yydebug;			/*  nonzero means print parse trace	*/

/* Since this is uninitialized, it does not stop multiple parsers

   from coexisting.  */

#endif



/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/



#ifndef	YYINITDEPTH

#define YYINITDEPTH 200

#endif



/*  YYMAXDEPTH is the maximum size the stacks can grow to

    (effective only if the built-in stack extension method is used).  */



#if YYMAXDEPTH == 0

#undef YYMAXDEPTH

#endif



#ifndef YYMAXDEPTH

#define YYMAXDEPTH 10000

#endif



/* Prevent warning if -Wstrict-prototypes.  */

#ifdef __GNUC__

int yyparse (void);

#endif



#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */

#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)

#else				/* not GNU C or C++ */

#ifndef __cplusplus



/* This is the most reliable way to avoid incompatibilities

   in available built-in functions on various systems.  */

static void

__yy_memcpy (from, to, count)

     char *from;

     char *to;

     int count;

{

  register char *f = from;

  register char *t = to;

  register int i = count;



  while (i-- > 0)

    *t++ = *f++;

}



#else /* __cplusplus */



/* This is the most reliable way to avoid incompatibilities

   in available built-in functions on various systems.  */

static void

__yy_memcpy (char *from, char *to, int count)

{

  register char *f = from;

  register char *t = to;

  register int i = count;



  while (i-- > 0)

    *t++ = *f++;

}



#endif

#endif



#line 192 "bison.simple"



/* The user can define YYPARSE_PARAM as the name of an argument to be passed

   into yyparse.  The argument should have type void *.

   It should actually point to an object.

   Grammar actions can access the variable by casting it

   to the proper pointer type.  */



#ifdef YYPARSE_PARAM

#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;

#else

#define YYPARSE_PARAM

#define YYPARSE_PARAM_DECL

#endif



int

yyparse(YYPARSE_PARAM)

     YYPARSE_PARAM_DECL

{

  register int yystate;

  register int yyn;

  register short *yyssp;

  register YYSTYPE *yyvsp;

  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */

  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */



  short	yyssa[YYINITDEPTH];	/*  the state stack			*/

  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/



  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */

  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */



#ifdef YYLSP_NEEDED

  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/

  YYLTYPE *yyls = yylsa;

  YYLTYPE *yylsp;



#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)

#else

#define YYPOPSTACK   (yyvsp--, yyssp--)

#endif



  int yystacksize = YYINITDEPTH;



#ifdef YYPURE

  int yychar;

  YYSTYPE yylval;

  int yynerrs;

#ifdef YYLSP_NEEDED

  YYLTYPE yylloc;

#endif

#endif



  YYSTYPE yyval;		/*  the variable used to return		*/

				/*  semantic values from the action	*/

				/*  routines				*/



  int yylen;



#if YYDEBUG != 0

  if (yydebug)

    fprintf(stderr, "Starting parse\n");

#endif



  yystate = 0;

  yyerrstatus = 0;

  yynerrs = 0;

  yychar = YYEMPTY;		/* Cause a token to be read.  */



  /* Initialize stack pointers.

     Waste one element of value and location stack

     so that they stay on the same level as the state stack.

     The wasted elements are never initialized.  */



  yyssp = yyss - 1;

  yyvsp = yyvs;

#ifdef YYLSP_NEEDED

  yylsp = yyls;

#endif



/* Push a new state, which is found in  yystate  .  */

/* In all cases, when you get here, the value and location stacks

   have just been pushed. so pushing a state here evens the stacks.  */

yynewstate:



  *++yyssp = yystate;



  if (yyssp >= yyss + yystacksize - 1)

    {

      /* Give user a chance to reallocate the stack */

      /* Use copies of these so that the &'s don't force the real ones into memory. */

      YYSTYPE *yyvs1 = yyvs;

      short *yyss1 = yyss;

#ifdef YYLSP_NEEDED

      YYLTYPE *yyls1 = yyls;

#endif



      /* Get the current used size of the three stacks, in elements.  */

      int size = yyssp - yyss + 1;



#ifdef yyoverflow

      /* Each stack pointer address is followed by the size of

	 the data in use in that stack, in bytes.  */

#ifdef YYLSP_NEEDED

      /* This used to be a conditional around just the two extra args,

	 but that might be undefined if yyoverflow is a macro.  */

      yyoverflow("parser stack overflow",

		 &yyss1, size * sizeof (*yyssp),

		 &yyvs1, size * sizeof (*yyvsp),

		 &yyls1, size * sizeof (*yylsp),

		 &yystacksize);

#else

      yyoverflow("parser stack overflow",

		 &yyss1, size * sizeof (*yyssp),

		 &yyvs1, size * sizeof (*yyvsp),

		 &yystacksize);

#endif



      yyss = yyss1; yyvs = yyvs1;

#ifdef YYLSP_NEEDED

      yyls = yyls1;

#endif

#else /* no yyoverflow */

      /* Extend the stack our own way.  */

      if (yystacksize >= YYMAXDEPTH)

	{

	  yyerror("parser stack overflow");

	  return 2;

	}

      yystacksize *= 2;

      if (yystacksize > YYMAXDEPTH)

	yystacksize = YYMAXDEPTH;

      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));

      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));

      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));

      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));

#ifdef YYLSP_NEEDED

      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));

      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));

#endif

#endif /* no yyoverflow */



      yyssp = yyss + size - 1;

      yyvsp = yyvs + size - 1;

#ifdef YYLSP_NEEDED

      yylsp = yyls + size - 1;

#endif



#if YYDEBUG != 0

      if (yydebug)

	fprintf(stderr, "Stack size increased to %d\n", yystacksize);

#endif



      if (yyssp >= yyss + yystacksize - 1)

	YYABORT;

    }



#if YYDEBUG != 0

  if (yydebug)

    fprintf(stderr, "Entering state %d\n", yystate);

#endif



  goto yybackup;

 yybackup:



/* Do appropriate processing given the current state.  */

/* Read a lookahead token if we need one and don't already have one.  */

/* yyresume: */



  /* First try to decide what to do without reference to lookahead token.  */



  yyn = yypact[yystate];

  if (yyn == YYFLAG)

    goto yydefault;



  /* Not known => get a lookahead token if don't already have one.  */



  /* yychar is either YYEMPTY or YYEOF

     or a valid token in external form.  */



  if (yychar == YYEMPTY)

    {

#if YYDEBUG != 0

      if (yydebug)

	fprintf(stderr, "Reading a token: ");

#endif

      yychar = YYLEX;

    }



  /* Convert token to internal form (in yychar1) for indexing tables with */



  if (yychar <= 0)		/* This means end of input. */

    {

      yychar1 = 0;

      yychar = YYEOF;		/* Don't call YYLEX any more */



#if YYDEBUG != 0

      if (yydebug)

	fprintf(stderr, "Now at end of input.\n");

#endif

    }

  else

    {

      yychar1 = YYTRANSLATE(yychar);



#if YYDEBUG != 0

      if (yydebug)

	{

	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);

	  /* Give the individual parser a way to print the precise meaning

	     of a token, for further debugging info.  */

#ifdef YYPRINT

	  YYPRINT (stderr, yychar, yylval);

#endif

	  fprintf (stderr, ")\n");

	}

#endif

    }



  yyn += yychar1;

  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)

    goto yydefault;



  yyn = yytable[yyn];



  /* yyn is what to do for this token type in this state.

     Negative => reduce, -yyn is rule number.

     Positive => shift, yyn is new state.

       New state is final state => don't bother to shift,

       just return success.

     0, or most negative number => error.  */



  if (yyn < 0)

    {

      if (yyn == YYFLAG)

	goto yyerrlab;

      yyn = -yyn;

      goto yyreduce;

    }

  else if (yyn == 0)

    goto yyerrlab;



  if (yyn == YYFINAL)

    YYACCEPT;



  /* Shift the lookahead token.  */



#if YYDEBUG != 0

  if (yydebug)

    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);

#endif



  /* Discard the token being shifted unless it is eof.  */

  if (yychar != YYEOF)

    yychar = YYEMPTY;



  *++yyvsp = yylval;

#ifdef YYLSP_NEEDED

  *++yylsp = yylloc;

#endif



  /* count tokens shifted since error; after three, turn off error status.  */

  if (yyerrstatus) yyerrstatus--;



  yystate = yyn;

  goto yynewstate;



/* Do the default action for the current state.  */

yydefault:



  yyn = yydefact[yystate];

  if (yyn == 0)

    goto yyerrlab;



/* Do a reduction.  yyn is the number of a rule to reduce with.  */

yyreduce:

  yylen = yyr2[yyn];

  if (yylen > 0)

    yyval = yyvsp[1-yylen]; /* implement default value of the action */



#if YYDEBUG != 0

  if (yydebug)

    {

      int i;



      fprintf (stderr, "Reducing via rule %d (line %d), ",

	       yyn, yyrline[yyn]);



      /* Print the symbols being reduced, and their result.  */

      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)

	fprintf (stderr, "%s ", yytname[yyrhs[i]]);

      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);

    }

#endif




  switch (yyn) {

case 1:
#line 131 "gsparser.y"
{parserTree=yyvsp[0].node;
    break;}
case 2:
#line 134 "gsparser.y"
{yyval.node = NULL;
    break;}
case 3:
#line 135 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 4:
#line 136 "gsparser.y"
{LinkNodes(yyvsp[-1].node, yyvsp[0].node);
    break;}
case 5:
#line 138 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 6:
#line 139 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 7:
#line 140 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 8:
#line 141 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 9:
#line 144 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_DEFINE, lexLine, 0, LexIdd(yyvsp[-1].str.sz,yyvsp[-1].str.len), yyvsp[0].node);
    break;}
case 10:
#line 145 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_IFDEF, lexLine, 0, LexIdd(yyvsp[-1].str.sz,yyvsp[-1].str.len), yyvsp[0].node);
    break;}
case 11:
#line 146 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_IFNDEF, lexLine, 0, LexIdd(yyvsp[-1].str.sz,yyvsp[-1].str.len), yyvsp[0].node);
    break;}
case 12:
#line 147 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_INCLUDE, lexLine, 0, LexString(yyvsp[0].str.sz,yyvsp[0].str.len));
    break;}
case 13:
#line 148 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ABORT, lexLine, 0);
    break;}
case 14:
#line 149 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_SCOPE, lexLine, 0, yyvsp[0].i);
    break;}
case 15:
#line 152 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_NUL);
    break;}
case 16:
#line 153 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_NUL);
    break;}
case 17:
#line 154 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_PTR);
    break;}
case 18:
#line 155 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_STR);
    break;}
case 19:
#line 156 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_INT, yyvsp[0].i);
    break;}
case 20:
#line 157 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_FLT, yyvsp[0].f);
    break;}
case 21:
#line 158 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_STR, LexString(yyvsp[0].str.sz,yyvsp[0].str.len));
    break;}
case 22:
#line 159 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_INT, -yyvsp[0].i);
    break;}
case 23:
#line 160 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_FLT, -yyvsp[0].f);
    break;}
case 24:
#line 163 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_DECVAR, lexLine, GS_NUL, yyvsp[0].node);
    break;}
case 25:
#line 164 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_DECVAR, lexLine, yyvsp[-1].i, yyvsp[0].node); ;
    break;}
case 26:
#line 167 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_DECFN, lexLine, 0, LexIdd(yyvsp[-6].str.sz,yyvsp[-6].str.len), yyvsp[-4].node, yyvsp[-1].node);
    break;}
case 27:
#line 168 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_DECFN, lexLine, 0, LexIdd(yyvsp[-5].str.sz,yyvsp[-5].str.len), NULL, yyvsp[-1].node);
    break;}
case 28:
#line 169 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_DECFN, lexLine, 0, yyvsp[0].node);
    break;}
case 29:
#line 172 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 30:
#line 173 "gsparser.y"
{LinkNodes(yyvsp[-2].node,yyvsp[0].node);
    break;}
case 31:
#line 176 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_IDD, lexLine, 0, LexIdd(yyvsp[0].str.sz,yyvsp[0].str.len));
    break;}
case 32:
#line 179 "gsparser.y"
{yyval.node = NULL;
    break;}
case 33:
#line 180 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 34:
#line 181 "gsparser.y"
{LinkNodes(yyvsp[-1].node, yyvsp[0].node);
    break;}
case 35:
#line 184 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_BREAK, lexLine, 0);
    break;}
case 36:
#line 185 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_CONTINUE, lexLine, 0);
    break;}
case 37:
#line 186 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_GOTO, lexLine, 0, LexIdd(yyvsp[-1].str.sz,yyvsp[-1].str.len));
    break;}
case 38:
#line 187 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_COD0, lexLine, GSOP_BREAKPOINT);
    break;}
case 39:
#line 188 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_COD0, lexLine, GSOP_STOP);
    break;}
case 40:
#line 189 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_RET, lexLine, 0);
    break;}
case 41:
#line 190 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_RET, lexLine, 1, yyvsp[-1].node);
    break;}
case 42:
#line 191 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_RET, lexLine, 1, yyvsp[-2].node);
    break;}
case 43:
#line 192 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 44:
#line 193 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 45:
#line 194 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 46:
#line 195 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 47:
#line 196 "gsparser.y"
{LinkNodes(yyvsp[-1].node, gsnew gsNode(GSNODE_COD0, lexLine, GSOP_POP));
    break;}
case 48:
#line 197 "gsparser.y"
{yyval.node = yyvsp[-1].node;
    break;}
case 49:
#line 198 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_BLOCK, lexLine, 0, yyvsp[-1].node);
    break;}
case 50:
#line 199 "gsparser.y"
{yyval.node = NULL;
    break;}
case 51:
#line 200 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_LABEL, lexLine, 0, LexIdd(yyvsp[-1].str.sz,yyvsp[-1].str.len));
    break;}
case 52:
#line 203 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_DO, lexLine, 0, yyvsp[-4].node, yyvsp[-1].node);
    break;}
case 53:
#line 206 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_FOR, lexLine, 0, yyvsp[-6].node, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 54:
#line 209 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_IF, lexLine, 0, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 55:
#line 210 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_IF, lexLine, 0, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 56:
#line 213 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_WHILE, lexLine, 0, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 57:
#line 216 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG0, lexLine, GSOP_SET, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 58:
#line 217 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG1, lexLine, GSOP_INC, yyvsp[-1].node);
    break;}
case 59:
#line 218 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG1, lexLine, GSOP_DEC, yyvsp[-1].node);
    break;}
case 60:
#line 219 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_ADD, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 61:
#line 220 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_SUB, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 62:
#line 221 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_MUL, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 63:
#line 222 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_DIV, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 64:
#line 223 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_MOD, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 65:
#line 224 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_AND, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 66:
#line 225 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_OR, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 67:
#line 226 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_XOR, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 68:
#line 227 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_SHR, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 69:
#line 228 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG2, lexLine, GSOP_SHL, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 70:
#line 229 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_ASG3, lexLine, GSOP_SETI, yyvsp[-5].node, yyvsp[-3].node, yyvsp[0].node);
    break;}
case 71:
#line 232 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 72:
#line 233 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_VAR, lexLine, 1, yyvsp[-1].node);
    break;}
case 73:
#line 234 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_IDX, lexLine, 0, yyvsp[-3].node, yyvsp[-1].node);
    break;}
case 74:
#line 235 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_GET, yyvsp[0].node );
    break;}
case 75:
#line 236 "gsparser.y"
{yyval.node = yyvsp[-1].node;
    break;}
case 76:
#line 239 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_VAR, lexLine, 0, LexIdd(yyvsp[0].str.sz,yyvsp[0].str.len));
    break;}
case 77:
#line 242 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_CALL, lexLine, 0, LexIdd(yyvsp[-3].str.sz,yyvsp[-3].str.len), yyvsp[-1].node);
    break;}
case 78:
#line 245 "gsparser.y"
{yyval.node = NULL;
    break;}
case 79:
#line 246 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 80:
#line 247 "gsparser.y"
{LinkNodes(yyvsp[-2].node, yyvsp[0].node);
    break;}
case 81:
#line 250 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_NUL);
    break;}
case 82:
#line 251 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_PTR);
    break;}
case 83:
#line 252 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_STR);
    break;}
case 84:
#line 253 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_INT, yyvsp[0].i);
    break;}
case 85:
#line 254 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_FLT, yyvsp[0].f);
    break;}
case 86:
#line 255 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_STR, LexString(yyvsp[0].str.sz,yyvsp[0].str.len));
    break;}
case 87:
#line 256 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OBJ, lexLine, GS_TAB, yyvsp[-1].node);
    break;}
case 88:
#line 257 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_NEW, lexLine, GS_STR, yyvsp[-1].node);
    break;}
case 89:
#line 258 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_NEW, lexLine, GS_TAB, yyvsp[-1].node);
    break;}
case 90:
#line 259 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_GET, yyvsp[0].node);
    break;}
case 91:
#line 260 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 92:
#line 261 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_TYPEOF, yyvsp[0].node);
    break;}
case 93:
#line 262 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_GETI, yyvsp[-3].node, yyvsp[-1].node);
    break;}
case 94:
#line 263 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_TYPEOF, yyvsp[-1].node);
    break;}
case 95:
#line 264 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_SIZEOF, yyvsp[-1].node);
    break;}
case 96:
#line 265 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 97:
#line 266 "gsparser.y"
{yyval.node = yyvsp[0].node;
    break;}
case 98:
#line 267 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_IFEXP, lexLine, 0, yyvsp[-4].node, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 99:
#line 268 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_OROR, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 100:
#line 269 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_ANDAND, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 101:
#line 270 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_OR, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 102:
#line 271 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_XOR, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 103:
#line 272 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_AND, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 104:
#line 273 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPE, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 105:
#line 274 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPNE, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 106:
#line 275 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPL, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 107:
#line 276 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPG, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 108:
#line 277 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPGE, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 109:
#line 278 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_CPLE, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 110:
#line 279 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_SHL, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 111:
#line 280 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_SHR, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 112:
#line 281 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_ADD, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 113:
#line 282 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_SUB, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 114:
#line 283 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_MUL, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 115:
#line 284 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_DIV, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 116:
#line 285 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP2, lexLine, GSOP_MOD, yyvsp[-2].node, yyvsp[0].node);
    break;}
case 117:
#line 286 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_SGN, yyvsp[0].node);
    break;}
case 118:
#line 287 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_NOT, yyvsp[0].node);
    break;}
case 119:
#line 288 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_OP1, lexLine, GSOP_INV, yyvsp[0].node);
    break;}
case 120:
#line 289 "gsparser.y"
{yyval.node = yyvsp[-1].node;
    break;}
case 121:
#line 292 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_CAST, lexLine, yyvsp[-2].i, yyvsp[0].node);
    break;}
case 122:
#line 293 "gsparser.y"
{yyval.node = gsnew gsNode(GSNODE_CAST, lexLine, yyvsp[-3].i, LexString(yyvsp[-2].str.sz,yyvsp[-2].str.len), yyvsp[0].node);
    break;}
case 123:
#line 296 "gsparser.y"
{yyval.i = GS_INT;
    break;}
case 124:
#line 297 "gsparser.y"
{yyval.i = GS_FLT;
    break;}
case 125:
#line 298 "gsparser.y"
{yyval.i = GS_STR;
    break;}
case 126:
#line 299 "gsparser.y"
{yyval.i = GS_TAB;
    break;}
case 127:
#line 300 "gsparser.y"
{yyval.i = GS_REF;
    break;}
case 128:
#line 301 "gsparser.y"
{yyval.i = GS_PTR;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */

#line 487 "bison.simple"



  yyvsp -= yylen;

  yyssp -= yylen;

#ifdef YYLSP_NEEDED

  yylsp -= yylen;

#endif



#if YYDEBUG != 0

  if (yydebug)

    {

      short *ssp1 = yyss - 1;

      fprintf (stderr, "state stack now");

      while (ssp1 != yyssp)

	fprintf (stderr, " %d", *++ssp1);

      fprintf (stderr, "\n");

    }

#endif



  *++yyvsp = yyval;



#ifdef YYLSP_NEEDED

  yylsp++;

  if (yylen == 0)

    {

      yylsp->first_line = yylloc.first_line;

      yylsp->first_column = yylloc.first_column;

      yylsp->last_line = (yylsp-1)->last_line;

      yylsp->last_column = (yylsp-1)->last_column;

      yylsp->text = 0;

    }

  else

    {

      yylsp->last_line = (yylsp+yylen-1)->last_line;

      yylsp->last_column = (yylsp+yylen-1)->last_column;

    }

#endif



  /* Now "shift" the result of the reduction.

     Determine what state that goes to,

     based on the state we popped back to

     and the rule number reduced by.  */



  yyn = yyr1[yyn];



  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;

  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)

    yystate = yytable[yystate];

  else

    yystate = yydefgoto[yyn - YYNTBASE];



  goto yynewstate;



yyerrlab:   /* here on detecting error */



  if (! yyerrstatus)

    /* If not already recovering from an error, report this error.  */

    {

      ++yynerrs;



#ifdef YYERROR_VERBOSE

      yyn = yypact[yystate];



      if (yyn > YYFLAG && yyn < YYLAST)

	{

	  int size = 0;

	  char *msg;

	  int x, count;



	  count = 0;

	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */

	  for (x = (yyn < 0 ? -yyn : 0);

	       x < (sizeof(yytname) / sizeof(char *)); x++)

	    if (yycheck[x + yyn] == x)

	      size += (int)strlen(yytname[x]) + 15, count++;

	  msg = (char *) malloc(size + 15);

	  if (msg != 0)

	    {

	      strcpy(msg, "parse error");



	      if (count < 5)

		{

		  count = 0;

		  for (x = (yyn < 0 ? -yyn : 0);

		       x < (sizeof(yytname) / sizeof(char *)); x++)

		    if (yycheck[x + yyn] == x)

		      {

			strcat(msg, count == 0 ? ", expecting `" : " or `");

			strcat(msg, yytname[x]);

			strcat(msg, "'");

			count++;

		      }

		}

	      yyerror(msg);

	      free(msg);

	    }

	  else

	    yyerror ("parse error; also virtual memory exceeded");

	}

      else

#endif /* YYERROR_VERBOSE */

	yyerror("parse error");

    }



  goto yyerrlab1;

yyerrlab1:   /* here on error raised explicitly by an action */



  if (yyerrstatus == 3)

    {

      /* if just tried and failed to reuse lookahead token after an error, discard it.  */



      /* return failure if at end of input */

      if (yychar == YYEOF)

	YYABORT;



#if YYDEBUG != 0

      if (yydebug)

	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);

#endif



      yychar = YYEMPTY;

    }



  /* Else will try to reuse lookahead token

     after shifting the error token.  */



  yyerrstatus = 3;		/* Each real token shifted decrements this */



  goto yyerrhandle;



yyerrdefault:  /* current state does not do anything special for the error token. */



#if 0

  /* This is wrong; only states that explicitly want error tokens

     should shift them.  */

  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/

  if (yyn) goto yydefault;

#endif



yyerrpop:   /* pop the current state because it cannot handle the error token */



  if (yyssp == yyss) YYABORT;

  yyvsp--;

  yystate = *--yyssp;

#ifdef YYLSP_NEEDED

  yylsp--;

#endif



#if YYDEBUG != 0

  if (yydebug)

    {

      short *ssp1 = yyss - 1;

      fprintf (stderr, "Error: state stack now");

      while (ssp1 != yyssp)

	fprintf (stderr, " %d", *++ssp1);

      fprintf (stderr, "\n");

    }

#endif



yyerrhandle:



  yyn = yypact[yystate];

  if (yyn == YYFLAG)

    goto yyerrdefault;



  yyn += YYTERROR;

  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)

    goto yyerrdefault;



  yyn = yytable[yyn];

  if (yyn < 0)

    {

      if (yyn == YYFLAG)

	goto yyerrpop;

      yyn = -yyn;

      goto yyreduce;

    }

  else if (yyn == 0)

    goto yyerrpop;



  if (yyn == YYFINAL)

    YYACCEPT;



#if YYDEBUG != 0

  if (yydebug)

    fprintf(stderr, "Shifting error token, ");

#endif



  *++yyvsp = yylval;

#ifdef YYLSP_NEEDED

  *++yylsp = yylloc;

#endif



  yystate = yyn;

  goto yynewstate;

}

#line 304 "gsparser.y"



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
