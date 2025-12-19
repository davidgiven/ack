.define Aar
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine gets the address of the array element
!
! AccAB: address of descriptor
! stack:
! +0, +1: rerurn address
! +2, +3: index
! +4, +5: base of array
!
! descriptor:
! +0, +1  lower bound
! +2, +3  upper bound
! +4, +5  object size
! +6...   array data

Aar:
	tsx
	ldx 0,x
	stx TMP
	stab ADDR+1	! address of descriptor (lowbyte)
	staa ADDR	! address of descriptor (highbyte)
	tsx
	ldab 3,x	! get index
	ldaa 2,x
	ldx ADDR	! index < lower bound ?
	subb 1,x
	sbca 0,x
	blt 9f		! trap EARRAY
	pshb
	psha
	subb 3,x	! index - lower bound > object size ?
	sbca 2,x
	pula
	pulb
	bgt 9f		! trap EARRAY
!
2:	ldx 4,x		! get object size
	beq Trap
	cpx #1
	beq 6f
	cpx #2
	beq 5f
	cpx #4
	beq 4f
!			! AccAB has index - lower bound
	stx ARTH
	jsr Mlu2	! AccAB = AccAB * ARTH (object size)
	bra 6f
!
4:	aslb		! object size == 4
	rola
!
5:	aslb		! object size == 2
	rola
!
6:			! object size == 1
	tsx
	addb 5,x	! add base of array
	adca 4,x
	ldx 0,x
	ins
	ins
	ins
	ins
	ins
	ins
	jmp 0,x
9:
	ldab #EARRAY
	clra
	jsr Trap
