.define .csa
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.csa:
	move.l	(sp)+,a0	! case descriptor
	move	(sp)+,d0	! index
	move.l	(a0)+,a1	! default address
	sub	(a0)+,d0	! index - lower bound
	blt	1f
	cmp	(a0)+,d0	! rel. index <-> upper - lower bound
	bgt	1f
	asl	#2,d0
	add	d0,a0
	move.l	(a0),d1		! test jump address
	move.l	d1,d0
	beq	1f
	move.l	d1,a1
	bra	3f
1:
	move.l	a1,d0		! test default jump address
	beq	2f
3:
	jmp	(a1)
2:
	move.w	#ECASE,-(sp)
	jmp	.fat
