#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _creat
_creat: SYSTEM(SYS_creat)
