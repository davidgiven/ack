#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getegid
_getegid:
	pea	SYS_getgid
	trap	#0
	move.l	d1,d0
	rts
