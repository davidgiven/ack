#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _mount
_mount: SYSTEM(SYS_mount)
