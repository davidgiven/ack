.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
getpid = 20
.define	__getpid

__getpid:
	.data2	0x0000
	chmk	$getpid
	ret
