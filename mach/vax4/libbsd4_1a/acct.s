.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_acct
acct = 51

.align	1
_acct:
	.data2	0x0000
	chmk	$acct
	bcc 	1f
	jmp 	errmon
1:
	ret
