.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__link
link = 9

.align	1
__link:
	.data2	0x0000
	chmk	$link
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
