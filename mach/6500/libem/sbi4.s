.define Sbi4

! This subroutine subtracts two fourbyte signed integers.


Sbi4:
	jsr Addsub	! initiate addresses
	sec
    1:	lda (ADDR+2),y	! get lowbyte+y first operand
	sbc (ADDR),y	! subtract lowbyte+y second operand
	sta (ADDR+2),y	! put on stack lowbyte+y result
	iny
	inx
	bne 1b
	rts


