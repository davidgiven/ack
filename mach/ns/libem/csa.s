.define	.csa
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

!r0 contains index
!r1 contains pointer to csa descriptor
.csa:	movd 4(r1), r2		!get lower bound
	cmpd r2, r0		
	bgt 1f			!index out of range
	addd 8(r1), r2		!get upper bound
	cmpd r2, r0
	blt 1f			!index out of range
	addr 12(r1), r2		!pointer to first pointer
	subd 4(r1), r0
	muld 4, r0
	addd r0,r2		!right pointer
	cmpqd 0, 0(r2)
	beq 1f			!null pointer
	movd 0(r2), tos		!jump now
	ret 4
1:				!jump to default pointer
	cmpqd 0, 0(r1)
	beq 2f			!null pointer
	movd 0(r1), tos
	ret 4
2:
	movd ECASE, tos
	jsr @.trp
	ret 0
