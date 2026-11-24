.define Trap,Rtt
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs the trap instruction.

! AccB: Trap number

Trap:
	cmpb #16
	bcc Dotrap
	pshb
	stab <TMP
	inc TMP
	ldab <IGNMASK+1
	ldaa <IGNMASK
1:
	lsra
	rorb
	dec TMP
	bne 1b
	pulb
	bcs 2f
	bra Dotrap
2:
	rts

Dotrap:
	stab <TRAPVAL
	ldx <ERRPROC
	bne 1f
	jmp Mtrap
1:	ldx #RETURN	! save return area
	ldab 3,x
	ldaa 2,x
	pshb
	psha
	ldab 1,x
	ldaa 0,x
	pshb
	psha
	ldab <hol0+5
	ldaa <hol0+4
	pshb		! save filename pointer
	psha
	ldab <hol0+1
	ldaa <hol0
	pshb		! save linenumber
	psha
	ldx <ERRPROC
	clr ERRPROC
	clr ERRPROC+1
	ldab <TRAPVAL
	clra
	pshb
	psha
	jsr 0,x
	ins
	ins
	pula
	pulb
	staa <hol0
	stab <hol0+1
	pula
	pulb
	staa <hol0+4
	stab <hol0+5
	pula
	pulb
	staa <RETURN
	stab <RETURN+1
	pula
	pulb
	staa <RETURN+2
	stab <RETURN+3
	rts

Rtt:
	jmp Ret0	! return to Trap

.sect .data
traptbl:
	.data1	0
	.data2	EARRAY
	.data1	1
	.data2	ERANGE
	.data1	2
	.data2	ESET
	.data1	3
	.data2	EIOVFL
	.data1	10
	.data2	ECONV
	.data1	16
	.data2	ESTACK
	.data1	17
	.data2	EHEAP
	.data1	19
	.data2	EODDZ
	.data1	20
	.data2	ECASE
	.data1	25
	.data2	EBADMON
	.data1	26
	.data2	EBADLIN
	.data1	27
	.data2	EBADGTO
trapend:
!
Mtrap:
	ldx #traptbl
	bra 2f
1:	inx
	inx
	inx
	cpx #trapend
	beq errorend
2:	cmpb 0,x
	bne 1b
	ldx 1,x
	jsr _putstr
	jmp errorend
errorend:
	ldx TRAPVAL
	jmp EXIT
