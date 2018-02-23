#include "sys.h"
.define	_rtp
.extern	_errno

_rtp:
	mov	2(sp),r0
	sys	rtp
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
