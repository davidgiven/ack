.define .sts
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 : # bytes
	! a0 : destination address
	.sect .text
.sts:
	move.l	(sp)+,a1
	move.w	(sp)+,d0
	move.l	(sp)+,a0
	cmp	#1,d0
	bne	1f
	move.w	(sp)+,d0
	move.b	d0,(a0)
	bra	3f
1:
	asr	#1,d0
2:
	move.w	(sp)+,(a0)+
	sub	#1,d0
	bgt	2b
3:
	jmp	(a1)
