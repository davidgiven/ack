#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _reboot
_reboot:
	pea	(SYS_reboot)
	trap	#0
	bcs	1f
	stop	#0
1:
	jmp	(cerror)
