.define	.cms

	! d0 contains set size

	.text
.cms:
	move.l	(sp)+,a2	! return address
	move.l	sp,a0
	move.l	sp,a1
	add	d0,a1
	move.w	d0,d1
	asr	#1,d0
1:
	cmp	(a0)+,(a1)+
	bne	2f
	sub.l	#1,d0
	bne	1b
2:
	asl	#1,d1
	add	d1,sp
	move.l	d0,-(sp)
	jmp	(a2)
