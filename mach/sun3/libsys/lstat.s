#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __lstat
__lstat: SYSTEM(SYS_lstat)
