.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
umount = 22
.define	_umount

_umount:
	.data2	0x0000
	chmk	$umount
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
