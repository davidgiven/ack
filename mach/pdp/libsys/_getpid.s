#include "sys.h"
.define	__getpid

__getpid:
	sys	getpid
	rts	pc
