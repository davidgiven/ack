.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_stat
stat = 18

.align	1
_stat:
	.data2	0x0000
	chmk	$stat
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
