.define Csb

! This subroutine performs the case jump by searching the table.
! The zeropage locations ADDR, ADDR+1 contain the address of the
! case descriptor, which also is the address of the default pointer.
! The zeropage locations ADDR+2, ADDR+3 are used to address the jump 
! pointers.


Csb:
	stx ADDR	! address of descriptor (lowbyte)
	sta ADDR+1	! address of descriptor (highbyte)
	stx ADDR+2
	sta ADDR+3
	ldy #2
	lda (ADDR),y	! number of entries (lowbyte)
	pha
	jsr Pop
	stx ARTH	! index (lowbyte)
	sta ARTH+1	! index (highbyte)
	pla
	tax
	inx
    2:	clc
	lda #4
	adc ADDR+2
	sta ADDR+2	! pointer (lowbyte)
	bcc 1f
	lda #0
	adc ADDR+3
	sta ADDR+3	! pointer (highbyte)
    1:	ldy #0
	lda (ADDR+2),y
	cmp ARTH
	bne 3f		! pointer (lowbyte) <> index (lowbyte)
	iny
	lda (ADDR+2),y
	cmp ARTH+1
	bne 3f		! pointer (highbyte) <> index (highbyte)
	iny
	lda (ADDR+2),y	! jump address (lowbyte)
	tax
	iny
	lda (ADDR+2),y	! jump address (highbyte)
	jmp 4f
    3:	dex
	bne 2b
    5:	ldy #0
	lda (ADDR),y	! default pointer (lowbyte)
	tax
	iny
	lda (ADDR),y	! default pointer (highbyte)
	beq 1f
    4:	bne 1f		! pointer (lowbyte) <> 0
	cpx #0
	bne 1f		! pointer (highbyte) <> 0
	beq 5b		! get default pointer
    1:	stx ADDR
	sta ADDR+1
	jmp (ADDR)	! jump


