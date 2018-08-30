#include "sys.h"
.define	_ftime

_ftime:
	mov	2(sp),0f+2
	sys	indir; .data2 0f
	rts	pc

.sect .data
0:
	sys	ftime
	.data2	0
