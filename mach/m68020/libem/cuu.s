.define	.ciu, .cui, .cuu
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
	! only converts between sizes of multiples of four
.ciu:
.cui:
.cuu:
	move.l	(sp)+, a0	! return address
	move.l	(sp)+, d0	! destination size
	sub.l	(sp)+, d0
	bgt	1f
	sub.l	d0, sp
	jmp	(a0)
1:
	asr.l	#2, d0
	sub.l	#1, d0
2:
	clr.l	-(sp)
	dbf	d0, 2b
	jmp	(a0)
.align 2
