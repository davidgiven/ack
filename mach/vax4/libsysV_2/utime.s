.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
utime = 30
.define	_utime
 
_utime:
	.data2	0x0000
	chmk	$utime
	bcc	1f
	jmp	cerror
1:
	ret
