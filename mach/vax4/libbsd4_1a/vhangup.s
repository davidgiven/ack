.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_vhangup
vhangup = 76

.align	1
_vhangup:
	.data2	0x0000
	chmk	$vhangup
	ret
