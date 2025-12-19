.define Gto
.sect .zero
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

!
! IX:	address of descriptor
!
! IX+0:	new PC
! IX+2:	new SP
! IX+4:	new LB

Gto:
	stx <ADDR
	ldx 4,x
	stx <LB
	ldab <LB+1
	ldaa <LB
	subb #BASE
	sbca #0
	stab <LBl+1
	staa <LBl
	ldx <ADDR
	ldx 2,x
	txs
	ldx  <ADDR
	ldx 0,x
	jmp 0,x
