.define	.cii
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.cii:
	move.l	(sp)+, a0	! return address
	move.l	(sp)+, d0	! destination size
	sub.l	(sp)+, d0	! destination - source size
	bgt	1f
	sub.l	d0, sp		! pop extra bytes
	jmp	(a0)
1:
	clr.l	d1
	tst.l	(sp)
	bne	4f
	not.l	d1		! d1 contains sign of source
4:
	asr.l	#2, d0
	sub.l	#1, d0
2:
	move.l	d1, -(sp)
	dbf	d0, 2b
	jmp	(a0)
.align 2
