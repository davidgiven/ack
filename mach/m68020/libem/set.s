.define	.set
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect	.text
ESET=2

.set:	! d0 bitnumber
	! d1 setsize in bytes
	! a1 saved d2
	! a0 return address

	move.l	(sp)+, a0
	move.l	d2, a1
	move.l	d1, d2
	asr.l	#2, d2
	sub.l	#1, d2
1:
	clr.l	-(sp)
	dbf	d2, 1b

	move.l	d0, d2
	asr.l	#3, d2		! offset from sp in bytes
	eor.l	#3, d2		! longs are stored in high-to-low order
	cmp.l	d1, d2
	blt	2f
	move.l	#ESET, -(sp)	! bitnumber too large
	move.l	a1, d2
	move.l	a0, -(sp)
	jmp	(.trp)
2:
	bset	d0, (sp, d2.l)
	move.l	a1, d2
	jmp	(a0)	! return
.align 2
