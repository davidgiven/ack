#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _readv
_readv: SYSTEM(SYS_readv)
