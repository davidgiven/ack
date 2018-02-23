#include "sys.h"
.define	_times

_times:
	mov	2(sp),0f+2
	sys	indir; .data2 0f
	rts	pc
.sect .data
0:
	sys	times
	.data2	0
