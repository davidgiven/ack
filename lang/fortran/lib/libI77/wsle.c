#include "f2c.h"
#include "fio.h"
#include "fmt.h"
#include "lio.h"
extern int l_write(), t_putc(), x_wSL();

integer s_wsle(a) cilist *a;
{
	int n;
	if(!init) f_init();
	if(n=c_le(a)) return(n);
	reading=0;
	external=1;
	formatted=1;
	putn = t_putc;
	lioproc = l_write;
	L_len = LINE;
	donewrec = x_wSL;
	if(curunit->uwrt != 1 && nowwriting(curunit))
		err(a->cierr, errno, "list output start");
	return(0);
	}

integer e_wsle()
{
	t_putc('\n');
	recpos=0;
	if (cf == stdout)
		fflush(stdout);
	else if (cf == stderr)
		fflush(stderr);
	return(0);
	}
