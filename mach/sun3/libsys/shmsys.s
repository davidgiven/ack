#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _shmsys
_shmsys: SYSTEM(SYS_shmsys)
