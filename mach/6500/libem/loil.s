.define Loil
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine pushes an object of size greater than four bytes
! onto the stack.


Loil:
	sta ADDR+1	! source address (lowbyte)
	stx ADDR	! source address (highbyte)
	sty NBYTES
	sec
	lda SP+2
	sbc NBYTES
	sta ADDR+2	! destination address (lowbyte)
	sta SP+2	! new stackpointer
	lda SP+1
	sbc NBYTES+1
	sta ADDR+3	! destination address (highbyte)
	sta SP+1	! new stackpointer
	inc NBYTES+1
	jmp Blmnp	! do the move


