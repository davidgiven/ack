.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
signal = 48
.define	_signal

_signal:
	.data2	0x0000
	chmk	$signal
	bcc 	1f
	jmp 	cerror
1:
	ret
