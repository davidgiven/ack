/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*		    L E X I C A L   A N A L Y Z E R			*/

#include	"lint.h"
#include	<alloc.h>
#include	"nofloat.h"
#include	"idfsize.h"
#include	"numsize.h"
#include	"debug.h"
#include	"strsize.h"
#include	"nopp.h"
#include	"input.h"
#include	"arith.h"
#include	"def.h"
#include	"idf.h"
#include	"LLlex.h"
#include	"Lpars.h"
#include	"class.h"
#include	"assert.h"
#include	"sizes.h"

/* Data about the token yielded */
struct token dot, ahead, aside;
int token_nmb = 0;		/* number of the ahead token */
int tk_nmb_at_last_syn_err = -5/*ERR_SHADOW*/;
				/* token number at last syntax error */

#ifndef NOPP
int ReplaceMacros = 1;		/* replacing macros			*/
int AccDefined = 0;		/* accept "defined(...)"		*/
int UnknownIdIsZero = 0;	/* interpret unknown id as integer 0	*/
int Unstacked = 0;		/* an unstack is done 			*/
#endif
int SkipEscNewline = 0;		/* how to interpret backslash-newline	*/
int AccFileSpecifier = 0;	/* return filespecifier <...>		*/
int EoiForNewline = 0;		/* return EOI upon encountering newline	*/
int File_Inserted = 0;		/* a file has just been inserted	*/
#ifdef LINT
extern int lint_skip_comment;
#endif

#define MAX_LL_DEPTH	2

static struct token LexStack[MAX_LL_DEPTH];
static LexSP = 0;

/*	In PushLex() the actions are taken in order to initialise or
	re-initialise the lexical scanner.
	E.g. at the invocation of a sub-parser that uses LLlex(), the
	state of the current parser should be saved.
*/
PushLex()
{
	ASSERT(LexSP < 2);
	ASSERT(ASIDE == 0);	/* ASIDE = 0;	*/
	GetToken(&ahead);
	LexStack[LexSP++] = dot;
}

PopLex()
{
	ASSERT(LexSP > 0);
	dot = LexStack[--LexSP];
}

int
LLlex()
{
	/*	LLlex() plays the role of Lexical Analyzer for the C parser.
		The look-ahead and putting aside of tokens are taken into
		account.
	*/
	if (ASIDE) {	/* a token is put aside		*/
		dot = aside;
		ASIDE = 0;
	}
	else {		/* read ahead and return the old one	*/
#ifdef	LINT
		lint_comment_ahead();
#endif	/* LINT */
		dot = ahead;
		/*	the following test is performed due to the dual
			task of LLlex(): it is also called for parsing the
			restricted constant expression following a #if or
			#elif.  The newline character causes EOF to be
			returned in this case to stop the LLgen parsing task.
		*/
		if (DOT != EOI)
			GetToken(&ahead);
		else
			DOT = EOF;
	}
	return DOT;
}

char *string_token();

int
GetToken(ptok)
	register struct token *ptok;
{
	/*	GetToken() is the actual token recognizer. It calls the
		control line interpreter if it encounters a "\n#"
		combination. Macro replacement is also performed if it is
		needed.
	*/
	char buf[(IDFSIZE > NUMSIZE ? IDFSIZE : NUMSIZE) + 1];
	register int ch, nch;

	token_nmb++;

	if (File_Inserted) {
		File_Inserted = 0;
		goto firstline;
	}

again:	/* rescan the input after an error or replacement	*/
#ifndef NOPP
	if (Unstacked) EnableMacros();
#endif
	LoadChar(ch);
go_on:	/* rescan, the following character has been read	*/
	if ((ch & 0200) && ch != EOI) /* stop on non-ascii character */
		fatal("non-ascii '\\%03o' read", ch & 0377);
	/* keep track of the place of the token in the file	*/
	ptok->tk_file = FileName;
	ptok->tk_line = LineNumber;

	switch (class(ch)) {	/* detect character class	*/
	case STNL:		/* newline, vertical space or formfeed	*/
firstline:
		LineNumber++;			/* also at vs and ff	*/
		ptok->tk_file = FileName;
		ptok->tk_line = LineNumber;
		if (EoiForNewline)	/* called in control line	*/
			/*	a newline in a control line indicates the
				end-of-information of the line.
			*/
			return ptok->tk_symb = EOI;
		while (LoadChar(ch), ch == '#') { /* a control line follows */
			domacro();
			if (File_Inserted) {
				File_Inserted = 0;
				goto firstline;
			}
		}
			/*	We have to loop here, because in
				`domacro' the nl, vt or ff is read. The
				character following it may again be a `#'.
			*/
		goto go_on;
	case STSKIP:		/* just skip the skip characters	*/
		goto again;
	case STGARB:		/* garbage character			*/
#ifndef NOPP
		if (SkipEscNewline && (ch == '\\')) {
			/* a '\\' is allowed in #if/#elif expression	*/
			LoadChar(ch);
			if (class(ch) == STNL) {	/* vt , ff ?	*/
				++LineNumber;
				goto again;
			}
			PushBack();
			ch = '\\';
		}
#endif /* NOPP */
		if (040 < ch && ch < 0177)
			lexerror("garbage char %c", ch);
		else
			lexerror("garbage char \\%03o", ch);
		goto again;
	case STSIMP:	/* a simple character, no part of compound token*/
		if (ch == '/') { /* probably the start of comment	*/
			LoadChar(ch);
			if (ch == '*') { /* start of comment */
				skipcomment();
				goto again;
			}
			else {
				PushBack();
				ch = '/';	/* restore ch	*/
			}
		}
		return ptok->tk_symb = ch;
	case STCOMP:	/* maybe the start of a compound token		*/
		LoadChar(nch);			/* character lookahead	*/
		switch (ch) {
		case '!':
			if (nch == '=')
				return ptok->tk_symb = NOTEQUAL;
			PushBack();
			return ptok->tk_symb = ch;
		case '&':
			if (nch == '&')
				return ptok->tk_symb = AND;
			PushBack();
			return ptok->tk_symb = ch;
		case '+':
			if (nch == '+')
				return ptok->tk_symb = PLUSPLUS;
			PushBack();
			return ptok->tk_symb = ch;
		case '-':
			if (nch == '-')
				return ptok->tk_symb = MINMIN;
			if (nch == '>')
				return ptok->tk_symb = ARROW;
			PushBack();
			return ptok->tk_symb = ch;
		case '<':
			if (AccFileSpecifier) {
				PushBack();	/* pushback nch */
				ptok->tk_bts = string_token("file specifier",
							'>', &(ptok->tk_len));
				return ptok->tk_symb = FILESPECIFIER;
			}
			if (nch == '<')
				return ptok->tk_symb = LEFT;
			if (nch == '=')
				return ptok->tk_symb = LESSEQ;
			PushBack();
			return ptok->tk_symb = ch;
		case '=':
			if (nch == '=')
				return ptok->tk_symb = EQUAL;
			PushBack();
			return ptok->tk_symb = ch;
		case '>':
			if (nch == '=')
				return ptok->tk_symb = GREATEREQ;
			if (nch == '>')
				return ptok->tk_symb = RIGHT;
			PushBack();
			return ptok->tk_symb = ch;
		case '|':
			if (nch == '|')
				return ptok->tk_symb = OR;
			PushBack();
			return ptok->tk_symb = ch;
		}
	case STIDF:
	{
		register char *tg = &buf[0];
		register int pos = -1;
		register int hash;
		register struct idf *idef;
		extern int idfsize;		/* ??? */

		hash = STARTHASH();
		do	{			/* read the identifier	*/
			if (++pos < idfsize) {
#ifndef NOPP
				if (Unstacked) EnableMacros();
#endif
				*tg++ = ch;
				hash = ENHASH(hash, ch, pos);
			}
			LoadChar(ch);
		} while (in_idf(ch));
		hash = STOPHASH(hash);
		if (ch != EOI)
			PushBack();
		*tg++ = '\0';	/* mark the end of the identifier	*/
		idef = ptok->tk_idf = idf_hashed(buf, tg - buf, hash);
		idef->id_file = ptok->tk_file;
		idef->id_line = ptok->tk_line;
#ifndef NOPP
		if (idef->id_macro && ReplaceMacros && replace(idef))
			/* macro replacement should be performed	*/
			goto again;
		if (UnknownIdIsZero && idef->id_reserved != SIZEOF) {
			ptok->tk_ival = (arith)0;
			ptok->tk_fund = INT;
			return ptok->tk_symb = INTEGER;
		}
#endif /* NOPP */
		ptok->tk_symb = (
			idef->id_reserved ? idef->id_reserved
			: idef->id_def && idef->id_def->df_sc == TYPEDEF ?
				TYPE_IDENTIFIER
			: IDENTIFIER
		);
		return IDENTIFIER;
	}
	case STCHAR:				/* character constant	*/
	{
		register arith val = 0;
		int size = 0;

		LoadChar(ch);
		if (ch == '\'')
			lexerror("character constant too short");
		else
		while (ch != '\'') {
			if (ch == '\n') {
				lexerror("newline in character constant");
				PushBack();
				break;
			}
			if (ch == '\\') {
				LoadChar(ch);
				if (ch == '\n')
					LineNumber++;
				ch = quoted(ch);
			}
			if (ch >= 128) ch -= 256;
			val = val*256 + ch;
			size++;
			LoadChar(ch);
		}
		if (size > (int)int_size)
			lexerror("character constant too long");
		ptok->tk_ival = val;
		ptok->tk_fund = INT;
		return ptok->tk_symb = INTEGER;
	}
	case STSTR:					/* string	*/
		ptok->tk_bts = string_token("string", '"', &(ptok->tk_len));
		return ptok->tk_symb = STRING;
	case STNUM:				/* a numeric constant	*/
	{
		/*	It should be noted that 099 means 81(decimal) and
			099.5 means 99.5 . This severely limits the tricks
			we can use to scan a numeric value.
		*/
		register char *np = &buf[1];
		register int base = 10;
		register int vch;
		register arith val = 0;

		if (ch == '.') {	/* an embarrassing ambiguity */
#ifndef NOFLOAT
			LoadChar(vch);
			PushBack();
			if (!is_dig(vch))	/* just a `.'	*/
				return ptok->tk_symb = ch;
			*np++ = '0';
			/*	in the rest of the compiler, all floats
				have to start with a digit.
			*/
#else /* NOFLOAT */
			return ptok->tk_symb = ch;
#endif /* NOFLOAT */
		}
		if (ch == '0') {
			*np++ = ch;
			LoadChar(ch);
			if (ch == 'x' || ch == 'X') {
				base = 16;
				LoadChar(ch);
			}
			else
				base = 8;
		}
		while (vch = val_in_base(ch, base), vch >= 0) {
			val = val*base + vch;
			if (np < &buf[NUMSIZE])
				*np++ = ch;
			LoadChar(ch);
		}
		if (ch == 'l' || ch == 'L') {
			ptok->tk_ival = val;
			ptok->tk_fund = LONG;
			return ptok->tk_symb = INTEGER;
		}
#ifndef NOFLOAT
		if (base == 16 || !(ch == '.' || ch == 'e' || ch == 'E'))
#endif /* NOFLOAT */
		{
			PushBack();
			ptok->tk_ival = val;
			/*	The semantic analyser must know if the
				integral constant is given in octal/hexa-
				decimal form, in which case its type is
				UNSIGNED, or in decimal form, in which case
				its type is signed, indicated by
				the fund INTEGER.
			*/
			ptok->tk_fund = 
				(base == 10 || (base == 8 && val == (arith)0))
					? INTEGER : UNSIGNED;
			return ptok->tk_symb = INTEGER;
		}
		/* where's the test for the length of the integral ???	*/
#ifndef NOFLOAT
		if (ch == '.'){
			if (np < &buf[NUMSIZE])
				*np++ = ch;
			LoadChar(ch);
		}
		while (is_dig(ch)){
			if (np < &buf[NUMSIZE])
				*np++ = ch;
			LoadChar(ch);
		}
		if (ch == 'e' || ch == 'E') {
			if (np < &buf[NUMSIZE])
				*np++ = ch;
			LoadChar(ch);
			if (ch == '+' || ch == '-') {
				if (np < &buf[NUMSIZE])
					*np++ = ch;
				LoadChar(ch);
			}
			if (!is_dig(ch)) {
				lexerror("malformed floating constant");
				if (np < &buf[NUMSIZE])
					*np++ = ch;
			}
			while (is_dig(ch)) {
				if (np < &buf[NUMSIZE])
					*np++ = ch;
				LoadChar(ch);
			}
		}
		PushBack();
		*np++ = '\0';
		buf[0] = '-';	/* good heavens...	*/
		if (np == &buf[NUMSIZE+1]) {
			lexerror("floating constant too long");
			ptok->tk_fval = Salloc("-0.0",(unsigned) 5) + 1;
		}
		else
			ptok->tk_fval = Salloc(buf,(unsigned) (np - buf)) + 1;
		return ptok->tk_symb = FLOATING;
#endif /* NOFLOAT */
	}
	case STEOI:			/* end of text on source file	*/
		return ptok->tk_symb = EOI;
	default:				/* this cannot happen	*/
		crash("bad class for char 0%o", ch);
	}
	/*NOTREACHED*/
}

skipcomment()
{
	/*	The last character read has been the '*' of '/_*'.  The
		characters, except NL and EOI, between '/_*' and the first
		occurring '*_/' are not interpreted.
		NL only affects the LineNumber.  EOI is not legal.

		Important note: it is not possible to stop skipping comment
		beyond the end-of-file of an included file.
		EOI is returned by LoadChar only on encountering EOF of the
		top-level file...
	*/
	register int c;

	NoUnstack++;
	LoadChar(c);
#ifdef	LINT
	if (! lint_skip_comment) {
		lint_start_comment();
		lint_comment_char(c);
	}
#endif	/* LINT */
	do {
		while (c != '*') {
			if (class(c) == STNL)
				++LineNumber;
			else
			if (c == EOI) {
				NoUnstack--;
#ifdef	LINT
				if (! lint_skip_comment) lint_end_comment();
#endif	/* LINT */
				return;
			}
			LoadChar(c);
#ifdef	LINT
			if (! lint_skip_comment) lint_comment_char(c);
#endif	/* LINT */
		} /* last Character seen was '*' */
		LoadChar(c);
#ifdef	LINT
		if (! lint_skip_comment) lint_comment_char(c);
#endif	/* LINT */
	} while (c != '/');
#ifdef	LINT
	if (! lint_skip_comment) lint_end_comment();
#endif	/* LINT */
	NoUnstack--;
}

char *
string_token(nm, stop_char, plen)
	char *nm;
	int *plen;
{
	register int ch;
	register int str_size;
	register char *str = Malloc((unsigned) (str_size = ISTRSIZE));
	register int pos = 0;
	
	LoadChar(ch);
	while (ch != stop_char) {
		if (ch == '\n') {
			lexerror("newline in %s", nm);
			PushBack();
			break;
		}
		if (ch == EOI) {
			lexerror("end-of-file inside %s", nm);
			break;
		}
		if (ch == '\\') {
			LoadChar(ch);
			if (ch == '\n') {
				LineNumber++;
				LoadChar(ch);
				continue;
			}
			ch = quoted(ch);
		}
		str[pos++] = ch;
		if (pos == str_size)
			str = Srealloc(str, (unsigned) (str_size += RSTRSIZE));
		LoadChar(ch);
	}
	str[pos++] = '\0'; /* for filenames etc. */
	*plen = pos;
	return str;
}

int
quoted(ch)
	register int ch;
{	
	/*	quoted() replaces an escaped character sequence by the
		character meant.
	*/
	/* first char after backslash already in ch */
	if (!is_oct(ch)) {		/* a quoted char */
		switch (ch) {
		case 'n':
			ch = '\n';
			break;
		case 't':
			ch = '\t';
			break;
		case 'b':
			ch = '\b';
			break;
		case 'r':
			ch = '\r';
			break;
		case 'f':
			ch = '\f';
			break;
		}
	}
	else {				/* a quoted octal */
		register int oct = 0, cnt = 0;

		do {
			oct = oct*8 + (ch-'0');
			LoadChar(ch);
		} while (is_oct(ch) && ++cnt < 3);
		PushBack();
		ch = oct;
	}
	return ch&0377;
}

/* provisional */
int
val_in_base(ch, base)
	register int ch;
{
	return
		is_dig(ch) ? ch - '0'
		: base != 16 ? -1
		: is_hex(ch) ? (ch - 'a' + 10) & 017
		: -1;
}
