.define Dup
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine duplicate's the top 4 bytes.

! IX:		n: number of bytes to copy
! Stack:
! +0		return address
! +2..+5	copy data

Dup:
	tsx
	ldx	0,x
	stx	TMP
	ins
	ins
	ldab	5,x
	ldaa	4,x
	pshb
	psha
	ldab	3,x
	ldaa	2,x
	pshb
	psha
	jmp	0,x
