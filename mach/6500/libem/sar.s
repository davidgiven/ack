.define Sar

! This subroutine performs the SAR instruction.
! For details see rapport IR-81.


Sar:
	jsr Aar		! get object address
	ldy NBYTES+1	! the size of the object (highbyte)
	bne 2f
	ldy NBYTES	! the size of the object (lowbyte)
	cpy #1
	bne 1f		! object size is one byte
	jmp Sti1	! put it in array
    1:	cpy #2
	bne 1f		! object size is two bytes
	jmp Sti		! put it in array
    1:	cpy #4
	bne 2f		! object size is fourbytes
	jmp Sdi		! put it in array
    2:	jmp Stil	! put it in array


