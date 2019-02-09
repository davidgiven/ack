/* Copyright (c) 1991 by the Vrije Universiteit, Amsterdam, the Netherlands.
 * For full copyright and restrictions on use see the file COPYING in the top
 * level of the LLgen tree.
 */

/*
 *  L L G E N
 *
 *  An Extended LL(1) Parser Generator
 *
 *  Author : Ceriel J.H. Jacobs
 */

/*
 * tokens.g
 * Defines the tokens for the grammar of LLgen.
 * The lexical analyser and LLmessage are also included here.
 */

{
#include <string.h>
#include "types.h"
# include "io.h"
# include "extern.h"
# include "assert.h"
# include "cclass.h"

# ifndef NORCSID
static string	rcsidc = "$Id$";
# endif

/* Here are defined : */
int scanner(void);
void LLmessage(int d); 
int input(void);
void unput(int c);
void skipcomment(int flag);
# ifdef LINE_DIRECTIVE
STATIC void linedirective(void); 
# endif
STATIC string cpy(int s,register string p,int inserted); 
STATIC string vallookup(int s);
STATIC void copyact(char ch1,char ch2,int flag,int level);

static int	nparams;
}
/* Classes */

%token	C_IDENT ;	/* lextoken.t_string contains the identifier read */
%token	C_NUMBER ;	/* lextoken.t_num contains the number read */
%token	C_LITERAL ;	/* lextoken.t_string contains the literal read */
%token	C_EXPR ;	/* A C expression (%if or %while) */
%token	C_PARAMS ;	/* formal or actual parameters */
%token	C_ACTION ;	/* a C action */

/* Keywords */

%token	C_TOKEN ;
%token	C_START ;
%token	C_IF ;
%token	C_WHILE ;
%token	C_PERSISTENT ;
%token	C_FIRST ;
%token	C_LEXICAL ;
%token	C_PREFIX ;
%token	C_ONERROR ;
%token	C_AVOID ;
%token	C_PREFER ;
%token	C_DEFAULT ;
%token	C_SUBSTART ;
%token	C_ERRONEOUS ;
%token	C_ILLEGAL ;

%lexical scanner ;

{

/*
 * Structure for a keyword
 */

typedef struct keyword {
	string	w_word;
	int	w_value;
} t_keyw, *p_keyw;

/*
 * The list of keywords, the most often used keywords come first.
 * Linear search is used, as there are not many keywords
 */

static t_keyw resword[] = {
	{ "token",	C_TOKEN		},
	{ "avoid",	C_AVOID		},
	{ "prefer",	C_PREFER	},
	{ "persistent", C_PERSISTENT	},
	{ "default",	C_DEFAULT	},
	{ "if",		C_IF		},
	{ "while",	C_WHILE		},
	{ "first",	C_FIRST		},
	{ "start",	C_START		},
	{ "lexical",	C_LEXICAL	},
	{ "onerror",	C_ONERROR	},
	{ "prefix",	C_PREFIX	},
#ifdef NON_CORRECTING
	{ "substart",	C_SUBSTART	},
	{ "erroneous",	C_ERRONEOUS	},
	{ "illegal",	C_ILLEGAL	},
#endif
	{ 0,		0		}
};

static t_token	savedtok;	/* to save lextoken in case of an insertion */
# ifdef LINE_DIRECTIVE
static	int	nostartline;	/* = 0 if at the start of a line */
# endif

STATIC void copyact(char ch1,char ch2,int flag,int level)
{
	/*
	 * Copy an action to file f. Opening bracket is ch1, closing bracket
	 * is ch2.
	 * If flag & 1, copy opening and closing parameters too.
	 * If flag & 2, don't allow ','.
	 */
	static int	text_seen = 0;
	register	FILE *f;
	register	int ch;		/* Current char */
	register	int match;		/* used to read strings */
	int		saved = linecount;
					/* save linecount */
	int		sav_strip = strip_grammar;

	f = fact;
	if (ch1 == '{' || flag != 1) strip_grammar = 0;
	if (!level) {
		text_seen = 0;
		nparams = 0;			/* count comma's */
		putc('\0',f);
		fprintf(f,"# line %d \"%s\"\n", linecount,f_input);
	}
	if (level || (flag & 1)) putc(ch1,f);
	for (;;) {
		ch = input();
		if (ch == ch2) {
			if (!level) {
				if (text_seen) nparams++;
			}
			if (level || (flag & 1)) putc(ch,f);
			if (strip_grammar != sav_strip) {
				if (ch1 == '{' || flag != 1) putchar(ch);
			}
			strip_grammar = sav_strip;
			return;
		}
		switch(ch) {
		  case ')':
		  case '}':
		  case ']':
			error(linecount,"Parentheses mismatch",NULL);
			break;
		  case '(':
			text_seen = 1;
			copyact('(',')',flag,level+1);
			continue;
		  case '{':
			text_seen = 1;
			copyact('{','}',flag,level+1);
			continue;
		  case '[':
			text_seen = 1;
			copyact('[',']',flag,level+1);
			continue;
		  case '/':
			ch = input();
			unput(ch);
			if (ch == '*') {
				putc('/', f);
				skipcomment(1);
				continue;
			}
			ch = '/';
			text_seen = 1;
			break;
		  case ';':
		  case ',':
			if (! level && text_seen) {
				text_seen = 0;
				nparams++;
				if (ch == ',' && (flag & 2)) {
					warning(linecount, "Parameters may not be separated with a ','",NULL);
					ch = ';';
				}
			}
			break;
		  case '\'':
		  case '"' :
			/*
			 * watch out for brackets in strings, they do not
			 * count !
			 */
			text_seen = 1;
			match = ch;
			putc(ch,f);
			while((ch = input())) {
				if (ch == match) break;
				if (ch == '\\') {
					putc(ch,f);
					ch = input();
				}
				if (ch == '\n') {
					error(linecount,"Newline in string",NULL);
					unput(match);
				}
				putc(ch,f);
			}
			if (ch == match) break;
			/* Fall through */
		    case EOF :
			if (!level) error(saved,"Action does not terminate",NULL);
			strip_grammar = sav_strip;
			return;
		    default:
			if (c_class[ch] != ISSPA) text_seen = 1;
		}
		putc(ch,f);
	}
}

int scanner(void) 
{
	/*
	 * Lexical analyser, what else
	 */
	register int	ch;		/* Current char */
	register char *p = ltext;
	int		reserved = 0;	/* reserved word? */
	char		*max = &ltext[LTEXTSZ - 1];
	static int	nextexpr;
	int		expect_expr = nextexpr;
	long		off;

	nextexpr = 0;
	if (savedtok.t_tokno) {
				/* A token has been inserted.
				 * Now deliver the last lextoken again
				 */
		lextoken = savedtok;
		savedtok.t_tokno = 0;
		return lextoken.t_tokno;
	}
	for (;;) {
		ch = input();
		if (ch == EOF) return ch;
# ifdef LINE_DIRECTIVE
		if (ch == '#' && !nostartline) {
			linedirective();
			continue;
		}
# endif
		switch(c_class[ch]) {
		  case ISACT :
			if (ch == '{') {
				copyact('{', '}', in_production, 0);
				return C_ACTION;
			}
			assert(ch == '(');
			if (expect_expr) {
				copyact('(', ')', 1, 0);
				return C_EXPR;
			}
			off = ftell(fact);
			copyact('(', ')', in_production != 0 ? 0 : 2, 0);
			if (nparams == 0) fseek(fact, off, 0);
			lextoken.t_num = nparams;
			return C_PARAMS;
		  case ISLIT :
			for (;;) {
				ch = input();
				if (ch == '\n' || ch == EOF) {
					error(linecount,"Missing '",NULL);
					break;
				}
				if (ch == '\'') break;
				if (ch == '\\') {
					*p++ = ch;
					ch = input();
				}
				*p++ = ch;
				if (p > max) p--;
			}
			*p = '\0';
			lextoken.t_string = ltext;
			return C_LITERAL;
		  case ISCOM :
			skipcomment(0);
			/* Fall through */
		  case ISSPA :
			continue;
		  case ISDIG : {
			register int i = 0;
			do {
				i = 10 * i + (ch - '0');
				ch= input();
			} while (c_class[ch] == ISDIG);
			lextoken.t_num = i;
			unput(ch);
			return C_NUMBER; }
		  default:
			return ch;
		  case ISKEY :
			reserved = 1;
			ch = input();
			/* Fall through */
		  case ISLET :
			do {
				if (reserved && ch >= 'A' && ch <= 'Z') {
					ch += 'a' - 'A';
				}
				*p++ = ch;
				if (p > max) p--;
				ch = input();
			} while (c_class[ch] == ISDIG || c_class[ch] == ISLET);
			unput(ch);
			*p = '\0';
			if (reserved) {	/*
					 * Now search for the keyword
					 */
				register p_keyw w;

				w = resword;
				while (w->w_word) {
					if (! strcmp(ltext,w->w_word)) {
						/*
						 * Return token number.
						 */
						if (w->w_value == C_IF ||
						    w->w_value == C_WHILE) {
							nextexpr = 1;
						}
						return w->w_value;
					}
					w++;
				}
				error(linecount,"Illegal reserved word",NULL);
			}
			lextoken.t_string = ltext;
			return C_IDENT;
		}
	}
}

static int	backupc;	/* for unput() */
static int	nonline;	/* = 1 if last char read was a newline */

int input(void) 
{
	/*
	 * Low level input routine, used by all other input routines
	 */
	register int c;

	if (c = backupc) {
			/* Last char was "unput()". Deliver it again
			 */
		backupc = 0;
		return c;
	}
	if ((c = getc(finput)) == EOF) {
		nonline = 0;
		return c;
	}
# ifdef LINE_DIRECTIVE
	nostartline = 1;
# endif
	if (!nonline) {
		linecount++;
# ifdef LINE_DIRECTIVE
		nostartline = 0;
# endif
		nonline = 1;
	}
	if (c == '\n') nonline = 0;
	if (strip_grammar) putchar(c);
	return c;
}

void unput(int c)
{
	/*
	 * "unread" c
	 */
	backupc = c;
}

void skipcomment(int flag) 
{
	/*
	 * Skip comment. If flag != 0, the comment is inside a fragment
	 * of C-code, so keep it.
	 */
	register int	ch;
	int		saved;	/* line count on which comment starts */

	saved = linecount;
	if (input() != '*') error(linecount,"Illegal comment",NULL);
	if (flag) putc('*', fact);
	do {
		ch = input();
		if (flag) putc(ch, fact);
		while (ch == '*') {
			ch = input();
			if (flag) putc(ch, fact);
			if (ch == '/') return;
		}
	} while (ch != EOF);
	error(saved,"Comment does not terminate",NULL);
}

# ifdef LINE_DIRECTIVE
STATIC void linedirective(void) 
{
	/*
	 * Read a line directive
	 */
	register int	ch;
	register int	i;
	string		s_error = "Illegal line directive";
	string		store();
	register string	c;

	do {	/*
		 * Skip to next digit
		 * Do not skip newlines
		 */
		ch = input();
	} while (ch != '\n' && c_class[ch] != ISDIG);
	if (ch == '\n') {
		error(linecount,s_error,NULL);
		return;
	}
	i = 0;
	do  {
		i = i*10 + (ch - '0');
		ch = input();
	} while (c_class[ch] == ISDIG);
	while (ch != '\n' && ch != '"') ch = input();
	if (ch == '"') {
		c = ltext;
		do {
			*c++ = ch = input();
		} while (ch != '"' && ch != '\n');
		if (ch == '\n') {
			error(linecount,s_error,NULL);
			return;
		}
		*--c = '\0';
		do {
			ch = input();
		} while (ch != '\n');
		/*
		 * Remember the file name
		 */
		if (strcmp(f_input,ltext)) f_input = store(ltext);
	}
	linecount = i;
}
# endif

STATIC string vallookup(int s) 
{
	/*
	 * Look up the keyword that has token number s
	 */
	register p_keyw p = resword;

	while (p->w_value) {
		if (p->w_value == s) return p->w_word;
		p++;
	}
	return 0;
}

STATIC string cpy(int s,register string p,int inserted) 
{
	/*
	 * Create a piece of error message for token s and put it at p.
	 * inserted = 0 if the token s was deleted (in which case we have
	 * attributes), else it was inserted
	 */
	register string t = 0;

	switch(s) {
	  case C_IDENT :
		if (!inserted) t = lextoken.t_string;
		else t = "identifier";
		break;
	  case C_NUMBER :
		t = "number";
		break;
	  case C_LITERAL :
		if (!inserted) {
			*p++ = '\'';
			t = lextoken.t_string;
			break;
		}
		t = "literal";
		break;
	  case C_ACTION:
		t = "C action";
		break;
	  case C_PARAMS:
		t = "C parameter section";
		break;
	  case C_EXPR:
		t = "C expression";
		break;
	  case EOFILE :
		t = "end-of-file";
		break;
	}
	if (!t && (t = vallookup(s))) {
		*p++ = '%';
	}
	if (t) {	/*
			 * We have a string for the token. Copy it
			 */
		while (*t) *p++ = *t++;
		if (s == C_LITERAL && !inserted) {
			*p++ = '\'';
		}
		return p;
	}
	/*
	 * The token is a literal
	 */
	*p++ = '\'';
	if (s >= 040 && s <= 0176) *p++ = s;
	else {
	    *p++ = '\\';
	    switch(s) {
	      case '\b' : *p++ = 'b'; break;
	      case '\f' : *p++ = 'f'; break;
	      case '\n' : *p++ = 'n'; break;
	      case '\r' : *p++ = 'r'; break;
	      case '\t' : *p++ = 't'; break;
	      default : *p++='0'+((s&0377)>>6); *p++='0'+((s>>3)&07);
			*p++='0'+(s&07);
	    }
	}
	*p++ = '\'';
	return p;
}


void LLmessage(int d) 
{
	/*
	 * d is either 0, in which case the current token has been deleted,
	 * or non-zero, in which case it represents a token that is inserted
	 * before the current token
	 */
	register string	s,t;
	char		buf[128];

	nerrors++;
	s = buf;
	if (d < 0) {
		strcpy(buf, "end-of-file expected");
	}
	else if (d == 0) {
#ifdef LLNONCORR
		t = " unexpected";
#else
		t = " deleted";
#endif
		s = cpy(LLsymb,s,0);
		do *s++ = *t; while (*t++);
	} else {
		s = cpy(d,s,1);
		t = " inserted in front of ";
		do *s++ = *t++; while (*t);
		s = cpy(LLsymb,s,0);
		*s = '\0';
	}
	if (d > 0) {	/*
			 * Save the current token and make up some
			 * attributes for the inserted token
			 */
		savedtok = lextoken;
		savedtok.t_tokno = LLsymb;
		if (d == C_IDENT) lextoken.t_string = "dummy_identifier";
		else if (d == C_LITERAL) lextoken.t_string = "dummy_literal";
		else if (d == C_NUMBER) lextoken.t_num = 1;
	}
#ifdef LLNONCORR
	else
#endif
	error(linecount, "%s", buf);
			/* Don't change this line to 
			 * error(linecount, buf).
			 * The string in "buf" might contain '%' ...
			 */
#ifdef LLNONCORR
	in_production = 1;
			/* To prevent warnings from copyact */
#endif
}
}
