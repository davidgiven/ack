.define .exg
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 : exchange size in bytes
	.sect .text
.exg:
	move.l	(sp)+,d2
	move.l	sp,a1
	sub.l	d0,sp
	move.l	sp,a2
	asr	#1,d0
	move.l	d0,d1
1:
	move.l	(a1)+,(a2)+
	sub	#1,d0
	bgt	1b
	move.l	sp,a1
	asr	#1,d1
1:
	move.l	(a1)+,(a2)+
	sub	#1,d1
	bgt	1b
	move.l	a1,sp
	move.l	d2,-(sp)
	rts
