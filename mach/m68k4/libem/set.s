.define .set

	! d0 : setsize in bytes
	! d1 : bitnumber
	.text
.set:
	movem.l	d1/d2/a1,.savreg
	move.l	(sp)+,.savret
	move.l	(sp)+,d1
	move.l	d0,d2
	asr.l	#2,d2
1:
	clr.l	-(sp)		!create empty set
	sub.l	#1,d2
	bgt	1b
	move.l	sp,a1		! set base
	move.l	d1,d2
	asr.l	#4,d2
	!bchg	#0,d2
	cmp.l	d0,d2
	bcs	1f
	move.w	#ESET,-(sp)
	move.l	.savret,-(sp)
	jmp	.trp
1:
	add.l	d2,a1
	move.l	(a1),d2
	bset	d1,d2
	move.l	d2,(a1)
	movem.l	.savreg,d1/d2/a1
	move.l	.savret,-(sp)
	rts
