.global fef8, Fd0

fef8:
	dec	4,%l0
	ld	[%l0+4], %f0
	ld	[%l0+8], %f1
	set	Fd0, %o0
	ldd	[%o0], %f8
	fcmpd	%f0, %f8
	nop
	fbne	1f
	nop
	st	%g0,[%l0]
	retl
	nop
1:	ld	[%l0+4],%o0
	srl	%o0, 20, %o1
	and	%o1, 0x7ff, %o1
	dec	0x3fe, %o1
	st	%o1,[%l0]
	set	0x7ff00000, %o1
	andn	%o0, %o1, %o0
	set	0x3fe00000, %o1
	or	%o0, %o1, %o0
	st	%o0,[%l0+4]
	retl
	nop
