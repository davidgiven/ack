.define	.csb
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

	.sect .text

!r0 contains index
!r1 contains pointer to csb descriptor
.csb:
	movd 4(r1), r2		!number of entries
	cmpqd 0, r2
	beq 3f
	movd r1, r3
1:
	addd 8, r3		!find addres of next index
	cmpd 0(r3), r0		!compare indices
	beq 2f
	acbd -1, r2, 1b
3:				!r1 now contains right pointer
	cmpqd 0, 0(r1)		!test destination addres
	beq 4f
	movd 0(r1), tos		!jump to destination
	ret 4
2:
	addr 4(r3), r1		!put destination pointer in r1
	br 3b
4:
	movd ECASE, tos
	jsr @.trp
	ret 0
