#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _stat
_stat: SYSTEM(SYS_stat)
