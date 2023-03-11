#ifndef _GSPARSER_
#define _GSPARSER_
#include "gslexer.h"
#include "gsnode.h"

gsNode*	YYParse();		// wrapper for yyparse; return parser tree if successful or NULL if failed
int		YYError();		// return error code after a failed call to YYParse; (0=ok, 1=error)
char*	YYErrorText();	// return error description after a failed call to YYParse; (debug mode required on parser)
int		YYErrorLine();	// return error line after a failed call to YYParse;

#endif

