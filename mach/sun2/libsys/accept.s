#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _accept
_accept: SYSTEM(SYS_accept)
