.define .adf4, .adf8, .sbf4, .sbf8, .mlf4, .mlf8, .dvf4, .dvf8
.define .ngf4, .ngf8, .fif4, .fif8, .fef4, .fef8
.define .cif4, .cif8, .cuf4, .cuf8, .cfi, .cfu, .cff4, .cff8
.define .cmf4, .cmf8
.sect .text
.sect .rom
.sect .data
.sect .bss

!	$Header$

	.sect .text
.adf4:
.adf8:
.sbf4:
.sbf8:
.mlf4:
.mlf8:
.dvf4:
.dvf8:
.ngf4:
.ngf8:
.fif4:
.fif8:
.fef4:
.fef8:
.cif4:
.cif8:
.cuf4:
.cuf8:
.cfi:
.cfu:
.cff4:
.cff8:
.cmf4:
.cmf8:
	move.l	#18,-(sp)
	jsr	.trp
	rts
