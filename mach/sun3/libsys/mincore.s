#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _mincore
_mincore: SYSTEM(SYS_mincore)
