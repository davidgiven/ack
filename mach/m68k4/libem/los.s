.define .los

	! d0 : # bytes
	! a0 : source address
	.text
.los:
	movem.l	d0/a0,.savreg
	move.l	(sp)+,.savret
	move.l	(sp)+,d0
	move.l	(sp)+,a0
	cmp.l	#1,d0
	bne	1f
	clr.l	d0		!1 byte
	move.b	(a0),d0
	move.l	d0,-(sp)
	bra	3f
1:
	cmp.l 	#2,d0
	bne	2f
	clr.l 	d0		!2 bytes
	add.l	#2,a0
	move.w	(a0),d0
	move.l	d0,-(sp)
	bra	3f
2:
	add.l	d0,a0		!>=4 bytes
	asr.l 	#2,d0

4:	move.l	-(a0),-(sp)	
	sub.l	#1,d0
	bgt	4b
3:
	movem.l	.savreg,d0/a0
	move.l	.savret,-(sp)
	rts
.align 2
