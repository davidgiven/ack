#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _connect
_connect: SYSTEM(SYS_connect)
