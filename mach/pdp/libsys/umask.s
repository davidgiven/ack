#include "sys.h"
.globl	_umask
.globl	_errno

_umask:
	mov	2(sp),0f+2
	sys	indir; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.data
0:
	sys	umask; ..
