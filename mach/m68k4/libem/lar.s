.define	.lar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
	! (on entry d0 contains the integer size in the descriptor)
.lar:
	cmp.l	#4, d0
	beq	9f
	move.l	#EILLINS, -(sp)
	jmp	.fatal
9:
	! register usage:
	! a0: descriptor address, later base addres
	! a1: return address
	! d0: index
	! d2: #bytes / element

	! For address calculation; see comment in 'aar.s'.

	move.l	(sp)+, a1	! return address
	move.l	(sp)+, a0	! descriptor address
	move.l	(sp)+, d0	! index
	sub.l	(a0), d0	! relative index
	move.l	8(a0), d2	! #bytes / element
	move.l	(sp)+, a0	! array base address
	clr.l	d1
	cmp.l	#1, d2
	bne	2f
	move.b	0(a0, d0.l), d1	! one byte element
	move.l	d1, -(sp)
	bra	5f
2:
	cmp.l	#2, d2
	bne	4f
	asl.l	#1, d0
	move.w	0(a0, d0.l), d1	! two byte element
	move.l	d1, -(sp)
	bra	5f
4:	
	move.l	a0,-(sp)
	move.l	d0, -(sp)
	move.l	d2, -(sp)
	jsr	.mlu
	move.l	(sp)+,a0
	add.l	d1, a0		! address of 4n byte element
	add.l	d2, a0		! because of predecrement
	asr.l	#2, d2
	sub.l	#1, d2
1:
	move.l	-(a0), -(sp)
	dbf	d2, 1b
5:
	jmp	(a1)
.align 2
