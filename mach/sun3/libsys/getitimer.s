#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getitimer
_getitimer: SYSTEM(SYS_getitimer)
