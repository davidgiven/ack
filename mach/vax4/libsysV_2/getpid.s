.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
getpid = 20
.define	_getpid

_getpid:
	.data2	0x0000
	chmk	$getpid
	ret
