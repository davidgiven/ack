.define	.cfi, .cif,.cfu,.cuf,.cff
.sect .text
.sect .rom
.sect .data
.sect .bss

	.sect .text

	! this is a dummy float conversion routine
.cfi:
.cif:
.cfu:
.cuf:
.cff:
	move.l	(sp)+, a0	! return address
	move.l	(sp)+, d0	! destination size
	sub.l	(sp)+, d0	! diff. in sizes
	sub.l	d0, sp
	jmp	(a0)
.align 2
