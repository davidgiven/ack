#include "sys.h"
.define	_gldav
.extern	_errno

_gldav:
	mov	2(sp),r0
	sys	local; .data2 0f
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
1:
	clr	r0
	rts	pc
.sect .data
0:
	sys	gldav
