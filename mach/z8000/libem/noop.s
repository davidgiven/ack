.define noop
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

noop:
	push	*RR14, hol0
	push	*RR14, $fmt
	calr	prf
	ret
.sect .data
fmt:	.asciz	"test %d\n"
