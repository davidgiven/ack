.define Com

! This subroutine performs a one complement on
! a group of atmost 254 bytes (number in register Y).
! This group is on the top of the stack.


Com:
	lda SP+1
	sta ADDR+1	! address (highbyte) of first byte
	lda SP+2
	sta ADDR	! address (lowbyte) of first byte
    1:	dey
    	lda (ADDR),y
	eor #0FFh	! one complement
	sta (ADDR),y
	tya
	bne 1b		! do it n times
	rts


