.define .adf4,.adf8,.sbf4,.sbf8,.mlf4,.mlf8,.dvf4,.dvf8
.define .ngf4,.ngf8,.fif4,.fif8,.fef4,.fef8
.define .cfi,.cif4,.cif8,.cuf4,.cuf8,.cff4,.cff8,.cfu
.define .cmf4,.cmf8
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Floating point is not implemented

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
.cfi:
.cif4:
.cif8:
.cuf4:
.cuf8:
.cff4:
.cff8:
.cfu:
.cmf4:
.cmf8:
	call eunimpl
	ret
