/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* TOKEN NAME DEFINITIONS */

#include	"idf.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"tokenname.h"
#include	"Lpars.h"

/*	To centralize the declaration of %tokens, their presence in this
	file is taken as their declaration. The Makefile will produce
	a grammar file (tokenfile.g) from this file.
	Moreover, rather than looking up a symbol in all these lists
	to find its printable name, a fast version of symbol2str() is
	generated from these tables.
	Consequenty some of these tables are not referenced explicitly
	in the C text any more.  To save space and to avoid lint confusion,
	these have been made pseudo-invisible by #ifdefs.
*/

#ifdef	____
struct tokenname tkspec[] =	{	/* the names of the special tokens */
	{IDENTIFIER, "identifier"},
	{TYPE_IDENTIFIER, "type_identifier"},
	{STRING, "string"},
	{FILESPECIFIER, "filespecifier"},
	{INTEGER, "integer"},
	{FLOATING, "floating"},
	{0, ""}
};
#endif	/* ____ */

#ifdef	____
struct tokenname tkcomp[] =	{	/* names of the composite tokens */
	{NOTEQUAL, "!="},
	{AND, "&&"},
	{PLUSPLUS, "++"},
	{MINMIN, "--"},
	{ARROW, "->"},
	{LEFT, "<<"},
	{LESSEQ, "<="},
	{EQUAL, "=="},
	{GREATEREQ, ">="},
	{RIGHT, ">>"},
	{OR, "||"},
	{0, ""}
};
#endif	/* ____ */

struct tokenname tkidf[] =	{	/* names of the identifier tokens */
	{ASM, "asm"},
	{AUTO, "auto"},
	{BREAK, "break"},
	{CASE, "case"},
	{CONTINUE, "continue"},
	{DEFAULT, "default"},
	{DO, "do"},
	{ELSE, "else"},
	{ENUM, "enum"},
	{EXTERN, "extern"},
	{FOR, "for"},
	{GOTO, "goto"},
	{IF, "if"},
	{LONG, "long"},
	{REGISTER, "register"},
	{RETURN, "return"},
	{SHORT, "short"},
	{SIZEOF, "sizeof"},
	{STATIC, "static"},
	{STRUCT, "struct"},
	{SWITCH, "switch"},
	{TYPEDEF, "typedef"},
	{UNION, "union"},
	{UNSIGNED, "unsigned"},
	{WHILE, "while"},
	{0, ""}
};

struct tokenname tkother[] =	{	/* additional keywords from the RM */
	{ENTRY, "entry"},
	{FORTRAN, "fortran"},
	{0, ""}
};

#ifdef	____
struct tokenname tkfunny[] =	{	/* internal keywords */
	{CHAR, "char"},
	{INT, "int"},
	{FLOAT, "float"},
	{DOUBLE, "double"},
	{VOID, "void"},

	{ARRAY, "array"},
	{FUNCTION, "function"},
	{POINTER, "pointer"},
	{FIELD, "field"},
	{NEWLINE, "newline"},

	{GLOBAL, "global"},
	{IMPLICIT, "implicit"},
	{FORMAL, "formal"},
	{LABEL, "label"},
	{ERRONEOUS, "erroneous"},

	{PARCOMMA, "parcomma"},
	{INITCOMMA, "initcomma"},
	{CAST, "cast"},
	{POSTINCR, "postfix ++"},
	{POSTDECR, "postfix --"},
	{PLUSAB, "+="},
	{MINAB, "-="},
	{TIMESAB, "*="},
	{DIVAB, "/="},
	{MODAB, "%="},
	{LEFTAB, "<<="},
	{RIGHTAB, ">>="},
	{ANDAB, "&="},
	{XORAB, "^="},
	{ORAB, "|="},

	{INT2INT, "int2int"},
	{INT2FLOAT, "int2float"},
	{FLOAT2INT, "float2int"},
	{FLOAT2FLOAT, "float2float"},
	{0, ""}
};
#endif	/* ____ */

reserve(resv)
	register struct tokenname resv[];
{
	/*	The names of the tokens described in resv are entered
		as reserved words.
	*/
	while (resv->tn_symbol)	{
		struct idf *idf = str2idf(resv->tn_name);
		
		if (idf->id_reserved)
			fatal("maximum identifier length insufficient");
		idf->id_reserved = resv->tn_symbol;
		resv++;
	}
}
