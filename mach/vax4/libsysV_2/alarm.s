.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
alarm = 27
.define	_alarm

_alarm:
	.data2	0x0000
	chmk	$alarm
	ret
