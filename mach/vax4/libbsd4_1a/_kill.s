.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__kill
kill = 37

.align	1
__kill:
	.data2	0x0000
	chmk	$kill
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
