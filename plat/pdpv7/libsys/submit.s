#include "sys.h"
.define	_submit
.extern	_errno

_submit:
	mov	2(sp),r0
	sys	local; .data2 0f
	bcc	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.sect .data
0:
	sys	submit
