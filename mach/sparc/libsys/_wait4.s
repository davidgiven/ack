#include "SYS.h"

.global __wait, __wait3

__wait:
	mov	0, %o0
	ld	[%l0], %o1
	mov	0, %o2
	b	wait4
	mov	0, %o3

__wait3:
	ld	[%l0+8], %o3
	ld	[%l0+4], %o2
	ld	[%l0], %o1
	b	wait4
	mov	0, %o0

_SYS_call_4(wait4)
