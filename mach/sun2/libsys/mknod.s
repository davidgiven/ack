#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _mknod
_mknod: SYSTEM(SYS_mknod)
