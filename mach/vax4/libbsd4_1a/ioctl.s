.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_ioctl
ioctl = 54
 
.align	1
_ioctl:
	.data2	0x0000
	chmk	$ioctl
	bcc	1f
	jmp	errmon
1:
	ret
