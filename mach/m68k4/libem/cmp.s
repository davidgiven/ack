.define	.cmp
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text
.cmp:
	move.l	(sp)+,d2	! return address
	move.l	#1,d1
	move.l	(sp)+,d0
	cmp.l	(sp)+,d0
	bne	1f
	clr.l	d1
	1:
	bcs	2f
	neg.l	d1
	2:
	move.l	d2,-(sp)
	rts

.align 2
