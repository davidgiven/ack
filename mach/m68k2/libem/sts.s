.define .sts
.define .sts4
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
	ext.l	d0
9:
	move.l	(sp)+,a0
	cmp.l	#1,d0
	bne	1f
	move.w	(sp)+,d0
	move.b	d0,(a0)
	bra	3f
1:
	asr.l	#1,d0
2:
	move.w	(sp)+,(a0)+
	sub.l	#1,d0
	bgt	2b
3:
	jmp	(a1)
.sts4:
	move.l	(sp)+,a1
	move.l	(sp)+,d0
	bra	9b
