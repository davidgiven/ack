.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__ftime
ftime = 35

.align	1
__ftime:
	.data2	0x0000
	chmk	$ftime
	ret
