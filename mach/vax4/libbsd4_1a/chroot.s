.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_chroot
chroot = 61
 
.define	errmon
_chroot:
	.data2	0x0000
	chmk	$chroot
	bcc	1f
	jmp	errmon
1:
	ret
