.define Ior
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs the logical inclusive or on two
! groups of bytes. The groups may consist of atmost 254 bytes.
! The two groups are on the stack.

Ior:
	lda SP+1
	sta ADDR+1	! address of the first group (highbyte)
	lda SP+2
	sta ADDR	! address of the first group (lowbyte)
	clc
	tya
	adc SP+2
	sta SP+2	! new stackpointer (lowbyte)
	sta ADDR+2	! address of second group (lowbyte)
	lda #0
	adc SP+1
	sta SP+1	! new stackpointer (highbyte)
	sta ADDR+3	! address of second group (highbyte)
    1:  dey
	lda (ADDR),y	! get byte first group
	ora (ADDR+2),y	! inclusive or with byte second group
	sta (ADDR+2),y	! restore result on stack
	tya
	bne 1b		! perform n times
	rts


