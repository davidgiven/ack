.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
sync = 36
.define	_sync

_sync:
	.data2	0x0000
	chmk	$sync
	ret
