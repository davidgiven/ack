#include "sys.h"
.globl	_times

_times:
	mov	2(sp),0f+2
	sys	indir; 0f
	rts	pc
.data
0:
	sys	times; ..
