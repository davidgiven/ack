#include "sys.h"
.define	_getuid
.define	_geteuid

_getuid:
	sys	getuid
	rts	pc

_geteuid:
	sys	getuid
	mov	r1,r0
	rts	pc
