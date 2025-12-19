.define Pro
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end

.sect .text

! This routine is called at the entry of a procedure.
! It saves the localbase of the invoking procedure, and sets the
! new localbase to the present value of the stackpointer.
! It then initializes the second localbase by subtracting
! BASE from the real one.


Pro:
	tsx
	ldx 0,x
	stx TMP
	ins
	ins
	ldab <LB+1	! get localbase (lowbyte)
	ldaa <LB	! get localbase (highbyte)
	pshb
	psha		! push localbase onto the stack
	tsx		! get stackpointer
	stx <LB		! new localbase
	ldab <LB+1
	ldaa <LB
	subb #BASE
	sbca #0
	stab <LBl+1	! second localbase (lowbyte)
	staa <LBl	! second localbase (highbyte)
	ldx TMP
    	jmp 0,x
