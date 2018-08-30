#include "sys.h"
.define	_setuid
.extern	_errno

_setuid:
	mov	2(sp),r0
	sys	setuid
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
