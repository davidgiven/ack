.global	strhp
EHEAP=17

strhp:				! %o0 = new heap pointer
	save	%sp, -64, %sp
	set	reghp, %o1
	st	%i0, [%o1]	! it'll probably go right, anyway
	set	limhp, %o1
	ld	[%o1], %o2
	cmp	%i0, %o2
	blt	1f
	nop
	add	%i0, 0x400, %i0	! extra 1K
	andn	%i0, 0x3ff, %o0
	st	%o0, [%o1]	! prematurely set limhp
	call	brk
	nop
	tst	%o0
	blt	2f		! failed
	nop
	set	limhp, %o1
	st	%o0, [%o1]
1:	ret
	restore
2:	
	restore
	set	EHEAP, %o0
	set	trp, %o1
	jmp	%o1
	nop
