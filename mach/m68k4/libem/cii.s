.define	.cii

	.text
.cii:
	movem.l	a0/d0/d1,.savreg
	move.l	(sp)+,a0	! return address
	move	(sp)+,d0	! destination size
	sub	(sp)+,d0	! destination - source size
	bgt	1f
	sub	d0,sp		! pop extra bytes
	bra	3f
1:
	move	(sp),d1
	ext.l	d1
	swap	d1
	asr	#1,d0
2:
	move.w	d1,-(sp)
	dbf	d0,2b
3:
	move.l	a0,-(sp)
	movem.l	.savreg,a0/d0/d1
	rts
.align 2
