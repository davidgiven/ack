.define Printhex

! This subroutine print the contents of register A to the screen
! in hexadecimal form.
! The subroutine WRCH is a special one provided by the BBC
! microcomputer.


Printhex:
	pha		! save A
	lsr a
	lsr a
	lsr a
	lsr a		! get four high bits
	jsr 1f
	pla		! restore A
	and #0Fh	! get four low bits
	jsr 1f
	rts
    1:	sed		! print in hex
	clc
	adc #90h
	adc #40h
	cld
	jmp WRCH


