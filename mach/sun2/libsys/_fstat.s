#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __fstat
__fstat: SYSTEM(SYS_fstat)
