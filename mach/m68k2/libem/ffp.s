.define .cmf8, .cif8, .dvf8, .adf8, .ngf8, .mlf8, .fif8, .fef8, .cff4
.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text
.cmf8:
.cif8:
.dvf8:
.adf8:
.ngf8:
.mlf8:
.fif8:
.fef8:
.cff4:
	move.w	#EILLINS,-(sp)
	jmp	.fat
