/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* D E F I N I T I O N S   F O R   T H E   L E X I C A L   A N A L Y Z E R */

/*	A token from the input stream is represented by an integer,
	called a "symbol", but it may have other information associated
	to it.
*/

/* the structure of a token:	*/
struct token	{
	int tok_symb;		/* the token itself */
	union	{
		char *tok_str;
		struct {
			int tok_unsigned;
			arith tok_val;		/* for INTEGER */
		} tok_int;
	} tok_data;
};

#include "file_info.h"

#define tk_symb	tok_symb
#define tk_str	tok_data.tok_str
#define tk_val	tok_data.tok_int.tok_val
#define tk_unsigned tok_data.tok_int.tok_unsigned

extern struct token dot;

extern int ReplaceMacros;	/* "LLlex.c"	*/
extern int AccDefined;		/* "LLlex.c"	*/
extern int Unstacked;		/* "LLlex.c"	*/
extern int UnknownIdIsZero;	/* "LLlex.c"	*/
extern int AccFileSpecifier;    /* "LLlex.c"    */

extern int NoUnstack;		/* buffer.c	*/

extern int err_occurred;	/* "error.c"	*/

#define	DOT	dot.tk_symb

#define EOF	(-1)
