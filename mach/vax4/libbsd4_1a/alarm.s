.globl	_alarm
.set	alarm,27

.align	1
_alarm:
	.word	0x0000
	chmk	$alarm
	ret
