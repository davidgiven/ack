#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _lstat
_lstat: SYSTEM(SYS_lstat)
