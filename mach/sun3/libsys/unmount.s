#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _unmount
_unmount: SYSTEM(SYS_umount)
