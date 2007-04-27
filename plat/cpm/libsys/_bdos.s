#
! $Source$
! $State$
! $Revision$

! Declare segments (the order is important).

.sect .text
.sect .rom
.sect .data
.sect .bss

.sect .text

! Calls a BDOS routine.

.define _cpm_bdos
_cpm_bdos:
	push b
	
	lda _cpm_a_register
	
	lhld _cpm_bc_register
	mov b, h
	mov c, l
	
	lhld _cpm_de_register
	mov d, h
	mov e, l
	
	lhld _cpm_hl_register
	
	call 5
	
	shld _cpm_hl_register
	
	mov h, d
	mov l, e
	shld _cpm_de_register
	
	mov h, b
	mov l, c
	shld _cpm_bc_register
	
	sta _cpm_a_register
	
	pop b
	ret

.sect .bss
.define _cpm_a_register, _cpm_bc_register, _cpm_de_register, _cpm_hl_register
.comm _cpm_a_register, 1
.comm _cpm_bc_register, 2
.comm _cpm_de_register, 2
.comm _cpm_hl_register, 2
