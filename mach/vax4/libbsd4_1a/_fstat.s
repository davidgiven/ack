.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__fstat
fstat = 28

.align	1
__fstat:
	.data2	0x0000
	chmk	$fstat
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
