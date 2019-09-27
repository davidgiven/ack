.define	.csa8
.sect .text
.sect .rom
.sect .data
.sect .bss
ECASE=20

.sect .text

.csa8:
		! jump table address in a0
		! index in (sp)

	move.l	(a0)+, a1	! default address
	move.l	(sp)+, d0
	move.l	(sp)+, d1	! index
	move.l	(a0)+, d2
	sub.l	(a0)+, d1
	subx.l	d2, d0		! index - lower bound in d0:d1
	move.l	d1, a2
	move.l	(a0)+, d2
	move.l	(a0)+, d1	! upper - lower bound
	sub.l	a2, d1
	subx.l	d0, d2		! upper - index
	bcs	1f
	move.l	a2, d1
	! assuming d0:d1 < 65536
	move.l	(a0,d1.l*4), d1	! jump address
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
