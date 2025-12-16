.define Csb,CsbX
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs the case jump by searching the table.
! The zeropage locations ADDR, ADDR+1 contain the address of the
! case descriptor, which also is the address of the default pointer.

!
! case_table:	Csb:AccAB, CsbX:IX
! default_addr: .data2 adrs
! num_of_case:	.data2 num
! case_1:	.data2 num
! jump_addr:	.data2 adrs
! case_2:	.data2 num
! jump_addr:	.data2 adrs
!

! switch value: TOS:Csb, AccAB:CsbX
! 

CsbX:	pshb
	psha
	stx <ADDR
	bra 0f
Csb:
	stab <ADDR+1	! address of descriptor (lowbyte)
	staa <ADDR	! address of descriptor (highbyte)
	ldx <ADDR
0:	inx
	inx
	ldab 1,x	! number of entries (lowbyte)
	ldaa 0,x
	stab <TMP+1
	beq 1f
	inca
1:	staa <TMP
	inx
	inx	
!
	pula		! get switch value
	pulb
2:
	cmpb 1,x
	bne 3f		! next case
	cmpa 0,x
	bne 3f		! next case
! match
	ldx 2,x
	jmp 0,x
! next
3:
	inx
	inx
	inx
	inx
	dec TMP+1
	bne 2b
! default
	ldx <ADDR
	ldx 0,x
	jmp 0,x
