.set	setpgrp,39
.globl	_setpgrp
.globl	_getpgrp

_setpgrp:
	.word	0x0000
	pushl	$1
	calls	$1,pgrp
	ret

_getpgrp:
	.word	0x0000
	pushl	$0
	calls	$1,pgrp
	ret

pgrp:
	.word	0x0000
	chmk	$setpgrp
	ret
