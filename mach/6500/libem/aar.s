.define Aar

! This subroutine gets the address of the array element


Aar:
	stx ADDR	! address of descriptor (lowbyte)
	sta ADDR+1	! address of descriptor (highbyte)
	ldy #0
	lda (ADDR),y	! lowerbound (lowbyte)
	tax
	iny
	lda (ADDR),y	! lowerbound (highbyte)
	jsr Sbi2	! index - lowerbound
	jsr Push
    2:	ldy #4
	lda (ADDR),y	! objectsize (lowbyte)
	sta NBYTES
	tax
	iny
	lda (ADDR),y	! objectsize (highbyte)
	sta NBYTES+1
	bne 5f
	cpx #1		! objectsize = 1 then return 
	bne 5f		! arrayaddress + index
	jsr Pop
	jmp Adi2
    5:	jsr Mli2	! objectsize > 1 then return 
	jmp Adi2	! arrayaddress + index * objectsize


