#include "sys.h"
.define	_setgid
.extern	_errno

_setgid:
	mov	2(sp),r0
	sys	setgid
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
