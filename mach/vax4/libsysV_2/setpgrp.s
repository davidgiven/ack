.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
setpgrp = 39
.define	_setpgrp
.define	_getpgrp

_setpgrp:
	.data2	0x0000
	pushl	$1
	calls	$1,Ipgrp
	ret

_getpgrp:
	.data2	0x0000
	pushl	$0
	calls	$1,Ipgrp
	ret

Ipgrp:
	.data2	0x0000
	chmk	$setpgrp
	ret
