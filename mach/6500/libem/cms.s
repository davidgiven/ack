.define Cms
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine compares two groups of bytes, bit for bit.
! The groups can consist of 2 or 4 bytes. This number is in
! register Y.
! The address of the first group is stored in zeropage locations
! ADDR and ADDR+1, the address of the second group in ADDR+2 and ADDR+3
! The routine returns a 0 on equality, a 1 otherwise.



Cms:
	lda SP+2
	ldx SP+1
	sta ADDR	! address of first group (lowbyte)
	stx ADDR+1	! address of second group (highbyte)
	clc
	tya
	adc SP+2
	sta SP+2
	sta ADDR+2	! address of second group (lowbyte)
	txa
	adc #0
	sta ADDR+3	! address of second group (highbyte)
	tax
	clc
	tya
	adc SP+2
	sta SP+2	! new stackpointer (lowbyte)
	txa
	adc #0
	sta SP+1	! new stackpointer (highbyte)
    1:	dey
	lda (ADDR),y	! get byte first group
	cmp (ADDR+2),y	! compare bit for bit with byte second group
	bne 2f
	tya
	bne 1b
	lda #0		! both groups are equal
	tax
	rts
    2:	lda #0		! there is a difference between the groups
	ldx #1
	rts


