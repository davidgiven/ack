#include "f2c.h"
#include "fio.h"
#include "lio.h"
#include "fmt.h"

x_wsne(a)
 cilist *a;
{
	Namelist *nl;
	char *s;
	Vardesc *v, **vd, **vde;
	ftnint *number, type;
	ftnlen *dims;
	ftnlen size;
	static ftnint one = 1;
	extern ftnlen typesize[];

	nl = (Namelist *)a->cifmt;
	PUT('&');
	for(s = nl->name; *s; s++)
		PUT(*s);
	PUT(' ');
	vd = nl->vars;
	vde = vd + nl->nvars;
	while(vd < vde) {
		v = *vd++;
		s = v->name;
		if (recpos+strlen(s)+2 >= L_len)
			(*donewrec)();
		while(*s)
			PUT(*s++);
		PUT(' ');
		PUT('=');
		number = (dims = v->dims) ? dims + 1 : &one;
		type = v->type;
		if (type < 0) {
			size = -type;
			type = TYCHAR;
			}
		else
			size = typesize[type];
		l_write(number, v->addr, size, type);
		if (vd < vde) {
			if (recpos+2 >= L_len)
				(*donewrec)();
			PUT(',');
			PUT(' ');
			}
		else if (recpos+1 >= L_len)
			(*donewrec)();
		}
	PUT('/');
	}
