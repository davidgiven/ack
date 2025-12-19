.define Mli2, Mli, Mlix
.define Mlu2, Mlu, Mlux
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss

! The subroutine Mli2 multiplies two signed integers. The integers
! are popped from the stack.
! The subroutine Mlinp expects the two integer to be in zeropage.
! While the subroutine Mul an unsigned multiply subroutine is.

! Mli2, Mlu2:	multiply AccAB * TOS
! Mlix, Mlux:	multiply AccAB * IX
! Mli,  Mlu:	multiply AccAB * ARTH

.sect .text
Mli2:
Mlu2:
	stab <ARTH+1
	staa <ARTH
	tsx
	ldx 0,x
	stx <TMP
	ins
	ins
	pula
	pulb
	bsr Mlu
	ldx <TMP
	jmp 0,x
Mlix:
Mlux:
	stx <ARTH
Mli:
Mlu:
	stab <ARTH+3
	staa <ARTH+2
	clrb
	clra
    	ldx #16
1:
	aslb
	rola
	rol ARTH+1
	rol ARTH
	bcc 2f
	addb <ARTH+3
	adca <ARTH+2
2:	dex
	bne 1b
	rts
