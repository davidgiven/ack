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
	move.l	(sp)+,d1
	move.l	d0,d2
	asr.l	#2,d2
1:
	clr.l	-(sp)		!create empty set
	sub.l	#1,d2
	bgt	1b
	move.l	sp,a1		! set base
	move.l	d1,d2
	asr.l	#3,d2
	cmp.l	d0,d2
	bcs	1f
	move.w	#ESET,-(sp)
	jsr	.trp
1:
	bclr	#0,d2
	bclr	#1,d2
	add.l	d2,a1
	clr.l	d2
	bset	d1,d2
	move.l	d2,(a1)
	jmp	(a0)
