.define .sar
.define .lar
.define .aar

	!register usage:
	! a0 : descriptor address
	! d0 : index
	! a1 : base address
	.text
.aar:
	movem.l d0/d1/a0/a1,.savreg
	move.l	(sp)+,.retara	! return address
	move.l	(sp)+,a0	! descriptor address
	move.l	(sp)+,d0	! index
	move.l	(sp)+,a1	! base address
	sub.l	(a0),d0		! index - lower bound : relative index
	move.l	8(a0),-(sp)	! # bytes / element
	move.l	d0,-(sp)
	jsr	.mlu
	add.l	d1,a1		! address of element
	move.l	a1,-(sp)	! returned on stack
	move.l	.retara,-(sp)
	movem.l	d0/d1/a0/a1,.savreg
	rts


.lar:
	! register usage: like .aar
	! d2 : # bytes / element

	movem.l d0/d1/d2/a0/a1,.savreg
	move.l	(sp)+,.retara	! return address
	move.l	(sp)+,a0
	move.l	(sp)+,d0
	move.l	(sp)+,a1
	sub.l	(a0),d0
	move.l	8(a0),d2	! # bytes / element
	move.l	d0,-(sp)
	move.l	d2,-(sp)
	jsr	.mlu
	add.l	d1,a1		! address of element
	add.l	d2,a1		! a1++ because of predecrement
	clr.l	d1		!?nodig?
	asr	#1,d2
	bne	3f
	move.b	-(a1),d1	! 1 byte element
	move.l	d1,-(sp)
	bra	5f
3:
	asr	#1,d2
	bne	4f
	move.w	-(a1),d1	! 2 byte element
	move.l	d1,-(sp)
	bra 	5f
4:
	sub.l	#1,d2
1:
	move.l	 -(a1),-(sp)	! 4n byte element (n = 1,2,...)
	dbf 	d2,1b
5:
	move.l	.retara,-(sp)
	movem.l	.savreg,d0/d1/d2/a0/a1
	rts


.sar:
	!register usage: same as lar

	movem.l	d0/d1/a0/a1,.savreg
	move.l	(sp)+,.retara
	move.l	(sp)+,a0
	move.l	(sp)+,d0
	move.l	(sp)+,a1
	sub.l	(a0),d0
	move.l	8(a0),d2	! # bytes / element
	move.l	d0,-(sp)
	move.l	d2,-(sp)
	jsr	.mlu
	add.l	d1,a1
	clr.l	d1		!?nodig?
	asr	#1,d2
	bne	3f
	move.l	(sp)+,d1
	move.b	d1,(a1)		! 1 byte element 
	bra	4f
3:
	asr	#1,d2
	bne	5f
	move.l	(sp)+,d1
	move.w	d1,(a1)		! 2 byte element 
	bra	4f
5:
	sub.l	#1,d2
1:
	move.l	(sp)+,(a1)+	! 4n byte element (n = 1,2,...)
	dbf	d2,1b
4:
	move.l	.retara,-(sp)
	movem.l	.savreg,d0/d1/a0/a1
	rts
.data
.retara:	.long 0
.align 2
