.define .set
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 : setsize in bytes
	! d1 : bitnumber
	.sect .text
.set:
	move.l	(sp)+,a0
	move.w	(sp)+,d1
	move.w	d0,d2
	asr	#1,d2
1:
	clr	-(sp)
	sub	#1,d2
	bgt	1b
	move.l	sp,a1		! set base
	move.w	d1,d2
	asr	#3,d2
	bchg	#0,d2
	cmp	d0,d2
	bcs	1f
	move.w	#ESET,-(sp)
	move.l	a0,-(sp)
	jmp	.trp
1:
	add	d2,a1
	bset	d1,(a1)
	jmp	(a0)
