.define Adf4
.define Adf8
.define Sbf4
.define Sbf8
.define Mlf4
.define Mlf8
.define Dvf4
.define Dvf8
.define Ngf4
.define Ngf8
.define Zrf4
.define Zrf8
.define Cmf4
.define Cmf8
.define Fef4
.define Fef8
.define Fif4
.define Fif8
.define Cfi
.define Cif
.define Cuf
.define Cff
.define Cfu
.define Lfr8
.define Ret8
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Dummy floating point package for 6500
! every EM floating point instruction results in an
! "Illegal EM instruction" trap.


Adf4:
Adf8:
Sbf4:
Sbf8:
Mlf4:
Mlf8:
Dvf4:
Dvf8:
Ngf4:
Ngf8:
Zrf4:
Zrf8:
Cmf4:
Cmf8:
Fef4:
Fef8:
Fif4:
Fif8:
Cfi:
Cif:
Cuf:
Cff:
Cfu:
Lfr8:
Ret8:
	ldx #Eillins
	lda #0
	jsr Trap
