#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __exit
__exit:
	pea	(SYS_exit)
	trap	#0
	jmp	(cerror)
