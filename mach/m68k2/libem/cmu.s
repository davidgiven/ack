.define .cmu
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 : # bytes of 1 block
	.sect .text
.cmu:
	move.l	(sp)+,d2	! reta
	move.l	sp,a0		! top block
	move.l	sp,a1
	move.l	d2,-(sp)
	add	d0,a1		! lower block
	move	d0,d2
	asr	#1,d0
	move.l	#1,d1		! greater
1:
	cmp	(a0)+,(a1)+
	bne	2f
	sub	#1,d0
	bgt	1b
	clr	d1		! equal
2:
	bcc	3f
	neg	d1		! less
3:
	move.l	(sp)+,a0
	asl	#1,d2
	add	d2,sp		! new sp
	jmp	(a0)
