.define .cif
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.cif :	cmpd 4, 4(sp)
	beq 4f
	cmpd 8, 4(sp)
	beq 8f
	ret 8
4:
	movdf 12(sp),12(sp)
	ret 8
8:
	movdl 12(sp), 8(sp)
	ret 4
