.define Sdi, Sdo

! The subroutine Sdi takes a fourbyte word and stores it
! at the address in registerpair AX.
! If the address is in zeropage, Sdo is used.


Sdi:
	stx ADDR	! address (lowbyte)
	sta ADDR+1	! address (highbyte)
Sdo:
	ldy #0
    1:  jsr Pop
	pha
	txa
	sta (ADDR),y	! store lowbyte
	iny
	pla
	sta (ADDR),y	! store highbyte
	iny
	cpy #4
	bne 1b
	rts


