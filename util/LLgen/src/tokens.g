/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
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
 * The lexical analyser and LLmes are also included here. 
 */

{
# include "types.h"
# include "io.h"
# include "tunable.h"
# include "extern.h"
# include "assert.h"

# ifndef NORCSID
static string	rcsidc = "$Header$";
# endif

/* Here are defined : */
extern int	scanner();
extern 		LLmessage();
extern int	input();
extern		unput();
extern		skipcomment();
STATIC		linedirective();
STATIC string	cpy();
STATIC string	vallookup();
}

/* Classes */

%token  C_IDENT ;	/* lextoken.t_string contains the identifier read */
%token	C_NUMBER ;	/* lextoken.t_num contains the number read */
%token	C_LITERAL ;	/* lextoken.t_string contains the literal read */

/* Keywords */

%token	C_TOKEN ;
%token	C_START ;
%token	C_IF ;
%token	C_WHILE ;
%token	C_PERSISTENT ;
%token	C_FIRST ;
%token	C_LEXICAL ;
%token	C_AVOID ;
%token	C_PREFER ;
%token	C_DEFAULT ;

%lexical scanner ;

{

/*
 * Structure for a keyword
 */

struct keyword {
	string	w_word;
	int	w_value;
};

/*
 * The list of keywords, the most often used keywords come first.
 * Linear search is used, as there are not many keywords
 */

static struct keyword resword[] = {
	{ "token",	C_TOKEN	},
	{ "avoid",	C_AVOID	},
	{ "prefer",	C_PREFER	},
	{ "persistent", C_PERSISTENT	},
	{ "default",	C_DEFAULT	},
	{ "if",		C_IF	},
	{ "while",	C_WHILE	},
	{ "first",	C_FIRST	},
	{ "start",	C_START	},
	{ "lexical",	C_LEXICAL	},
	{ 0,		0	}
};

static t_token	savedtok;	/* to save lextoken in case of an insertion */
static  int	nostartline;	/* = 0 if at the start of a line */

scanner() {
	/*
	 * Lexical analyser, what else
	 */
	register	ch;		/* Current char */
	register	i;
	register	reserved = 0;	/* reserved word? */
	int		last;		/* Char before current char */

	if (savedtok.t_tokno) {	/*
				 * A token has been inserted.
				 * Now deliver the last lextoken again
				 */
		lextoken = savedtok;
		savedtok.t_tokno = 0;
		return lextoken.t_tokno;
	}
	for (;;) {	/*
			 * First, skip space, comments, line directives, etc
			 */
		do	ch = input();
		while(isspace(ch));
		if (ch == '/') skipcomment(0);
		else if (ch == '#' && !nostartline) linedirective();
		else break;
	}
	/*
	 * Now we have a first character of a token
	 */
	switch(ch) {
	  case EOF :
	 	 return EOF;
	  case '\'':	/*
			 * Literal, put it in ltext
			 */
		i = 0;
		for (;;) {
			last = ch;
			ch = input();
			if (ch == '\n' || ch == EOF) {
				error(linecount,"missing '");
				break;
			}
			if (ch == '\'' && last != '\\') break;
			ltext[i] = ch;
			if (i < LTEXTSZ - 1) ++i;
		}
		ltext[i] = '\0';
		lextoken.t_string = ltext;
		return C_LITERAL;
	  case '%' :	/*
			 * Start of a reserved word
			 */
		reserved = 1;
		ch = input();
		/* Fall through */
	  default  :
		i = 0;
		if (isdigit(ch)) {
			if (reserved) {
				error(linecount," A reserved number ?");
			}
			while (isdigit(ch)) {
				i = 10 * i + (ch - '0');
				ch= input();
			}
			lextoken.t_num = i;
			unput(ch);
			return C_NUMBER;
		}
		if (isalpha(ch) || ch == '_') {
			do {
				if (reserved && isupper(ch)) ch += 'a' - 'A';
				ltext[i] = ch;
				if (i < LTEXTSZ - 1) ++i;
				ch = input();
			} while (isalnum(ch) || ch == '_');
		} else	return ch;
		unput(ch);
	}
	ltext[i] = '\0';
	if (reserved) {	/*
			 * Now search for the keyword
			 */
		register struct keyword *w;

		w = resword;
		while (w->w_word) {
			if (! strcmp(ltext,w->w_word)) {
				/*
				 * Found it. Return token number.
				 */
				return w->w_value;
			}
			w++;
		}
		error(linecount,"illegal reserved word");
	}
	lextoken.t_string = ltext;
	return C_IDENT;
}

static int	backupc;	/* for unput() */
static int	nonline;	/* = 1 if last char read was a newline */

input() {
	/*
	 * Low level input routine, used by all other input routines
	 */
	register	c;
	register FILE	*f;

        if(backupc) {	/*
			 * Last char was "unput()". Deliver it again
			 */
		c = backupc;
		backupc = 0;
                return c;
	}
	f = finput;
	if ((c = getc(f)) == EOF) return c;
	nostartline = 1;
	if (!nonline) {
		linecount++;
		nostartline = 0;
		nonline = 1;
	}
	if (c == ' ' || c == '\t') {	/*
					 * Deliver space, but only once
					 */
		do	c = getc(f);
		while (c == ' ' || c == '\t');
		ungetc(c,f);
		return ' ';
	}
	if (c == '\n') nonline = 0;
	return c;
}

unput(c) {
	/*
	 * "unread" c
	 */
	backupc = c;
}

skipcomment(flag) {
	/*
	 * Skip comment. If flag != 0, the comment is inside a fragment
	 * of C-code, so the newlines in it must be copied to enable the
	 * C-compiler to keep a correct line count
	 */
	register	ch;
	int		saved;	/* line count on which comment starts */

	saved = linecount;
	if (input() != '*') error(linecount,"illegal comment");
	ch = input();
	while (ch != EOF) {
		if (flag && ch == '\n') putc(ch,fact);
		while (ch == '*') {
			if ((ch = input()) == '/') return;
			if (flag && ch == '\n') putc(ch,fact);
		}
		ch = input();
	}
	error(saved,"Comment does not terminate");
}

STATIC
linedirective() {
	/*
	 * Read a line directive
	 */
	register	ch;
	register	i;
	string		s_error = "Illegal line directive";
	string		store();
	register string	c;

	do {	/*
		 * Skip to next digit
		 * Do not skip newlines
		 */
		ch = input();
	} while (ch != '\n' && ! isdigit(ch));
	if (ch == '\n') {
		error(linecount,s_error);
		return;
	}
	i = ch - '0';
	ch = input();
	while (isdigit(ch)) {
		i = i*10 + (ch - '0');
		ch = input();
	}
	while (ch != '\n' && ch != '"') ch = input();
	if (ch == '"') {
		c = ltext;
		do {
			*c++ = ch = input();
		} while (ch != '"' && ch != '\n');
		if (ch == '\n') {
			error(linecount,s_error);
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

STATIC string
vallookup(s) {
	/*
	 * Look up the keyword that has token number s
	 */
	register struct keyword *p = resword;

	while (p->w_value) {
		if (p->w_value == s) return p->w_word;
		p++;
	}
	return 0;
}

STATIC string
cpy(s,p,flag) register s; register string p; {
	/*
	 * Create a piece of error message for token s and put it at p.
	 * flag = 0 if the token s was deleted (in which case we have
	 * attributes), else it was inserted
	 */
	register string t = 0;

	switch(s) {
	  case C_IDENT : 	
		if (!flag) t = lextoken.t_string;
		else t = "identifier";
		break;
	  case C_NUMBER :
		t = "number";
		break;
	  case C_LITERAL :
		if (!flag) {
			*p++ = '"';
			*p++ = '\'';
			t = lextoken.t_string;
			break;
		}
		t = "literal";
		break;
	  case EOFILE :
		t = "endoffile";
		break;
	}
	if (!t) {
		t = vallookup(s);
		if (t) {
			*p++ = '%';
		}
	}
	if (t) {	/*
			 * We have a string for the token. Copy it
			 */
		while (*t) *p++ = *t++;
		if (s == C_LITERAL && !flag) {
			*p++ = '\'';
			*p++ = '"';
		}
		return p;
	}
	/*
	 * The token is a literal
	 */
	*p++ = '\'';
	if (s >= 040 && s <= 0176) *p++ = s;
	else switch(s) {
	  case '\b' : *p++ = '\\'; *p++ = 'b'; break;
	  case '\f' : *p++ = '\\'; *p++ = 'f'; break;
	  case '\n' : *p++ = '\\'; *p++ = 'n'; break;
	  case '\r' : *p++ = '\\'; *p++ = 'r'; break;
	  case '\t' : *p++ = '\\'; *p++ = 't'; break;
	  default : *p++='0'+((s&0377)>>6); *p++='0'+((s>>3)&07);
		    *p++='0'+(s&07);
	}
	*p++ = '\'';
	return p;
}

LLmessage(d) {
	/*
	 * d is either 0, in which case the current token has been deleted,
	 * or non-zero, in which case it represents a token that is inserted
	 * before the current token
	 */
	register string	s,t;
	char		buf[128];

	nerrors++;
	s = buf;
	if (d == 0) {
		s = cpy(LLsymb,s,0);
		t = " deleted";
		do *s++ = *t; while (*t++);
	} else {
		s = cpy(d,s,1);
		t = " inserted in front of ";
		do *s++ = *t++; while (*t);
		s = cpy(LLsymb,s,0);
		*s = '\0';
	}
	error(linecount,buf);
	if (d) {	/*
			 * Save the current token and make up some
			 * attributes for the inserted token
			 */
		savedtok = lextoken;
		if (d == C_IDENT) lextoken.t_string = "dummy_identifier";
		else if (d == C_LITERAL) lextoken.t_string = "dummy_literal";
		else if (d == C_NUMBER) lextoken.t_num = 1;
	}
}
}
