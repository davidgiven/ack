.define Ret0
.define Ret1
.define Ret2
.define Ret4
.define Ret8,Lfr8

.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end

.sect .text
! Copy the eight-byte result from the stack into the return area.
Ret8:
	ldx	#RETURN
1:	pula
	staa	0,x
	inx
	cpx	#RETURN+8
	bne	1b
	bra	Ret0

! Save the helper's return address before pushing the result.
Lfr8:
	tsx
	ldx	0,x
	stx	<TMP
	ins
	ins
	ldx	#RETURN+8
1:	dex
	ldaa	0,x
	psha
	cpx	#RETURN
	bne	1b
	ldx	<TMP
	jmp	0,x

Ret1:
	stab	<RETURN
	bra	1f
Ret4:
	stab	<RETURN+1
	staa	<RETURN
	pula
	pulb
	stab	<RETURN+3
	staa	<RETURN+2
	bra	1f
Ret2:
	stab	<RETURN+1
	staa	<RETURN
1:
Ret0:
	ldx	<LB
	txs
	pula
	pulb
	stab	<LB+1
	staa	<LB
	subb	#BASE
	sbca	#0
	stab	<LBl+1
	staa	<LBl
	rts
