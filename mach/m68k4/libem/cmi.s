.define	.cmi, .cmi_

	! NUM == 4
	! result in d1
	.text
.cmi:
.cmi_:
	move.l	(sp)+,.savret
	move.l	d0,.savd0
	move.l	#1,d1
	move.l	(sp)+,d0
	cmp.l	(sp)+,d0
	bne	1f
	clr.l	d1
	1:
	ble	2f
	neg.l	d1
	2:
	move.l	.savd0,d0
	move.l	.savret,-(sp)
	rts
.align 2
