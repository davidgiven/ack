.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__execve
exec = 59

.align	1
__execve:
	.data2	0x0000
	chmk	$exec
	jmp 	errmon
