#include "sys.h"
.globl	_ucall

_ucall:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	mov	6(sp),0f+6
	mov	10(sp),0f+10
	sys	local; 0f
	rts	pc
.data
0:
	sys	ucall; ..; ..; ..; ..
