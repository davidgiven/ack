#include "f2c.h"
extern int (*lioproc)();

integer do_lio(type,number,ptr,len) ftnint *number,*type; char *ptr; ftnlen len;
{
	return((*lioproc)(number,ptr,len,*type));
}
