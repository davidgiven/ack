.define .sar
.define .lar
.define .aar
.sect .text
.sect .rom
.sect .data
.sect .bss

	!register usage:
	! a0 : descriptor address
	! d0 : index
	! a1 : base address
	.sect .text
.aar:
	move.l	(sp)+,d2	! return address
	move.l	(sp)+,a0	! descriptor address
	move.l	(sp)+,d0	! index
	move.l	(sp)+,a1	! base address
	sub.l	(a0),d0		! index - lower bound : relative index
	move.l	8(a0),-(sp)	! # bytes / element
	move.l	d0,-(sp)
	jsr	.mlu
	add.l	d1,a1		! address of element
	move.l	a1,-(sp)	! returned on stack
	move.l	d2,-(sp)
	rts

.lar:
	! register usage: like .aar

	move.l	(sp)+,d2	! return address
	move.l	(sp)+,a0
	move.l	(sp)+,d0
	move.l	(sp)+,a1
	sub.l	(a0),d0
	move.l	d0,-(sp)
	move.l	8(a0),d0
	move.l	d0,-(sp)
	jsr	.mlu
	add.l	d1,a1		! address of element
	add.l	8(a0),a1	! a1++ because of predecrement
	clr.l	d1		!?nodig?
	asr	#1,d0
	bne	3f
	move.b	-(a1),d1	! 1 byte element
	move.l	d1,-(sp)
	bra	5f
3:
	asr	#1,d0
	bne	4f
	move.w	-(a1),d1	! 2 byte element
	move.l	d1,-(sp)
	bra 	5f
4:
	sub.l	#1,d0
1:
	move.l	 -(a1),-(sp)	! 4n byte element (n = 1,2,...)
	dbf 	d0,1b
5:
	move.l	d2,-(sp)
	rts


.sar:
	!register usage: same as lar

	move.l	(sp)+,d2
	move.l	(sp)+,a0
	move.l	(sp)+,d0
	move.l	(sp)+,a1
	sub.l	(a0),d0
	move.l	d0,-(sp)
	move.l	8(a0),d0	! # bytes / element
	move.l	d0,-(sp)
	jsr	.mlu
	add.l	d1,a1
	clr.l	d1		!?nodig?
	asr	#1,d0
	bne	3f
	move.l	(sp)+,d1
	move.b	d1,(a1)		! 1 byte element 
	bra	4f
3:
	asr	#1,d0
	bne	5f
	move.l	(sp)+,d1
	move.w	d1,(a1)		! 2 byte element 
	bra	4f
5:
	sub.l	#1,d0
1:
	move.l	(sp)+,(a1)+	! 4n byte element (n = 1,2,...)
	dbf	d0,1b
4:
	move.l	d2,-(sp)
	rts

.align 2
