.define Sts

! This subroutine stores indirect a number of bytes.
! The number of bytes is in the registerpair AX.


Sts:
	cmp #0
	bne 3f		! number of bytes > 255
	cpx #1
	bne 1f		! onebyte storage
	jsr Pop		! get the address
	jmp Sti1	! store the byte
    1:	cpx #2
	bne 2f		! twobyte storage
	jsr Pop		! get the address
	jmp Sti		! store the word
    2:	cpx #4
	bne 3f		! fourbyte storage
	jsr Pop		! get the address
	jmp Sdi		! store the double word
    3:	sta ARTH+1	! objectsize > 4
	txa
	tay
	jsr Pop		! get address
	jmp Stil	! store the object


