#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _ustat
_ustat: SYSTEM(SYS_ustat)
