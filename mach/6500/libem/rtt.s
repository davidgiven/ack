.define Rtt
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs the return from trap.


Rtt:
	ldy #0
	jsr Ret		! restore old stackpointer and localbase
	jsr Pop		! remove trapnumber
	jsr Pop
	sta hol0+1
	stx hol0	! restore linenumber
	jsr Pop
	sta hol0+5
	stx hol0+4	! restore filename pointer
	lda #0
	ldx #RETURN
	jsr Sdi		! restore return area
	rts


