.define Sti1

! This subroutine stores an onebyte wordfractional at the address
! which is in registerpair AX.


Sti1:
	stx ADDR	! address of byte (lowbyte)
	sta ADDR+1	! address of byte (highbyte)
	jsr Pop		! get byte
	ldy #0
	txa
	sta (ADDR),y	! store byte
	rts


