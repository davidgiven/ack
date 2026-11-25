.define Rck
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! AccAB: range descriptor address
! Stack: value to check (kept on the stack)

Rck:
	stab <ADDR+1
	staa <ADDR
	tsx
	ldaa 2,x
	ldab 3,x
	ldx <ADDR
	cmpa 0,x
	blt 3f
	bgt 1f
	cmpb 1,x
	bcs 3f
1:
	cmpa 2,x
	blt 2f
	bgt 3f
	cmpb 3,x
	bhi 3f
2:
	rts
3:
	ldab #Erange
	jmp Trap
