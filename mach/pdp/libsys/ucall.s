#include "sys.h"
.define	_ucall

_ucall:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	mov	6(sp),0f+6
	mov	010(sp),0f+010
	sys	local; .data2 0f
	rts	pc
.sect .data
0:
	sys	ucall
	.data2	0, 0, 0, 0
