.define	.aar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

.aar:	! (on entry d0 contains the integer size in the descriptor)

	cmp.l	#4, d0
	beq	9f
	move.l	#EILLINS, -(sp)
	jmp	(.fatal)
9:
	! register usage:
	! a0: descriptor address
	! a1: return address
	! d0: index

	! The address is calculated by taking the relative index
	! (index - lower bound), multiplying that with the element
	! size and adding the result to the array base address.

	move.l	(sp)+, a1	! return address
	move.l	(sp)+, a0	! descriptor address
	move.l	(sp)+, d0	! index
	sub.l	(a0), d0	! relative index
	muls.l	(8,a0), d0
	move.l	(sp)+, a0	! array base address
	add.l	d0, a0		! address of array element
	jmp	(a1)		! return
.align 2
