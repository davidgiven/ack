/* PREPROCESSOR DRIVER */

#include <system.h>
#include "input.h"
#include "maxincl.h"
#include "obufsize.h"
#include "LLlex.h"
#include "class.h"
#include "idf.h"
#include "idfsize.h"
#include "bits.h"

char _obuf[OBUFSIZE];

#ifdef DOBITS
char bits[128];
#endif

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
	int lineno = 32767;	/* force line directive */
	extern char options[];

#define flush(X)	(sys_write(STDOUT,_obuf,X))
#define echo(ch) 	if (op == ob) { Xflush(); op = _obuf; } *op++ = (ch);
#define newline()	echo('\n')

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
				char Xbuf[256];
				register char *p = Xbuf;

				sprint(p, "# %d \"%s\"\n", LineNumber,
								FileName);
				while (*p) {
					echo(*p++);
				}
			}
		}
		for (;;) {
			if (c & 0200)  {
				if (c == EOI) {
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
						if (class(c) == STNL) {
							++LineNumber;
							++lineno;
							echo(c);
						}
						else if (c == EOI) {
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
	
				do {
					echo(c);
					LoadChar(c);
					if (c == '\\') {
						echo(c);
						LoadChar(c);
					}
					if (c == '\n') {
						++LineNumber;
						lineno++;
					}
					else if (c == EOI) {
						flush(op-_obuf);
						return;
					}
				}
				while (c != stopc);
				echo(c);
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
					load;
				}
			endidf:
				PushBack();
				*tg = '\0';	/* mark the end of the identifier */
				idef = findidf(buf);
				if ((idef && idef->id_macro && replace(idef))) {
					LoadChar(c);
					continue;
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
