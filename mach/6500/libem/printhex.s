.define Printhex
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

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
	and #0x0F	! get four low bits
	jsr 1f
	rts
    1:	sed		! print in hex
	clc
	adc #0x90
	adc #0x40
	cld
	jmp WRCH


