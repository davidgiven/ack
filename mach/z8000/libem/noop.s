.define noop

noop:
	push	*RR14, hol0
	push	*RR14, $fmt
	calr	prf
	ret
.data
fmt:	.asciz	"test %d\n"
