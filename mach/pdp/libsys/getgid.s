#include "sys.h"
.define	_getgid
.define	_getegid

_getgid:
	sys	getgid
	rts	pc

_getegid:
	sys	getgid
	mov	r1,r0
	rts	pc
