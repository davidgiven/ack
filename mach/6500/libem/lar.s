.define Lar

! This subroutine performs the LAR instruction.
! For details see rapport IR-81.


Lar:
	jsr Aar		! get object address
	ldy NBYTES+1	! the size of the object (highbyte)
	bne 2f
	ldy NBYTES	! the size of the object (lowbyte)
	cpy #1
	bne 1f		! object size is one byte
	jsr Loi1	! get object
	jmp Push	! push on the stack
    1:	cpy #2
	bne 1f		! object size is a word
	jsr Loi		! get word
	jmp Push	! push on the stack
    1:	cpy #4
	bne 2f		! object size is four bytes
	jmp Ldi		! get object
    2:	jmp Loil	! get object


