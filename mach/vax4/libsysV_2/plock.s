.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
plock = 45
.define	_plock

_plock:
	.data2	0x0000
	chmk	$plock
	bcc	1f
	jmp	cerror
1:
	ret
