/* $Header$ */
/* PREPROCESSOR: MACRO-TEXT REPLACEMENT ROUTINES */

#include	"nopp.h"

#ifndef NOPP
#include	"debug.h"	/* UF */
#include	"pathlength.h"	/* UF */
#include	"strsize.h"	/* UF */

#include	"string.h"
#include	"alloc.h"
#include	"idf.h"
#include	"input.h"
#include	"macro.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"class.h"
#include	"assert.h"
#include	"interface.h"

EXPORT int
replace(idef)
	struct idf *idef;
{
	/*	replace() is called by the lexical analyzer to perform
		macro replacement.  "idef" is the description of the
		identifier which leads to the replacement.  If the
		optional actual parameters of the macro are OK, the text
		of the macro is prepared to serve as an input buffer,
		which is pushed onto the input stack.
		replace() returns 1 if the replacement succeeded and 0 if
		some error has occurred.
	*/
	register char c;
	register char flags = idef->id_macro->mc_flag;
	char **actpars, **getactuals();
	char *reptext, *macro2buffer();
	int size;

	if (idef->id_macro->mc_nps != -1) {	/* with parameter list	*/
		LoadChar(c);
		c = skipspaces(c);

		if (c != '(') {		/* no replacement if no ()	*/
			lexerror("(warning) macro %s needs arguments",
				idef->id_text);
			PushBack();
			return 0;
		}

		actpars = getactuals(idef);	/* get act.param. list	*/
	}

	if (flags & PREDEF) {	/* don't replace this one...	*/
		return 0;
	}

	if (flags & FUNC) {	/* this macro leads to special action	*/
		macro_func(idef);
	}

	/* create and input buffer	*/
	reptext = macro2buffer(idef, actpars, &size);
	InsertText(reptext, size);

	return 1;
}

PRIVATE
macro_func(idef)
	struct idf *idef;
{
	/*	macro_func() performs the special actions needed with some
		macros.  These macros are __FILE__ and __LINE__ which
		replacement texts must be evaluated at the time they are
		used.
	*/
	static char FilNamBuf[PATHLENGTH];

	/* This switch is very blunt...	*/
	switch (idef->id_text[2]) {

	case 'F' :			/* __FILE__	*/
		FilNamBuf[0] = '"';
		strcpy(&FilNamBuf[1], FileName);
		strcat(FilNamBuf, "\"");
		idef->id_macro->mc_text = FilNamBuf;
		idef->id_macro->mc_length = strlen(FilNamBuf);
		break;

	case 'L' :			/* __LINE__	*/
		idef->id_macro->mc_text = itos(LineNumber);
		idef->id_macro->mc_length = 1;
		break;

	default :
		crash("(macro_func) illegal macro %s\n", idef->id_text);

	}
}

PRIVATE char *
macro2buffer(idef, actpars, siztext)
	struct idf *idef;
	char **actpars;
	int *siztext;
{
	/*	Macro2buffer() turns the macro replacement text, as it is
		stored, into an input buffer, while each occurrence of the
		non-ascii formal parameter mark is replaced by its
		corresponding actual parameter specified in the actual
		parameter list actpars.  A pointer to the beginning of the
		constructed text is returned, while *siztext is filled
		with its length.

		If there are no parameters, this function behaves
		the same as strcpy().
	*/
	register int size = 8;
	register char *text = Malloc(size);
	register pos = 0;
	register char *ptr = idef->id_macro->mc_text;

	text[pos++] = '\0';			/* allow pushback	*/

	while (*ptr) {
		if (*ptr & FORMALP) {	/* non-asc formal param. mark	*/
			register int n = *ptr++ & 0177;
			register char *p;

			ASSERT(n != 0);

			/*	copy the text of the actual parameter
				into the replacement text
			*/
			for (p = actpars[n - 1]; *p; p++) {
				text[pos++] = *p;

				if (pos == size) {
					text = Srealloc(text,
							size += RSTRSIZE);
				}
			}
		}
		else {
			text[pos++] = *ptr++;

			if (pos == size) {
				text = Srealloc(text, size += RSTRSIZE);
			}
		}
	}

	text[pos] = '\0';
	*siztext = pos;
	return text;
}
#endif NOPP
