#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getpagesize
_getpagesize: SYSTEM(SYS_getpagesize)
