#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _getdirentries
_getdirentries: SYSTEM(SYS_getdirentries)
