.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
alarm = 27
.define	__alarm

__alarm:
	.data2	0x0000
	chmk	$alarm
	ret
