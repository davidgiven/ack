#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _symlink
_symlink: SYSTEM(SYS_symlink)
