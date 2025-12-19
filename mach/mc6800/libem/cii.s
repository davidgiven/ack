.define Cii
.sect .zero
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


! AccAB: convert to
! Stack:
! +0,+1: return address
! +2,+3: convert from
! +4,+5: convert value low word
! +6,+7: (high word when dword)

Cii:
	tsx
	ldx 0,x		! get return address
	ins
	ins
	cmpb #1
	beq Cii_1	! a conversion from ? to 1
	cmpb #2
	beq Cii_2	! a conversion from ? to 2
	pula		! a conversion from ? to 4
	pulb
!
Cii_4:	cmpb #4
	bne 1f		! a conversion 4 to 4 (skip)
	pula		! top half word to R16
	pulb
	jmp 0,x
!
1:	cmpb #1
	bne 2f
	pula		! a conversion 1 to 4
	pulb
	pshb
	clra		! sign extend
	aslb
	sbca #0
	psha		! push upper byte
	tab		! top half word is equal to sign
	jmp 0,x
2:	pulb		! a conversion 2 to 4
	pshb
	clra
	rolb
	sbca #0		! sign extend
	tab
	jmp 0,x
!
Cii_1:			! a conversion from ? to 1
	pula
	pulb
	cmpb #1
	beq 5f		! a conversion from 1 to 1 (skip)
3:	cmpb #2
	beq 5f 		! a conversion from 2 to 1
4:	ins		! a conversion from 4 to 1	
	ins
5:	pula	
	pulb
	clra
	jmp 0,x
!
Cii_2:			! a conversion from ? to 2
	pula
	pulb
	cmpb #1		! a conversion from 1 to 2 ?
	bne 2f
	pula
	pulb
	clra
	asrb
	rolb
	sbca #0
	jmp 0,x
!
2:	cmpb #2
	beq 3f		! a conversion from 2 to 2 (skip)
	ins		! a conversion from 4 to 2
	ins
	pula
	pulb
	jmp 0,x
3:	pula
	pulb
8:	jmp 0,x
