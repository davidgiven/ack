.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
stat = 18
.define	_stat

_stat:
	.data2	0x0000
	chmk	$stat
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
