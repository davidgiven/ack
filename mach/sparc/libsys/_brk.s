.global cerror, __brk, __sbrk, limhp, brk, sbrk

__brk:	
	ld	[%l0], %o0
brk:	inc	7, %o0
	andn	%o0, 7, %o0
	mov     %o0, %o2
	mov     0x11, %g1
	ta	%g0
	nop
	bgeu	0f
	nop
	set	cerror, %o5
	jmp	%o5
	nop
0:
	set	limhp, %g1
	st	%o2, [%g1]
	retl
	nop


__sbrk:	
	ld	[%l0], %o0
sbrk:	inc	7, %o0
	andn	%o0, 7, %o0
	set	limhp, %o1
	ld	[%o1], %o2
	inc	7, %o2
	andn	%o2, 7, %o3
	add	%o3, %o0, %o0
	mov	%o0, %o4
	mov     0x11, %g1
	ta	%g0
	nop
	bgeu	0f
	nop
	set	cerror, %o5
	jmp	%o5
	nop
0:
	set	limhp, %g1
	st	%o4, [%g1]
	mov	%o3, %o0
	retl
	nop
