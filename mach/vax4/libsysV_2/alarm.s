.set	alarm,27
.globl	_alarm

_alarm:
	.word	0x0000
	chmk	$alarm
	ret
