.define Lxa1
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine loads the address of AB zero static levels back.

Lxa1:
	ldy LB+1	! load address of localbase (highbyte)
	ldx LB		! load address of localbase (lowbyte)
	inx
	inx		! argumentbase = localbase + 2
	bne 1f
	iny
    1:	tya
	rts


