.define Lxa2

! This subroutine load the address of AB n (255 >= n > 0) static levels
! back.


Lxa2:
	lda LB
	sta ADDR	! address of localbase (lowbyte)
	lda LB+1
	sta ADDR+1	! address of localbase (highbyte)
    1:	ldy #2
	lda (ADDR),y	! static level LB (lowbyte)
	pha
	iny
	lda (ADDR),y	! static level LB (highbyte)
	sta ADDR+1	! static level LB (highbyte)
	pla
	sta ADDR	! static level LB (lowbyte)
	dex
	bne 1b
	tax
	ldy ADDR+1
	inx
	inx		! argumentbase = localbase + 2
	bne 1f
	iny
    1:	tya
	rts



