.define Pop
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .end

.sect .text

Pop:
	tsx
	ldx 0,x
	ins
	ins
	pula
	pulb
	stab TMP+1
	staa TMP
	pula
	pulb
	stab TMP2+1
	staa TMP2
	ldab TMP+1
	ldaa TMP
	pshb
	psha
	ldab TMP2+1
	ldaa TMP2
    	jmp 0,x
