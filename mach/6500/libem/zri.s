.define Zrl, Zro

! The subroutine Zrl makes a local zero which offset is to big.
! The offset of the local is in registerpair AX.
! The subroutine Zro is used if the address is already in zeropage.


Zrl:
	jsr Locaddr	! get address of local
Zro:
	lda #0
	tay
	sta (ADDR),y	! lowbyte = 0
	iny
	sta (ADDR),y	! highbyte = 0
	rts


