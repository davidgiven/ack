.global	csa

ECASE=20
STACK_CLICK=4

csa:
		! jump table address in %o0
		! index in %o1

	ld	[%o0], %o4		! default address
	ld	[%o0+4], %o2
	subcc	%o1, %o2, %o1		! index - lower bound
	bl	1f
	ld	[%o0+2*4], %o3
	cmp	%o1, %o3		! rel. index, upper - lower bound
	bgt	1f
	nop
	sll	%o1, 2, %o5
	add	%o0, %o5, %o3		! %o3 = @jump address
	ld	[%o3+12], %o2		! %o2 = jump address
	tst	%o2
	be	1f
	nop
	jmp	%o2
	nop
1:
	tst	%o4
	beq	2f
	nop
	jmp	%o4
	nop
2:
	mov	ECASE, %o0
	set	trp, %g1
	jmp	%g1
	nop
.align 4
