#include "f2c.h"
#include "fio.h"
#include "lio.h"

extern flag lquit;
extern int lcount;
extern int l_read();
extern char *icptr;
extern char *icend;
extern icilist *svic;
extern int icnum, recpos;
extern int (*l_getc)(), (*l_ungetc)();

int i_getc()
{
	if(++recpos >= svic->icirlen) {
		if (recpos == svic->icirlen)
			return '\n';
		z_rnew();
		}
	if(icptr >= icend) err(svic->iciend,(EOF),"endfile");
	return(*icptr++);
	}

int i_ungetc(ch)
 int ch;
{
	if (--recpos == svic->icirlen)
		return '\n';
	if (recpos < -1)
		err(svic->icierr,110,"recend");
	/* *--icptr == ch, and icptr may point to read-only memory */
	return *--icptr /* = ch */;
	}

 static void
c_lir(a)
 icilist *a;
{
	extern int l_eof;
	reading = 1;
	external = 0;
	formatted = 1;
	svic = a;
	L_len = a->icirlen;
	recpos = -1;
	icnum = recpos = 0;
	cursor = 0;
	l_getc = i_getc;
	l_ungetc = i_ungetc;
	l_eof = 0;
	icptr = a->iciunit;
	icend = icptr + a->icirlen*a->icirnum;
	cf = 0;
	curunit = 0;
	}


integer s_rsli(a) icilist *a;
{
	lioproc = l_read;
	lquit = 0;
	lcount = 0;
	c_lir(a);
	return(0);
	}

integer e_rsli()
{ return 0; }

s_rsni(a)
 icilist *a;
{
	cilist ca;
	ca.ciend = a->iciend;
	ca.cierr = a->icierr;
	ca.cifmt = a->icifmt;
	c_lir(a);
	return x_rsne(&ca);
	}
