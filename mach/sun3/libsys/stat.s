#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _stat
_stat: pea (SYS_stat); trap #0; bcc 1f; jmp (cerror)
1:
	move.l	(sp)+,(sp)
	jmp	(___statcvt)
