#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _readlink
_readlink: SYSTEM(SYS_readlink)
