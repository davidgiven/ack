#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define __creat
__creat: SYSTEM(SYS_creat)
