.define .inn
.sect .text
.sect .rom
.sect .data
.sect .bss

! d0 : set size in bytes
! d1 : bitnumber

	.sect .text
.inn:
	move.l	(sp)+,a1
	move.l	(sp)+,d1
	move.l	d0,-(sp)
	move.l	sp,a0
	add.l	#4,a0
	move.l	d1,d2
	asr.l	#4,d2
	!bchg.l	#0,d2
	cmp.l	d0,d2
	bcc	1f
	add.l	d2,a0
	move.l	(a0),d2
	btst	d1,d2	!eigenlijk .l
	beq	1f
	move.l	#1,d0
	bra	2f
1:
	clr.l	d0
2:
	move.l	(sp)+,d1
	add.l	d1,sp
	move.l	a1,-(sp)
	rts
