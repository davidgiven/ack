.define .inn
.sect .text
.sect .rom
.sect .data
.sect .bss

! d0 : set size in bytes
! d1 : bitnumber

	.sect .text
.inn:
	move.l	(sp)+,d2	! return address
	move.w	(sp)+,d1
	move.l	sp,a1
	add	d0,a1
	move.l	sp,a0
	move.l	d2,-(sp)
	move	d1,d2
	asr	#3,d2
	bchg	#0,d2
	cmp	d0,d2
	bcc	1f
	add	d2,a0
	btst	d1,(a0)
	beq	1f
	move.l	#1,d0
	bra	2f
1:
	clr	d0
2:
	move.l	(sp)+,a0
	move.l	a1,sp
	jmp	(a0)
