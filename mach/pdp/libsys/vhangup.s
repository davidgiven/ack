#include "sys.h"
.globl	_vhangup
.globl	_errno

_vhangup:
	sys	local; 0f
	bec	1f
	mov	r0,_errno
	mov	$-1,r0
1:
	rts	pc
.data
0:
	sys	vhangup
