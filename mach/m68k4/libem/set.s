.define	.set
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.set:	! d0 bitnumber
	! d1 setsize in bytes

	move.l	(sp)+,a0
	move.l	d1, d2
	asr.l	#2, d2
	sub.l	#1, d2
1:
	clr.l	-(sp)
	dbf	d2, 1b

	move.l	d0, d2
	asr.l	#3, d2		! offset .sect from sp in bytes
	eor.l	#3, d2		! longs are stored in high-to-low order
	cmp.l	d1, d2
	blt	2f
	move.l	a0, -(sp)
	move.l	#ESET, -(sp)	! bitnumber too large
	jmp	.trp
2:
	bset	d0, 0(sp, d2.l)
	jmp	(a0)
.align 2
