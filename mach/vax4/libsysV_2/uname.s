.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
utssys = 57

.define	_uname

_uname:
	.data2	0x0000
	pushl	$0
	pushl	$0
	pushl	4(ap)
	calls	$3,Isys
	ret

Isys:
	.data2	0x0000
	chmk	$utssys
	bcc	1f
	jmp	cerror
1:
	clrl	r0
	ret
