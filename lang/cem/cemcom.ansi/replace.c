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

struct	repl	*ReplaceList;	/* list of currently active macros */

int
replace(idf)
	register struct idf *idf;
{
	/*	replace is called by the lexical analyzer to perform
		macro replacement. The routine actualy functions as a
		higher interface to the real thing: expand_macro().
	*/
	struct repl *repl;
	int size;
	
	repl = new_repl();
	repl->r_ptr = repl->r_text;
	repl->r_args = new_args();
	if (!expand_macro(repl, idf, (struct idf *)0))
		return 0;
	free_args(repl->r_args);
	InsertText(repl->r_text, repl->r_ptr - repl->r_text);
	repl->next = ReplaceList;
	ReplaceList = repl;
	return 1;
}

unstackrepl()
{
	struct repl *repl = ReplaceList;

#ifdef PERSONAL_TOUCH
	if (repl == NO_REPL) {
		print("Leendert, you don't understand the principle yet\n");
		return;
	}
#else
	ASSERT(repl != NO_REPL);
#endif
	ReplaceList = repl->next;
	free_repl(repl);
}

expand_macro(repl, idf, previdf)
	register struct repl *repl;
	register struct idf *idf;
	struct idf *previdf;
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
	*/
	register struct macro *mac = idf->id_macro;
	struct args *args = repl->r_args;
	register int ch;

	if (mac->mc_nps != -1) {	/* with parameter list	*/
		if (mac->mc_flag & FUNC) {
			/* the following assertion won't compile:
			ASSERT(!strcmp("defined", idf->id_text));
			*/
			if (!AccDefined) return 0;
		}

		ch = GetChar();
		ch = skipspaces(ch,1);
		if (ch != '(') {	/* no replacement if no () */
			/*	This is obscure. See the examples for the replace
				algorithm in section 3`.8.3.5.
			lexwarning("macro %s needs arguments", idf->id_text);
			*/
			UnGetChar();
			return 0;
		} else
			getactuals(args, idf);

		if (mac->mc_flag & FUNC) {
			struct idf *param = str2idf(args->a_rawbuf);

			*repl->r_ptr++ = param->id_macro ? '1' : '0';
			*repl->r_ptr = '\0';
			return 1;
		}
	}

	if (mac->mc_flag & FUNC) /* this macro leads to special action */
		macro_func(idf);

	if (mac->mc_nps == -1) {
		register int size = mac->mc_length;
		register char *text = mac->mc_text;

		ASSERT((repl->r_ptr+size) < &(repl->r_text[LAPBUF]));
		while (size-- > 0)
			*repl->r_ptr++ = *text++;
		*repl->r_ptr = '\0';
	} else
		macro2buffer(repl, idf, args);

	/*	According to the ANSI definition:

			#define	a +
			a+b; --> + + b ;
		
		'a' must be substituded, but the result should be
		three tokens: + + ID. Because this preprocessor is
		character based, we have a problem.
		For now: just insert a space after all tokens,
		until ANSI fixes this flaw.
	*/
	*repl->r_ptr++ = ' ';
	*repl->r_ptr = '\0';

	if (idf != previdf)
		maccount(repl, idf);
	return 1;
}

getactuals(args, idf)
	register struct args *args;
	register struct idf *idf;
{
	/*	Get the actual parameters from the input stream.
		The hard part is done by actual(), only comma's and
		other syntactic trivialities are checked here.
	*/
	register int nps = idf->id_macro->mc_nps;
	register int argcnt;
	register int ch;

	argcnt = 0;
	args->a_expvec[0] = args->a_expptr = &args->a_expbuf[0];
	args->a_rawvec[0] = args->a_rawptr = &args->a_rawbuf[0];
	if ((ch = GetChar()) != ')') {
		PushBack();
		while ((ch = actual(args, idf)) != ')' ) {
			if (ch != ',') {
				lexerror("illegal macro call");
				return;
			}
			stash(args, '\0');
			++argcnt;
			args->a_expvec[argcnt] = args->a_expptr;
			args->a_rawvec[argcnt] = args->a_rawptr;
			if (argcnt == STDC_NPARAMS)
				strict("number of parameters exceeds ANSI standard");
			if (argcnt >= NPARAMS)
				fatal("argument vector overflow");
		}
		stash(args, '\0');
		++argcnt;
	}
	if (argcnt < nps)
		lexerror("too few macro arguments");
	if (argcnt > nps)
		lexerror("too many macro arguments");
}

int
actual(args, idf)
	register struct args *args;
	register struct idf *idf;
{
	/*	This routine deals with the scanning of an actual parameter.
		It keeps in account the openning and clossing brackets,
		preprocessor numbers, strings and character constants.
	*/
	register int ch;
	register int level = 0;

	while (1) {
		ch = GetChar();

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
			int size;

			hash = STARTHASH();
			do {
				if (++pos < idfsize) {
					*p++ = ch;
					hash = ENHASH(hash, ch, pos);
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
			if (idef->id_macro)	/* expand macro identifier */
				expand_actual(args, idef, idf);
			else
				for (p = buf; *p != '\0'; p++)
					stash(args, *p);
		} else if (class(ch) == STNUM || class(ch) == '.') {
			/*	preprocessor number token. No this is no joke,
				the commitee decided (in all it's wisdom) that
				a preprocessing number has the following regular
				expression:
					[0-9"."]{[0-9"."a-zA-Z_]|{[Ee][+-]}}*
			*/
			do {
				stash(args, ch);
				if ((ch = GetChar()) == 'e' || ch == 'E') {
					ch = GetChar();
					if (ch == '+' || ch == '-') {
						stash(args, ch);
						ch = GetChar();
					}
				}
			} while (class(ch) == STNUM || class(ch) == STIDF ||
				 class(ch) == STELL || ch == '.');
			UnGetChar();
		} else if (ch == '(' || ch == '[' || ch == '{') {
			/* a comma may occur within these constructions */
			level++;
			stash(args, ch);
		} else if (ch == ')' || ch == ']' || ch == '}') {
			level--;
			/* clossing parenthesis of macro call */
			if (ch == ')' && level < 0)
				return ')';
			stash(args, ch);
		} else if (ch == ',') {
			if (level <= 0) { /* comma separator for next argument */
				if (level)
					lexerror("unbalanced parenthesis");
				return ',';
			}
			stash(args, ch);
		} else if (ch == '\n') {
			/* newlines are accepted as white spaces */
			LineNumber++;
			while ((ch = GetChar()), class(ch) == STSKIP)
				/* VOID */;

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
			UnGetChar();
			stash(args, ' ');
		} else if (ch == '/') {
			/* comments are treated as one white space token */
			if ((ch = GetChar()) == '*') {
				skipcomment();
				stash(args, ' ');
			} else {
				UnGetChar();
				stash(args, '/');
			}
		} else if (ch == '\'' || ch == '"') {
			/*	Strings are considered as ONE token, thus no
				replacement within strings.
			*/
			register int match = ch;

			stash(args, ch);
			while ((ch = GetChar()) != EOI) {
				if (ch == match)
					break;
				if (ch == '\\') {
					stash(args, ch);
					ch = GetChar();
				} else if (ch == '\n') {
					lexerror("newline in string");
					LineNumber++;
					stash(args, match);
					break;
				}
				stash(args, ch);
			}
			if (ch != match) {
				lexerror("unterminated macro call");
				return ')';
			}
			stash(args, ch);
		} else
			stash(args, ch);
	}
}

expand_actual(args, idef, idf)
	register struct args *args;
	register struct idf *idf, *idef;
{
	struct repl *nrepl = new_repl();
	register char *p;

	nrepl->r_args = new_args();
	nrepl->r_ptr = nrepl->r_text;
	if (expand_macro(nrepl, idef, idf)) {
		register struct args *ap = nrepl->r_args;

		for (p = nrepl->r_text; p < nrepl->r_ptr; p++)
			*args->a_expptr++ = *p;

		/* stash idef name */
		for (p = idef->id_text; *p != '\0'; p++)
			*args->a_rawptr++ = *p;

		/*	The following code deals with expanded function
			like macro calls. It makes the following code
			work:

				#define def(a,b)	x(a,b)
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
			*--args->a_rawptr = ')';
			++args->a_rawptr;	/* one too far */
		}
	} else	/* something happened during the macro expansion */
		for (p = idef->id_text; *p != '\0'; p++)
			stash(args, *p);
	free_args(nrepl->r_args);
	free_repl(nrepl);
}

maccount(repl, idf)
	register struct repl *repl;
	register struct idf *idf;
{
	/*	To prevent re-expansion of already expanded macro's we count
		the occurrences of the currently expanded macro name in the
		replacement list. This is mainly to prevent recursion as in:

			#define	f(a)	f(2 * (a))
			f(y+1);

		This results in:

			f(2*(y+1));

		When reading the inserted text we decrement the count of a
		macro name until it's zero. Then we start expanding it again.
	*/
	register char *text = repl->r_text;
	register int pos = -1;
	extern int idfsize;

	while (*text != '\0') {
		if (*text == '\'' || *text == '"') {
			register int delim;

			for (delim = *text++; *text != delim; text++)
				if (*text == '\\')
					text++;
			text++;
		} else
		if (class(*text) == STIDF || class(*text) == STELL) {
			char buf[(IDFSIZE > NUMSIZE ? IDFSIZE : NUMSIZE) + 1];
			register char *p = buf;

			do {
				if (++pos < idfsize)
					*p++ = *text;
				text++;
			} while (in_idf(*text));
			*p++ = '\0';

			if (!strcmp(idf->id_text, buf))
				idf->id_macro->mc_count++;
		} else
			text++;
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
		mac->mc_length = 1;
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
				while (is_wsp(*repl->r_ptr) &&
				       repl->r_ptr >= repl->r_text)
					--repl->r_ptr;

				/*	## occurred at the beginning of the
					replacement list.
				*/
				if (repl->r_ptr == repl->r_text)
					goto paste;
				++repl->r_ptr;

				/* skip space in macro replacement list */
				while ((*ptr & FORMALP) == 0 && is_wsp(*ptr))
					ptr++;

				/*	## occurred at the end of the
					replacement list.
				*/
				if (*ptr & FORMALP) {
					register int n = *ptr++ & 0177;
					register char *p;
					
					ASSERT(n != 0);
					p = args->a_rawvec[n-1];
					while (is_wsp(*p))
						p++;
					while (*p)
						*repl->r_ptr++ = *p++;
				} else if (*ptr == '\0')
					goto paste;
			} else
				ptr = stringify(repl, ptr, args);
		} else if (*ptr & FORMALP) {
			/* insert actual parameter */
			register int n = *ptr++ & 0177;
			register char *p, *q;
			
			ASSERT(n != 0);

			/*	This is VERY dirty, we look ahead for the
				## operater. If it's found we use the raw
				argument buffer instead of the expanded
				one.
			*/
			for (p = ptr; (*p & FORMALP) == 0 && is_wsp(*p); p++)
				/* VOID */;
			if (*p == '#' && p[1] == '#')
				q = args->a_rawvec[n-1];
			else
				q = args->a_expvec[n-1];

			while (*q)
				*repl->r_ptr++ = *q++;

			*repl->r_ptr++ = ' ';
		} else
			*repl->r_ptr++ = *ptr++;
	}
	*repl->r_ptr = '\0';
	return;

paste:
	/*	Sorry, i know this looks a bit like
		a unix device driver code.
	*/
	lexerror("illegal use of the ## operator");
	return;
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
		p = args->a_expvec[n-1];
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
			*repl->r_ptr++ = *p++;
		}

		/* trim spaces in the replacement list */
		for (--repl->r_ptr; is_wsp(*repl->r_ptr); repl->r_ptr--)
			/* VOID */;
		*++repl->r_ptr = '"';
		++repl->r_ptr;	/* oops, one to far */
	} else
		error("illegal use of # operator");
	return ptr;
}

stash(args, ch)
	register struct args *args;
	register int ch;
{
	/*	Stash characters into the macro expansion buffer.
	*/
	if (args->a_expptr >= &(args->a_expbuf[ARGBUF]))
		fatal("macro argument buffer overflow");
	*args->a_expptr++ = ch;

	if (args->a_rawptr >= &(args->a_rawbuf[ARGBUF]))
		fatal("raw macro argument buffer overflow");
	*args->a_rawptr++ = ch;

			
}
#endif NOPP
