/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* L L l e x . c
 *
 * Very simple lexical analyzer. 
 * Also contains LLmessage().
 */
# include <ctype.h>
# include <stdio.h>
# include "tunable.h"
# include "token.h"
# include "Lpars.h"

struct token dot;		/* current token */
static struct token aside;	/* to put currrent token aside, when a token
				 * is inserted
				 */
int	newline, lineno;	/* To keep track of linenumbers */
extern FILE *input;		/* file descriptor of machine table */

extern void error(char *s, char* s1);


int LLlex(void) {
	register int c;

	if (aside.t_tokno) {	/* A token was pushed aside, return it now */
		dot = aside;
		aside.t_tokno = 0;
		return dot.t_tokno;
	}
	if (newline) {		/* delayed increment of lineno, needed to give
				 * correct line numbers in error messages
				 */
	    	lineno++;
		newline = 0;
	}
	c = getc(input);
	while (c == '/') {	/* Could be a comment */
		if ((c = getc(input)) == '*') {
		    		/* Yes, it is a comment */
			int l;

			l = lineno;
        		do {
			    do {
				if (c == '\n') lineno++;
				c = getc(input);
			    } while (c != '*' && c != EOF);
                            if (c != EOF) c = getc(input);
                        } while (c != '/' && c != EOF);
			if (c == EOF) {
			    c = lineno;
			    lineno = l;
			    error("Unterminated comment", "");
			    lineno = c;
			    c = EOF;
			}
			else c = getc(input);
                }
		else {
			ungetc(c, input);
			c = '/';
			break;
		}
	}
	dot.t_tokno = c;
	dot.t_attrib = c;
	if (isupper(c) || islower(c) || c == '_') {
		dot.t_tokno = LETTER;
		return LETTER;
	}
	if (isdigit(c)) {
		dot.t_tokno = DIGIT;
		return DIGIT;
	}
	switch(c) {
	  case line_term :
		dot.t_tokno = LINE_TERMINATOR;
		return LINE_TERMINATOR;
	  case operand_sep :
		dot.t_tokno = OPERAND_SEPARATOR;
		return OPERAND_SEPARATOR;
	  case instruction_sep :
		dot.t_tokno = INSTRUCTION_SEPARATOR;
		return INSTRUCTION_SEPARATOR;
	  case '-' :
		c = getc(input);
		if (c == '>') {
			dot.t_tokno = PATTERN_SEPARATOR;
			return PATTERN_SEPARATOR;
		}
		ungetc(c,input);
		dot.t_tokno = OTHER;
		return OTHER;
	  case open_bracket :
	  	dot.t_tokno = OPEN_BRACKET;
		return OPEN_BRACKET;
	  case close_bracket :
		dot.t_tokno = CLOSE_BRACKET;
		return CLOSE_BRACKET;
	  case '\n' :
		newline = 1;
		/* Fall through */
	  case ' ' :
	  case '\t' :
		dot.t_tokno = SPACE;
		return SPACE;
	  case '%' :
	  case EOF :
		return c;
	  default :
		/* Let the C-compiler find out what is illegal! */
		dot.t_tokno = OTHER;
		return OTHER;
	}
}

void LLmessage(int d)
{
    static int savlineno;
    
    if (savlineno != lineno) {
	/* Only an error message if on another line number */
	savlineno = lineno;
	error("Syntax error","");
    }
    if (d > 0) {
	/* "d" is the token to be inserted.
	 * This is the place to put the current token aside and
	 * give the inserted token an attribute ... but who cares
	 */
	aside = dot;
    }
}
