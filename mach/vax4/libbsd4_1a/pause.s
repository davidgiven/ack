.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_pause
pause = 29

.align	1
_pause:
	.data2	0x0000
	chmk	$pause
	ret
