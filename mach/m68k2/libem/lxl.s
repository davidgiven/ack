.define .lxl
.sect .text
.sect .rom
.sect .data
.sect .bss
.extern .lxl
	.sect .text
.lxl:
	! #levels on stack (> 0)

	move.l	(sp)+,a0	! return address
	move.w	(sp)+,d2	! d2 is not destroyed by .lpb
	move.l	a0,-(sp)
	sub.w	#1,d2
	move.l	a6,a0
1:
	move.l	a0,-(sp)
	jsr	.lpb
	move.l	(a0),a0
	dbf	d2,1b
	rts			! result in a0
