#include "sys.h"
.define	_getpid

_getpid:
	sys	getpid
	rts	pc
