.define Loi1

! This routine loads a one byte object in registerpair AX.


Loi1:
	stx ADDR	! address of byte (lowbyte)
	sta ADDR+1	! address of byte (highbyte)
	ldy #0
	lda (ADDR),y	! load byte
	tax		! store byte in X
	tya		! clear highbyte of AX
	rts


