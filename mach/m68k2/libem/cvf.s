.define	.cfi,.cif,.cfu,.cuf,.cff

	.text

	! this is a dummy float conversion routine
.cfi:
.cif:
.cfu:
.cuf:
.cff:
	move.l	(sp)+,a0	! return address
	move	(sp)+,d0	! destination size
	sub	(sp)+,d0	! diff. in sizes
	sub	d0,sp
	jmp	(a0)
