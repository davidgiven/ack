.define Sar
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! AccAB holds the descriptor. Stack operands are index, base, value.

Sar:
	tsx
	ldx 0,x
	stx <TMP2	! save the return address
	ins
	ins
	stab <ADDR+1
	staa <ADDR
	ldx <ADDR
	ldx 4,x
	stx <NBYTES
	jsr Aar
	stab <ADDR+1
	staa <ADDR
	ldx <NBYTES
	dex
	beq 2f

	addb <NBYTES+1
	adca <NBYTES
	stab <ADDR+3
	staa <ADDR+2
	ldx <ADDR
1:
	pulb
	stab 0,x
	inx
	cpx <ADDR+2
	bne 1b
3:
	ldx <TMP2
	jmp 0,x
2:
	ins
	pulb
	ldx <ADDR
	stab 0,x
	bra 3b
