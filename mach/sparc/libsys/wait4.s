#include "SYS.h"

.global _wait, _wait3

_wait:
	mov	0, %o0
	ld	[%l0], %o1
	mov	0, %o2
	b	wait4
	mov	0, %o3

_wait3:
	ld	[%l0+8], %o3
	ld	[%l0+4], %o2
	ld	[%l0], %o1
	b	wait4
	mov	0, %o0

SYS_call_4(wait4)
