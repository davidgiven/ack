.define	.cii
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

!r3 will save return addres
.cii:	
	movd tos, r3
	cmpqd 4, tos
	bne 3f
	movd tos, r0
	cmpqd 1, r0
	beq 1f
	cmpqd 2, r0
	beq 2f
	cmpqd 4, r0
	beq 4f
3:
	movd EILLINS, tos
	jsr @.trp
4:
	movd r3, tos
	ret 0
1:	
	movxbd 0(sp), 0(sp)
	br 4b
2:
	movxwd 0(sp), 0(sp)
	br 4b
