.define Mprint
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine prints a zero terminated ascii string.
! The registerpair AX contains the start of the string.
! The subroutine WRCH is a special routine on the BBC microcomputer
! which prints the character in A to the screen.
! The subroutine WRCH is a special one provided by the BBC
! microcomputer.


Mprint:
	stx ADDR	! start address of string (lowbyte)
	sta ADDR+1	! start address of string (highbyte)
	ldy #0
    1:	lda (ADDR),y	! get ascii character
	beq 2f
	jsr WRCH	! put it on the screen
	iny
	bne 1b
    2:  rts


