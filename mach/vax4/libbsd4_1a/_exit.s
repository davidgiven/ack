.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__exit
exit = 1

.align	1
__exit:
	.data2	0x0000
	chmk	$exit
	halt
