.define Blm, Blmnp

! This subroutine copies bytes from one place in memory to
! another. The source address is in registerpair AX and is stored
! in zeropage locations ADDR and ADDR+1.
! The destination address is popped from the stack and stored in
! zeropage locations ADDR+2 and ADDR+3.
! The number of bytes to be copied is in register Y (lowbyte) and
! zeropage location NBYTES+1 (highbyte).
! The subroutine Blmnp is used when the source and destination
! addresses are already in zeropage.


Blm:
	stx ADDR+2	! source address (lowbyte)
	sta ADDR+3	! source address (highbyte)
	jsr Pop
	stx ADDR	! destination address (lowbyte)
	sta ADDR+1	! destination address (highbyte)
Blmnp:	ldx NBYTES+1
    1:	dey
	lda (ADDR),y	! get source byte
	sta (ADDR+2),y	! copy to destination
	tya
	bne 1b
	dec ADDR+1	! 256 bytes copied
	dec ADDR+3	! decrement source and destination address
	ldy #0
	dex
	bne 1b		! do it n times
	rts


