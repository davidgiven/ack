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
	jmp	(.fatal)
9:
	! register usage:
	! a0: descriptor address
	! a1: base address
	! d0: index
	! d1: # bytes / element
	! d2: return address

	! For address calculation; see comment in 'aar.s'.
	! If the element size is a fraction of EM_WSIZE (4)
	! the calculation is done by adding.

	move.l	(sp)+, d2	! return address
	move.l	(sp)+, a0
	move.l	(sp)+, d0
	move.l	(sp)+, a1
	sub.l	(a0), d0
	move.l	(8,a0), d1	! # bytes / element
	cmp.l	#1, d1
	bne 	2f
	move.l	(sp)+, d1
	move.b	d1, (a1,d0.l)	! store one byte element
	bra	5f
2:
	cmp.l	#2, d1
	bne 	4f
	move.l	(sp)+, d1
	move.w	d1, (a1,d0.l*2)	! store two byte element
	bra	5f
4:				! here: 4n byte element
	muls.l	d1, d0
	add.l	d0, a1		! address of 4n byte element
	asr.l	#2, d1
	sub.l	#1, d1
1:
	move.l	(sp)+, (a1)+
	dbf	d1, 1b
5:
	move.l	d2,a0
	jmp	(a0)
.align 2
