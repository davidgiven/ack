#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _geteuid
_geteuid:
	pea	(SYS_getuid)
	trap	#0
	move.l	d1,d0
	rts
