.define Gto
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs the non_local goto.
! The address of the descriptor is stored in zeropage locations
! ADDR, ADDR+1.
! Since there are two stacks (hardware_stack and the real_stack),
! the stackpointer of the hard_stack is resetted by searching the
! new localbase in the real_stack while adjusting the hardware_stack.


Gto:
	stx ADDR	! address of descripto (lowbyte)
	sta ADDR+1	! address of descriptor (highbyte)
	pla		! remove
	pla		! __gto return address.
	ldy #4
	lda (ADDR),y	! new localbase (lowbyte)
	sta ARTH
	tax
	iny
	lda (ADDR),y	! new localbase (highbyte)
	sta ARTH+1
	cmp LB+1
	bne 1f
	cpx LB
	beq 2f		! goto within same procedure
    1:	ldy #0
	lda (LB),y	! get localbase (lowbyte)
	tax
	iny
	lda (LB),y	! get localbase (highbyte)
	cmp ARTH+1
	bne 3f
	cpx ARTH
	beq 2f		! found localbase
    3:	stx LB		! temporary save of localbase
	sta LB+1
	pla		! adjust
	pla		! hardware_stack
	jmp 1b
    2:	sta LB+1	! store localbase (highbyte)
	pha
	stx LB		! store localbase (lowbyte)
	sec
	txa
	sbc #BASE
	sta LBl		! localbase - 240 (lowbyte)
	pla
	sbc #0
	sta LBl+1	! localbase - 240 (highbyte)
	ldy #3
	lda (ADDR),y	! new stackpointer (highbyte)
	sta SP+1
	dey
	lda (ADDR),y	! new stackpointer (lowbyte)
	sta SP+2
	dey
	lda (ADDR),y	! jump address (highbyte)
	sta ADDR+3
	dey
	lda (ADDR),y	! jump address (lowbyte)
	sta ADDR+2
	jmp (ADDR+2)	! jump to address


