.define	.rck
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

.rck:	
	cmpd 0(4(sp)), 8(sp)
	bgt 1f
	cmpd 4(4(sp)), 8(sp)
	bge 2f
1:
	movd ERANGE, tos
	jsr @.trp
2:
	ret 4
