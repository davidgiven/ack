.define Csa

! This subroutine performs the case jump by indexing.
! The zeropage locations ADDR, ADDR+1 contain the address of
! the case descriptor which also is the address of the
! default pointer.
! The zeropage locations ADDR+2, ADDR+3 contain the address of the
! indextable which is the casedescriptor + 6.

Csa:
	stx ADDR	! address of descriptor (lowbyte)
	sta ADDR+1	! address of descriptor (highbyte)
	tay
	txa
	clc
	adc #6
	sta ADDR+2	! address of index table (lowbyte)
	tya
	adc #0
	sta ADDR+3	! address of index table (highbyte)
	jsr Pop		! fetch index
	pha		! subtract lowerbound
	txa
	ldy #2
	sec
	sbc (ADDR),y
	sta ARTH	! lowerbound (lowbyte)
	pla
	iny
	sbc (ADDR),y
	sta ARTH+1	! lowerbound (highbyte)
	bmi 1f		! index < lowerbound
	ldy #5
	lda (ADDR),y
	cmp ARTH+1
	bcc 1f		! index (highbyte) > upperbound - lowerbound
	bne 2f		! index (highbyte) <= upperbound - lowerbound
	dey
	lda (ADDR),y
	cmp ARTH
	bcc 1f		! index (lowbyte) > upperbound - lowerbound
    2:	asl ARTH
	rol ARTH+1	! index * 2
	clc
	lda ADDR+2
	adc ARTH
	sta ADDR+2	! address of pointer (lowbyte)
	lda ADDR+3
	adc ARTH+1
	sta ADDR+3	! address of pointer (highbyte)
	ldy #0
	lda (ADDR+2),y	! jump address (lowbyte)
	tax
	iny
	lda (ADDR+2),y	! jump address (highbyte)
	bne 3f
	cpx #0
	beq 1f
    3:	stx ADDR	! pointer <> 0
	sta ADDR+1
	jmp (ADDR)	! jump to address
    1:	ldy #0		! pointer = 0
	lda (ADDR),y	! get default pointer (lowbyte)
	tax
	iny
	lda (ADDR),y	! get default pointer (highbyte)
	bne 3b
	cpx #0
	bne 3b		! default pointer <> 0


