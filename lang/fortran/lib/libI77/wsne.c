#include "f2c.h"
#include "fio.h"
#include "lio.h"

 integer
s_wsne(a)
 cilist *a;
{
	int n;
	extern int (*donewrec)(), t_putc(), x_wSL();
	extern integer e_wsle();

	if(!init)
		f_init();
	if(n=c_le(a))
		return(n);
	reading=0;
	external=1;
	formatted=1;
	putn = t_putc;
	L_len = LINE;
	donewrec = x_wSL;
	if(curunit->uwrt != 1 && nowwriting(curunit))
		err(a->cierr, errno, "namelist output start");
	x_wsne(a);
	return e_wsle();
	}
