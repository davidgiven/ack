#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __stat
__stat: SYSTEM(SYS_stat)
