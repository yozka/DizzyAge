#ifndef _GSLEXER_
#define _GSLEXER_
#include "gsutil.h"

typedef struct	yy_buffer_state *YY_BUFFER_STATE;				// buffer state pointer

YY_BUFFER_STATE	YYScanBuffer	( char* buffer, int size );		// use this to set a buffer for parseing
void			YYDeleteBuffer	( YY_BUFFER_STATE yybs );		// delet a buffer after parseing

		int		yylex();			// lexer function used by the parser
#endif