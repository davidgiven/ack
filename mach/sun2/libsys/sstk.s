#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _sstk
_sstk: SYSTEM(SYS_sstk)
