#include "syscall.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define _semsys
_semsys: SYSTEM(SYS_semsys)
