#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _munmap
_munmap: SYSTEM(SYS_munmap)
