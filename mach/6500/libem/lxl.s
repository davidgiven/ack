.define Lxl

! This subroutine loads LB n (255 => n > 0) static levels back.


Lxl:
	lda LB
	sta ADDR	! address of localbase (lowbyte)
	lda LB+1
	sta ADDR+1	! address of localbase (highbyte)
    1:	ldy #2
	lda (ADDR),y	! get localbase (lowbyte) 1 level back
	pha
	iny
	lda (ADDR),y	! get localbase (highbyte) 1 level back
	sta ADDR+1	! new localbase (highbyte)
	pla
	sta ADDR	! new localbase (lowbyte)
	dex
	bne 1b		! n levels
	tax
	lda ADDR+1
	rts


