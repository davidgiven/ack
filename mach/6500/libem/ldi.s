.define Ldi, Ldo
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! The subroutine Ldi pushes a four byte object onto the stack.
! The address is in registerpair AX.
! If the address is already in zeropage Ldo is used.


Ldi:
	stx ADDR	! address of object (lowbyte)
	sta ADDR+1	! address of object (highbyte)
Ldo:
	ldy #3
    1:	lda (ADDR),y	! get lowbyte
	pha
	dey
	lda (ADDR),y	! get highbyte
	tax
	pla
	jsr Push	! do the push
	dey
	bpl 1b		! perform 2 times
	rts


