.define .inn

! d0 : set size in bytes
! d1 : bitnumber

	.text
.inn:
	movem.l	d1/d2/a0/a1,.savreg
	move.l	(sp)+,.savret
	move.l	(sp)+,d1
	move.l	sp,a1
	add.l	d0,a1
	move.l	sp,a0
	move.l	d1,d2
	asr.l	#4,d2
	!bchg.l	#0,d2
	cmp.l	d0,d2
	bcc	1f
	add.l	d2,a0
	move.l	(a0),d2
	btst	d1,d2	!eigenlijk .l
	beq	1f
	move.l	#1,d0
	bra	2f
1:
	clr.l	d0
2:
	move.l	a1,sp
	movem.l	.savreg,d1/d2/a0/a1
	move.l	.savret,-(sp)
	rts
