.define Pro
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This routine is called at the entry of a procedure.
! It saves the localbase of the invoking procedure, and sets the
! new localbase to the present value of the stackpointer.
! It then initializes the second localbase by subtracting
! BASE from the real one.


Pro:
	ldx LB		! get localbase (lowbyte)
	lda LB+1	! get localbase (highbyte)
	jsr Push	! push localbase onto the stack
	ldx SP+2	! get stackpointer (lowbyte)
	lda SP+1	! get stackpointer (highbyte)
	stx LB		! new localbase (lowbyte)
	sta LB+1	! new localbse (highbyte)
	tay
	txa
	sec
	sbc #BASE
	sta LBl		! second localbase (lowbyte)
	tya
	sbc #0
	sta LBl+1	! second localbase (highbyte)
    	rts


