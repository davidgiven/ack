.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_times
times = 43

.align	1
_times:
	.data2	0x0000
	chmk	$times
	ret
