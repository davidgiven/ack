.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_creat
creat = 8

.align	1
_creat:
	.data2	0x0000
	chmk	$creat
	bcc 	1f
	jmp 	errmon
1:
	ret
