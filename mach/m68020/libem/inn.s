.define	.inn
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

ESET=2

.inn:	! d0 bitnumber
	! d1 setsize in bytes
	! on exit: 0 or 1 in d0

	move.l	d2, -(sp)
	move.l	d0, d2
	asr.l	#3, d2		! offset from sp in bytes
	eor.l	#3, d2		! longs are stored in high-to-low order
	cmp.l	d1, d2
	bge	1f		! bitnumber too large
	btst	d0, (8,sp,d2.l)
	beq	2f
	move.l	#1, d0
	bra	3f
1:
	move.l	#ESET, -(sp)
	jsr	(.trp)
2:
	clr.l	d0
3:
	move.l	(sp)+, d2
	move.l	(sp)+, a0	! return address
	add.l	d1, sp		! pop bitset
	jmp	(a0)		! return
.align 2
