.define .csa4
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.csa4:
	! case descriptor in a0
	! index in d0
	move.l	(a0)+,a1	! default address
	sub.l	(a0)+,d0	! index - lower bound
	blt	1f
	cmp.l	(a0)+,d0	! rel. index <-> upper - lower bound
	bgt	1f
	asl.l	#2,d0
	add.l	d0,a0
	move.l	(a0),d0		! test jump address
	beq	1f
	move.l	d0,-(sp)
	bra	3f
1:
	move.l	a1,d0		! test default jump address
	beq	2f
	move.l	a1,-(sp)	! jump address
3:
	rts			! not a real rts
2:
	move.l	#ECASE,-(sp)
	jmp	.fatal

.align 2
