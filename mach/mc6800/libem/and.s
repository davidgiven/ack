.define And
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs logical and on  two fourbyte integers,
! which are on the stack.
! The result is pushed back onto the stack

! AccAB     number of bytes (n)
! +0        rerurn address
! +2  , n+1 nbyte integer
! +n+2,2n+1 nbyte integer (and result)

And:
	tsx
	ldx 0,x
	stx TMP
	ins	! skip return address
	ins
	tsx
	stab TMP+1	! save number of bytes
	beq 1f
	inca
1:	staa TMP
	stx ADDR
	addb ADDR+1	! calculate result address
	adca ADDR
	stab ADDR+1
	staa ADDR
	ldx ADDR
	ldab TMP+1
2:
	pula
	anda 0,x
	staa 0,x
	inx
	decb
	bne 2b
	dec TMP
	bne 2b
	jmp 0,x
