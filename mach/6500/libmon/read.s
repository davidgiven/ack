.define Mread
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine reads characters from the standard input.
! It ignores the filedes.
! It reads atmost 255 characters. So the runtime system must
! provide a way of dealing with this.
! The subroutine RDCH is a special one provided by the BBC
! microcomputer.


Mread:
	jsr Pop		! ignore filedescriptor
	jsr Pop		! bufptr
	stx ADDR	! address of character buffer (lowbyte)
	sta ADDR+1	! address of character buffer (highbyte)
	jsr Pop		! number of characters
	ldy #0		! <= 255
	inx
    1:	jsr RDCH	! read a character from the current inputstream
	bcs 8f
	sta (ADDR),y
	iny
	dex
	bne 1b
    8:	tya
	tax
	lda #0
	jsr Push	! number of characters red.
	tax		! report a succesfull read.
	rts


