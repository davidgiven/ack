.define .sts

	! d0 : # bytes
	! a0 : destination address
	.text
.sts:
	movem.l	d0/a0,.savreg
	move.l	(sp)+,.savret
	move.l	(sp)+,d0
	move.l	(sp)+,a0
	cmp	#1,d0
	bne	1f
	move.l	(sp)+,d0
	move.b	d0,(a0)
	bra	4f

1:	cmp	#2,d0
	bne	2f
	move.l	(sp)+,d0
	add.l	#2,a0
	move.w	d0,(a0)
	bra	4f
2:
	asr	#2,d0
	sub.l	#1,d0
3:
	move.l	(sp)+,(a0)+
	dbf	d0,3b
4:
	movem.l	.savreg,d0/a0
	move.l	.savret,-(sp)
	rts
.align 2
