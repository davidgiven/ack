.define Sti, Sext, Stii
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! The subroutine Sti stores an twobyte word at the address which
! is in registerpair AX.
! The subroutine Sext is used when the address is already in 
! zeropage.
! The subroutine Stii is used when the address is in zeropage
! and the registerpair AX contains the word.


Sti:
	stx ADDR	! address of word (lowbyte)
	sta ADDR+1	! address of word (highbyte)
Sext:
	jsr Pop		! get word
Stii:
	ldy #1
	sta (ADDR),y	! store highbyte
	dey
	txa
	sta (ADDR),y	! store lowbyte
	rts


