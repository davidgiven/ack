.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__ioctl
ioctl = 54
 
.align	1
__ioctl:
	.data2	0x0000
	chmk	$ioctl
	bcc	1f
	jmp	errmon
1:
	ret
