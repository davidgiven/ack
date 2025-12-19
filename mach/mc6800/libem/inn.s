.define Inn
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine checks if a certain bit is set in a set
! of n bytes on top of the stack.

! AccAB: data size
! Stack:
! +0,+1: return address
! +2,+3: bit number
! +4...: data

Inn:
	tsx
	ldx 0,x
	stx <TMP
	ins
	ins
!
	stab <ARTH+1	! save data size
	staa <ARTH
!
	tsx
	inx
	inx
	stx <ADDR	! save top of data address
	addb <ADDR+1
	adca <ADDR
	stab <ADDR+3	! save end of data address
	staa <ADDR+2
!
	pula		! get bit number
	pulb
!
	stab <ARTH+3	! save bit number
	staa <ARTH+2
!
	bmi 7f		! bit number < 0
!
	lsra		! bit/8
	rorb
	lsra
	rorb
	lsra
	rorb
!
	pshb
	psha
	subb <ARTH+1	! check in range
	sbca <ARTH
	bpl	7f	! (bit number/8)>=size
!
	pula
	pulb
	addb <ADDR+1	! calcucalte data address
	adca <ADDR
	eorb #1		! byte swap
	stab <ADDR+1
	staa <ADDR
	ldx <ADDR
!
	ldaa #1		! bit pos
	ldab <ARTH+3	! get bit number
	andb #7		! bit number % 8
	beq 2f
1:
	asla
	decb
	bne 1b
2:
!			! here, AccB==0
	anda 0,x	! bit(i,s)
	beq 9f
	incb
	bra 8f
!
7:	clrb
8:	clra
9:	ldx <ADDR+2	! adjust stack
	txs
	ldx <TMP
	jmp 0,x
