#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _fstat
_fstat: SYSTEM(SYS_fstat)
