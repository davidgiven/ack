#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getfh
_getfh: SYSTEM(SYS_getfh)
