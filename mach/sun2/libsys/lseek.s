#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _lseek
_lseek: SYSTEM(SYS_lseek)
