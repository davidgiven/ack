#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _umount
_umount: SYSTEM(SYS_umount)
