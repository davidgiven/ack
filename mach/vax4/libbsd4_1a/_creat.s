.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__creat
creat = 8

.align	1
__creat:
	.data2	0x0000
	chmk	$creat
	bcc 	1f
	jmp 	errmon
1:
	ret
