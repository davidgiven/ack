#include "SYS.h"

LABEL_(brk)
	mov	%o7,%g2
	ld	[%l0], %o0
	inc	7, %o0
	andn	%o0, 7, %o0
	call	brk
	mov     %o0, %o2
	mov	%g2,%o7
	tst	%o0
	bge	0f
	nop
	retl
	nop
0:
	set	limhp, %g1
	st	%o2, [%g1]
	retl
	nop


LABEL_(sbrk)
	mov	%o7,%g2
	ld	[%l0], %o0
	inc	7, %o0
	andn	%o0, 7, %o0
	set	limhp, %o1
	ld	[%o1], %o2
	inc	7, %o2
	andn	%o2, 7, %o3
	add	%o3, %o0, %o0
	call	brk
	mov	%o0, %o4
	mov	%g2,%o7
	tst	%o0
	blt	0f
	nop
	set	limhp, %g1
	st	%o4, [%g1]
	mov	%o3, %o0
0:
	retl
	nop
