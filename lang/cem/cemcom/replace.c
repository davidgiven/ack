/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* PREPROCESSOR: MACRO-TEXT REPLACEMENT ROUTINES */

#include	"nopp.h"

#ifndef NOPP
#include	"debug.h"	/* UF */
#include	"pathlength.h"	/* UF */
#include	"strsize.h"	/* UF */
#include	<alloc.h>
#include	"idf.h"
#include	"input.h"
#include	"macro.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"class.h"
#include	"assert.h"
#include	"interface.h"
#include	"static.h"

char *strcpy(), *strcat();
char *long2str();
extern int InputLevel;

PRIVATE struct mlist	*ReplaceList;	/* list of currently active macros */

PRIVATE macro_func();
PRIVATE char *macro2buffer();
extern char **getactuals();

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
	register struct mlist *repl;
	register int c;
	char **actpars;
	char *reptext;
	int size;

	if (mac->mc_flag & NOREPLACE) {
		lexwarning("macro %s is recursive", idef->id_text);
		return 0;
	}
	if (mac->mc_nps != -1) {	/* with parameter list	*/
		if (mac->mc_flag & FUNC) {
					/* must be "defined".
					   Unfortunately, the next assertion
					   will not compile ...
			ASSERT( ! strcmp("defined", idef->id_text));
					*/
			if (! AccDefined)
				return 0;
		}
		if (++mac->mc_count > 100) {
			/* 100 must be some number in Parameters */
			lexwarning("macro %s is assumed recursive",
				    idef->id_text);
			return 0;
		}
		LoadChar(c);
		c = skipspaces(c,! (mac->mc_flag & FUNC));
		if (c != '(') {		/* no replacement if no ()	*/
			PushBack();
			if (! (mac->mc_flag & FUNC)) {
				lexerror("(warning) macro %s needs arguments",
					idef->id_text);
				return 0;
			}
		}
		if (mac->mc_flag & FUNC) {
			struct idf *param;
			extern struct idf *GetIdentifier();

			UnknownIdIsZero = 0;
			param = GetIdentifier();
			UnknownIdIsZero = 1;
			if (c == '(') {
				LoadChar(c);
				c = skipspaces(c,0);
				if (c != ')') error(") missing");
			}
			if (! param) {
				error("identifier missing");
			}
			repl = new_mlist();
			if (param && param->id_macro) 
				reptext = "1";
			else
				reptext = "0";
			InsertText(reptext, 1);
			InputLevel++;
			repl->m_level = InputLevel;
			repl->next = ReplaceList;
			ReplaceList = repl;
			repl->m_mac = mac;
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
		mac->mc_flag |= NOREPLACE;
	}
	else {
		reptext = macro2buffer(idef, actpars, &size); /* create input buffer */
		repl->m_repl = reptext;
	}
	InsertText(reptext, size);
	InputLevel++;
	repl->m_level = InputLevel;
	repl->next = ReplaceList;
	ReplaceList = repl;
	return 1;
}

GSTATIC char FilNamBuf[PATHLENGTH];

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
		FilNamBuf[0] = '"';
		strcpy(&FilNamBuf[1], FileName);
		strcat(FilNamBuf, "\"");
		mac->mc_text = FilNamBuf;
		mac->mc_length = strlen(FilNamBuf);
		break;
	case 'L' :			/* __LINE__	*/
		mac->mc_text = long2str((long)LineNumber, 10);
		mac->mc_length = 1;
		break;
	default :
		crash("(macro_func)");
		/*NOTREACHED*/
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
	register int pos = 0;
	register char *ptr = idef->id_macro->mc_text;

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
				if (pos == size)
					text = Srealloc(text, size += RSTRSIZE);
			}
		}
		else {
			text[pos++] = *ptr++;
			if (pos == size)
				text = Srealloc(text, size += RSTRSIZE);
		}
	}
	text[pos] = '\0';
	*siztext = pos;
	return text;
}

EXPORT
DoUnstack()
{
	Unstacked = 1;
}

EXPORT
EnableMacros()
{
	register struct mlist *p = ReplaceList, *prev = 0;

	ASSERT(Unstacked > 0);
	while (p) {
		struct mlist *nxt = p->next;

		if (p->m_level > InputLevel) {
			p->m_mac->mc_flag &= ~NOREPLACE;
			if (p->m_mac->mc_count) p->m_mac->mc_count--;
			if (p->m_repl) free(p->m_repl);
			if (! prev) ReplaceList = nxt;
			else prev->next = nxt;
			free_mlist(p);
		}
		else prev = p;
		p = nxt;
	}
	Unstacked = 0;
}
#endif /* NOPP */
