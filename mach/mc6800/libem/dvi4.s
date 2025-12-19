.define Dvi4, Dvu4, Div32x32
.define Rmi4, Rmu4
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! The subroutine Dvi2 performs a signed division.
! Its operands are on the stack.
! The subroutine Dvi performs also a signed division, ecxept that
! its operand are already in zeropage.
! The subroutine Dvu performs a n unsigned division.


!
!	stack frame:
!	+0,+1: return address
!	+2,+5: divisor (ARTH, ARTH+3)
!	+6,+9,: diviend (ARTH+4, ARTH_7)
!
Store:
	ldab 9,x
	ldaa 8,x
	stab <ARTH+7	! store dividend
	staa <ARTH+6
	ldab 7,x
	ldaa 6,x
	stab <ARTH+5
	staa <ARTH+4
!
	ldab 5,x
	ldaa 4,x
	stab <ARTH+3
	staa <ARTH+2
	ldab 3,x
	ldaa 2,x
	stab <ARTH+1
	staa <ARTH
	rts
!
neg4:
	com 0,x
	com 1,x
	com 2,x
	neg 3,x
	bne 1f
	inc 2,x
	bne 1f
	inc 1,x
	bne 1f
	inc 0,x
1:
	rts
!
Dvi4:
	tsx
	bsr Store
!
	ldaa <ARTH
	staa <SIGN
	bpl 1f
	ldx #ARTH
	bsr neg4
1:
	ldaa <ARTH+4
	bpl 2f
	ldx #ARTH+4
	bsr neg4
2:
	bsr Div32x32
	tst SIGN
	bpl 3f
	ldx #ARTH+4
	bsr neg4
!	
3:
	tsx
	ldab <ARTH+7
	ldaa <ARTH+6
	stab 9,x
	staa 8,x
	ldab <ARTH+5
	ldaa <ARTH+4
	stab 7,x
	staa 6,x
Div4Ret:
	ldx 0,x
	ins
	ins
	ins
	ins
	ins
	ins
	jmp 0,x
!
Dvu4:
	tsx
	bsr Store
	bsr Div32x32
	bra 3b
!
!
Div32x32:
	ldx #32		! loop counter
	clra
	clrb
	clr ARTH+9
	clr ARTH+8
			! here, carry = 0
2:
	asl ARTH+7	! shift divient
	rol ARTH+6
	rol ARTH+5
	rol ARTH+4
	rolb
	rola
	rol ARTH+9
	rol ARTH+8
	subb <ARTH+3	! divisor
	sbca <ARTH+2
	pshb
	psha
	ldab <ARTH+9
	ldaa <ARTH+8
	sbcb <ARTH+1
	sbca <ARTH
	bcs 8f
	stab <ARTH+9
	stab <ARTH+8
	pula
	pulb
	inc ARTH+7
	bra 9f
8:
	pula
	pulb
	addb <ARTH+3
	adca <ARTH+2
9:
	dex		! quotient bit will be added to ARTH+3 at next loop.
	bne 2b
	rts
!
Rmi4:
	tsx
	jsr Store
	ldaa <ARTH
	bpl 1f
	ldx #ARTH
	jsr neg4
1:
	ldaa <ARTH+4
	staa <SIGN
	bpl 2f
	ldx #ARTH+4
	jsr neg4
2:
	jsr Div32x32
	tst SIGN
	bpl 8f
	com ARTH
	com ARTH+1
	coma
	negb
	bne 8f
	inca
	bne 8f
	inc ARTH+1
	bne 8f
	inc ARTH
8:
	tsx
	stab 9,x
	staa 8,x
	ldab ARTH+9
	ldaa ARTH+8
	stab 7,x
	staa 6,x
9:
	jmp Div4Ret
!
Rmu4:
	tsx
	jsr Store
	jsr Div32x32
	bra 8b
