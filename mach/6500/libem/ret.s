.define Ret

! This subroutine stores the returnvalue in the return area.
! This area is in zeropage.
! The size of the object to be returned is in zeropage location
! RETSIZE.
! It also restores the localbases and the stackpointer of the
! invoking procedure.


Ret:
	sty RETSIZE	! save returnsize
	beq 1f		! the return size is zero
	lda #0		! address of returnvalue area (highbyte)
	ldx #RETURN	! address of returnvalue area (lowbyte)
	cpy #2
	bne 2f
	jsr Sti		! store word
	jmp 1f
    2:	cpy #4
	jsr Sdi		! store fourbyte word
    1:	ldx LB		! get old stackpointer (lowbyte)
	stx SP+2
	lda LB+1	! get old stackpointer (highbyte)
	sta SP+1
	inc LB
	inc LB
	bne 1f
	inc LB+1
    1:	jsr Pop		! get old localbase
	stx LB		! localbase (lowbyte)
	sta LB+1	! localbase (highbyte)
	pha
	sec
	txa
	sbc #BASE
	sta LBl		! second localbase (lowbyte)
	pla
	sbc #0
	sta LBl+1	! second localbase (highbyte)
	rts


