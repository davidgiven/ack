#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _socketpair
_socketpair: SYSTEM(SYS_socketpair)
