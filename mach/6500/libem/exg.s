.define Exg

! This subroutine exchanges two groups of bytes on the top of the
! stack. The groups may consist of atmost 255 bytes.
! This number is in register Y.
! The exchange is from ADDR, ADDR+1 to ADDR+2, ADDR+3


Exg:
	lda SP+1
	ldx SP+2
	stx ADDR	! address of first group (lowbyte)
	sta ADDR+1	! address of first group (highbyte)
	sty Ytmp	! save number of bytes to be exchanged
	clc
	lda SP+2
	adc Ytmp
	sta ADDR+2	! address of second group (lowbyte)
	lda SP+1
	adc #0
	sta ADDR+3	! address of second group (highbyte)
    1:	dey
    	lda (ADDR),y	! get byte from first group
	pha		! temporary save
	lda (ADDR+2),y	! get byte from second group
	sta (ADDR),y	! store in first group
	pla		! get temporary saved byte
	sta (ADDR+2),y	! store in second group
	tya
	bne 1b		! perform n times
	rts


