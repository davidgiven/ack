.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_ftime
ftime = 35

.align	1
_ftime:
	.data2	0x0000
	chmk	$ftime
	ret
