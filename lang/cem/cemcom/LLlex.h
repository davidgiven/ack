/* $Header$ */
/* D E F I N I T I O N S   F O R   T H E   L E X I C A L   A N A L Y Z E R */

/*	A token from the input stream is represented by an integer,
	called a "symbol", but it may have other information associated
	to it.
*/

/* the structure of a token:	*/
struct token	{
	int tok_symb;		/* the token itself */
	char *tok_file;		/* the file it (probably) comes from */
	unsigned int tok_line;	/* the line it (probably) comes from */
	union	{
		struct idf *tok_idf;	/* for IDENTIFIER & TYPE_IDENTIFIER */
		struct	{		/* for STRING */
			char *tok_bts;	/* row of bytes	*/
			int tok_len;	/* length of row of bytes */
		} tok_string;
		struct	{		/* for INTEGER */
			int tok_fund;	/* INT or LONG */
			arith tok_ival;
		} tok_integer;
		char *tok_fval;
	} tok_data;
};

#define tk_symb	tok_symb
#define tk_file	tok_file
#define tk_line	tok_line
#define tk_idf	tok_data.tok_idf
#define tk_bts	tok_data.tok_string.tok_bts
#define tk_len	tok_data.tok_string.tok_len
#define tk_fund	tok_data.tok_integer.tok_fund
#define tk_ival	tok_data.tok_integer.tok_ival
#define tk_fval	tok_data.tok_fval

extern struct token dot, ahead, aside;
extern unsigned int LineNumber;	/* "LLlex.c"	*/
extern char *FileName;		/* "LLlex.c"	*/

extern int ReplaceMacros;	/* "LLlex.c"	*/
extern int EoiForNewline;	/* "LLlex.c"	*/
extern int PreProcKeys;		/* "LLlex.c"	*/
extern int AccFileSpecifier;	/* "LLlex.c"	*/
extern int AccDefined;		/* "LLlex.c"	*/
extern int UnknownIdIsZero;	/* "LLlex.c"	*/
extern int SkipEscNewline;	/* "LLlex.c"	*/

extern int NoUnstack;		/* buffer.c	*/

extern int err_occurred;	/* "error.c"	*/

#define	DOT	dot.tk_symb
#define	AHEAD	ahead.tk_symb
#define	ASIDE	aside.tk_symb

#define EOF	(-1)
