#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _mmap
_mmap: SYSTEM(SYS_mmap)
