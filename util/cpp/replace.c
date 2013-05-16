/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* PREPROCESSOR: MACRO-TEXT REPLACEMENT ROUTINES */

#include <stdlib.h>
#include <string.h>
#include	"debug.h"	/* UF */
#include	"pathlength.h"	/* UF */
#include	"textsize.h"	/* UF */

#include	<alloc.h>
#include	<assert.h>
#include	"idf.h"
#include	"input.h"
#include	"macro.h"
#include	"LLlex.h"
#include	"class.h"
#include	"interface.h"

char *strcpy(), *strcat();
char *long2str();
extern int InputLevel;

PRIVATE struct mlist *ReplList;	/* list of currently active macros */

EXPORT int
replace(idef)
	register struct idf *idef;
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
	register struct macro *mac = idef->id_macro;
	register char c;
	char **actpars, **getactuals();
	char *reptext, *macro2buffer();
	register struct mlist *repl;
	int size;

	if (mac->mc_flag & NOREPLACE) {
		warning("macro %s is recursive", idef->id_text);
		return 0;
	}
	if (mac->mc_nps != -1) {	/* with parameter list	*/
		if (mac->mc_flag & FUNC) {
					/* must be "defined".
					   Unfortunately, the next assertion
					   will not compile ...
			assert( ! strcmp("defined", idef->id_text));
					*/
			if (! AccDefined)
				return 0;
		}
		if (++mac->mc_count > 100) {
			/* 100 must be some number in parameters.h */
			warning("macro %s is assumed recursive",
				    idef->id_text);
			return 0;
		}
		LoadChar(c);
		c = skipspaces(c,! (mac->mc_flag & FUNC));
		if (c != '(') {		/* no replacement if no ()	*/
			PushBack();
			if (! (mac->mc_flag & FUNC)) {
				warning("macro %s needs arguments",
					idef->id_text);
				return 0;
			}
		}
		if (mac->mc_flag & FUNC) {
			struct idf *param;
			char *nam;
			extern char *GetIdentifier();

			UnknownIdIsZero = 0;
			nam = GetIdentifier();
			if (nam) {
				param = findidf(nam);
			}
			else	param = 0;
			UnknownIdIsZero = 1;
			if (c == '(') {
				LoadChar(c);
				c = skipspaces(c, 0);
				if (c != ')') error(") missing");
			}
			if (! nam) {
				error("identifier missing");
			}
			repl = new_mlist();
			if (param && param->id_macro) 
				reptext = "1 ";
			else
				reptext = "0 ";
			InsertText(reptext, 2);
			InputLevel++;
			repl->m_level = InputLevel;

			repl->next = ReplList;
			ReplList = repl;
			repl->m_mac = mac;
			if (nam) free(nam);
			return 1;
		}
		actpars = getactuals(idef);	/* get act.param. list	*/
	}

	repl = new_mlist();
	repl->m_mac = mac;
	if (mac->mc_flag & FUNC) /* this macro leads to special action	*/
		macro_func(idef);
	if (mac->mc_nps <= 0) {
		reptext = mac->mc_text;
		size = mac->mc_length;
		mac->mc_flag |= NOREPLACE;	/* a file called __FILE__ ??? */
	}
	else {
		reptext = macro2buffer(idef, actpars, &size); /* create input buffer */
		repl->m_repl = reptext;
	}
	InsertText(reptext, size);
	InputLevel++;
	repl->m_level = InputLevel;
	repl->next = ReplList;
	ReplList = repl;
	return 1;
}

char FilNamBuf[PATHLENGTH];

PRIVATE
macro_func(idef)
	register struct idf *idef;
{
	/*	macro_func() performs the special actions needed with some
		macros.  These macros are __FILE__ and __LINE__ which
		replacement texts must be evaluated at the time they are
		used.
	*/
	register struct macro *mac = idef->id_macro;

	switch (idef->id_text[2]) { /* This switch is very blunt... */
	case 'F' :			/* __FILE__	*/
		mac->mc_length = strlen(FileName) + 2;
		mac->mc_text = FilNamBuf;
		mac->mc_text[0] = '"';
		strcpy(&(mac->mc_text[1]), FileName);
		strcat(mac->mc_text, "\"");
		break;
	case 'L' :			/* __LINE__	*/
	{
		mac->mc_text = long2str((long) LineNumber, 10);
		mac->mc_length = strlen(mac->mc_text);
		break;
	}
	default :
		crash("(macro_func)");
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
	register unsigned int size = idef->id_macro->mc_length + ITEXTSIZE;
	register char *text = Malloc(size);
	register int pos = 0;
	register char *ptr = idef->id_macro->mc_text;

	while (*ptr) {
		if (*ptr & FORMALP) {	/* non-asc formal param. mark	*/
			register int n = *ptr++ & 0177;
			register char *p;

			assert(n != 0);
			/*	copy the text of the actual parameter
				into the replacement text
			*/
			for (p = actpars[n - 1]; *p; p++) {
				text[pos++] = *p;
				if (pos == size)
					text = Realloc(text, size <<= 1);
			}
		}
		else {
			text[pos++] = *ptr++;
			if (pos == size)
				text = Realloc(text, size <<= 1);
		}
	}
	text[pos] = '\0';
	*siztext = pos;
	return Realloc(text, pos+1);
}

EXPORT
DoUnstack()
{
	Unstacked = 1;
}

EXPORT
EnableMacros()
{
	register struct mlist *p = ReplList, *prev = 0;

	assert(Unstacked > 0);
	while (p) {
		struct mlist *nxt = p->next;

		if (p->m_level > InputLevel) {
			p->m_mac->mc_flag &= ~NOREPLACE;
			if (p->m_mac->mc_count) p->m_mac->mc_count--;
			if (p->m_repl) free(p->m_repl);
			if (! prev) ReplList = nxt;
			else prev->next = nxt;
			free_mlist(p);
		}
		else prev = p;
		p = nxt;
	}
	Unstacked = 0;
}
