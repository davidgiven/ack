.define Mwrite
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs the monitor call write.
! Writing is always done to standardoutput.
! A zero is returned on exit.
! The subroutine WRCH is a special routine of the BBC
! microcomputer.


Mwrite:
	jsr Pop		! get fildes
	jsr Pop		! get address of characterbuffer
	stx ADDR	! bufferaddress (lowbyte)
	sta ADDR+1	! bufferaddress (highbyte)
	jsr Pop		! number of characters to be writen.
	ldy #0
    1:	lda (ADDR),y
	cmp #10
	bne 2f
	pha
	lda #13
	jsr WRCH
	pla
    2:	jsr WRCH
	iny
	dex
	bne 1b
	tya
	tax
	lda #0
	jsr Push
	tax
	rts

