#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getrlimit
_getrlimit: SYSTEM(SYS_getrlimit)
