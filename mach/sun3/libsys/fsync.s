#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _fsync
_fsync: SYSTEM(SYS_fsync)
