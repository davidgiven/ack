.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_umount
umount = 22

.align	1
_umount:
	.data2	0x0000
	chmk	$umount
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
