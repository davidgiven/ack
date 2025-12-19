.define Mli4, Mlu4,  Mlinp4, Mul4
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss

! The subroutine Mli4 multiplies two signed long. The long
! are popped from the stack.
! The subroutine Mlinp expects the two long to be in zeropage.
! While the subroutine Mul an unsigned multiply subroutine is.

.sect .text
Mli4:
Mlu4:
	tsx
	ldx 0,x
	stx <TMP
	ins
	ins
	pula
	pulb
	stab <ARTH+1
	staa <ARTH+0
	pula
	pulb
	stab <ARTH+3
	staa <ARTH+2
	pula
	pulb
	stab <ARTH+5
	staa <ARTH+4
	pula
	pulb
	stab <ARTH+7
	staa <ARTH+6
!
	bsr Mul4
!
	tsx
	ldx <TMP
	pshb
	psha
	ldab <ARTH+9
	ldaa <ARTH+8
	pshb
	psha
	ldx TMP
	jmp 0,x
! multiply long (ARTH+8) = (ARTH) * (ARTH+4)
Mul4:
Mlinp4:	
	clrb
	clra
	clr ARTH+8
	clr ARTH+9
!	clr ARTH+10
!	clr ARTH+11
    	ldx #32
1:
	aslb
	rola
	rol ARTH+9
	rol ARTH+8
	rol ARTH+3
	rol ARTH+2
	rol ARTH+1
	rol ARTH
	bcc 3f
	addb <ARTH+7
	adca <ARTH+6
	psha
	ldaa <ARTH+9
	adca <ARTH+5
	staa <ARTH+9
	ldaa <ARTH+8
	adca <ARTH+4
	staa <ARTH+8
	pula
3:
	dex
	bne 1b
	rts
