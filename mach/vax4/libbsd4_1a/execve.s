.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_execve
exec = 59

.align	1
_execve:
	.data2	0x0000
	chmk	$exec
	jmp 	errmon
