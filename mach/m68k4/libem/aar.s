.define	.aar
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
	! (on entry d0 contains the integer size in the descriptor)
.aar:
	cmp.l	#4, d0
	beq	9f
	move.l	#EILLINS, -(sp)
	jmp	.fatal
9:
	! register usage:
	! a0: descriptor address
	! a1: base address
	! d0: index

	! The address is calculated by taking the relative index
	! (index - lower bound), multiplying that with the element
	! size and adding the result to the array base address.

	move.l	(sp)+,a1	! return address
	move.l	(sp)+, a0	! descriptor address
	move.l	(sp)+, d0	! index
	sub.l	(a0), d0	! relative index
	move.l	a1,-(sp)
	move.l	d0, -(sp)
	move.l	8(a0), -(sp)	! # bytes / element
	jsr	.mlu
	move.l	(sp)+,a1
	move.l	(sp)+, a0	! array base address
	lea	0(a0, d1.l), a0	! address of array element
	jmp	(a1)
.align 2
