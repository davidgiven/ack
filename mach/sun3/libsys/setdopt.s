#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _setdopt
_setdopt: SYSTEM(SYS_setdopt)
