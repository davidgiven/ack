#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _chmod
_chmod: SYSTEM(SYS_chmod)
