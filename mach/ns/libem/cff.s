.define .cff
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.cff :	cmpd 4(sp), 8(sp)
	beq 1f
	cmpd 4, 4(sp)
	beq 4f
	cmpd 8, 4(sp)
	beq 8f
1:
	ret 8
4:
	movlf 12(sp),16(sp)
	ret 12
8:
	movfl 12(sp), 8(sp)
	ret 4
