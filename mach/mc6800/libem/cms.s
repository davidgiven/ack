.define Cms
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine compares two groups of bytes, bit for bit.
! The groups can consist of 2 or 4 bytes. This number is in
! register AccB.
! The address of the first group is stored in zeropage locations
! ADDR and ADDR+1, the address of the second group in ADDR+2 and ADDR+3
! The routine returns a 0 on equality, a 1 otherwise.

! AccAB:
!		size of group (byte)
! Stack:
! +0		return address
! +2,  2+n-1
! +2+n,2+n*2-1


Cms:
	tsx
	ldx 0,x		! get return address
	ins
	ins
	stx <TMP
	tsx
	tsta
	bne Cms_N
	cmpb #2
	beq Cms_2	! 2byte compare
	cmpb #4
	beq Cms_4	! 4byte compara
!
Cms_N:
	stab <ARTH+1	! size of gtoup (byte)
	staa <ARTH
	stx <ADDR
	addb <ADDR+1
	adca <ADDR
	stab <ADDR+1	! save top + size
	staa <ADDR
	addb <ARTH+1
	adca <ARTH
	stab <ADDR+3	! save top + size*2
	staa <ADDR+2
!
	ldx <ADDR
!
1:	pulb
	cmpb 0,x
	bne 9f
	inx
	cpx <ADDR+2
	bne 1b
!
	clrb
8:	clra
	txs		! X==ADDR+2 (top + size*2)
	ldx TMP
	jmp 0,x
9:
	ldab #1
	ldx <ADDR+2
	bra 8b
!
Cms_2:
	ldx 0,x		! get first group
	stx <ARTH
	tsx
	ldx 2,x		! compare second group
	ins
	ins
	ins
	ins
	cpx <ARTH
!
	beq 1f
	ldab #1
	bra 2f
1:	clrb
2:	clra
	ldx <TMP
	jmp 0,x
!
Cms_4:
	clrb
	ldx 0,x		! get first group
	stx <ARTH
	tsx
	ldx 2,x
	stx <ARTH+2
	tsx
	ldx 4,x		! get second group
	cpx <ARTH
	bne 2f
	tsx
	ldx 6,x
	cpx <ARTH+2
	bne 2f
1:	clrb
	bra 3f
2:	ldab #1
3:	clra
	ins
	ins
	ins
	ins
	ins
	ins
	ins
	ins
	ldx <TMP
	jmp 0,x
