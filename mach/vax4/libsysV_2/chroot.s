.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
chroot = 61
.define	_chroot

_chroot:
	.data2	0x0000
	chmk	$chroot
	bcc	1f
	jmp	cerror
1:
	ret
