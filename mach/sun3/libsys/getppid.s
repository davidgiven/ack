#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getppid
_getppid:
	pea	(SYS_getpid)
	trap	#0
	move.l	d1,d0
	rts
