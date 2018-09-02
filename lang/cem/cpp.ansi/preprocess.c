/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* PREPROCESSOR DRIVER */

#include <stdlib.h>
#include <stdio.h>
#include <system.h>
#include <alloc.h>
#include "input.h"
#include "parameters.h"
#include "arith.h"
#include "LLlex.h"
#include "class.h"
#include "macro.h"
#include "idf.h"
#include "bits.h"

char _obuf[OBUFSIZE];
#ifdef DOBITS
char bits[128];
#endif
extern int InputLevel;

extern char* sprint();

Xflush()
{
	sys_write(STDOUT, _obuf, OBUFSIZE);
}

static char* SkipComment();
extern char options[];

/* #pragma directives are saved here and passed to the compiler later on.
 */
struct prag_info
{
	int pr_linnr;
	char* pr_fil;
	char* pr_text;
};
static struct prag_info* pragma_tab;
static int pragma_nr;

do_pragma()
{
	register int size = ITEXTSIZE;
	char* cur_line = Malloc((unsigned)size);
	register char* c_ptr = cur_line;
	register int c = GetChar();
	register int delim = 0;

	while (c != '\n')
	{
		if (c_ptr + 1 - cur_line == size)
		{
			cur_line = Realloc(cur_line, (unsigned)(size + ITEXTSIZE));
			c_ptr = cur_line + size - 1;
			size += ITEXTSIZE;
		}
		if (delim)
		{
			if (c == delim)
			{
				delim = 0;
			}
			else if (c == '\\')
			{
				*c_ptr++ = c;
				c = GetChar();
				if (c == '\n')
					break;
			}
		}
		else if (c == '\'' || c == '"')
		{
			delim = c;
		}
		else if (c == '/')
		{
			if (!InputLevel)
			{
				c = GetChar();
				if (c == '*')
				{
					skipcomment();
					continue;
				}
				else if (c == '/')
				{
					skiplinecomment();
					continue;
				}
				*c_ptr++ = '/';
			}
		}
		*c_ptr++ = c;
		c = GetChar();
	}
	*c_ptr = '\0';
	if (!pragma_nr)
	{
		pragma_tab = (struct prag_info*)Malloc(sizeof(struct prag_info));
	}
	else
	{
		pragma_tab = (struct prag_info*)Realloc(
		    (char*)pragma_tab, (unsigned)(sizeof(struct prag_info) * (pragma_nr + 1)));
	}
	if (delim)
	{
		error("unclosed opening %c", delim);
	}
	pragma_tab[pragma_nr].pr_linnr = LineNumber;
	pragma_tab[pragma_nr].pr_fil = FileName;
	pragma_tab[pragma_nr].pr_text = cur_line;
	pragma_nr++;
	LineNumber++;
}

char Xbuf[256];

void preprocess(fn) char* fn;
{
	register int c;
	register char* op = _obuf;
	register char* ob = &_obuf[OBUFSIZE];
	int lineno = 0;
	int startline;

#define flush(X) (sys_write(STDOUT, _obuf, X))
#define echo(ch)                                                                                   \
	if (op == ob)                                                                                  \
	{                                                                                              \
		Xflush();                                                                                  \
		op = _obuf;                                                                                \
	}                                                                                              \
	*op++ = (ch);
#define newline()                                                                                  \
	op--;                                                                                          \
	while (op >= _obuf && (*op == ' ' || *op == '\t'))                                             \
		op--;                                                                                      \
	op++;                                                                                          \
	echo('\n')

	if (!options['P'])
	{
		/* Generate a line directive communicating the
		   source filename
		*/
		register char* p = Xbuf;

		sprint(p, "%s 1 \"%s\"\n", LINE_PREFIX, FileName);
		while (*p)
		{
			echo(*p++);
		}
	}

#define do_line_dir(lineno, fn)                                                                    \
	if (lineno != LineNumber || fn != FileName)                                                    \
	{                                                                                              \
		fn = FileName;                                                                             \
		lineno = LineNumber;                                                                       \
		if (!options['P'])                                                                         \
		{                                                                                          \
			register char* p = Xbuf;                                                               \
			sprint(Xbuf, "%s %d \"%s\"\n", LINE_PREFIX, (int)LineNumber, FileName);                \
			op--;                                                                                  \
			while (op >= _obuf && (class(*op) == STSKIP || *op == '\n'))                           \
				op--;                                                                              \
			op++;                                                                                  \
			newline();                                                                             \
			while (*p)                                                                             \
			{                                                                                      \
				echo(*p++);                                                                        \
			}                                                                                      \
		}                                                                                          \
	}

	for (;;)
	{
		LineNumber++;
		lineno++;
		startline = 1;
		c = GetChar();
		while (startline)
		{
			/* first flush the saved pragma's */
			if (pragma_nr)
			{
				register int i = 0;
				int LiNo = LineNumber;
				char* FiNam = FileName;

				while (i < pragma_nr)
				{
					register char* c_ptr = "#pragma";

					LineNumber = pragma_tab[i].pr_linnr;
					FileName = pragma_tab[i].pr_fil;
					do_line_dir(lineno, fn);
					while (*c_ptr)
					{
						echo(*c_ptr++);
					}
					c_ptr = pragma_tab[i].pr_text;
					while (*c_ptr)
					{
						echo(*c_ptr++);
					}
					newline();
					lineno++;
					free(pragma_tab[i].pr_text);
					i++;
				}
				free((char*)pragma_tab);
				pragma_tab = (struct prag_info*)0;
				pragma_nr = 0;
				LineNumber = LiNo;
				FileName = FiNam;
				do_line_dir(lineno, fn);
			}

			while (class(c) == STSKIP || c == '/')
			{
				if (c == '/')
				{
					if (!InputLevel)
					{
						c = GetChar();
						if (c == '*')
						{
							op = SkipComment(op, &lineno);
							if (!op)
								return;
							if (!options['C'])
							{
								echo(' ');
							}
							c = GetChar();
							continue;
						}
						else if (c == '/')
						{
							skiplinecomment();
							c = GetChar();
							continue;
						}
						UnGetChar();
						c = '/';
					}
					break;
				}
				echo(c);
				c = GetChar();
			}

			if (c == '#')
			{
				domacro();
				lineno++;
				newline();
				do_line_dir(lineno, fn);
				c = GetChar();
			}
			else
				startline = 0;
		}
		do_line_dir(lineno, fn);
		for (;;)
		{

			/* illegal character */
			if (c & 0200)
			{
				if (c == EOI)
				{
					newline();
					flush((int)(op - _obuf));
					return;
				}
				fatal("non-ascii character read");
			}

			/* comments */
			if (c == '/' && !InputLevel)
			{
				c = GetChar();
				if (c == '*')
				{
					op = SkipComment(op, &lineno);
					if (!op)
						return;
					if (!options['C'])
					{
						echo(' ');
					}
					c = GetChar();
					continue;
				}
				else if (c == '/')
				{
					skiplinecomment();
					c = GetChar();
					continue;
				}
				echo('/');
				continue;
			}

			/* switch on character */
			switch (class(c))
			{
				case STNL:
					newline();
					break;
				case STSTR:
				case STCHAR:
				{
					register int stopc = c;
					int escaped;

					do
					{
						escaped = 0;
						echo(c);
						c = GetChar();
						if (c == '\n')
						{
							/* the compiler will complain */
							break;
						}
						else if (c == EOI)
						{
							newline();
							flush((int)(op - _obuf));
							return;
						}
						if (c == '\\')
						{
							echo(c);
							c = GetChar();
							if (c == '\n')
							{
								++LineNumber;
								lineno++;
							}
							else
								escaped = 1;
						}
					} while (escaped || c != stopc);
					echo(c);
					if (c == '\n')
						break; /* Don't eat # */
					c = GetChar();
					continue;
				}
				case STNUM:
					/* The following code is quit ugly. This because
					 * ..3 == . .3 , whereas ...3 == ... 3
					 */
					echo(c);
					if (c == '.')
					{
						c = GetChar();
						if (c == '.')
						{
							if ((c = GetChar()) == '.')
							{
								echo('.');
								echo('.');
								c = GetChar();
								continue;
							}
							UnGetChar();
							c = '.';
							continue;
						}
						else if (!is_dig(c))
						{
							continue;
						}
						else
						{
							echo(c);
						}
					}
					c = GetChar();
					while (in_idf(c) || c == '.')
					{
						echo(c);
						if (c == 'e' || c == 'E')
						{
							c = GetChar();
							if (c == '+' || c == '-')
							{
								echo(c);
								c = GetChar();
							}
						}
						else
							c = GetChar();
					}
					continue;
				case STELL:
					c = GetChar();
					UnGetChar();
					if (c == '"' || c == '\'')
					{
						echo('L');
						continue;
					}
					c = 'L';
				case STIDF:
				{
					extern int idfsize; /* ??? */
					char buf[IDFSIZE + 1];
					register char* tg = &buf[0];
					register char* maxpos = &buf[idfsize];
					register struct idf* idef;
					int NoExpandNext = 0;

#define tstmac(bx)                                                                                 \
	if (!(bits[c] & bx))                                                                           \
	goto nomac
#define cpy *tg++ = c
#define load                                                                                       \
	c = GetChar();                                                                                 \
	if (!in_idf(c))                                                                                \
	goto endidf

					/* unstack macro's when allowed. */
					if (Unstacked)
						EnableMacros();
					if (c == NOEXPM)
					{
						NoExpandNext = 1;
						c = GetChar();
					}

#ifdef DOBITS
					cpy;
					tstmac(bit0);
					load;
					cpy;
					tstmac(bit1);
					load;
					cpy;
					tstmac(bit2);
					load;
					cpy;
					tstmac(bit3);
					load;
					cpy;
					tstmac(bit4);
					load;
					cpy;
					tstmac(bit5);
					load;
					cpy;
					tstmac(bit6);
					load;
					cpy;
					tstmac(bit7);
					load;
#endif

					for (;;)
					{
						if (tg < maxpos)
						{
							cpy;
						}
						load;
					}
				endidf:
					if (c != EOF)
						UnGetChar();
					*tg = '\0'; /* mark the end of the identifier */
					if ((idef = findidf(buf)) && idef->id_macro && ReplaceMacros && !NoExpandNext)
					{
						if (replace(idef))
						{
							echo(' ');
							c = GetChar();
							continue;
						}
						tg = buf;
						while (*tg)
						{
							echo(*tg++);
						}
						c = GetChar();
						if (in_idf(c))
						{
							echo(' ');
						}
						continue;
					}
				nomac:
					*tg = '\0';
					tg = buf;
					while (*tg)
					{
						echo(*tg++);
					}
					c = GetChar();
					while (in_idf(c))
					{
						echo(c);
						c = GetChar();
					}
					continue;
				}
				case STMSPEC:
					if (InputLevel)
					{
						echo(' '); /* seperate tokens */
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

static char* SkipComment(op, lineno) char* op;
int* lineno;
{
	char* ob = &_obuf[OBUFSIZE];
	register int c, oldc = '\0';

	NoUnstack++;
	if (options['C'])
	{
		echo('/');
		echo('*');
	}
	c = GetChar();
	for (;;)
	{
		if (c == EOI)
		{
			newline();
			flush((int)(op - _obuf));
			op = 0;
			break;
		}
		if (options['C'])
		{
			echo(c);
		}
		if (c == '\n')
		{
			++LineNumber;
			++*lineno;
			if (!options['C'])
			{
				echo(c);
			}
		}
		if (c == '*')
		{
			c = GetChar();
			if (c == '/')
			{
				if (options['C'])
				{
					echo(c);
				}
				break; /* for(;;) */
			}
			else if (oldc == '/')
			{
				warning("comment inside comment ?");
			}
			oldc = '*';
		}
		else
		{
			oldc = c;
			c = GetChar();
		}
	}
	NoUnstack--;
	return op;
}
