.global cfu4, cfu8

.align 8
Fdmaxint:
.double 0r2147483648
Fsmaxint:
.single 0r2147483648
maxint:
.word	0x80000000

cfu4:
	ld	[%l0], %f0
	set	Fsmaxint, %o0
	ld	[%o0], %f1
	fcmps	%f0, %f1
	nop
	fbul	1f
	nop
	fsubs	%f0, %f1, %f0
	fstoi	%f0, %f1
	st	%f1,[%l0]
	ld	[%l0],%o0
	set	maxint, %o1
	ld	[%o1],%o2
	add	%o0, %o2, %o0
	ba	2f
	nop
1:
	fstoi	%f0, %f1
	st	%f1,[%l0]
	ld	[%l0],%o0
2:
	inc	4,%l0
	retl
	nop

cfu8:
	ld	[%l0], %f0
	ld	[%l0+4], %f1
	set	Fdmaxint, %o0
	ldd	[%o0], %f2
	fcmpd	%f0, %f2
	nop
	fbul	1f
	nop
	fsubd	%f0, %f2, %f0
	fdtoi	%f0, %f2
	st	%f2,[%l0]
	ld	[%l0],%o0
	set	maxint, %o1
	ld	[%o1],%o2
	add	%o0, %o2, %o0
	ba	2f
	nop
1:
	fdtoi	%f0, %f2
	st	%f2,[%l0]
	ld	[%l0],%o0
2:
	inc	8,%l0
	retl
	nop
