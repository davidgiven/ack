#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _statfs
_statfs: SYSTEM(SYS_statfs)
