.define .lxa
.extern .lxa
.lxa:
	! #levels (>= 0) on stack

	move.l	(sp)+,a0	! return address
	move.w	(sp)+,d2
	move.l	a0,-(sp)
	move.l	a6,a0
1:
	move.l	a0,-(sp)
	jsr	.lpb
	sub	#1,d2
	blt	2f
	move.l	(a0),a0
	bra	1b
2:
	rts
