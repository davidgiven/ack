.define	.exg
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
	! d0 : exchange size in bytes
.exg:
	lea	(4,sp,d0), a0	! address of bottom block
	lea	(4,sp), a1	! address of top block
	asr.l	#2, d0
1:
	move.l	(a1), d1
	move.l	(a0), (a1)+
	move.l	d1, (a0)+
	sub.l	#1, d0
	bne	1b
	rts
.align 2
