.define Stil
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine stores indirect a block of bytes if 
! the number of bytes is greater than four.
! The destination address is in registerpair AX.
! The lowbyte of the number of bytes is in Y,
! the highbyte is in zeropage location NBYTES+1.


Stil:
	sta ADDR+3	! destination address (highbyte)
	stx ADDR+2	! destination address (lowbyte)
	sty NBYTES	! number of bytes (lowbyte)
	clc
	lda SP+2
	sta ADDR	! source address (lowbyte)
	adc NBYTES
	sta SP+2	! new stackpointer (lowbyte)
	lda SP+1
	sta ADDR+1	! source address (highbyte)
	adc NBYTES+1
	sta SP+1	! new stackpointer (highbyte)
	inc NBYTES+1
	jmp Blmnp	! do the move


