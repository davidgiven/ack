#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _ptrace
.extern _errno
_ptrace:
	clr.l	_errno
	SYSTEM(SYS_ptrace)
