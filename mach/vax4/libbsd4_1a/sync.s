.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_sync
sync = 36

.align	1	
_sync:
	.data2	0x0000
	chmk	$sync
	ret
