.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
creat = 8
.define	_creat

_creat:
	.data2	0x0000
	chmk	$creat
	bcc 	1f
	jmp 	cerror
1:
	ret
