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
	move.l	(sp)+,a0
	move.w	(sp)+,d0
	move.l	(sp)+,a1
	sub	(a0),d0		! index - lower bound : relative index
	!chk	2(a0),d0
	!blt	9f
	!cmp	2(a0),d0
	!bgt	9f
	mulu	4(a0),d0	! total # bytes 
	add	d0,a1		! address of element
	move.l	a1,-(sp)
	move.l	d2,-(sp)
	rts


.lar:
	move.l	(sp)+,d2	! return address
	move.l	(sp)+,a0
	move.w	(sp)+,d0
	move.l	(sp)+,a1
	sub	(a0),d0
	!chk	2(a0),d0
	!blt	9f
	!cmp	2(a0),d0
	!bgt	9f
	move	4(a0),d1
	mulu	d1,d0
	add	d0,a1
	add	d1,a1
	asr	#1,d1
	bne	3f
	clr	d1
	move.b	-(a1),d1
	move	d1,-(sp)
	bra	4f
3:
	move	-(a1),-(sp)
	sub	#1,d1
	bgt	3b
4:
	move.l	d2,-(sp)
	rts


!9:
	!move.w	#EARRAY,-(sp)
	!jmp	.fat
.sar:
	move.l	(sp)+,d2
	move.l	(sp)+,a0
	move.w	(sp)+,d0
	move.l	(sp)+,a1
	sub	(a0),d0
	!chk	2(a0),d0
	!blt	9b
	!cmp	2(a0),d0
	!bgt	9b
	move	4(a0),d1
	mulu	d1,d0
	add	d0,a1
	asr	#1,d1
	bne	3f
	move	(sp)+,d1
	move.b	d1,(a1)
	bra	4f
3:
	move	(sp)+,(a1)+
	sub	#1,d1
	bgt	3b
4:
	move.l	d2,-(sp)
	rts
