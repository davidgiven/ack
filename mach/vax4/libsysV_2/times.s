.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
times = 43
.define	_times

_times:
	.data2	0x0000
	chmk	$times
	bcc	1f
	jmp	cerror
1:
	ret
