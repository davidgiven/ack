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
1:	
	fdtox	%f0,%f0
	st	%f0,[%l0+4]
	ld	[%l0+4],%o0
	srl	%o0,16,%o0
	set	0x7fff,%o2
	and	%o0,%o2,%o1
	set	16382,%o3
	sub	%o1,%o3,%o1
	st	%o1,[%l0]
	set	0x8000,%o2
	and	%o0,%o2,%o0
	or	%o0,%o3,%o0
	sll	%o0,16,%o0
	ld	[%l0+4],%o1
	set	0xffff,%o2
	and	%o1,%o2,%o1
	or	%o1,%o0,%o0
	st	%o0,[%l0+4]
	ld	[%l0+4],%f0
	fxtod	%f0,%f0
	st	%f0,[%l0+4]
	st	%f1,[%l0+8]
	retl
	nop
