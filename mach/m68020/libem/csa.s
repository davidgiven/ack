.define	.csa4
.define .csa
.sect .text
.sect .rom
.sect .data
.sect .bss
ECASE=20

.sect .text

.csa4:
.csa:
		! jump table address in a0
		! index in d0

	move.l	(a0)+, a1	! default address
	sub.l	(a0)+, d0	! index - lower bound
	blt	1f
	cmp.l	(a0)+, d0	! rel. index, upper - lower bound
	bgt	1f
	move.l	(a0,d0.l*4), d1	! jump address
	beq	1f
	move.l	d1,a1
	jmp	(a1)
1:
	move.l	a1, d0
	beq	2f
	jmp	(a1)		! jump to specified address
2:
	move.l	#ECASE, -(sp)
	jmp	(.fatal)
.align 2
