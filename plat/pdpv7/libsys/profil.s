#include "sys.h"
.define	_profil

_profil:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	mov	6(sp),0f+6
	mov	010(sp),0f+010
	sys	indir; .data2 0f
	rts	pc
.sect .data
0:
	sys	profil
	.data2	0, 0, 0, 0
