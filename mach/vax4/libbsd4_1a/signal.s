.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_signal
signal = 48

.align	1
_signal:
	.data2	0x0000
	chmk	$signal
	bcc	1f
	jmp	errmon
1:
	ret
