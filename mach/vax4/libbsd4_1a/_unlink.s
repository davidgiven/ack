.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__unlink
unlink = 10

.align	1
__unlink:
	.data2	0x0000
	chmk	$unlink
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
