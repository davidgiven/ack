.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_vtimes
vtimes = 107

.align	1
_vtimes:
	.data2	0x0000
	chmk	$vtimes
	ret
