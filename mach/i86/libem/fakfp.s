.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define .mlf,.dvf,.ngf,.adf,.sbf,.cmf,.zrf,.fif,.fef
.define .mlf8,.dvf8,.ngf8,.adf8,.sbf8,.cmf8,.zrf8,.fif8,.fef8
.define .mlf4,.dvf4,.ngf4,.adf4,.sbf4,.cmf4,.zrf4,.fif4,.fef4
.define .cif,.cfi,.cuf,.cfu,.cff

.mlf:
.dvf:
.ngf:
.adf:
.sbf:
.cmf:
.zrf:
.fif:
.fef:
.mlf4:
.dvf4:
.ngf4:
.adf4:
.sbf4:
.cmf4:
.zrf4:
.fif4:
.fef4:
.mlf8:
.dvf8:
.ngf8:
.adf8:
.sbf8:
.cmf8:
.zrf8:
.fif8:
.fef8:
.cif:
.cfi:
.cuf:
.cfu:
.cff:
	pop     bx              ! return address
.extern EILLINS
.extern .fat
	mov     ax,EILLINS
	push    ax
	jmp     .fat
