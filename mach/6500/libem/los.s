.define Los

! This subroutine perfoms the LOS instruction.
! For detail see rapport IR-81.


Los:
	cmp #0
	bne 3f
	cpx #1
	bne 1f		! the size is one
	jsr Pop		! get address
	jsr Loi1	! push it on the stack
	jmp Push
    1:	cpx #2
	bne 2f		! the size is two
	jsr Pop		! get address
	jsr Loi		! push it on the stack
	jmp Push
    2:	cpx #4
	bne 3f		! the size is four
	jsr Pop		! get address
	jmp Ldi		! push it on the stack
    3:	sta ARTH+1	! the size is greater than four
	txa
	tay
	jsr Pop		! get address
	jmp Loil	! push it on the stack


