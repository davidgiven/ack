.define Dup,DupX,DupAB
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine duplicate's the top n bytes.

! IX:		n: number of bytes to copy
! Stack:
! +0		return address
! +2..+2+n-1	copy data

Dup:
	tsx
	ldx	0,x
	stx	<TMP
	ins
	ins
	pula
	pulb
	bra	0f
DupX:
	stx	<NBYTES
	ldab	<NBYTES+1
	ldaa	<NBYTES
DupAB:
	tsx
	ldx	0,x
	stx	<TMP
	ins
	ins
0:
	tsx
	stx	<ADDR
	addb	<ADDR+1
	adca	<ADDR
	stab	<ADDR+3		! copy end address+1
	staa	<ADDR+2
	ldx	<ADDR+2
1:	dex
	ldab	0,x
	pshb
	cpx	<ADDR
	bne	1b
	ldx	<TMP
	jmp	0,x
