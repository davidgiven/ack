.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__pause
pause = 29

.align	1
__pause:
	.data2	0x0000
	chmk	$pause
	ret
