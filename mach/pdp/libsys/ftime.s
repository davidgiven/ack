#include "sys.h"
.globl	_ftime

_ftime:
	mov	2(sp),0f+2
	sys	indir; 0f
	rts	pc

.data
0:
	sys	ftime; ..
