/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* TOKEN NAME DEFINITIONS */

#include	"parameters.h"
#include    "idf.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"tokenname.h"
#include	"Lpars.h"
#include    "error.h"

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
	{WCHAR, "wchar"},
	{FILESPECIFIER, "filespecifier"},
	{INTEGER, "integer"},
	{FLOATING, "floating"},
	{0, ""}
};
#endif	/* ____ */

#ifdef	____
struct tokenname tkcomp[] =	{	/* names of the composite tokens */
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
	{ELLIPSIS, "..."},
	{0, ""}
};
#endif	/* ____ */

struct tokenname tkidf[] =	{	/* names of the identifier tokens */
	{AUTO, "auto"},
	{BREAK, "break"},
	{CASE, "case"},
	{CONST, "const"},
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
	{SIGNED, "signed"},
	{SIZEOF, "sizeof"},
	{STATIC, "static"},
	{STRUCT, "struct"},
	{SWITCH, "switch"},
	{TYPEDEF, "typedef"},
	{UNION, "union"},
	{UNSIGNED, "unsigned"},
	{VOLATILE, "volatile"},
	{WHILE, "while"},

	{VOID, "void"},
	{CHAR, "char"},
	{INT, "int"},
	{FLOAT, "float"},
	{DOUBLE, "double"},
	{0, ""}
};

#ifdef	____
struct tokenname tkfunny[] =	{	/* internal keywords */
	{LNGLNG, "long long"},
	{LNGDBL, "long double"},
	{ULONG, "unsigned long"},
	{ULNGLNG, "unsigned long long"},

	{ARRAY, "array"},
	{FUNCTION, "function"},
	{POINTER, "pointer"},
	{FIELD, "field"},

	{GLOBAL, "global"},
	{FORMAL, "formal"},
	{LABEL, "label"},
	{ERRONEOUS, "erroneous"},

	{PARCOMMA, "parcomma"},
	{INITCOMMA, "initcomma"},
	{CAST, "cast"},
	{CASTAB, "castab"},
	{ADDRESSOF,"unary &"},
	{POSTINCR, "postfix ++"},
	{POSTDECR, "postfix --"},

	{INT2INT, "int2int"},
	{INT2FLOAT, "int2float"},
	{FLOAT2INT, "float2int"},
	{FLOAT2FLOAT, "float2float"},
	{0, ""}
};
#endif	/* ____ */

void reserve(register struct tokenname resv[])
{
	/*	The names of the tokens described in resv are entered
		as reserved words.
	*/
	while (resv->tn_symbol)	{
		struct idf *idf = str2idf(resv->tn_name, 0);
		
		if (idf->id_reserved)
			fatal("maximum identifier length insufficient");
		idf->id_reserved = resv->tn_symbol;
		resv++;
	}
}
