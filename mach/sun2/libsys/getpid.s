#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getpid
_getpid: SYSTEM(SYS_getpid)
