.define	.lar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
EILLINS=18
	! (on entry d0 contains the integer size in the descriptor)
.lar:
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

	! For address calculation; see comment in 'aar.s'.

	move.l	(sp)+, d1	! return address
	move.l	(sp)+, a0	! descriptor address
	move.l	(sp)+, d0	! index
	move.l	(sp)+, a1	! array base address
	sub.l	(a0), d0	! relative index
	move.l	d1,-(sp)
	move.l	(8,a0), d1	! # bytes / element
	cmp.l	#1, d1
	bne	2f
	move.l	(sp),a0
	clr.l	(sp)
	move.b	(a1,d0.l), (3,sp) ! one byte element
	jmp	(a0)
2:
	cmp.l	#2, d1
	bne	4f
	move.l	(sp),a0
	clr.l	(sp)
	move.w	(a1,d0.l*2), (2,sp) ! two byte element
	jmp	(a0)
4:	
	muls.l	d1, d0
	add.l	d0, a1		! address of 4n byte element
	add.l	d1, a1		! because of predecrement
	asr.l	#2, d1
	move.l	(sp)+,a0
1:
	move.l	-(a1), -(sp)
	sub.l	#1, d1
	bne	1b
	jmp	(a0)
.align 2
