.define	.cms
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 contains set size

	.sect .text
.cms:
	move.l	(sp)+,d2	! return address
	move.l	sp,a0
	move.l	sp,a1
	add	d0,a1
	move.w	d0,d1
	asr	#1,d0
1:
	cmp	(a0)+,(a1)+
	bne	2f
	sub	#1,d0
	bgt	1b
2:
	asl	#1,d1
	add	d1,sp
	move.w	d0,-(sp)
	move.l	d2,-(sp)
	rts
