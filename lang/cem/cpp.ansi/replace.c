/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/*  M A C R O   R E P L A C E M E N T */

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#include	"parameters.h"
#include	"alloc.h"
#include	"idf.h"
#include	"input.h"
#include	"macro.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"class.h"
#include	"assert.h"
#include	"replace.h"

extern char *GetIdentifier();
extern char *strcpy();
extern char *strcat();
extern int InputLevel;
struct repl *ReplaceList;	/* list of currently active macros */

void expand_defined();
void getactuals();
void macro2buffer();

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
	repl->r_ptr = repl->r_text = Malloc((unsigned)(repl->r_size = LAPBUF));
	repl->r_args = new_args();
	repl->r_idf = idf;
	if (!expand_macro(repl, idf))
		return 0;
	InputLevel++;
	InsertText(repl->r_text, (int)(repl->r_ptr - repl->r_text));
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

freeargs(args)
	struct args *args;
{
	register int i;

	/* We must don't know how many parameters were specified, so be
	 * prepared to free all NPARAMS parameters.
	 * When an expvec is !NULL, the rawvec will also be !NULL.
	 * When an expvec is NULL, all remaining vectors will also be NULL.
	 */
	for (i = 0; i < NPARAMS; i++) {
		if (args->a_expvec[i]) {
			free(args->a_expvec[i]);
			free(args->a_rawvec[i]);
		} else break;
	}
	free_args(args);
}

EnableMacros()
{
	register struct repl *r = ReplaceList, *prev = 0;

	assert(Unstacked > 0);
	while(r) {
		struct repl *nxt = r->next;

		if (r->r_level > InputLevel) {
			r->r_idf->id_macro->mc_flag &= ~NOREPLACE;
			if (!prev) ReplaceList = nxt;
			else prev->next = nxt;
			free(r->r_text);
			freeargs(r->r_args);
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
		by parenthesis. The function expand_defined() handles this.
	*/
	register struct macro *mac = idf->id_macro;
	struct args *args = repl->r_args;
	register int ch;

	if (mac->mc_nps != -1) {	/* with parameter list	*/
		if (mac->mc_flag & FUNC) {
			/* the following assertion won't compile:
			assert(!strcmp("defined", idf->id_text));
			*/
			if (!AccDefined) return 0;
			expand_defined(repl);
			return 1;
		}

		ch = GetChar();
		ch = skipspaces(ch,1);
		if (ch != '(') {	/* no replacement if no () */
			ChPushBack(ch);
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
		three tokens: + + ID. Therefore a token separator is
		inserted after the replacement.
	*/
	if (repl->r_text == repl->r_ptr || *(repl->r_ptr -1) != TOKSEP) {
		add2repl(repl, TOKSEP);
	}
	return 1;
}

void
expand_defined(repl)
	register struct repl *repl;
{
	register int ch = GetChar();
	struct idf *id;
	char *str;
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
		if (!parens || ch != ')') ChPushBack(ch);
		add2repl(repl,'0');
		return;
	}
	ChPushBack(ch);
	str = GetIdentifier(0);
	if (str) {
		id = findidf(str);
		free(str);
	} else	id = 0;
	ch = GetChar();
	ch = skipspaces(ch, 0);
	if (parens && ch != ')') error(") missing");
	if (!parens || ch != ')') ChPushBack(ch);
	add2repl(repl, (id && id->id_macro) ? '1' : '0');
	add2repl(repl, ' ');
}

newarg(args)
	struct args *args;
{
	args->a_expptr = args->a_expbuf = Malloc((unsigned)(args->a_expsize = ARGBUF));
	args->a_rawptr = args->a_rawbuf = Malloc((unsigned)(args->a_rawsize = ARGBUF));
}

void
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
	newarg(args);
	if ((ch = GetChar()) != ')') {
		UnGetChar();
		while ((ch = actual(repl)) != ')' ) {
			if (ch != ',') {
				error("illegal macro call");
				return;
			}
			stash(repl, '\0', 1);
			args->a_expvec[argcnt] = args->a_expbuf;
			args->a_rawvec[argcnt] = args->a_rawbuf;
			++argcnt;
			if (argcnt == STDC_NPARAMS)
				strict("number of parameters exceeds ANSI standard");
			if (argcnt >= NPARAMS)
				fatal("argument vector overflow");
			newarg(args);
		}
		stash(repl, '\0', 1);
		args->a_expvec[argcnt] = args->a_expbuf;
		args->a_rawvec[argcnt] = args->a_rawbuf;
		++argcnt;
	}
	if (argcnt < nps)
		error("too few macro arguments");
	else if (argcnt > nps)
		error("too many macro arguments");
}

saveraw(repl)
struct repl *repl;
{
	register struct repl *nrepl = ReplaceList;
	register struct args *ap = nrepl->r_args;
	register char *p;

	/* stash identifier name */
	for (p = nrepl->r_idf->id_text; *p != '\0'; p++)
		stash(repl, *p, -1);

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

		for (i = 0; ap->a_rawvec[i] != (char *)0; i++) {
			if (i == 0) stash(repl, '(', -1);
			else stash(repl, ',', -1);
			for (p = ap->a_rawvec[i]; *p != '\0'; p++)
				 stash(repl, *p, -1);
		}
		stash(repl, ')', -1);
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
	register int ch = 0;
	register int level = 0, nostashraw = 0;
	int lastch;
	static int Unstacked_missed;

	while (1) {
		lastch = ch;
		ch = GetChar();

		if (nostashraw 
		    && nostashraw >= Unstacked_missed) {
			nostashraw -= Unstacked_missed;
			Unstacked_missed = 0;
		}
		if (Unstacked) {
			nostashraw -= Unstacked;
			if (nostashraw < 0) {
				Unstacked_missed = -nostashraw;
				nostashraw = 0;
			}
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
			extern int idfsize;
			int NoExpandMacro;

			if (ch == NOEXPM) {
				NoExpandMacro= 1;
				ch = GetChar();
			} else NoExpandMacro = 0;

			do {
				if (++pos < idfsize) {
					*p++ = ch;
				}
				ch = GetChar();
			} while (in_idf(ch));
			*p++ = '\0';
			ch = '\0';	/* Could be a non-stashed TOKSEP */
			UnGetChar();

			/*	When the identifier has an associated macro
				replacement list, it's expanded.
			*/
			idef = findidf(buf);
			if (!idef || NoExpandMacro || !replace(idef)) {
				if (NoExpandMacro
				    || (idef && idef->id_macro
					&& (idef->id_macro->mc_flag & NOREPLACE)))
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
					ch = '\0';	/* Could be a non-stashed TOKSEP */
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
			ch = '\0';	/* Could be a non-stashed TOKSEP */
			UnGetChar();
		} else if (ch == '(') {
			/* a comma may occur within parentheses */
			level++;
			stash(repl, ch, !nostashraw);
		} else if (ch == ')') {
			level--;
			/* test on closing parenthesis of macro call */
			if (level < 0) return ')';
			stash(repl, ch, !nostashraw);
		} else if (ch == ',') {
			if (level <= 0) { /* comma separator for next argument */
				if (level)
					error("unbalanced parenthesis");
				if (!nostashraw)
					return ',';	/* ??? */
			}
			stash(repl, ch, !nostashraw);
		} else if (ch == '\n') {
			/* newlines are accepted as white spaces */
			LineNumber++;
			/*	This piece of code needs some explanation:
				consider the call of a macro defined as:
					#define sum(a,b) (a+b)
				in the following form:
					sum(
					/_* comment *_/ #include phone_number
					,2);
				in which case the include must be handled
				interpreted as such.
			*/

a_new_line:		ch = GetChar();
			while (class(ch) == STSKIP || ch == '/') {
			    if (ch == '/') {
				    if ((ch = GetChar()) == '*' && !InputLevel) {
					skipcomment();
					stash(repl, ' ', !nostashraw);
					ch = GetChar();
					continue;
				    } else {
					UnGetChar();
					ch = '/';
				    }
				    stash(repl, '/', !nostashraw);
				    break;
			    } else ch = GetChar();
			}

			if (ch == '#') {
				domacro();
				goto a_new_line;
			} else if (ch == EOI) {
				error("unterminated macro call");
				return ')';
			}
			if (ch != '/') {
				UnGetChar();
				ch = ' ';
				stash(repl, ' ', !nostashraw);
			}
		} else if (ch == '/') {
			/* comments are treated as one white space token */
			if ((ch = GetChar()) == '*' && !InputLevel) {
				skipcomment();
				stash(repl, ' ', !nostashraw);
			} else {
				UnGetChar();
				ch = '/';
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
					error("newline in string");
					LineNumber++;
					stash(repl, match, !nostashraw);
					break;
				}
				stash(repl, ch, !nostashraw);
			}
			if (ch != match) {
				error("unterminated macro call");
				return ')';
			}
			stash(repl, ch, !nostashraw);
		} else {
			if (lastch == TOKSEP && ch == TOKSEP) continue;
			stash(repl, ch, !nostashraw);
		}
	}
}

macro_func(idef)
	register struct idf *idef;
{
	/*	macro_func() performs the special actions needed with some
		macros.  These macros are __FILE__ and __LINE__ which
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

void
macro2buffer(repl, idf, args)
	register struct repl *repl;
	register struct idf *idf;
	register struct args *args;
{
	/*	macro2buffer expands the replacement list and places the
		result onto the replacement buffer. It deals with the #
		and ## operators, and inserts the actual parameters.
		The argument buffer contains the raw argument (needed
		for the # and ## operators), and the expanded argument
		(for all other parameter substitutions).

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
	int err = 0;
	int func = idf->id_macro->mc_nps != -1;
	char *stringify();

	assert(ptr[idf->id_macro->mc_length] == '\0');
	while (*ptr) {
	    if (*ptr == '\'' || *ptr == '"') {
		register int delim = *ptr;

		do {
		    add2repl(repl, *ptr);
		    if (*ptr == '\\')
			    add2repl(repl, *++ptr);
		    if (*ptr == '\0') {
			    error("unterminated string");
			    return;
		    }
		    ptr++;
		} while (*ptr != delim || *ptr == '\0');
		add2repl(repl, *ptr++);
	    } else if (*ptr == '#' && (func || *(ptr+1) == '#')) {
		if (*++ptr == '#') {
		    register int tmpindex;
			/* ## - paste operator */
		    ptr++;

			/* trim the actual replacement list */
		    --repl->r_ptr;
		    while (repl->r_ptr >= repl->r_text
			    &&  is_wsp(*repl->r_ptr))
			--repl->r_ptr;

		    /*	## occurred at the beginning of the replacement list.
		    */
		    if (repl->r_ptr < repl->r_text) {
			    err = 1;
			    break;
		    }

		    if (repl->r_ptr >= repl->r_text
			    && *repl->r_ptr == TOKSEP)
			--repl->r_ptr;

		    ++repl->r_ptr;
		    tmpindex = repl->r_ptr - repl->r_text;
		    /* tmpindex can be 0 */

		    /* skip space in macro replacement list */
		    while ((*ptr & FORMALP) == 0 && is_wsp(*ptr))
			    ptr++;

		    /*	## occurred at the end of the replacement list.
		    */
		    if (*ptr & FORMALP) {
			register int n = *ptr++ & 0177;
			register char *p;

			assert(n > 0);
			p = args->a_rawvec[n-1];
			if (p) {	/* else macro argument missing */
			    while (is_wsp(*p)) p++;
			    if (*p == NOEXPM) p++;
			    while (*p)
				add2repl(repl, *p++);
			}
			while (tmpindex > 0
				&& in_idf(repl->r_text[tmpindex]))
			    tmpindex--;
			if (tmpindex >= 0
			    && repl->r_text[tmpindex] == NOEXPM)
				repl->r_text[tmpindex] = TOKSEP;
		    } else if (*ptr == '\0') {
			    err = 1;
			    break;
		    } else {
			    if (in_idf(*ptr)) {
				tmpindex--;
				while (tmpindex > 0
					&& in_idf(repl->r_text[tmpindex]))
				    tmpindex--;
				if (tmpindex >= 0
				    && repl->r_text[tmpindex] == NOEXPM)
					repl->r_text[tmpindex] = TOKSEP;
			    }
		    }
		} else {			/* # operator */
			ptr = stringify(repl, ptr, args);
		}
	    } else if (*ptr & FORMALP) {
		/* insert actual parameter */
		register int n = *ptr++ & 0177;
		register char *p, *q;

		assert(n > 0);

		/*	This is VERY dirty, we look ahead for the
			## operator. If it's found we use the raw
			argument buffer instead of the expanded
			one.
		*/
		for (p = ptr; (*p & FORMALP) == 0 && is_wsp(*p); p++)
			/* EMPTY */;
		if (*p == '#' && p[1] == '#')
			q = args->a_rawvec[n-1];
		else
			q = args->a_expvec[n-1];

		if (q)			/* else macro argument missing */
		    while (*q)
			add2repl(repl, *q++);

		if (repl->r_text == repl->r_ptr || *(repl->r_ptr-1) != TOKSEP)
			add2repl(repl, TOKSEP);
	    } else {
		add2repl(repl, *ptr++);
	    }
	}
	if (err)
		error("illegal use of ## operator");
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

		assert(n != 0);
		p = args->a_rawvec[n-1];
		add2repl(repl, '"');
		while (*p) {
			if (is_wsp(*p)) {
				if (!space) {
					space = 1;
					add2repl(repl, ' ');
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
				add2repl(repl, '\\');
			if (*p == TOKSEP || *p == NOEXPM) p++;
			else add2repl(repl, *p++);
		}

		/* trim spaces in the replacement list */
		for (--repl->r_ptr; is_wsp(*repl->r_ptr); repl->r_ptr--)
			/* EMPTY */;
		++repl->r_ptr;		/* oops, one to far */
		add2repl(repl, '"');
	} else
		error("illegal use of # operator");
	return ptr;
}

/* The following routine is also called from domacro.c.
 */
add2repl(repl, ch)
	register struct repl *repl;
	int ch;
{
	register int index = repl->r_ptr - repl->r_text;
 
	assert(index < repl->r_size);
	if (index + 2 >= repl->r_size) {
		repl->r_text = Realloc(repl->r_text, (unsigned)(repl->r_size <<= 1));
		repl->r_ptr = repl->r_text + index;
	}
	*repl->r_ptr++ = ch;
	*repl->r_ptr = '\0';
}

/* If the variable stashraw is negative, we must only stash into the raw
 * buffer. If the variable is zero, we must only stash into the expanded
 * buffer. Otherwise, we must use both buffers.
 */
stash(repl, ch, stashraw)
        struct repl *repl;
        register int ch;
        int stashraw;
{
	/* Stash characters into the macro expansion buffer.
	 */
	register struct args *args = repl->r_args;
	register int index = args->a_expptr - args->a_expbuf;
 
	if (stashraw >= 0) {
		assert(index < args->a_expsize);
		if (index + 1 >= args->a_expsize) {
			args->a_expbuf = Realloc(args->a_expbuf,
						    (unsigned)(args->a_expsize <<= 1));
			args->a_expptr = args->a_expbuf + index;
		}
		*args->a_expptr++ = ch;
	}
 
	if (stashraw) {
		index = args->a_rawptr - args->a_rawbuf;
		assert(index < args->a_rawsize);
		if (index + 1 >= args->a_rawsize) {
			args->a_rawbuf = Realloc(args->a_rawbuf,
						    (unsigned)(args->a_rawsize <<= 1));
			args->a_rawptr = args->a_rawbuf + index;
		}
		*args->a_rawptr++ = ch;
	}
}
