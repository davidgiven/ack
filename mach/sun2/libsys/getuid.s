#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getuid
_getuid: SYSTEM(SYS_getuid)
