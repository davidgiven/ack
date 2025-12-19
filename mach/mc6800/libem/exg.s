.define Exg
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine exchanges two groups of bytes on the top of the
! stack. The groups may consist of atmost 255 bytes.
! This number is in register Y.
! The exchange is from ADDR, ADDR+1 to ADDR+2, ADDR+3

! AccAB: n (bytes)
! Stack:
! +0,+1:     return address
! +2,+n+1:   first group
! +n+2,+2n+1:second group

Exg:
	tsx
	ldx 0,x
	stx <TMP
	ins
	ins
	tsx
	stx <ADDR
!
	stab <ARTH+1
	staa <ARTH
	addb <ADDR+1	! calculate second group's address
	adca <ADDR
	stab <ADDR+3
	staa <ADDR+2
!
	ldab <ARTH+1	! adjust size for loop
	beq 1f
	inc ARTH
1:
!	ldx <ADDR
	dex
2:
	inx
	ldab 0,x
	stx <ADDR
	ldx <ADDR+2
	ldaa 0,x
	stab 0,x
	inx
	stx <ADDR+2
	ldx <ADDR
	staa 0,x
	dec ARTH+1
	bne 2b
	dec ARTH
	bne 2b
!
	ldx <TMP
	jmp 0,x
