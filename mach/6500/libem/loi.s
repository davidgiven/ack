.define Loi, Lext
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
! This subroutine performs an indirect load on a word of two bytes.
! Lext is used when the address is already in zeropage.


Loi: 
	stx ADDR	! address of object (lowbyte)
	sta ADDR+1	! address of object (highbyte)
Lext:
	ldy #0
	lda (ADDR),y	! get lowbyte
	tax		
	iny
	lda (ADDR),y	! get highbyte
	rts


