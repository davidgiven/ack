.global fif8, Fd0, Fd1, Fd80000000, Fs80000000, Fs0

.align 8
Fd0:
.double 0r0
Fd1:
.double 0r1
FxE15:
.word 0x43300000, 0x0	! magic const: (a.b + x) - x == a.0
Fd80000000:
.align 8
.double 0r4294967296
Fs80000000:
.single 0r2147483648
Fs0:
.single 0r0

fif8:
	ld	[%l0], %f0
	ld	[%l0+4], %f1
	ld	[%l0+8], %f2
	ld	[%l0+12], %f3
	fmuld	%f0, %f2, %f0
	fmovs	%f0, %f6
	fmovs	%f1, %f7
	set	Fd0, %o0
	ldd	[%o0], %f8
	fcmpd	%f0, %f8
	nop
	fbuge,a	1f
	mov	%g0, %o1
	set	1, %o1
	fnegs	%f0, %f0
1:
	set	FxE15, %o0
	ldd	[%o0], %f10
	fcmpd	%f0, %f10
	nop
	fbuge	2f
	nop
	faddd	%f0, %f10, %f4
	fsubd	%f4, %f10, %f4
	fsubd	%f0, %f4, %f2
	set	Fd1, %o0
	ldd	[%o0], %f12
4:	fcmpd	%f2, %f12
	nop
	fbge,a	4b
	fsubd	%f2, %f12, %f2
5:	fcmpd	%f2, %f8
	nop
	fbl,a	5b
	faddd	%f2, %f12, %f2
	fsubd	%f0, %f2, %f0
2:
	tst	%o1
	bz	3f
	nop
	fnegs	%f0, %f0
3:
	fsubd	%f6, %f0, %f2
	st	%f0, [%l0]
	st	%f1, [%l0+4]
	st	%f2, [%l0+8]
	st	%f3, [%l0+12]
	retl
	nop
