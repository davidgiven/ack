.define Cii
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine converts integers to integers.
! Convertions of integers with the same source size as destination
! size aren't done, there just return the source.
! A convertion from 4 bytes to 2 bytes just strips the two
! most significant bytes.
! A convertion from 2 bytes to 4 bytes tests the sign of the
! source so that sign extentension takes place if neccesairy.


Cii:
	cpx #2
	beq Cii_2	! a conversion from ? to 2
	jsr Pop		! a conversion from 4 to ?
	cpx #4
	beq 8f		! a conversion 4 to 4 (skip)
	jsr Pop
	tay		! save A for sign test
	pha		! save A 
	txa
	pha		! save X
	tya		! test on negative
	bmi 1f		! negative means sign extension
	lda #0		! no sign extension here
	tax
	beq 2f
    1:	lda #0FFh	! sign extension here
	tax
    2:	jsr Push	! push twobyte integer
	pla
	tax		! get X
	pla		! get A
	jmp Push
Cii_2:			! a conversion from 2 to ?
	jsr Pop
	cpx #2
	beq 8f		! a conversion from 2 to 2 (skip)
	jsr Pop		! a conversion from 4 to 2
	pha		! save A
	txa
	pha		! save X
	jsr Pop		! strip most significant bytes
	pla		! get X
	tax
	pla		! get A
	jmp Push	! push result
    8:	rts


