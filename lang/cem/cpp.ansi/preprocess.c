/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/* PREPROCESSOR DRIVER */

#include	<system.h>
#include	"input.h"
#include	"obufsize.h"
#include	"arith.h"
#include	"LLlex.h"
#include	"class.h"
#include	"macro.h"
#include	"idf.h"
#include	"idfsize.h"
#include	"bits.h"
#include	"line_prefix.h"

char	_obuf[OBUFSIZE];
#ifdef DOBITS
char	bits[128];
#endif
extern int InputLevel;

Xflush()
{
	sys_write(STDOUT, _obuf, OBUFSIZE);
}

preprocess(fn)
	char *fn;
{
	register int c;
	register char *op = _obuf;
	register char *ob = &_obuf[OBUFSIZE];
	char Xbuf[256];
	int lineno = 0;
	extern char options[];

#define flush(X)	(sys_write(STDOUT,_obuf,X))
#define echo(ch) 	if (op == ob) { Xflush(); op = _obuf; } *op++ = (ch);
#define newline()	echo('\n')

	if (!options['P']) {
		/* Generate a line directive communicating the
		   source filename
		*/
		register char *p = Xbuf;

		sprint(p, "%s 1 \"%s\"\n",
			LINE_PREFIX,
			FileName);
		while (*p) {
			echo(*p++);
		}
	}
#define	do_line(lineno, fn)						\
		if (lineno != LineNumber || fn != FileName) {		\
			fn = FileName;					\
			lineno = LineNumber;				\
			if (! options['P']) {				\
				register char *p = Xbuf;		\
									\
				sprint(p, "%s %d \"%s\"\n",		\
					LINE_PREFIX,			\
					LineNumber,			\
					FileName);			\
				while (*p) {				\
					echo(*p++);			\
				}					\
			}						\
		}

	for (;;) {
		LineNumber++;
		lineno++;
		c = GetChar();
		while (class(c) == STSKIP) {
			echo(c);
			c = GetChar();
		}

		while (c == '#') {
			if (!domacro()) {	/* pass pragma's to compiler */
				register char *p = "#pragma";

				do_line(lineno, fn);

				while(*p) {
					echo(*p++);
				}
				while ((c = GetChar()) != EOI) {
					if (class(c) == STNL) break;
					echo(c);
				}
			}
			lineno++;
			newline();
			c = GetChar();
			while (class(c) == STSKIP) {
				echo(c);
				c = GetChar();
			}
		}
		do_line(lineno, fn);
		for (;;) {

			/* illegal character */
			if (c & 0200)  {
				if (c == EOI) {
					newline();
					flush(op-_obuf);
					return;
				}
				fatal("non-ascii character read");
			}

			/* comments */
			if (c == '/' && !InputLevel) {
				c = GetChar();
				if (c == '*') {
					NoUnstack++;
					if (options['C']) {
						echo('/');
						echo('*');
					}
					for (;;) {
						c = GetChar();
						if (c == '\n') {
							++LineNumber;
							++lineno;
							echo(c);
						}
						else if (c == EOI) {
							newline();
							flush(op - _obuf);
							return;
						}
						else if (c == '*') {
							if (options['C']) {
								echo(c);
							}
							c = GetChar();
							if (c == '/') {
							   if (options['C']) {
								echo(c);
							   }
							   break;
							}
							else {
								UnGetChar();
							}
						}
						else if (options['C']) {
							echo(c);
						}
					}
					NoUnstack--;
					c = GetChar();
					continue;
				}
				echo('/');
				continue;
			}

			/* switch on character */
			switch(class(c)) {
			case STNL:
				echo(c);
				break;
			case STSTR:
			case STCHAR:
				{
				register int stopc = c;
				int escaped;

				do {

					escaped = 0;
					echo(c);
					c = GetChar();
					if (c == '\n') {
						break;
					}
					else if (c == EOI) {
						newline();
						flush(op-_obuf);
						return;
					}
					if (c == '\\') {
						echo(c);
						c = GetChar();
						if (c == '\n') {
							++LineNumber;
							lineno++;
						}
						else if (c == '\'') escaped = 1;
					}
				} while (escaped || c != stopc);
				echo(c);
				if (c == '\n')
					break;	/* Don't eat # */
				c = GetChar();
				continue;
				}
			case STNUM:
			/* The following code is quit ugly. This because
			 * ..3 == . .3 , whereas ...3 == ... 3
			 */
				echo(c);
				if (c == '.') {
					c = GetChar();
					if (c == '.') {
						if ((c = GetChar()) == '.') {
							echo('.'); echo('.');
							c = GetChar();
							continue;
						}
						UnGetChar();
						c = '.';
						continue;
					} else if (!is_dig(c)) {
						continue;
					}
				}
				c = GetChar();
				while (in_idf(c) || c == '.') {
					echo(c);
					if (c == 'e' || c == 'E') {
						c = GetChar();
						if (c == '+' || c == '-') {
							echo(c);
							c = GetChar();
						}
					} else	c = GetChar();
				}
				continue;
			case STELL:
				c = GetChar();
				UnGetChar();
				if (c == '"' || c == '\'') {
					echo('L');
					continue;
				}
				c = 'L';
			case STIDF: {
				extern int idfsize;		/* ??? */
				char buf[IDFSIZE + 1];
				register char *tg = &buf[0];
				register char *maxpos = &buf[idfsize];
				register struct idf *idef;
				int NoExpandNext = 0;

#define tstmac(bx)	if (!(bits[c] & bx)) goto nomac
#define cpy		*tg++ = c
#define load		c = GetChar(); if (!in_idf(c)) goto endidf

				/* unstack macro's when allowed. */
				if (Unstacked)
					EnableMacros();
				if (c == NOEXPM) {
					NoExpandNext = 1;
					c = GetChar();
				}

#ifdef DOBITS
				cpy; tstmac(bit0); load;
				cpy; tstmac(bit1); load;
				cpy; tstmac(bit2); load;
				cpy; tstmac(bit3); load;
				cpy; tstmac(bit4); load;
				cpy; tstmac(bit5); load;
				cpy; tstmac(bit6); load;
				cpy; tstmac(bit7); load;
#endif

				for(;;) {
					if (tg < maxpos) {
						cpy;
					}
					load;
				}
			endidf:
				if (c != EOF) UnGetChar();
				*tg = '\0';	/* mark the end of the identifier */
				if ((idef = findidf(buf))
				    && idef->id_macro
				    && ReplaceMacros && !NoExpandNext) {
					if (replace(idef)) {
						c = GetChar();
						continue;
					}
					tg = buf;
					while (*tg) {
						echo(*tg++);
					}
					c = GetChar();
					if (in_idf(c)) echo(' ');
					continue;
				}
			nomac:
				*tg = '\0';
				tg = buf;
				while (*tg) {
					echo(*tg++);
				}
				c = GetChar();
				while (in_idf(c)) {
					echo(c);
					c = GetChar();
				}
				continue;
				}
			case STMSPEC:
				if (InputLevel) {
					echo(' ');	/* seperate tokens */
					c = GetChar();
					continue;
				}
				/* else fallthrough */
			default:
			    	echo(c);
			    	c = GetChar();
			    	continue;
			}
			break;
		}
	}
	/*NOTREACHED*/
}
