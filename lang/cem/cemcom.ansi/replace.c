/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*  M A C R O   R E P L A C E M E N T */
#include	"nopp.h"

#ifndef NOPP

#include	"debug.h"
#include	"pathlength.h"
#include	"strsize.h"
#include	"nparams.h"
#include	"idfsize.h"
#include	"numsize.h"
#include	<alloc.h>
#include	"idf.h"
#include	"input.h"
#include	"macro.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"class.h"
#include	"assert.h"
#include	"static.h"
#include	"lapbuf.h"
#include	"argbuf.h"
#include	"replace.h"

extern struct idf *GetIdentifier();
extern int InputLevel;
struct repl *ReplaceList;	/* list of currently active macros */

int
replace(idf)
	register struct idf *idf;
{
	/*	replace is called by the lexical analyzer to perform
		macro replacement. The routine actualy functions as a
		higher interface to the real thing: expand_macro().
	*/
	struct repl *repl;

	if (!(idf->id_macro)) return 0;
	if (idf->id_macro->mc_flag & NOREPLACE)
		return 0;
	repl = new_repl();
	repl->r_ptr = repl->r_text;
	repl->r_args = new_args();
	repl->r_idf = idf;
	if (!expand_macro(repl, idf))
		return 0;
	InputLevel++;
	InsertText(repl->r_text, repl->r_ptr - repl->r_text);
	idf->id_macro->mc_flag |= NOREPLACE;
	repl->r_level = InputLevel;
	repl->next = ReplaceList;
	ReplaceList = repl;
	return 1;
}

unstackrepl()
{
	Unstacked++;
}

EnableMacros()
{
	register struct repl *r = ReplaceList, *prev = 0;

	ASSERT(Unstacked > 0);
	while(r) {
		struct repl *nxt = r->next;

		if (r->r_level > InputLevel) {
			r->r_idf->id_macro->mc_flag &= ~NOREPLACE;
			if (!prev) ReplaceList = nxt;
			else prev->next = nxt;
			free_args(r->r_args);
			free_repl(r);
		}
		else prev = r;
		r = nxt;
	}
	Unstacked = 0;
}

expand_macro(repl, idf)
	register struct repl *repl;
	register struct idf *idf;
{
	/*	expand_macro() does the actual macro replacement.
		"idf" is a description of the identifier which
		caused the replacement.
		If the identifier represents a function-like macro
		call, the number of actual parameters is checked
		against the number of formal parameters. Note that
		in ANSI C the parameters are expanded first;
		this is done by calling getactuals().
		When the possible parameters are expanded, the replace-
		ment list associated with "idf" is expanded.
		expand_macro() returns 1 if the replacement succeeded
		and 0 if some error occurred.

		A special case is "defined". This acts as a unary operator
		on a single, unexpanded identifier, which may be surrounded
		by parenthesis. The function expand_defined handles this.
	*/
	register struct macro *mac = idf->id_macro;
	struct args *args = repl->r_args;
	register int ch;

	if (mac->mc_nps != -1) {	/* with parameter list	*/
		if (mac->mc_flag & FUNC) {
			/* the following assertion won't compile:
			ASSERT(!strcmp("defined", idf->id_text));
			expand the assert macro by hand (??? dirty, temporary)
			*/
#ifdef	DEBUG
			if (strcmp("defined", idf->id_text))
				crash("in %s, %u: assertion %s failed",
					__FILE__, __LINE__ - 2,
					"strcmp(\"defined\", idf->id_text)");
#endif
			if (!AccDefined) return 0;
			expand_defined(repl);
			return 1;
		}

		ch = GetChar();
		ch = skipspaces(ch,1);
		if (ch != '(') {	/* no replacement if no () */
			UnGetChar();
			return 0;
		} else
			getactuals(repl, idf);

	}

	if (mac->mc_flag & FUNC) /* this macro leads to special action */
		macro_func(idf);

	macro2buffer(repl, idf, args);

	/*	According to the ANSI definition:

			#define	a +
			a+b; --> + + b ;

		'a' must be substituded, but the result should be
		three tokens: + + ID. Because this preprocessor is
		character based, we have a problem.
		For now: just insert a space after all tokens,
		until ANSI fixes this flaw.
		^^^^^^^^^^^^^^^^^^^^^^^^^^	tsk tsk tsk
	*/
	if (*repl->r_ptr != TOKSEP) *repl->r_ptr++ = TOKSEP;
	*repl->r_ptr = '\0';

	return 1;
}

expand_defined(repl)
	register struct repl *repl;
{
	register int ch = GetChar();
	struct idf *id;
	int parens = 0;

	ch = skipspaces(ch, 0);

	if (ch == '(') {
		parens++;
		ch = GetChar();
		ch = skipspaces(ch, 0);
	}
	if ((class(ch) != STIDF) && (class(ch) != STELL)) {
		error("identifier missing");
		if (parens && ch != ')') error(") missing");
		if (!parens || ch != ')') UnGetChar();
		*repl->r_ptr++ = '0';
		*repl->r_ptr = '\0';
		return;
	}
	UnGetChar();
	id = GetIdentifier(0);
	ASSERT(id || class(ch) == STELL);
	ch = GetChar();
	ch = skipspaces(ch, 0);
	if (parens && ch != ')') error(") missing");
	if (!parens || ch != ')') UnGetChar();
	*repl->r_ptr++ = (id && id->id_macro) ? '1' : '0';
	*repl->r_ptr = '\0';
}

getactuals(repl, idf)
	struct repl *repl;
	register struct idf *idf;
{
	/*	Get the actual parameters from the input stream.
		The hard part is done by actual(), only comma's and
		other syntactic trivialities are checked here.
	*/
	register struct args *args = repl->r_args;
	register int nps = idf->id_macro->mc_nps;
	register int argcnt;
	register int ch;

	argcnt = 0;
	args->a_expvec[0] = args->a_expptr = &args->a_expbuf[0];
	args->a_rawvec[0] = args->a_rawptr = &args->a_rawbuf[0];
	if ((ch = GetChar()) != ')') {
		UnGetChar();
		while ((ch = actual(repl)) != ')' ) {
			if (ch != ',') {
				lexerror("illegal macro call");
				return;
			}
			stash(repl, '\0', 1);
			++argcnt;
			args->a_expvec[argcnt] = args->a_expptr;
			args->a_rawvec[argcnt] = args->a_rawptr;
			if (argcnt == STDC_NPARAMS)
				strict("number of parameters exceeds ANSI standard");
			if (argcnt >= NPARAMS)
				fatal("argument vector overflow");
		}
		stash(repl, '\0', 1);
		++argcnt;
	}
	if (argcnt < nps)
		lexerror("too few macro arguments");
	else if (argcnt > nps)
		lexerror("too many macro arguments");
}

saveraw(repl)
struct repl *repl;
{
	register struct repl *nrepl = ReplaceList;
	register struct args *ap = nrepl->r_args;
	struct args *args = repl->r_args;
	register char *p;

	/* stash identifier name */
	for (p = nrepl->r_idf->id_text; *p != '\0'; p++)
		*args->a_rawptr++ = *p;

	/*	The following code deals with expanded function
		like macro calls. It makes the following code
		work:

			#define	def(a,b)	x(a,b)
			#define	glue(a,b)	a ## b

			glue(abc,def(a,b))

		Results in:

			abcdef(a,b);
	*/
	if (ap->a_rawvec[0]) {
		/* stash arguments */
		register int i;

		*args->a_rawptr++ = '(';
		for (i = 0; ap->a_rawvec[i] != (char *)0; i++) {
			for (p = ap->a_rawvec[i]; *p != '\0'; p++)
				*args->a_rawptr++ = *p;
			*args->a_rawptr++ = ',';
		}
		*(args->a_rawptr-1) = ')';	/* delete last ',' */
	}
}

int
actual(repl)
	struct repl *repl;
{
	/*	This routine deals with the scanning of an actual parameter.
		It keeps in account the opening and closing brackets,
		preprocessor numbers, strings and character constants.
	*/
	register int ch;
	register int level = 0, nostashraw = 0;

	while (1) {
		ch = GetChar();

		if (Unstacked) {
			nostashraw -= Unstacked;
			if (nostashraw < 0) nostashraw = 0;
			EnableMacros();
		}
		if (class(ch) == STIDF || class(ch) == STELL) {
			/*	Scan a preprocessor identifier token. If the
				token is a macro, it is expanded first.
			*/
			char buf[(IDFSIZE > NUMSIZE ? IDFSIZE : NUMSIZE) + 1];
			register char *p = buf;
			register struct idf *idef;
			register int pos = -1;
			register int hash;
			extern int idfsize;
			int NoExpandMacro;

			if (ch == NOEXPM) {
				NoExpandMacro= 1;
				ch = GetChar();
			} else NoExpandMacro = 0;

			hash = STARTHASH();
			do {
				if (++pos < idfsize) {
					*p++ = ch;
					hash = ENHASH(hash, ch);
				}
				ch = GetChar();
			} while (in_idf(ch));
			hash = STOPHASH(hash);
			*p++ = '\0';
			UnGetChar();

			/*	When the identifier has an associated macro
				replacement list, it's expanded.
			*/
			idef = idf_hashed(buf, p - buf, hash);
			if (NoExpandMacro || !replace(idef)) {
				if ((idef->id_macro
				    && (idef->id_macro->mc_flag & NOREPLACE))
				    || NoExpandMacro)
					stash(repl, NOEXPM, !nostashraw);
				for (p = buf; *p != '\0'; p++)
					stash(repl, *p, !nostashraw);
			} else {
				if (!nostashraw) saveraw(repl);
				nostashraw++;
			}
		} else if (class(ch) == STNUM) {
			/*	a preprocessing number has the following
				regular expression:
				    [0-9|"."[0-9]]{[0-9"."a-zA-Z_]|{[Ee][+-]}}*
			*/
			stash(repl, ch, !nostashraw);
			if (ch == '.') {
				ch = GetChar();
				if (class(ch) != STNUM) {
					UnGetChar();
					continue;
				}
				else stash(repl, ch, !nostashraw);
			}
			ch = GetChar();
			while (in_idf(ch) || ch == '.') {
				stash(repl, ch, !nostashraw);
				if ((ch = GetChar()) == 'e' || ch == 'E') {
					stash(repl, ch, !nostashraw);
					ch = GetChar();
					if (ch == '+' || ch == '-') {
						stash(repl, ch, !nostashraw);
						ch = GetChar();
					}
				}
			}
			UnGetChar();
		} else if (ch == '(' || ch == '[' || ch == '{') {
			/* a comma may occur within these constructions ???
			 */
			level++;
			stash(repl, ch, !nostashraw);
		} else if (ch == ')' || ch == ']' || ch == '}') {
			level--;
			/* clossing parenthesis of macro call */
			if (ch == ')' && level < 0)
				return ')';
			stash(repl, ch, !nostashraw);
		} else if (ch == ',') {
			if (level <= 0) { /* comma separator for next argument */
				if (level)
					lexerror("unbalanced parenthesis");
				if (!nostashraw)
					return ',';	/* ??? */
			}
			stash(repl, ch, !nostashraw);
		} else if (ch == '\n') {
			/* newlines are accepted as white spaces */
			LineNumber++;
			while ((ch = GetChar()), class(ch) == STSKIP)
				/* EMPTY */;

			/*	This piece of code needs some explanation:
				consider the call of a macro defined as:
					#define sum(a,b) (a+b)
				in the following form:
					sum(
					#include phone_number
					,2);
				in which case the include must be handled
				interpreted as such.
			*/
			if (ch == '#')
				domacro();
			else if (ch == EOI) {
				lexerror("unterminated macro call");
				return ')';
			}
			UnGetChar();
			stash(repl, ' ', !nostashraw);
		} else if (ch == '/') {
			/* comments are treated as one white space token */
			if ((ch = GetChar()) == '*' && !InputLevel) {
				skipcomment();
				stash(repl, ' ', !nostashraw);
			} else {
				UnGetChar();
				stash(repl, '/', !nostashraw);
			}
		} else if (ch == '\'' || ch == '"') {
			/*	Strings are considered as ONE token, thus no
				replacement within strings.
			*/
			register int match = ch;

			stash(repl, ch, !nostashraw);
			while ((ch = GetChar()) != EOI) {
				if (ch == match)
					break;
				if (ch == '\\') {
					stash(repl, ch, !nostashraw);
					ch = GetChar();
				} else if (ch == '\n') {
					lexerror("newline in string");
					LineNumber++;
					stash(repl, match, !nostashraw);
					break;
				}
				stash(repl, ch, !nostashraw);
			}
			if (ch != match) {
				lexerror("unterminated macro call");
				return ')';
			}
			stash(repl, ch, !nostashraw);
		} else
			stash(repl, ch, !nostashraw);
	}
}

macro_func(idef)
	register struct idf *idef;
{
	/*	macro_func() performs the special actions needed with some
		macros. These macros are __FILE__ and __LINE__ which
		replacement texts must be evaluated at the time they are
		used.
	*/
	register struct macro *mac = idef->id_macro;
	static char FilNamBuf[PATHLENGTH];
	char *long2str();

	switch (idef->id_text[2]) {
	case 'F':			/* __FILE__	*/
		FilNamBuf[0] = '"';
		strcpy(&FilNamBuf[1], FileName);
		strcat(FilNamBuf, "\"");
		mac->mc_text = FilNamBuf;
		mac->mc_length = strlen(FilNamBuf);
		break;
	case 'L':			/* __LINE__	*/
		mac->mc_text = long2str((long)LineNumber, 10);
		mac->mc_length = strlen(mac->mc_text);
		break;
	default:
		crash("(macro_func)");
		/*NOTREACHED*/
	}
}

macro2buffer(repl, idf, args)
	register struct repl *repl;
	register struct idf *idf;
	register struct args *args;
{
	/*	macro2buffer expands the replacement list and places the
		result onto the replacement buffer. It deals with the #
		and ## operators, and inserts the actual parameters.
		The argument buffer contains the raw argument (needed
		for the ## operator), and the expanded argument (for
		all other parameter substitutions).

		The grammar of the replacement list is:

			repl_list:	TOKEN repl_list
				|	PARAMETER repl_list
				|	'#' PARAMETER
				|	TOKEN '##' TOKEN
				|	PARAMETER '##' TOKEN
				|	TOKEN '##' PARAMETER
				|	PARAMETER '##' PARAMETER
				;

		As the grammar indicates, we could make a DFA and
		use this finite state machine for the replacement
		list parsing (inserting the arguments, etc.).

		Currently we go through the replacement list in a
		linear fashion. This is VERY expensive, something
		smarter should be done (but even a DFA is O(|s|)).
	*/
	register char *ptr = idf->id_macro->mc_text;
	register char *tmpptr;
	int err = 0;
	char *stringify();

	while (*ptr) {
	    ASSERT(repl->r_ptr < &(repl->r_text[LAPBUF]));
	    if (*ptr == '\'' || *ptr == '"') {
		register int delim = *ptr;

		do {
		    *repl->r_ptr++ = *ptr;
		    if (*ptr == '\\')
			*repl->r_ptr++ = *++ptr;
		    if (*ptr == '\0') {
			lexerror("unterminated string");
			*repl->r_ptr = '\0';
			return;
		    }
		    ptr++;
		} while (*ptr != delim || *ptr == '\0');
		*repl->r_ptr++ = *ptr++;
	    } else if (*ptr == '#') {
		if (*++ptr == '#') {
			/* ## - paste operator */
		    ptr++;

			/* trim the actual replacement list */
		    --repl->r_ptr;
		    while (is_wsp(*repl->r_ptr)
			    && repl->r_ptr >= repl->r_text)
			--repl->r_ptr;

		    /*	## occurred at the beginning of the replacement list.
		     */
		    if (repl->r_ptr == repl->r_text
				&& is_wsp(*repl->r_ptr)) {
			err = 1;
			break;
		    }

		    while(*repl->r_ptr == TOKSEP
			    && repl->r_ptr >= repl->r_text)
			--repl->r_ptr;

		    tmpptr = repl->r_ptr;
		    ++repl->r_ptr;

		    /* skip space in macro replacement list */
		    while ((*ptr & FORMALP) == 0 && is_wsp(*ptr))
			    ptr++;

		    /*	## occurred at the end of the replacement list.
		     */
		    if (*ptr & FORMALP) {
			register int n = *ptr++ & 0177;
			register char *p;

			ASSERT(n > 0);
			p = args->a_rawvec[n-1];
			if (p) {	/* else macro argument missing */
			    while (is_wsp(*p))
				p++;
			    if (*p == NOEXPM) p++;
			    while (*p)
				*repl->r_ptr++ = *p++;
			}
			if (in_idf(*tmpptr + 1)) {
				while (in_idf(*tmpptr)
					    && tmpptr >= repl->r_text)
					tmpptr--;
				if (*tmpptr == NOEXPM) *tmpptr = TOKSEP;
			}
		    } else if (*ptr == '\0') {
			    err = 1;
			    break;
		    } else {
			    if (in_idf(*ptr)) {
				while (in_idf(*tmpptr)
					    && tmpptr >= repl->r_text)
					tmpptr--;
				if (*tmpptr == NOEXPM) *tmpptr = TOKSEP;
			    }
		    }
		} else			/* # operator */
		    ptr = stringify(repl, ptr, args);
	    } else if (*ptr & FORMALP) {
		/* insert actual parameter */
		register int n = *ptr++ & 0177;
		register char *p, *q;

		ASSERT(n > 0);

		/*	This is VERY dirty, we look ahead for the
			## operater. If it's found we use the raw
			argument buffer instead of the expanded
			one.
		*/
		for (p = ptr; (*p & FORMALP) == 0 && is_wsp(*p); p++)
			/* EMPTY */;
		if (*p == '#' && p[1] == '#')
			q = args->a_rawvec[n-1];
		else
			q = args->a_expvec[n-1];

		p = repl->r_ptr;
		if (q)			/* else macro argument missing */
		    while (*q)
			*repl->r_ptr++ = *q++;

		if (*repl->r_ptr != TOKSEP)
			*repl->r_ptr++ = TOKSEP;
	    } else
		*repl->r_ptr++ = *ptr++;
	}
	*repl->r_ptr = '\0';
	if (err)
		lexerror("illegal use of the ## operator");
}

char *
stringify(repl, ptr, args)
	register struct repl *repl;
	register char *ptr;
	register struct args *args;
{
	/*	If a parameter is immediately preceded by a # token
		both are replaced by a single string literal that
		contains the spelling of the token sequence for the
		corresponding argument.
		Each occurrence of white space between the argument's
		tokens become a single space character in the string
		literal. White spaces before the first token and after
		the last token comprising the argument are deleted.
		To retain the original spelling we insert backslashes
		as appropriate. We only escape backslashes if they
		occure within string tokens.
	*/
	register int space = 1;		/* skip leading spaces */
	register int delim = 0;		/* string or character constant delim */
	register int backslash = 0;	/* last character was a \ */

	/* skip spaces macro replacement list */
	while ((*ptr & FORMALP) == 0 && is_wsp(*ptr))
		ptr++;

	if (*ptr & FORMALP) {
		register int n = *ptr++ & 0177;
		register char *p;

		ASSERT(n != 0);
		p = args->a_rawvec[n-1];
		*repl->r_ptr++ = '"';
		while (*p) {
			if (is_wsp(*p)) {
				if (!space) {
					space = 1;
					*repl->r_ptr++ = ' ';
				}
				p++;
				continue;
			}
			space = 0;

			if (!delim && (*p == '"' || *p == '\''))
				delim = *p;
			else if (*p == delim && !backslash)
				delim = 0;
			backslash = *p == '\\';
			if (*p == '"' || (delim && *p == '\\'))
				*repl->r_ptr++ = '\\';
			if (*p == TOKSEP || *p == NOEXPM) p++;
			else *repl->r_ptr++ = *p++;
		}

		/* trim spaces in the replacement list */
		for (--repl->r_ptr; is_wsp(*repl->r_ptr); repl->r_ptr--)
			/* EMPTY */;
		*++repl->r_ptr = '"';
		++repl->r_ptr;	/* oops, one to far */
	} else
		error("illegal use of # operator");
	*repl->r_ptr = '\0';
	return ptr;
}

stash(repl, ch, stashraw)
	struct repl *repl;
	register int ch;
	int stashraw;
{
	/*	Stash characters into the macro expansion buffer.
	*/
	register struct args *args = repl->r_args;

	if (args->a_expptr >= &(args->a_expbuf[ARGBUF]))
		fatal("macro argument buffer overflow");
	*args->a_expptr++ = ch;

	if (stashraw) {
		if (args->a_rawptr >= &(args->a_rawbuf[ARGBUF]))
			fatal("raw macro argument buffer overflow");
		*args->a_rawptr++ = ch;
	}
}
#endif NOPP
