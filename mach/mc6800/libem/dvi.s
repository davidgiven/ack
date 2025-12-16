.define Dvi2, Dvu2, Dvi, Dvu, Div16x16
.define Rmi2, Rmu2, Rmi, Rmu
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
!	+2,+3: diviend
!
Store:
	stab <ARTH+1	! store divisor
	staa <ARTH
	ldab 3,x
	ldaa 2,x
	stab <ARTH+3	! store dividend
	staa <ARTH+2
	rts
Dvi2:
	tsx
	bsr Store
	bsr DviSub
1:
	tsx
	ldx 0,x
	ins
	ins
	ins
	ins
	jmp 0,x
!
Dvu2:
	tsx
	bsr Store
	bsr DvuSub
	bra 1b
!
Dvi:
	stab <ARTH+3	! store dividend
	staa <ARTH+2
DviSub:
	ldaa <ARTH
	staa <SIGN
	bpl 1f
	nega
	neg ARTH+1
	sbca #0
	staa <ARTH
1:
	ldaa <ARTH+2
	bpl 2f
	nega
	neg ARTH+3
	sbca #0
	staa <ARTH+2
	com SIGN
2:
	bsr Div16x16
	ldab <ARTH+3
	ldaa <ARTH+2
	tst SIGN
	bpl 3f
	nega
	negb
	sbca #0
3:
	rts
!	
Dvu:
	stab <ARTH+3	! store dividend
	staa <ARTH+2
DvuSub:
	bsr Div16x16
	ldab <ARTH+3
	ldaa <ARTH+2
	rts
!
Rmu:
	stab <ARTH+3	! store dividend
	staa <ARTH+2
RmuSub:
!
Div16x16:
	ldx #16		! loop counter
	clra
	clrb		! carry is also cleared
8:
	rol ARTH+3	! shift divient
	rol ARTH+2
	rolb
	rola
	subb <ARTH+1	! divisor
	sbca <ARTH
	bcc 9f
	addb <ARTH+1
	adca <ARTH	! C must 1
9:
	dex		! quotient bit will be added to ARTH+3 at next loop.
	bne 8b
	rol ARTH+3	! Shift to get the remaining quotient 1 bit
	rol ARTH+2
	com ARTH+3
	com ARTH+2
	rts
!
Rmi2:
	tsx
	jsr Store
	bsr RmiSub
1:
	tsx
	ldx 0,x
	ins
	ins
	ins
	ins
	jmp 0,x
!
Rmu2:
	tsx
	jsr Store
	jsr RmuSub
	bra 1b
!
Rmi:
	stab <ARTH+3	! store dividend
	staa <ARTH+2
RmiSub:
	ldaa <ARTH
	bpl 1f
	nega
	neg ARTH+1
	sbca #0
	staa <ARTH
1:
	ldaa <ARTH+2
	staa <SIGN
	bpl 2f
	nega
	neg ARTH+3
	sbca #0
	staa <ARTH+2
2:
	jsr RmuSub
	tst SIGN
	bpl 3f
	nega
	negb
	sbca #0
3:
	rts
