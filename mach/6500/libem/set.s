.define Set
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine creates a set of n (n <= 256) bytes.
! In this set a certain bit, which number is in registerpair AX,
! is set. The rest is zero.


Set:
	stx ARTH	! save bitnumber (lowbyte)
	sta ARTH+1	! save bitnumber (highbyte)
	jsr Zer		! create n zerobytes
	lda ARTH
	and #07h	! n mod 8 (bitnumber in byte)
	tax
	lda #1
	cpx #0
	beq 2f
    1:	asl a		! set bit (n mod 8)
	dex
	bne 1b
    2:	sta ARTH+2
	ldx #3
    1:	lsr ARTH+1	! shiftright n 3 times (= n div 8)
	ror ARTH	! this is the bytenumber
	dex
	bne 1b
	ldy ARTH	! load bytenumber
	lda SP+1
	ldx SP+2
	stx ADDR	! address of set (lowbyte)
	sta ADDR+1	! address of set (highbyte)
	lda ARTH+2	! get bit
	sta (ADDR),y	! store byte with bit on
	rts


