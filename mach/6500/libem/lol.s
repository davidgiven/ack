.define Lol
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine loads a local in registerpair AX which
! offset from the localbase is to big.


Lol:
	jsr Locaddr	! get the address of local
	ldy #0
	lda (ADDR),y	! get lowbyte
	tax
	iny
	lda (ADDR),y	! get highbyte
	rts


