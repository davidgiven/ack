.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
exit = 1
.define	__exit

__exit:
	.data2	0x0000
	chmk	$exit
	halt
