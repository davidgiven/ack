/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* PREPROCESSOR DRIVER */

#include <system.h>
#include "input.h"
#include "obufsize.h"
#include "LLlex.h"
#include "class.h"
#include "idf.h"
#include "idfsize.h"
#include "bits.h"
#include "line_prefix.h"

#ifdef DOBITS
char bits[128];
#endif

char _obuf[OBUFSIZE];
extern int do_preprocess;

Xflush()
{
	if (do_preprocess) sys_write(STDOUT, _obuf, OBUFSIZE);
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

#define flush(X)	(! do_preprocess || sys_write(STDOUT,_obuf,X))
#define echo(ch) 	if (op == ob) { Xflush(); op = _obuf; } *op++ = (ch);
#define newline()	echo('\n')

	if (! options['P']) {
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
	for (;;) {
		LineNumber++;
		lineno++;
		LoadChar(c);
		while (c == '#') {
			domacro();
			lineno++;
			newline();
			LoadChar(c);
		}
		if (lineno != LineNumber || fn != FileName) {
			fn = FileName;
			lineno = LineNumber;
			if (! options['P']) {
				register char *p = Xbuf;

				sprint(p, "%s %d \"%s\"\n",
					LINE_PREFIX,
					LineNumber,
					FileName);
				while (*p) {
					echo(*p++);
				}
			}
		}
		for (;;) {
			if (c & 0200)  {
				if (c == EOI) {
					newline();
					flush(op-_obuf);
					return;
				}
				fatal("non-ascii character read");
			}
			if (c == '/') {
				LoadChar(c);
				if (c == '*') {
					NoUnstack++;
					if (options['C']) {
						echo('/');
						echo('*');
					}
					for (;;) {
						LoadChar(c);
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
							LoadChar(c);
							if (c == '/') {
							   if (options['C']) {
								echo(c);
							   }
							   break;
							}
							else {
								PushBack();
							}
						}
						else if (options['C']) {
							echo(c);
						}
					}
					NoUnstack--;
					LoadChar(c);
					continue;
				}
				echo('/');
				continue;
			}
			switch(class(c)) {
			case STNL:
				echo(c);
				break;
			case STSTR:
			case STCHAR: {
				register int stopc = c;
				int escaped;
	
				do {

					escaped = 0;
					echo(c);
					LoadChar(c);
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
						LoadChar(c);
						if (c == '\n') {
							++LineNumber;
							lineno++;
						}
						else escaped = 1;
					}
				} while (escaped || c != stopc);
				echo(c);
				if (c == '\n')
					break;	/* Don't eat # */
				LoadChar(c);
				continue;
				}
			case STNUM:
#define getdec(c)	do { echo(c); LoadChar(c);} while (is_dig(c))
#define gethex(c)	do { echo(c); LoadChar(c);} while (is_hex(c))

				if (c != '0') {
					getdec(c);
					if (c == '.') getdec(c);
					if (c == 'e') {
						echo(c);
						LoadChar(c);
						if (c == '+' || c == '-') {
							echo(c);
							LoadChar(c);
						}
						if (is_dig(c)) getdec(c);
					}
				}
				else {
					echo(c);
					LoadChar(c);
					if (c == 'x' || c == 'X') gethex(c);
					else if (is_dig(c)) getdec(c);
				}
				continue;
			    case STIDF: {
				extern int idfsize;		/* ??? */
				char buf[IDFSIZE + 1];
				register char *tg = &buf[0];
				register char *maxpos = &buf[idfsize];
				register struct idf *idef;

#define tstmac(bx)	if (!(bits[c] & bx)) goto nomac
#define cpy		if (Unstacked) EnableMacros(); *tg++ = c
#define load		LoadChar(c); if (!in_idf(c)) goto endidf

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
					else break;
					load;
				}
			endidf:
				PushBack();
				*tg = '\0';	/* mark the end of the identifier */
				idef = findidf(buf);
				if (idef && idef->id_macro) {
					do {
						LoadChar(c);
					} while  (in_idf(c));
					PushBack();
					if (replace(idef)) {
						LoadChar(c);
						continue;
					}
				}
			nomac:
				*tg = 0;
				tg = buf;
				while (*tg) {
					echo(*tg++);
				}
				LoadChar(c);
				while (in_idf(c)) {
					echo(c);
					LoadChar(c);
				}
				continue;
				}
			    default:
			    	echo(c);
			    	LoadChar(c);
			    	continue;
			}
			break;
		}
	}
	/*NOTREACHED*/
}
