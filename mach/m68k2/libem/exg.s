.define .exg
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 : exchange size in bytes
	.sect .text
.exg:
	move.l	(sp)+,d2	! return address
	move.l	sp,a1
	sub.w	d0,sp
	move.l	sp,a0
	move.w	d0,d1
1:
	move.w	(a1)+,(a0)+
	sub	#1,d0
	bgt	1b
	move.l	sp,a1
	asr	#1,d1
1:
	move.w	(a1)+,(a0)+
	sub	#1,d1
	bgt	1b
	move.l	a1,sp
	move.l	d2,-(sp)
	rts
