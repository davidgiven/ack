.define .los
.define .los4
.sect .text
.sect .rom
.sect .data
.sect .bss

	! d0 : # bytes
	! a0 : source address
	.sect .text
.los:
	move.l	(sp)+,a1
	move.w	(sp)+,d0
	ext.l	d0
9:
	move.l	(sp)+,a0
	cmp	#1,d0
	bne	1f
	clr	d0
	move.b	(a0),d0
	move.w	d0,-(sp)
	bra	3f
1:
	add.l	d0,a0
	asr.l	#1,d0
2:
	move	-(a0),-(sp)
	sub.l	#1,d0
	bgt	2b
3:
	jmp	(a1)

	! d0 : # bytes
	! a0 : source address
	.sect .text
.los4:
	move.l	(sp)+,a1
	move.l	(sp)+,d0
	bra 9b
.align 2
