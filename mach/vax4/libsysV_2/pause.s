.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
pause = 29
.define	_pause

_pause:
	.data2	0x0000
	chmk	$pause
	bcc	1f
	jmp	cerror
1:
	clrl	r0
	ret
