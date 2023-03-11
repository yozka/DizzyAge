%option nomain
%option noyywrap

%{

/* ------------------------------------------------------------------
   Initial code (copied verbatim to the output file)
   ------------------------------------------------------------------ */

// Includes
#include <string.h>		// strcpy
#include <io.h>			// isatty
#include "gslexerdef.h"
#include "gslexer.h"

#ifdef MSVC
#define  isatty _isatty  // for some reason isatty is called _isatty in VC..
#endif

// Forward references
void	LexCommentOut();
float	LexFloat();
int		LexHex();
int		LexBin();

int		lexLine = 1;		// line index

%}

/* ------------------------------------------------------------------
   Some macros (standard regular expressions)
   ------------------------------------------------------------------ */

DIGIT		[0-9]
INTEGER		{DIGIT}+
FLOAT		({INTEGER}\.{INTEGER})|({INTEGER}(\.{INTEGER})?[eE][+-]?{INTEGER})
LETTER		[A-Za-z_]
IDDTAIL		[A-Za-z0-9_]
IDD			{LETTER}{IDDTAIL}*
SPACE		[ \t\v\f]
SPACENL		[ \t\v\f\r\n]
HEXDIGIT	[a-fA-F0-9]
BINDIGIT	[01]

/* ------------------------------------------------------------------
   The lexer rules
   ------------------------------------------------------------------ */
%%
							;
{SPACE}+					;
"/*"						{ LexCommentOut(); }
"//"[^\n\r]*				;
[\r]						;
[\n]						{ lexLine++;}
\"(\\.|\"{SPACENL}*\"|[^\\"\r\n])*\"		{ yylval.str.sz = yytext; yylval.str.len = yyleng; return(TOK_STRVAL); }
`(\\.|`{SPACENL}*`|[^\\`\r\n])*`			{ yylval.str.sz = yytext; yylval.str.len = yyleng; return(TOK_STRVAL); }
\'.\'						{ yylval.i = yytext[1]; return(TOK_INTVAL); }
"=="					    { return(TOK_CPE); }
"!="					    { return(TOK_CPNE); }
">="					    { return(TOK_CPGE); }
"<="					    { return(TOK_CPLE); }
"&&"					    { return(TOK_ANDAND); }
"||"					    { return(TOK_OROR); }
"<<"					    { return(TOK_SHL); }
">>"					    { return(TOK_SHR); }
"++"					    { return(TOK_ASGINC); }
"--"					    { return(TOK_ASGDEC); }
"+="					    { return(TOK_ASGADD); }
"-="					    { return(TOK_ASGSUB); }
"*="					    { return(TOK_ASGMUL); }
"/="					    { return(TOK_ASGDIV); }
"%="					    { return(TOK_ASGMOD); }
"&="					    { return(TOK_ASGAND); }
"|="					    { return(TOK_ASGOR); }
"^="					    { return(TOK_ASGXOR); }
">>="					    { return(TOK_ASGSHR); }
"<<="					    { return(TOK_ASGSHL); }
"[["					    { return(TOK_BRKL); }
"]]"					    { return(TOK_BRKR); }
"?" |
"[" |
"]" |
"(" | 
")" | 
"+" | 
"-" | 
"*" | 
"/" |
"%" |
"<" | 
">" |  
"." | 
"!" |
":" | 
";" |
"{" | 
"}" | 
"," |
"&" |
"|" |
"^" |
"~" |
"@" |
"$" |
"#" |
"="							{ return(yytext[0]); }
"#abort"					{ return(TOK_ABORT); }
"#def"						{ return(TOK_DEFINE); }
"#ifdef"					{ return(TOK_IFDEF); }
"#ifndef"					{ return(TOK_IFNDEF); }
"#include"					{ return(TOK_INCLUDE); }
"#scope"					{ return(TOK_SCOPE); }
"break"						{ return(TOK_BREAK); }
"breakpoint"				{ return(TOK_BREAKPOINT); }
"continue"					{ return(TOK_CONTINUE); }
"do"						{ return(TOK_DO); }
"else"						{ return(TOK_ELSE); }
"false"						{ yylval.i = 0; return TOK_INTVAL; }
"float"						{ return(TOK_FLT); }
"for"						{ return(TOK_FOR); }
"func"						{ return(TOK_FUNC); }
"goto"						{ return(TOK_GOTO); }
"if"						{ return(TOK_IF); }
"int"						{ return(TOK_INT); }
"NUL"						{ return(TOK_NULVAL); }
"NULL"						{ return(TOK_NULLVAL); }
"ptr"						{ return(TOK_PTR); }
"ref"						{ return(TOK_REF); }
"return"					{ return(TOK_RETURN); }
"sizeof"					{ return(TOK_SIZEOF); }
"stop"						{ return(TOK_STOP); }
"str"						{ return(TOK_STR); }
"tab"						{ return(TOK_TAB); }
"true"						{ yylval.i = 1; return TOK_INTVAL; }
"typeof"					{ return(TOK_TYPEOF); }
"var"						{ return(TOK_VAR); }
"VOID"						{ return(TOK_VOIDVAL); }
"while"						{ return(TOK_WHILE); }
{IDD}						{ yylval.str.sz = yytext; yylval.str.len = yyleng; return(TOK_IDD); }
0[xX]{HEXDIGIT}+			{ yylval.i = LexHex(); return(TOK_INTVAL); }
0[bB]{BINDIGIT}+			{ yylval.i = LexBin(); return(TOK_INTVAL); }
{INTEGER}					{ yytext[yyleng] = 0; yylval.i = atoi(yytext); return TOK_INTVAL; }
{FLOAT}						{ yylval.f = LexFloat(); return(TOK_FLTVAL); }
.							{ return(TOK_ERROR); }

%%


/* ------------------------------------------------------------------
   Additional code (again copied verbatim to the output file)
   ------------------------------------------------------------------ */

void LexCommentOut()
{
    int c;
    for(;;)
	{
		c = yyinput();
		if(c=='\n') lexLine++;
        while(c!='*' && c!=EOF)
		{
			c = yyinput();
			if(c=='\n') lexLine++;
		}

        if(c=='*')
		{
			c = yyinput();
			if(c=='\n') lexLine++;
			while(c=='*')
			{
				c = yyinput();
				if(c=='\n') lexLine++;
			}
            if(c=='/') break;
		}

        if(c==EOF) break;
    }
}

float LexFloat()
{
	yytext[yyleng] = 0;
	return (float)atof(yytext);
}

int LexHex()
{
	int val = 0;
	sscanf(yytext, "%x", &val);
	return val;
}

int LexBin()
{
	int val = 0;
	for(int i=yyleng-1;i>=2;i--)
		if(yytext[i]=='1') val += (1 << ((yyleng-1)-i));
	return val;
}

YY_BUFFER_STATE	YYScanBuffer( char* buffer, int size )
{
	return yy_scan_buffer(buffer, size);
}

void YYDeleteBuffer( YY_BUFFER_STATE yybs )
{
	if(yybs) yy_delete_buffer(yybs);
}
