.define Dup

! This subroutine duplicate's the top n (in register Y) bytes.
! N is atmost 256.
! The duplicating takes place as follows.
! The registerpair is filled with the bytes at stackpointer + N
! and stackpopinter + N-1.
! These two bytes then are pushed onto the stack.
! Next the offset N is decremented and the next two byte are taken
! care off. Until N = 0.


Dup:
	lda SP+1
	ldx SP+2
	stx ADDR	! address of group (lowbyte)
	sta ADDR+1	! address of group (highbyte)
    1:	dey
	lda (ADDR),y	! get lowbyte
	pha
	dey
	lda (ADDR),y	! get highbyte
	tax
	pla
	jsr Push	! push them
	tya
	bne 1b
	rts


