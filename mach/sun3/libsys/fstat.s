#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define ___fstat
___fstat: SYSTEM(SYS_fstat)
