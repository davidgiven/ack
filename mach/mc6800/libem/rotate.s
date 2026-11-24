.define Rol,Ror
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! AccAB: bit count. X: byte count. The value stays on the stack.

Rol:
	bsr Setup
	beq 3f
1:
	ldx <ADDR
	ldaa 0,x
	asla
	ldx <ADDR+2
2:
	dex
	rol 0,x
	cpx <ADDR
	bne 2b
	ldx <ARTH
	dex
	stx <ARTH
	bne 1b
3:
	rts

Ror:
	bsr Setup
	beq 3f
1:
	ldx <ADDR+2
	dex
	ldaa 0,x
	lsra
	ldx <ADDR
2:
	ror 0,x
	inx
	cpx <ADDR+2
	bne 2b
	ldx <ARTH
	dex
	stx <ARTH
	bne 1b
3:
	rts

Setup:
	stab <ARTH+1
	staa <ARTH
	stx <ADDR+2
	beq 1f
	tsx
	inx
	inx
	inx
	inx
	stx <ADDR
	ldab <ADDR+3
	ldaa <ADDR+2
	addb <ADDR+1
	adca <ADDR
	stab <ADDR+3
	staa <ADDR+2
	ldx <ARTH
1:
	rts
