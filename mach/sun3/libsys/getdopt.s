#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getdopt
_getdopt: SYSTEM(SYS_getdopt)
