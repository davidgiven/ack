/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* PREPROCESSOR: PRAGMA INTERPRETER */

#include	"arith.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"debug.h"
#include	"idf.h"
#include	"input.h"
#include	"ifdepth.h"	
#include	"botch_free.h"	
#include	"nparams.h"	
#include	"parbufsize.h"	
#include	"textsize.h"	
#include	"idfsize.h"	
#include	"assert.h"
#include	<alloc.h>
#include	"class.h"
#include	"macro.h"

#define P_UNKNOWN	0
#define P_FLAGS		1

struct pkey {
	char *pk_name;
	int pk_key;
} pragmas[] = {
	{"cemflags",	P_FLAGS},
	{0,		P_UNKNOWN}
};

extern struct idf *GetIdentifier();

do_pragma()
{
	register struct pkey *pkp;
	register struct idf *id;
	struct token tk;

	if ((id = GetIdentifier(1)) != (struct idf *)0) {
		/*	Lineair search - why bother ?
		*/
		for (pkp = &pragmas[0]; pkp->pk_key != P_UNKNOWN; pkp++)
			if (strcmp(pkp->pk_name, id->id_text) == 0)
				break;

		switch (pkp->pk_key) {
		case P_FLAGS:
			if (GetToken(&tk) == STRING)
				do_option(tk.tk_bts);
			break;

		case P_UNKNOWN:
		default:
			break;
		}
		SkipToNewLine();
	}
}
