.define .csa4
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.csa4:
	move.l	(a0)+,a1	! default address
	sub.l	(a0)+,d0	! index - lower bound
	blt	1f
	cmp.l	(a0)+,d0	! rel. index <-> upper - lower bound
	bgt	1f
	asl.l	#2,d0
	add.l	d0,a0
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
