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


extern YYSTYPE yylval;
