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
	move.l	(sp), d1
	lsr.l	#1, d0
	bcs	1f		! branch if source size == 1
	lsr.l	#1, d0
	bcs	2f		! branch if source size == 2
	tst.l	d1
	bra	4f
1:	lsr.l	#1, d0		! size difference / 4
	ext.w	d1
2:	ext.l	d1
	move.l	d1, (sp)
4:	slt	d1
	extb.l	d1		! d1 contains sign of source
	sub.l	#1, d0
2:
	move.l	d1, -(sp)
	dbf	d0, 2b
	jmp	(a0)
.align 2
