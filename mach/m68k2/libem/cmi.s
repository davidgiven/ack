.define	.cmi, .cmi_

	! NUM == 4
	! result in d1
	.text
.cmi:
.cmi_:
	move.l	(sp)+,a0
	move.l	#1,d1
	move.l	(sp)+,d0
	cmp.l	(sp)+,d0
	bne	1f
	clr	d1
	1:
	ble	2f
	neg	d1
	2:
	jmp	(a0)
