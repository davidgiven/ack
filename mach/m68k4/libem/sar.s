.define	.sar
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
	! (on entry d0 contains the integer size in the descriptor)
.sar:
	cmp.l	#4, d0
	beq	9f
	move.l	#EILLINS, -(sp)
	jmp	.fatal
9:
	! register usage:
	! a0: descriptor address, later base address
	! a1: return address
	! d0: index
	! d2: # bytes / element

	! For address calculation; see comment in 'aar.s'.
	! If the element size is a fraction of EM_WSIZE (4)
	! the calculation is done by adding.

	move.l	(sp)+,a1	! return address
	move.l	(sp)+, a0
	move.l	(sp)+, d0
	sub.l	(a0), d0
	move.l	8(a0), d2	! # bytes / element
	move.l	(sp)+, a0
	clr.l	d1
	cmp.l	#1, d2
	bne 	2f
	move.l	(sp)+, d1
	move.b	d1, 0(a0, d0.l)	! store one byte element
	bra	5f
2:
	cmp.l	#2, d2
	bne 	4f
	asl.l	#1, d0
	move.l	(sp)+, d1
	move.w	d1, 0(a0, d0.l)	! store two byte element
	bra	5f
4:				! here: 4n byte element
	move.l	a0,-(sp)
	move.l	d0, -(sp)
	move.l	d2, -(sp)
	jsr	.mlu
	move.l	(sp)+,a0
	add.l	d1, a0		! address of 4n byte element
	asr.l	#2, d2
	sub.l	#1, d2
1:
	move.l	(sp)+, (a0)+
	dbf	d2, 1b
5:
	jmp	(a1)
.align 2
