.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
execve = 59
.define	_exect

_exect:
	.data2	0x0000
	bispsw	$0x10
	chmk	$execve
	jmp 	cerror
