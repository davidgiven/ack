.define And

! This subroutine performs the logical and on two groups of
! atmost 254 bytes. The number of bytes is in register Y.
! The two groups are on the stack.
! First the value of the stackpointer is saved in zeropage
! locations ADDR, ADDR+1. Then an offset of Y is added
! and stored in ADDR+2, ADDR+3.
! The result is pushed back on the stack.


And:
	lda SP+1
	sta ADDR+1	! address of first group (lowbyte)
	lda SP+2
	sta ADDR	! address of first group (highbyte)
	clc
	tya
	adc SP+2
	sta SP+2	! new stackpointer (lowbyte)
	sta ADDR+2	! stackpointer + Y (lowbyte)
	lda #0
	adc SP+1
	sta SP+1	! new stackpointer (highbyte)
	sta ADDR+3	! stackpointer + Y (highbyte)
    1:  dey
	lda (ADDR),y	! get byte first group
	and (ADDR+2),y	! perform logical and with second group
	sta (ADDR+2),y	! push result on real_stack
	tya
	bne 1b		! do it n times
	rts


