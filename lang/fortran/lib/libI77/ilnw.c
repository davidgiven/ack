#include "f2c.h"
#include "fio.h"
#include "lio.h"
extern char *icptr;
extern char *icend;
extern icilist *svic;
extern int icnum;
extern int (*donewrec)();
extern int z_putc(), l_write();

 static int
z_wSL()
{
	extern int z_rnew();
	while(recpos < svic->icirlen)
		z_putc(' ');
	return z_rnew();
	}

 int
c_liw(a)
 icilist *a;
{
	reading = 0;
	external = 0;
	formatted = 1;
	putn = z_putc;
	L_len = a->icirlen;
	donewrec = z_wSL;
	svic = a;
	icnum = recpos = 0;
	cursor = 0;
	cf = 0;
	curunit = 0;
	icptr = a->iciunit;
	icend = icptr + a->icirlen*a->icirnum;
	}

s_wsni(a)
 icilist *a;
{
	cilist ca;

	c_liw(a);
	ca.cifmt = a->icifmt;
	x_wsne(&ca);
	z_wSL();
	return 0;
	}

integer s_wsli(a) icilist *a;
{
	lioproc = l_write;
	c_liw(a);
	return(0);
	}

integer e_wsli()
{
	z_wSL();
	return(0);
	}
