.define Exg2

! This subroutine exchanges two words on top of the stack.
! The top word of the stack is really in the AX registerpair.
! So this word is exchanged with the top of the real stack.


Exg2:
	pha		! save A
	txa
	pha		! save X
	jsr Pop		! get top real stack
	stx EXG
	sta EXG+1	! save top of real stack
	pla		! get X
	tax
	pla		! get A
	jsr Push	! push on real stack
	ldx EXG		! get new X
	lda EXG+1	! get new A
	rts


