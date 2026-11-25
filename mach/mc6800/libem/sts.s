.define Sts
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! AccAB holds the byte count. Stack operands are address, value.

Sts:
	tsx
	ldx 0,x
	stx <TMP2
	tsx
	ldx 2,x
	stx <ADDR
	ins
	ins
	ins
	ins
	tsta
	bne 1f
	tstb
	beq 3f
	cmpb #1
	bne 1f
	ins
1:
	addb <ADDR+1
	adca <ADDR
	stab <ADDR+3
	staa <ADDR+2
	ldx <ADDR
2:
	pulb
	stab 0,x
	inx
	cpx <ADDR+2
	bne 2b
3:
	ldx <TMP2
	jmp 0,x
