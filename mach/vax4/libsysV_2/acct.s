.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
acct = 51
.define	_acct
.define  cerror

_acct:
	.data2	0x0000
	chmk	$acct
	bcc 	1f
	jmp 	cerror
1:
	ret
