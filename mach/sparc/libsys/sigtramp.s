.global __sigtramp

__sigtramp:
	dec	0x10, %sp
	st	%g1, [%sp+0x44]
	std	%l0, [%sp+0x48]
	save	%sp, %g4, %sp
	st	%g0, [%sp + 64]
	clr	%l1
	mov	%fp, %l0

	ld	[%fp + 0x58], %o2
	set	0x1000, %l3
	ld	[%o2 + 0x14], %o0
	rd	%y, %l4
	andcc	%o0, %l3, %g0
	be	1f
	st	%l4, [%sp + 0xe4]
	std	%f0, [%sp + 0x60]
	std	%f2, [%sp + 0x68]
	std	%f4, [%sp + 0x70]
	std	%f6, [%sp + 0x78]
	std	%f8, [%sp + 0x80]
	std	%f10, [%sp + 0x88]
	std	%f12, [%sp + 0x90]
	std	%f14, [%sp + 0x98]
	std	%f16, [%sp + 0xa0]
	std	%f18, [%sp + 0xa8]
	std	%f20, [%sp + 0xb0]
	std	%f22, [%sp + 0xb8]
	std	%f24, [%sp + 0xc0]
	std	%f26, [%sp + 0xc8]
	std	%f28, [%sp + 0xd0]
	std	%f30, [%sp + 0xd8]
	st	%fsr, [%sp + 0xe0]
1:
	std	%g2, [%sp + 0xe8]
	std	%g4, [%sp + 0xf0]
	std	%g6, [%sp + 0xf8]

	ld	[%fp + 0x50], %o0
	dec	4, %l0
	st	%o0, [%l0]
	set	__sigfunc, %g1
	sll	%o0, 2, %g2
	add	%g1, %g2, %g3
	ld	[%g3], %g2

	call	%g2
	nop
	add	%fp, %g4, %sp		! cannot trust %sp after call
	ld      [%fp + 0x5c], %o3
	ld      [%fp + 0x58], %i0
	ld      [%sp + 0xe4], %l1
	ld      [%i0 + 0x14], %o0
	mov     %l1, %y
	andcc   %o0, %l3, %g0
	be      2f
	ldd     [%sp + 0xe8], %g2
	ldd     [%sp + 0x60], %f0
	ldd     [%sp + 0x68], %f2
	ldd     [%sp + 0x70], %f4
	ldd     [%sp + 0x78], %f6
	ldd     [%sp + 0x80], %f8
	ldd     [%sp + 0x88], %f10
	ldd     [%sp + 0x90], %f12
	ldd     [%sp + 0x98], %f14
	ldd     [%sp + 0xa0], %f16
	ldd     [%sp + 0xa8], %f18
	ldd     [%sp + 0xb0], %f20
	ldd     [%sp + 0xb8], %f22
	ldd     [%sp + 0xc0], %f24
	ldd     [%sp + 0xc8], %f26
	ldd     [%sp + 0xd0], %f28
	ldd     [%sp + 0xd8], %f30
	ld      [%sp + 0xe0], %fsr
2:
	ldd     [%sp + 0xf0], %g4
	ldd     [%sp + 0xf8], %g6

	restore	%g0, 0x8b, %g1

	ldd	[%sp+0x48], %l0
	inc	0x10, %sp
	ta	%g0
	nop
