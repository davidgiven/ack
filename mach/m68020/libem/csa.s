.define	.csa
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

.csa:
	move.l	(sp)+, a0	! jump table address
	move.l	(a0)+, a1	! default address
	move.l	(sp)+, d0	! index
	sub.l	(a0)+, d0	! index - lower bound
	blt	1f
	cmp.l	(a0)+, d0	! rel. index, upper - lower bound
	bgt	1f
	move.l	(a0,d0.l*4), d1	! jump address
	beq	1f
	move.l	d1,a1
1:
	move.l	a1, d0
	beq	2f
	jmp	(a1)		! jump to specified address
2:
	move.l	#ECASE, -(sp)
	jmp	(.fatal)
.align 2
