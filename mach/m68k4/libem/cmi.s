.define	.cmi, .cmi_
.sect .text
.sect .rom
.sect .data
.sect .bss

	! NUM == 4
	! result in d1
	.sect .text
.cmi:
.cmi_:
	move.l	(sp)+,d2
	move.l	#1,d1
	move.l	(sp)+,d0
	cmp.l	(sp)+,d0
	bne	1f
	clr.l	d1
	1:
	ble	2f
	neg.l	d1
	2:
	move.l	d2,-(sp)
	rts

.align 2
