.define Xor

! This subroutine performs the exclusive or on two groups of bytes.
! The groups consists of atmost 254 bytes.
! The result is on top of the stack.


Xor:
	lda SP+1
	sta ADDR+1	! address of first group (lowbyte)
	lda SP+2
	sta ADDR	! address of first group (highbyte)
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
	eor (ADDR+2),y	! exclusive or with byte second group
	sta (ADDR+2),y	! restore result
	tya
	bne 1b
	rts


