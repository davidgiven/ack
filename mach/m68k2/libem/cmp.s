.define	.cmp
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.cmp:
	move.l	(sp)+,a0	! return address
	move.l	#1,d1
	move.l	(sp)+,d0
	cmp.l	(sp)+,d0
	bne	1f
	clr	d1
	1:
	bcs	2f
	neg	d1
	2:
	jmp	(a0)
