#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getdtablesize
_getdtablesize: SYSTEM(SYS_getdtablesize)
