.define .cmu
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 : # bytes of 1 block
	.sect .text
.cmu:
	move.l	sp,a0		! top block
	add.l	#4,a0
	move.l	a0,a1
	add.l	d0,a1		! lower block
	move.l	d0,d2
	asr.l	#2,d0
	sub.l	#1,d0
	move.l	#1,d1		! greater
1:
	cmp.l	(a0)+,(a1)+
	bne	2f
	dbf	d0,1b
	clr.l	d1		! equal
2:
	bcc	3f
	neg.l	d1		! less
3:
	asl.l	#1,d2
	move.l	(sp)+,a0
	add.l	d2,sp		! new sp
	jmp	(a0)

.align 2
