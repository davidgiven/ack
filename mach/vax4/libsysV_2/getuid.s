.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
getuid = 24
.define	_getuid

_getuid:
	.data2	0x0000
	chmk	$getuid
	ret
