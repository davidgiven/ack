.define	.cmp

	.text
.cmp:
	move.l	(sp)+,.savret	! return address
	move.l	d0,.savd0
	move.l	#1,d1
	move.l	(sp)+,d0
	cmp.l	(sp)+,d0
	bne	1f
	clr.l	d1
	1:
	bcs	2f
	neg.l	d1
	2:
	move.l	.savd0,d0
	move.l	.savret,-(sp)
	rts
.align 2
