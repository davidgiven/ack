#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __lseek
__lseek: SYSTEM(SYS_lseek)
