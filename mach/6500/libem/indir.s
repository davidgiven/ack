.define Indir
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs an indirect procedurecall.
! This must be done this way since the jump instruction
! is the only one which can indirect change the programcounter.
! The address of the function must be in zeropage loactions
! ADDR, ADDR+1.


Indir:
	jmp (ADDR)


