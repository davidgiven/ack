.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__alarm
alarm = 27

.align	1
__alarm:
	.data2	0x0000
	chmk	$alarm
	ret
