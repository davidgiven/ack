#include "sys.h"
.define	__close
.extern	_errno

__close:
	mov	2(sp),r0
	sys	close
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
