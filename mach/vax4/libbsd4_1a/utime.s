.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_utime
utime = 30
 
.align	1
_utime:
	.data2	0x0000
	chmk	$utime
	bcc	1f
	jmp	errmon
1:
	ret
