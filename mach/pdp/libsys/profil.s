#include "sys.h"
.globl	_profil

_profil:
	mov	2(sp),0f+2
	mov	4(sp),0f+4
	mov	6(sp),0f+6
	mov	10(sp),0f+10
	sys	indir; 0f
	rts	pc
.data
0:
	sys	profil; ..; ..; ..; ..
