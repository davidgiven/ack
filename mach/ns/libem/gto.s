.define	.gto
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

!r0 contains descriptor addres
.gto:	
	lprd	fp,8(r0)
	lprd	sp,4(r0)
	movd	0(r0),tos
	ret	0
