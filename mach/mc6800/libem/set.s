.define Set
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Create a singleton EM set on the stack.
! AccAB: element number, X: size in bytes.

Set:
	stx <ARTH
	beq 8f
	staa <ARTH+2
	ldaa <ARTH+1
	bita #1
	bne 8f
	ldaa <ARTH+2
	bmi 9f
	stab <ARTH+3
	lsra
	rorb
	lsra
	rorb
	lsra
	rorb
	eorb #1		! select the byte within the EM word
	staa <ADDR
	stab <ADDR+1
	subb <ARTH+1
	sbca <ARTH
	bcc 9f

	tsx
	ldx 0,x
	stx <TMP
	ins
	ins
	ldx <ARTH
	clra
1:
	psha
	dex
	bne 1b

	tsx
	stx <ARTH
	ldab <ADDR+1
	ldaa <ADDR
	addb <ARTH+1
	adca <ARTH
	stab <ADDR+1
	staa <ADDR
	ldx <ADDR
	ldab <ARTH+3
	andb #7
	ldaa #1
	tstb
	beq 3f
2:
	asla
	decb
	bne 2b
3:
	staa 0,x
	ldx <TMP
	jmp 0,x

8:
	ldab #Eoddz
	jmp Trap
9:
	ldab #Eset
	jmp Trap
