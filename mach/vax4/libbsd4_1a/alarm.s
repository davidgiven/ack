.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_alarm
alarm = 27

.align	1
_alarm:
	.data2	0x0000
	chmk	$alarm
	ret
