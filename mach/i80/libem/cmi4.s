.define .cmi4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Compare 32 bits integers
! Expects:	operands on stack
!		a-register = 1 for signed integers
!		a-register = 0 for unsigned integers
! Yields in de-registers: -1 if second operand < first operand
!			   0 if second operand = first operand
!			   1 if second operand > first operand

.cmi4:	pop h
	shld .retadr

	lxi b,4
	lxi h,0
	dad sp
	dad b
	dcx h
	mov d,h
	mov e,l		!now de points to the first operand
	dad b		!and hl to the second
	ora a		!is it a cmi or cmu?
	jz 1f

!for cmi only:
	mov a,m
	ral
	jnc 2f
	ldax d		!second operand is negative
	ral
	jc 1f		!jump if both operands are negative
	lxi d,-1	!second operand is smaller
	jmp 4f
2:	ldax d		!second operand is positive
	ral
	jnc 1f		!jump if both operand are positive
	lxi d,1		!second operand is larger
	jmp 4f

!cmi and cmu rejoin here
1:	ldax d
	cmp m
	jz 3f
	jnc 2f
	lxi d,1		!second operand is larger
	jmp 4f
2:	lxi d,-1	!second operand is smaller
	jmp 4f
3:	dcx d
	dcx h
	dcr c
	jnz 1b
	lxi d,0		!operands are equal

4:	lxi h,8
	dad sp
	sphl

	lhld .retadr
	pchl
