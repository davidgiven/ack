.define Stl
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs the storage of a local which offset
! is to big.


Stl:
	jsr Locaddr	! get the local address
	jsr Pop		! get the word
	ldy #1
	sta (ADDR),y	! store highbyte
	dey
	txa
	sta (ADDR),y	! store lowbyte
	rts


