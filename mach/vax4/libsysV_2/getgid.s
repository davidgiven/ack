.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
getgid = 47
.define	_getgid

_getgid:
	.data2	0x0000
	chmk	$getgid
	ret
