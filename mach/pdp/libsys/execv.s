#include "sys.h"
.define	_execv
.extern	_environ
.extern	_errno

_execv:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	mov	_environ,0f+6
	sys	indir; .data2 0f
	mov	r0,_errno
	mov	$-1,r0
	rts	pc
.sect .data
0:
	sys	exece
	.data2	0, 0, 0
